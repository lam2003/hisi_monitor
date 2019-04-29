#ifndef SOCK_HANDLER_H_
#define SOCK_HANDLER_H_

class ServerSock;
struct AcceptHandler
{
    virtual ~AcceptHandler() = default;
    virtual int OnAccept(ServerSock *server, int fd) = 0;
};

#endif