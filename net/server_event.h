#ifndef SERVER_EVENT_H
#define SERVER_EVENT_H

#include "server_channel.h"

typedef struct EventLoop EventLoop;

typedef struct {
    const char *name;  // "select" "poll" "epoll"

    void* (*Init)();
    void (*Add)(EventLoop *event_loop, Channel *channel);
    void (*Del)(EventLoop *event_loop, Channel *channel);
    int (*Update)(EventLoop *event_loop, Channel *channel);

    int (*Dispatch)(EventLoop *event_loop);
} EventDispatcher;

struct ChannelEntry {
    Channel *channel;
    struct ChannelEntry *next;
};

struct EventLoop {
    int stop;  // 0 run   1 stop
    // 事件处理及其数据
    EventDispatcher *event_dispatcher;
    void *event_dispatcher_data;

    // 事件队列
    ChannelEntry *queue_head;
    ChannelEntry *queue_tail;

    // 操作事件队列时需要加锁
    pthread_mutex_t mutex;
    pthread_cond_t cond;

} EventLoop;

// 事件处理器初始化
EventLoop* EventLoopInit();

// 
void EventLoopAddChannelEvent(EventLoop *event_loop, Channel *channel);

// 开始事件循环
void EventLoopRun(EventLoop *event_loop);

#endif  // SERVER_EVENT_H