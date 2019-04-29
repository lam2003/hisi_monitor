#include "common/server_sock.h"
#include "common/res_code.h"

template <class T, void (T::*method)(int), class W>
void ev_io_cb(struct ev_loop *loop, W *ev, int events)
{
    T *obj = static_cast<T *>(ev->data);
    (obj->*method)(events);
}

void ServerSock::SetAcceptHandler(AcceptHandler *hdl)
{
    hdl_ = hdl;
}

void ServerSock::SetLoop(struct ev_loop *loop)
{
    loop_ = loop;
}

ServerSock::ServerSock()
{
    ev_init(&ev_, &(ev_io_cb<ServerSock, &ServerSock::OnAccept>));
    ev_.data = this;
}

ServerSock::~ServerSock()
{
    if (loop_)
        ev_io_stop(loop_, &ev_);
}

int ServerSock::ListenOn(const std::string &ip, uint8_t port, int max_conn)
{
    if (sock_helper_.Socket() < 0)
    {
        log_e("create socket failed,%s", strerror(errno));
        return static_cast<int>(KSystemError);
    }

    if (sock_helper_.SetReuse() != 0)
    {
        log_e("socket set reuse failed,%s", strerror(errno));
        return static_cast<int>(KSystemError);
    }

    if (sock_helper_.SetNonBlock() != 0)
    {
        log_e("socket set nonblock failed,%s", strerror(errno));
        return static_cast<int>(KSystemError);
    }

    if (sock_helper_.SetNonDelay() != 0)
    {
        log_e("socket set nondelay failed,%s", strerror(errno));
        return static_cast<int>(KSystemError);
    }

    ev_io_set(&ev_, sock_helper_.GetSocket(), EV_READ);

    if (sock_helper_.Bind(ip, port) != 0)
    {
        log_e("socket bind %s:%d failed,%s", ip.c_str(), port, strerror(errno));
        return static_cast<int>(KSystemError);
    }

    if (sock_helper_.Listen(max_conn) != 0)
    {
        log_e("socket listen failed,%s", strerror(errno));
        return static_cast<int>(KSystemError);
    }

    ev_io_start(loop_, &ev_);

    return static_cast<int>(KSuccess);
}

void ServerSock::OnAccept(int revents)
{
    int sockfd;
    while ((sockfd = sock_helper_.Accept()) > 0)
    {
        if (hdl_ && hdl_->OnAccept(this, sockfd) != 0)
            sock_helper_.Close(sockfd);
    }
}
