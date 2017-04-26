#ifndef NET_CONNECT_HPP
#define NET_CONNECT_HPP

#include <string>

namespace net {

class socket;

socket connect_tcp(const std::string &hostname, const std::string &port);
socket connect_local(const std::string &path);

} /* namespace net */

#endif /* NET_CONNECT_HPP */
