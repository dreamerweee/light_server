#include "acceptor.h"

struct Acceptor* AcceptorInit(int port)
{
    Acceptor *acceptor = (Acceptor*)malloc(sizeof(Acceptor));
    acceptor->listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (acceptor->listen_fd < 0) {
        ErrorExit("AcceptorInit socket failed");
    }

    // 设置非阻塞
    SetNonBlocking(acceptor->listen_fd);

    struct sockaddr_in svr_addr;
    memset(&svr_addr, 0, sizeof(svr_addr));
    svr_addr.sin_family = AF_INET;
    svr_addr.sin_port = htons(port);
    svr_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    // 地址复用
    int on = 1;
    int ret = setsockopt(acceptor->listen_fd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
    if (ret < 0) {
        ErrorExit("AcceptorInit setsockopt failed");
    }

    ret = bind(acceptor->listen_fd, (struct sockaddr *)&svr_addr, sizeof(svr_addr));
    if (ret < 0) {
        ErrorExit("AcceptorInit bind failed");
    }

    ret = listen(acceptor->listen_fd, 64);
    if (ret < 0) {
        ErrorExit("AcceptorInit listen failed");
    }

    return acceptor;
}
