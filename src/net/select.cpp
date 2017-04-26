#include "select.hpp"

#include <stdexcept>
#include <cassert>

namespace net {

fdset::fdset()
{
    sys_fdset_init(&fdset_);
}

void fdset::add(const int fd)
{
    if (SYS_OK != sys_fdset_add(&fdset_, fd)) {
        throw std::runtime_error("net::fdset::add");
    }
}

void fdset::del(const int fd)
{
    if (SYS_OK != sys_fdset_del(&fdset_, fd)) {
        throw std::runtime_error("net::fdset::del");
    }
}

bool fdset::readable(const int fd) const
{
    return sys_fdset_readable(&fdset_, fd);
}

void select_inplace(fdset &fds)
{
    if (SYS_OK != sys_select(&fds.fdset_)) {
        throw std::runtime_error("net::select");
    }
}

fdset select(const fdset &fds)
{
    auto result = fds;
    select_inplace(result);
    return result;
}

} /* namespace net */
