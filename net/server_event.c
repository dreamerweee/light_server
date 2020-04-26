#include "server_event.h"

// 创建一个EventLoop对象
EventLoop* EventLoopInit()
{
    EventLoop *event_loop = malloc(sizeof(EventLoop));
    event_loop->stop = 0;
    event_loop->event_dispatcher = &epoll_dispatcher;
    event_loop->event_dispatcher_data = event_loop->event_dispatcher->Init();

    return event_loop;
}

void EventLoopAddChannelEvent(EventLoop *event_loop, Channel *channel)
{
    
}

// 开始事件循环
void EventLoopRun(EventLoop *event_loop)
{
    while (!event_loop->stop) {
        event_loop->event_dispatcher->Dispatch(event_loop);

        // 处理事件
        EventHandleActive(event_loop);
    }
}
