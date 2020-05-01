#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/epoll.h>
#include <arpa/inet.h>
#include <signal.h>
#include "../base/timer.h"
#include <map>
#include <errno.h>

using namespace std;

// 用 ListTimer 来处理非活跃链接

#define MAX_EVENT_COUNT 1024
#define MAX_BUFFER_SIZE 1024
#define MAX_CLIENT_COUNT 256
#define TIME_SLOT 50

static int pipe_fd[2];

void AcceptorHandler(int , ListTimer &, int , map<int, TimerUnit*> &);
void EventSignal(int , bool &, bool &);
void EventClient(int sock_fd, ListTimer &lst_timer, map<int, TimerUnit*> &client_timers);

void SetNonBlocking(int fd)
{
    int flags = fcntl(fd, F_GETFL);
    flags |= O_NONBLOCK;
    if (fcntl(fd, F_SETFL, flags) < 0) {
        printf("SetNonBlocking fcntl failed");
        exit(-1);
    }
}

int CreateListenFD(int port)
{
    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    int listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_fd < 0) {
        printf("socket failed.\n");
        exit(-1);
    }
    int reuse = 1;
    setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));
    if (bind(listen_fd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) != 0) {
        printf("bind failed.\n");
        exit(-1);
    }
    if (listen(listen_fd, 5) < 0) {
        printf("listen failed.\n");
        exit(-1);
    }
    return listen_fd;
}

struct TimerCBData {
    int sock_fd;
    int epoll_fd;
};

void EpollAddFD(int epoll_fd, int fd)
{
    struct epoll_event event;
    event.data.fd = fd;
    event.events = EPOLLIN | EPOLLET;
    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd, &event) < 0) {
        printf("epoll_ctl failed.\n");
        exit(-1);
    }
    SetNonBlocking(fd);
}

void SignalHandler(int sig)
{
    int msg = sig;
    send(pipe_fd[1], (char *)&msg, 1, 0);
}

void AddSignal(int sig)
{
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = SignalHandler;
    sa.sa_flags |= SA_RESTART;
    sigfillset(&sa.sa_mask);
    if (sigaction(sig, &sa, NULL) < 0) {
        printf("sigaction failed.\n");
        exit(-1);
    }
}

void TimerHandler(ListTimer &lst_timer)
{
    printf("TimerHandler.\n");
    lst_timer.Tick();
    alarm(TIME_SLOT);
}

void TimerCallbackFunc(void *args)
{
    TimerCBData *data = static_cast<TimerCBData *>(args);
    epoll_ctl(data->epoll_fd, EPOLL_CTL_DEL, data->sock_fd, 0);
    close(data->sock_fd);
    printf("close fd[%d]\n", data->sock_fd);
}

int main(int argc, char *argv[])
{
    if (argc != 2) {
        printf("usage: %s port\n", argv[0]);
        exit(-1);
    }
    int port = atoi(argv[1]);

    int listen_fd = CreateListenFD(port);
    struct epoll_event events[MAX_EVENT_COUNT];
    int epoll_fd = epoll_create(5);
    if (epoll_fd == -1) {
        printf("epoll_create failed.\n");
        exit(-1);
    }
    EpollAddFD(epoll_fd, listen_fd);

    int ret = socketpair(AF_LOCAL, SOCK_STREAM, 0, pipe_fd);
    if (ret < 0) {
        printf("socketpair failed.\n");
        exit(-1);
    }

    EpollAddFD(epoll_fd, pipe_fd[0]);

    // 设置信号处理函数
    AddSignal(SIGALRM);
    AddSignal(SIGTERM);

    // 定时
    alarm(TIME_SLOT);

    // 客户端对应的timer
    map<int, TimerUnit*> client_timers;

    ListTimer lst_timer;

    bool run_server = true;
    bool timeout = false;
    printf("begin run server.\n");
    while (run_server) {
        int cnt = epoll_wait(epoll_fd, events, MAX_EVENT_COUNT, -1);
        if (cnt < 0 && errno != EINTR) {
            printf("epoll_wait failed.\n");
            break;
        }
        for (int i = 0; i < cnt; ++i) {
            int sock_fd = events[i].data.fd;
            if (sock_fd == listen_fd) {
                AcceptorHandler(sock_fd, lst_timer, epoll_fd, client_timers);
            } else if (sock_fd == pipe_fd[0] && (events[i].events & EPOLLIN)) {
                // 信号处理
                EventSignal(sock_fd, timeout, run_server);
            } else if (events[i].events & EPOLLIN) {
                // 链接上的数据处理
                EventClient(sock_fd, lst_timer, client_timers);
            }
        }
        // 超时
        if (timeout) {
            TimerHandler(lst_timer);
            timeout = false;
        }
        lst_timer.Print();
    }

}

void AcceptorHandler(int listen_fd, ListTimer &lst_timer, int epoll_fd, map<int, TimerUnit*> &client_timers)
{
    int conn_fd = accept(listen_fd, NULL, NULL);
    if (conn_fd < 0) {
        printf("accept failed.\n");
        return ;
    }

    EpollAddFD(epoll_fd, conn_fd);
    TimerUnit *timer = new TimerUnit;

    TimerCBData *data = new TimerCBData;
    data->sock_fd = conn_fd;
    data->epoll_fd = epoll_fd;

    client_timers[conn_fd] = timer;

    timer->SetExpire(time(NULL) + 3 * TIME_SLOT);
    timer->SetCallback(TimerCallbackFunc, data);

    lst_timer.AddTimer(timer);
}

void EventSignal(int sock_fd, bool &timeout, bool &run_server)
{
    int sig;
    char signals[32];
    int ret = recv(sock_fd, signals, sizeof(signals), 0);
    if (ret == -1 || ret == 0) {
        return ;
    }
    for (int i = 0; i < ret; ++i) {
        switch (signals[i]) {
            case SIGALRM:
                timeout = true;
                break;
            case SIGTERM:
                run_server = false;
                break;
        }
    }
}

void EventClient(int sock_fd, ListTimer &lst_timer, map<int, TimerUnit*> &client_timers)
{
    char buff[MAX_BUFFER_SIZE];
    int ret = recv(sock_fd, buff, MAX_BUFFER_SIZE - 1, 0);
    map<int, TimerUnit*>::iterator itr = client_timers.find(sock_fd);
    if (itr == client_timers.end()) {
        return ;
    }
    if (ret <= 0) {
        printf("client %d error.\n", sock_fd);
        close(sock_fd);
        lst_timer.DelTimer(itr->second);
    } else {
        time_t now_time = time(NULL);
        itr->second->SetExpire(now_time + 3 * TIME_SLOT);
        printf("client[%d] change timer.\n", sock_fd);
        lst_timer.ChangeTimer(itr->second);
    }
}