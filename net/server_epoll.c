#include <sys/epoll.h>
#include "server_event.h"
#include "../base/log.h"

const int kMaxEvents = 128;

typedef struct {
    int ep_fd;
    struct epoll_event *events;
} EventEpollData;

static void* EpollInit();
static void EpollAdd(EventLoop *event_loop, Channel *channel);
static void EpollDel(EventLoop *event_loop, Channel *channel);
static void EpollUpdate(EventLoop *event_loop, Channel *channel);
static void EpollDispatch(EventLoop *event_loop);

const EventDispatcher epoll_dispatcher = {
    "epoll",
    EpollInit,
    EpollAdd,
    EpollDel,
    EpollUpdate,
    EpollDispatch,
}

void* EpollInit()
{
    EventEpollData *event_epoll_data = malloc(sizeof(EventEpollData));
    if (event_epoll_data == NULL) {
        ErrorExit("EpollInit malloc failed");
    }
    int ep_fd = epoll_create(1024);
    if (ep_fd < 0) {
        ErrorExit("EpollInit epoll_create failed");
    }
    event_epoll_data->ep_fd = ep_fd;
    event_epoll_data->events = calloc(kMaxEvents, sizeof(struct epoll_event));
    if (event_epoll_data->events == NULL) {
        ErrorExit("EpollInit calloc failed");
    }
    return event_epoll_data;
}

void EpollAdd(EventLoop *event_loop, Channel *channel)
{
    EventEpollData *event_epoll_data = (EventEpollData *)event_loop->event_dispatcher_data;

    int fd = channel->fd;

    struct epoll_event event;
    event.data.fd = fd;
    event.events = 0;
    if (channel->events & EVENT_READ) {
        event.events |= EPOLLIN;
    }
    if (channel->events & EVENT_WRITE) {
        event.events |= EPOLLOUT;
    }
    event.events |= EPOLLET;   // 开启ET模式
    
    int ret = epoll_ctl(event_epoll_data->ep_fd, EPOLL_CTL_ADD, fd, &event);
    if (ret == -1) {
        ErrorExit("EpollAdd epoll_ctl failed");
    }
}

void EpollDel(EventLoop *event_loop, Channel *channel)
{
    EventEpollData *event_epoll_data = (EventEpollData *)event_loop->event_dispatcher_data;

    int fd = channel->fd;

    struct epoll_event event;
    event.data.fd = fd;
    event.events = 0;
    if (channel->events & EVENT_READ) {
        event.events |= EPOLLIN;
    }
    if (channel->events & EVENT_WRITE) {
        event.events |= EPOLLOUT;
    }
    
    int ret = epoll_ctl(event_epoll_data->ep_fd, EPOLL_CTL_DEL, fd, &event);
    if (ret == -1) {
        ErrorExit("EpollDel epoll_ctl failed");
    }
}

void EpollUpdate(EventLoop *event_loop, Channel *channel)
{
    EventEpollData *event_epoll_data = (EventEpollData *)event_loop->event_dispatcher_data;

    int fd = channel->fd;

    struct epoll_event event;
    event.data.fd = fd;
    event.events = 0;
    if (channel->events & EVENT_READ) {
        event.events |= EPOLLIN;
    }
    if (channel->events & EVENT_WRITE) {
        event.events |= EPOLLOUT;
    }

    int ret = epoll_ctl(event_epoll_data->ep_fd, EPOLL_CTL_MOD, fd, &event);
    if (ret == -1) {
        ErrorExit("EpollUpdate epoll_ctl failed");
    }
}

void EpollDispatch(EventLoop *event_loop)
{
    EventEpollData *event_epoll_data = (EventEpollData *)event_loop->event_dispatcher_data;
    int n = epoll_wait(event_epoll_data->ep_fd, event_epoll_data->events, kMaxEvents, -1);
    if (n == -1) {
        ErrorExit("EpollDispatch epoll_wait failed");
    }
    LogInfo("EpollDispatch epoll_wait n[%d]", n);
    for (int i = 0; i < n; ++i) {
        // 出错
        if ((event_epoll_data->events[i].events & EPOLLERR) ||
            (event_epoll_data->events[i].events & EPOLLHUP)) {
            LogWarn("EpollDispatch fd[%d] disconnect", event_epoll_data->events[i].data.fd);
            close(event_epoll_data->events[i].data.fd);
            continue;
        }
        // 可读
        if (event_epoll_data->events[i].events & EPOLLIN) {

        }
        // 可写
        if (event_epoll_data->events[i].events & EPOLLOUT) {

        }
    }
}
