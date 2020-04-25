#ifndef TCP_SERVER_H
#define TCP_SERVER_H

#include "acceptor.h"
#include "server_event.h"
#include "server_channel.h"
#include "tcp_connection.h"

typedef struct {
    EventLoop *event_loop;
    Acceptor *acceptor;
    ConnectCompletedCallBack connect_completed_cb;  // 连接完成回调
    ReadMessageCallBack      read_msg_cb;           // 应用层读取数据的回调
    WriteCompletedCallBack   write_completed_cb;    // 发送完数据给应用层的回调
} TcpServer;

TcpServer* TcpServerInit(EventLoop *event_loop, Acceptor *acceptor,
                         ConnectCompletedCallBack connect_completed_cb,
                         ReadMessageCallBack read_msg_cb,
                         WriteCompletedCallBack write_completed_cb);

// 开始运行
void TcpServerStart(TcpServer *tcp_server);

#endif  // TCP_SERVER_H