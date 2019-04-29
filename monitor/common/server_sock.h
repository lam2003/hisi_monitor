#ifndef SERVER_SOCK_H_
#define SERVER_SOCK_H_

extern "C"
{
#include <ev.h>
}

#include "common/sock_handler.h"
#include "common/sock_helper.h"

class ServerSock
{
public:
    ServerSock();
    ~ServerSock();

    int ListenOn(const std::string &ip, uint8_t port, int max_conn);

    void SetAcceptHandler(AcceptHandler *hdl);

    void SetLoop(struct ev_loop *loop);

    void OnAccept(int revent);

private:
    SockHelper sock_helper_;
    struct ev_io ev_;
    struct ev_loop *loop_;
    AcceptHandler *hdl_;
};

#endif