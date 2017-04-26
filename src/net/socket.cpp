#include "socket.hpp"

#include "sys/server.h"
#include "sys/socket_message.h"

#include <stdexcept>
#include <cassert>

namespace net {

socket::socket()
:	fd_(-1)
{
}

socket::socket(int fd)
:	fd_(std::move(fd))
{
}

void socket::listen()
{
	if (SYS_OK != sys_listen(fd())) {
        throw std::runtime_error("net::listen");
	}
}

socket socket::accept()
{
    int connect_sock(-1);
    if (SYS_OK != sys_accept(fd(), &connect_sock)) {
        throw std::runtime_error("net::accept");
    }
    return socket(connect_sock);
}

void socket::send(std::experimental::string_view data)
{
    if (SYS_OK != sys_socket_message_send(fd(), data.data(), data.size())) {
        throw std::runtime_error("net::send");
    }
}

std::experimental::optional<std::string> socket::recv()
{
    std::uint32_t length(0);
    const auto result = sys_socket_message_recv_length(fd(), &length);
    if (SYS_EOF == result) {
        return {};
    }
    if (SYS_OK != result) {
        throw std::runtime_error("net::recv");
    }
    std::string data;
    data.resize(length);
    if (length > 0) {
        if (SYS_OK != sys_socket_message_recv_data(fd(), &data[0], length)) {
            throw std::runtime_error("net::recv");
        }
    }
    return data;
}

socket::socket(socket &&other) noexcept
:	socket()
{
    *this = std::move(other);
}

socket &socket::operator=(socket &&other) noexcept
{
    std::swap(fd_, other.fd_);
    return *this;
}

socket::~socket() noexcept
{
    if (fd_ >= 0) {
        ::close(fd_);
    }
}

socket::operator bool() const
{
    return fd_ >= 0;
}

int socket::fd() const
{
    assert(fd_ >= 0);
    return fd_;
}

} /* namespace net */
