#ifndef NET_EPOLL_HPP
#define NET_EPOLL_HPP

#include "sys/epoll.h"

namespace net {

class epoll {
public:
    epoll();
    ~epoll() noexcept;

    void add(int fd);
    void del(int fd);

    /** Returns the file descriptor of the socket. */
    int wait();
    
    epoll(epoll &&) noexcept = delete;
    epoll &operator=(epoll &&) noexcept = delete;
    const epoll &operator=(const epoll &) noexcept = delete;
    epoll(epoll const &) noexcept = delete;
    
private:
    sys_epoll_t epoll_;
};

class epoll_ptr {
public:
    epoll_ptr();
    ~epoll_ptr() noexcept;

    void add(int fd, void *ptr);
    void del(int fd);

    /** Returns pointer provided with the socket. */
    void *wait();
    
    epoll_ptr(epoll_ptr &&) noexcept = delete;
    epoll_ptr &operator=(epoll_ptr &&) noexcept = delete;
    const epoll_ptr &operator=(const epoll_ptr &) noexcept = delete;
    epoll_ptr(epoll_ptr const &) noexcept = delete;
    
private:
    sys_epoll_t epoll_;
};

} /* namespace net */

#endif /* NET_EPOLL_HPP */
