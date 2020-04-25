#include "tcp_server.h"

TcpServer* TcpServerInit(EventLoop *event_loop, Acceptor *acceptor,
                         ConnectCompletedCallBack connect_completed_cb,
                         ReadMessageCallBack read_msg_cb,
                         WriteCompletedCallBack write_completed_cb)
{
    TcpServer *tcp_server = (TcpServer *)malloc(sizeof(TcpServer));
    tcp_server->event_loop = event_loop;
    tcp_server->acceptor = acceptor;
    tcp_server->connect_completed_cb = connect_completed_cb;
    tcp_server->read_msg_cb = read_msg_cb;
    tcp_server->write_completed_cb = write_completed_cb;

    return tcp_server;
}

// 监听套接字可读时的回调处理函数
static void HandleConnection(void *args)
{
    TcpServer *tcp_server = (TcpServer *)args;
    Acceptor *acceptor = tcp_server->acceptor;
    int conn_fd = accept(acceptor->listen_fd, NULL, NULL);
    if (conn_fd < 0) {
        LogError("HandleConnection accept faileds");
        return;
    }
}

void TcpServerStart(TcpServer *tcp_server)
{
    EventLoop *event_loop = tcp_server->event_loop;
    Acceptor *acceptor = tcp_server->acceptor;
    // 将 acceptor 作为 channel 加入 event_loop 中
    Channel *channel = ChannelInit(acceptor->listen_fd, EVENT_READ, HandleConnection, NULL, tcp_server);
    EventLoopAddChannelEvent(event_loop, channel);
}
