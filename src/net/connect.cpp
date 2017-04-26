#include "connect.hpp"
#include "socket.hpp"
#include "sys/client.h"

#include <stdexcept>
#include <cassert>

namespace net {

socket connect(const std::string &hostname, const std::string &port)
{
    int connect_sock(-1);
    if (SYS_OK != sys_connect(hostname.c_str(), port.c_str(), &connect_sock)) {
        throw std::runtime_error("net::connect");
    }
    return socket(connect_sock);
}

socket connect_local(const std::string &path)
{
    int connect_sock(-1);
    if (SYS_OK != sys_connect_local(path.c_str(), &connect_sock)) {
        throw std::runtime_error("net::connect_local");
    }
    return socket(connect_sock);
}

} /* namespace net */
