#include "tcp_connection.h"

static void HandleRead(void *args)
{
    TcpConnection *tcp_connection = (TcpConnection *)args;
}

static void HandleWrite(void *args)
{
    TcpConnection *tcp_connection = (TcpConnection *)args;
}

TcpConnection* TcpConnectionInit(int fd, EventLoop *event_loop, 
                                 ConnectCompletedCallBack connect_completed_cb,
                                 ReadMessageCallBack read_msg_cb,
                                 WriteCompletedCallBack write_completed_cb)
{
    TcpConnection *tcp_connection = (TcpConnection *)malloc(sizeof(TcpConnection));
    if (tcp_connection == NULL) {
        ErrorExit("TcpConnectionInit malloc failed");
    }
    tcp_connection->event_loop = event_loop;
    tcp_connection->input_buffer = BufferInit();
    tcp_connection->output_buffer = BufferInit();
    tcp_connection->connect_completed_cb = connect_completed_cb;
    tcp_connection->read_msg_cb = read_msg_cb;
    tcp_connection->write_completed_cb = write_completed_cb;

    // 将 tcp_connection 作为 channel 加入 event_loop 中
    Channel *channel = ChannelInit(fd, EVENT_READ, HandleRead, HandleWrite, tcp_connection);
    EventLoopAddChannelEvent(event_loop, channel);

    return tcp_connection;
}
