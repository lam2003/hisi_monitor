#ifndef SOCK_HELPER_H
#define SOCK_HELPER_H

#include <string>

class SockHelper
{
public:
    SockHelper();
    ~SockHelper();

    int Socket(int type = SOCK_STREAM)
    {
        sockfd_ = ::socket(AF_INET, type, 0);
        return sockfd_;
    }

    int SetReuse()
    {
        int op = 1;
        return ::setsockopt(GetSocket(), SOL_SOCKET, SO_REUSEADDR, (const char *)&op, sizeof(op));
    }

    int SetNonBlock()
    {
        int fflags;
        if ((fflags = ::fcntl(GetSocket(), F_GETFL)) != 0)
            return -1;

        fflags |= O_NONBLOCK;

        if (::fcntl(GetSocket(), F_SETFL, fflags))
            return -1;
        return 0;
    }

    int SetNonDelay()
    {
        int op = 1;
        return ::setsockopt(GetSocket(), IPPROTO_TCP, TCP_NODELAY, (const char *)&op, sizeof(op));
    }

    int GetSocket()
    {
        return sockfd_;
    }

    int Bind(const std::string &ip, int8_t port)
    {
        struct sockaddr_in sa;
        memset(&sa, 0, sizeof(sa));

        sa.sin_family = AF_INET;
        sa.sin_port = htons(port);
        if (inet_aton(ip.c_str(), &sa.sin_addr) != 0)
            return -1;

        if (::bind(GetSocket(), (struct sockaddr *)&sa, sizeof(sa)) != 0)
            return -1;

        ip_ = ip;
        port_ = port;

        return 0;
    }

    int Listen(int max_conn)
    {
        return ::listen(GetSocket(), max_conn);
    }

    int Accept()
    {
        struct sockaddr_in sa;
        socklen_t len = sizeof(sa);
        return ::accept(GetSocket(), (struct sockaddr *)&sa, &len);
    }

    void Close(int sockfd)
    {
        ::close(sockfd);
    }

    void Close()
    {
        Close(GetSocket());
    }

private:
    int sockfd_;
    std::string ip_;
    int8_t port_;
};

#endif
