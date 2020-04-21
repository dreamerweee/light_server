#include "utils.h"
#include "log.h"

void SetNonBlocking(int fd)
{
    int flags = fcntl(fd, F_GETFL);
    flags |= O_NONBLOCK;
    if (fcntl(fd, F_SETFL, flags) < 0) {
        ErrorExit("SetNonBlocking fcntl failed");
    }
}