#ifndef SERVER_CHANNEL_H
#define SERVER_CHANNEL_H

#include "server_event.h"

#define EVENT_READ  0x01  // 可读事件
#define EVENT_WRITE 0x02  // 可写事件

typedef void (*ReadCallBack)(void *args);
typedef void (*WriteCallBack)(void *args);

typedef struct {
    int fd;
    uint32_t events;
    ReadCallBack read_cb;
    WriteCallBack write_cb;
    void *data;
} Channel;

// 初始化channel
Channel* ChannelInit(int fd, uint32_t events, ReadCallBack read_cb, WriteCallBack write_cb, void *data);

// 设置可读
void ChannelSetReadEnable(Channel *channel);

// 设置可写
void ChannelSetWriteEnable(Channel *channel);

#endif  // SERVER_CHANNEL_H