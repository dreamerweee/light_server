#ifndef EPOLL_H
#define EPOLL_H

#include <sys/epoll.h>

// 初始化 epoll
int EpollInit();
// 向 epoll 添加fd
void EpollAdd();
void EpollDel();
void EpollMod();
void EpollDispatch();

#endif  // EPOLL_H