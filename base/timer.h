#ifndef TIMER_H
#define TIMER_H

#include <time.h>

/*
* 基于升序链表的定时器，使用链表作为容器来串联所有的定时器
* 定时器通常包含：超时时间、任务回调函数，有时可能包含回调函数所需的参数
*/

typedef void (*TimerCallback)(void *args);

struct TimerUnit {
    time_t expire;    // 任务超时时间，使用时间戳
    TimerCallback cb_func;  // 任务回调函数
    void *cb_args;    // 回调函数参数
    TimerUnit *prev;
    TimerUnit *next;

    TimerUnit() : prev(NULL), next(NULL)
    { }
    void SetExpire(time_t time_val)
    {
        expire = time_val;
    }
    void SetCallback(TimerCallback func, void *args)
    {
        cb_func = func;
        cb_args = args;
    }
};

class ListTimer {
public:
    ListTimer() : m_head(NULL), m_tail(NULL)
    { }
    ~ListTimer();

    void AddTimer(TimerUnit *timer);
    void ChangeTimer(TimerUnit *timer);
    void DelTimer(TimerUnit *timer);
    // SIGALRM 信号每次被触发就在信号处理函数中调用此函数
    void Tick();
    void Print() const;

private:
    // 调整定时器
    void AdjustTimer(TimerUnit *timer);

private:
    TimerUnit *m_head;
    TimerUnit *m_tail;
};

#endif  // TIMER_H