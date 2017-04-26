#include "bind.hpp"
#include "socket.hpp"

#include "sys/server.h"

#include <stdexcept>
#include <cassert>

namespace net {

socket bind_tcp(const std::string &bind_port)
{
    int bind_sock(-1);
    if (SYS_OK != sys_bind(bind_port.c_str(), &bind_sock)) {
        throw std::runtime_error("net::bind_tcp");
    }    
    return socket(bind_sock);
}

socket bind_local(const std::string &path)
{
    int bind_sock(-1);
    if (SYS_OK != sys_bind_local(path.c_str(), &bind_sock)) {
        throw std::runtime_error("net::bind_local");
    }
    return socket(bind_sock);
}

} /* namespace net */
