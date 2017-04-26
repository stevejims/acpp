#include "net_coroutines.hpp"
#include "net/epoll.hpp"

using namespace net_coroutines::detail;

namespace net_coroutines {

class scheduler {
public:
    void add(socket &);
    void del(socket &);
    void schedule(coro_t);
    void loop();

private:
    net::epoll_ptr epoll_;
};

void scheduler::add(socket &s)
{
    epoll_.add(s.socket_.fd(), s.blocked_.get());
}

void scheduler::del(socket &s)
{
    epoll_.del(s.socket_.fd());
}

void scheduler::schedule(coro_t coro)
{
    if (coro) {
        auto &socket = coro.get();
        assert (! *socket.blocked_);
        *socket.blocked_ = std::move(coro);
    }
}

void scheduler::loop()
{
    for (;;) {
        auto *coro_p = static_cast<coro_t *>(epoll_.wait());
        (*coro_p)();
        schedule(std::move(*coro_p));
    }
}

scheduler &get_scheduler()
{
    static scheduler instance;
    return instance;
}

namespace detail {

void schedule(coro_t coro)
{
    get_scheduler().schedule(std::move(coro));
}

} /* namespace detail */


void loop()
{
    get_scheduler().loop();
}

socket::socket(net::socket s)
:	socket_(std::move(s)),
    blocked_(std::make_unique<coro_t>())
{
    get_scheduler().add(*this);
}

socket::~socket() noexcept
{
    if (socket_) {
        get_scheduler().del(*this);
    }
}
    
void socket::listen()
{
    socket_.listen();
}

socket socket::accept(yield_t &yield)
{
    yield(*this);
    return socket(socket_.accept());
}

void socket::send(std::experimental::string_view data)
{
    socket_.send(data);
}
    
std::experimental::optional<std::string> socket::recv(yield_t &yield)
{
    yield(*this);
    return socket_.recv();
}

socket bind_tcp(const std::string &bind_port)
{
    return socket(net::bind_tcp(bind_port));
}

socket bind_local(const std::string &path)
{
    return socket(net::bind_local(path));
}

} /* namespace net_coroutines */
