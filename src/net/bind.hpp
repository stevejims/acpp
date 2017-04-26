#ifndef NET_BIND_HPP
#define NET_BIND_HPP

#include <string>

namespace net {

class socket;

socket bind_tcp(const std::string &port);
socket bind_local(const std::string &path);

} /* namespace net */

#endif /* NET_BIND_HPP */
