#include "timer.h"
#include <stdio.h>

ListTimer::~ListTimer()
{
    TimerUnit *curr = m_head;
    while (curr) {
        m_head = curr->next;
        delete curr;
        curr = m_head;
    }
}

void ListTimer::AddTimer(TimerUnit *timer)
{
    if (m_head == NULL) {
        m_head = m_tail = timer;
        return ;
    }
    TimerUnit *prev = NULL;
    TimerUnit *curr = m_head;
    while (curr && timer->expire > curr->expire) {
        prev = curr;
        curr = curr->next;
    }
    if (prev == NULL) {
        timer->next = m_head;
        m_head->prev = timer;
        m_head = timer;
    } else if (curr == NULL) {
        prev->next = timer;
        timer->prev = prev;
        m_tail = timer;
    } else {
        prev->next = timer;
        curr->prev = timer;
        timer->next = curr;
        timer->prev = prev;
    }
}

void ListTimer::ChangeTimer(TimerUnit *timer)
{
    AdjustTimer(timer);
    AddTimer(timer);
}

void ListTimer::DelTimer(TimerUnit *timer)
{
    AdjustTimer(timer);
    delete timer;
}

void ListTimer::AdjustTimer(TimerUnit *timer)
{
    TimerUnit *prev = timer->prev;
    TimerUnit *next = timer->next;
    if (prev) {
        prev->next = next;
    } else {
        m_head = next;
    }
    if (next) {
        next->prev = prev;
    } else {
        m_tail = prev;
    }
    timer->prev = NULL;
    timer->next = NULL;
}

void ListTimer::Tick()
{
    if (!m_head) {
        return ;
    }
    printf("ListTimer Tick\n");
    time_t now_time = time(NULL);
    TimerUnit *curr = m_head;
    while (curr) {
        if (now_time < curr->expire) {
            break;
        }
        curr->cb_func(curr->cb_args);
        m_head = curr->next;
        if (m_head) {
            m_head->prev = NULL;
        }
        delete curr;
        curr = m_head;
    }
}

void ListTimer::Print() const
{
    TimerUnit *curr = m_head;
    printf("TimerList: ");
    int cnt = 0;
    while (curr) {
        printf("%d ", curr->expire);
        curr = curr->next;
        ++cnt;
        if (cnt > 10) {
            break;
        }
    }
    printf("\n\n");
}