#ifndef TCP_SERVER_H
#define TCP_SERVER_H

#include "server_event.h"

struct TcpServer {
    int port;
    struct EventLoop *event_loop;
};

struct TcpServer* TcpServerInit();

void TcpServerStart(struct TcpServer *tcp_server);

#endif  // TCP_SERVER_H