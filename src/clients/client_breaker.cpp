#include "net/connect.hpp"
#include "net/socket.hpp"

using namespace net;

int main()
{
    auto socket = connect_local("local_socket");
    socket.send("unknown");
    socket.recv();
        
    return 0;
}
