#ifndef TCP_CONNECTION_H
#define TCP_CONNECTION_H

#include "server_event.h"
#include "../base/buffer.h"
#include "../base/log.h"

// 连接成功回调函数
typedef void (*ConnectCompletedCallBack)(TcpConnection *tcp_connection);
// 应用层从 input_buffer 读取数据的回调
typedef void (*ReadMessageCallBack)(Buffer *buffer, TcpConnection *tcp_connection);
// 往 channel 发送完 output_buffer 中的数据的回调
typedef void (*WriteCompletedCallBack)(TcpConnection *tcp_connection);

typedef struct {
    EventLoop *event_loop;

    Buffer *input_buffer;   // 接收缓冲区
    Buffer *output_buffer;  // 发送缓冲区

    ConnectCompletedCallBack connect_completed_cb;
    ReadMessageCallBack read_msg_cb;
    WriteCompletedCallBack write_completed_cb;

} TcpConnection;

// 创建
TcpConnection* TcpConnectionInit(int fd, EventLoop *event_loop, 
                                 ConnectCompletedCallBack connect_completed_cb,
                                 ReadMessageCallBack read_msg_cb,
                                 WriteCompletedCallBack write_completed_cb);

// 

#endif  // TCP_CONNECTION_H