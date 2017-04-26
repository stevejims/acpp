#ifndef NET_COROUTINES_HPP
#define NET_COROUTINES_HPP

#include "net/bind.hpp"
#include "net/socket.hpp"
#include <functional>
#include <boost/coroutine/all.hpp>

namespace net_coroutines {

class socket;

namespace detail {

using coroutine_type = boost::coroutines::asymmetric_coroutine<socket&>;
using coro_t = coroutine_type::pull_type;

void schedule(coro_t coro);

} /* namespace detail */

using yield_t = detail::coroutine_type::push_type;

template <typename F>
void spawn(F func)
{
    detail::schedule(detail::coro_t(std::move(func)));
}

void loop();

class socket {
public:
    explicit socket(net::socket s);

    socket(socket &&) noexcept = default;
    socket &operator=(socket &&) noexcept = default;
    ~socket() noexcept;
    const socket &operator=(const socket &) = delete;
    socket(socket const &) = delete;
    
    void listen();
    socket accept(yield_t&);
    void send(std::experimental::string_view data);
    std::experimental::optional<std::string> recv(yield_t&);
    
private:
    net::socket socket_;
    
    std::unique_ptr<detail::coro_t> blocked_;
    friend class scheduler;
};

socket bind_tcp(const std::string &bind_port);
socket bind_local(const std::string &path);

} /* namespace net_coroutines */

#endif /* NET_COROUTINES_HPP */
