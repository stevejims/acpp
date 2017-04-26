#ifndef NET_SOCKET_HPP
#define NET_SOCKET_HPP

#include <string>
#include <experimental/string_view>
#include <experimental/optional>

namespace net {

class socket {
public:
    socket();
    explicit socket(int);
    
    void listen();
    socket accept();
    void send(std::experimental::string_view);
    std::experimental::optional<std::string> recv();

    socket(socket &&) noexcept;
    socket &operator=(socket &&) noexcept;
    ~socket() noexcept;
    const socket &operator=(const socket &) = delete;
    socket(socket const &) = delete;

    explicit operator bool() const;
    int fd() const;

private:
    int fd_;
};

} /* namespace net */

#endif /* NET_SOCKET_HPP */
