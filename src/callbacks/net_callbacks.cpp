#include "net_callbacks.hpp"
#include "net/epoll.hpp"

#include <cassert>

namespace net_callbacks {

class reactor {
public:
    void add(socket &);
    void del(socket &);
    void loop();

private:
    net::epoll_ptr epoll_;
};

void reactor::add(socket &s)
{
    epoll_.add(s.socket_.fd(), s.callback_.get());
}

void reactor::del(socket &s)
{
    epoll_.del(s.socket_.fd());
}

void reactor::loop()
{
    for (;;) {
        auto *callback_p = static_cast<socket::callback *>(epoll_.wait());
        auto callback = std::move(*callback_p);
        (*callback_p) = socket::callback();        
        std::move(callback)();
    }
}

inline
reactor &get_reactor()
{
    static reactor instance;
    return instance;
}

void loop()
{
    get_reactor().loop();
}

socket::socket(net::socket s)
:	socket_(std::move(s)),
    callback_(std::make_unique<callback>())
{
    get_reactor().add(*this);
}

socket::~socket() noexcept
{
    if (socket_) {
        get_reactor().del(*this);
    }
}
    
void socket::listen()
{
    socket_.listen();
}

void socket::accept(function<void(socket)> cb)
{
    on_readable([this, cb=std::move(cb)] {
        cb(socket(socket_.accept()));
    });
}

void socket::send(string_view data)
{
    socket_.send(data);
}
    
void socket::recv(function<void(optional<string>)> cb)
{
    on_readable([this, cb=std::move(cb)] {
        cb(socket_.recv());
    });        
}

void socket::on_readable(callback cb)
{
    assert (! *callback_);
    *callback_ = std::move(cb);
}

socket bind_tcp(const string &bind_port)
{
    return socket(net::bind_tcp(bind_port));
}

socket bind_local(const string &path)
{
    return socket(net::bind_local(path));
}

} /* namespace net_callbacks */
