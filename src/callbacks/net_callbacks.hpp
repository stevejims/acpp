#ifndef NET_CALLBACKS_HPP
#define NET_CALLBACKS_HPP

#include "net/bind.hpp"
#include "net/socket.hpp"
#include <functional>
#include <memory>

namespace net_callbacks {

void loop();

using std::experimental::optional;
using std::experimental::string_view;
using std::function;
using std::string;

class socket {
public:
    explicit socket(net::socket s);

    socket(socket &&) noexcept = default;
    socket &operator=(socket &&) noexcept = default;
    ~socket() noexcept;
    const socket &operator=(const socket &) = delete;
    socket(socket const &) = delete;
    
    void listen();
    void accept(function<void(socket)> cb);
    void send(string_view data);
    void recv(function<void(optional<string>)> cb);
    
private:
    net::socket socket_;
    
    using callback = function<void()>;
    std::unique_ptr<callback> callback_;
    friend class reactor;
    void on_readable(callback);
};

socket bind_tcp(const string &bind_port);
socket bind_local(const string &path);

} /* namespace net_callbacks */

#endif /* NET_CALLBACKS_HPP */
