#ifndef ACCEPTOR_H
#define ACCEPTOR_H

#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <memory.h>
#include "../base/utils.h"
#include "../base/log.h"

typedef struct {
    int listen_fd;
} Acceptor;

struct Acceptor* AcceptorInit(int port);

#endif  // ACCEPTOR_H