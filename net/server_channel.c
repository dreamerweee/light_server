#include "server_channel.h"

// 初始化
Channel* ChannelInit(int fd, uint32_t events, ReadCallBack read_cb, WriteCallBack write_cb, void *data)
{
    Channel *channel = malloc(sizeof(Channel));
    channel->fd = fd;
    channel->events = events;
    channel->read_cb = read_cb;
    channel->write_cb = write_cb;
    channel->data = data;

    return channel;
}

// 设置可读
void ChannelSetReadEnable(Channel *channel)
{
    
}

// 设置可写
void ChannelSetWriteEnable(Channel *channel)
{

}
