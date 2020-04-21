#ifndef UTILS_H
#define UTILS_H

#include <fcntl.h>

// 置fd为非阻塞
void SetNonBlocking(int fd);

#endif  // UTILS_H