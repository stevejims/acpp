#include "epoll.hpp"

#include <stdexcept>
#include <cassert>

namespace net {

epoll::epoll()
{
    if (SYS_OK != sys_epoll_init(&epoll_)) {
        throw std::runtime_error("net::epoll");
    }
}

epoll::~epoll() noexcept
{
    sys_epoll_fini(&epoll_);
}

void epoll::add(int fd)
{
    if (SYS_OK != sys_epoll_add(&epoll_, fd)) {
        throw std::runtime_error("net::epoll::add");
    }
}

void epoll::del(int fd)
{
    if (SYS_OK != sys_epoll_delete(&epoll_, fd)) {
        throw std::runtime_error("net::epoll::del");
    }
}

int epoll::wait()
{
    int fd;
    if (SYS_OK != sys_epoll_wait(&epoll_, &fd)) {
        throw std::runtime_error("net::epoll::wait");
    }
    return fd;
}

epoll_ptr::epoll_ptr()
{
    if (SYS_OK != sys_epoll_init(&epoll_)) {
        throw std::runtime_error("net::epoll_ptr");
    }
}

epoll_ptr::~epoll_ptr() noexcept
{
    sys_epoll_fini(&epoll_);
}

void epoll_ptr::add(int fd, void *data)
{
    if (SYS_OK != sys_epoll_add_ptr(&epoll_, fd, data)) {
        throw std::runtime_error("net::epoll_ptr::add");
    }
}

void epoll_ptr::del(int fd)
{
    if (SYS_OK != sys_epoll_delete(&epoll_, fd)) {
        throw std::runtime_error("net::epoll_ptr::del");
    }
}

void *epoll_ptr::wait()
{
    void *data;
    if (SYS_OK != sys_epoll_wait_ptr(&epoll_, &data)) {
        throw std::runtime_error("net::epoll_ptr::wait");
    }
    return data;
}

} /* namespace net */
