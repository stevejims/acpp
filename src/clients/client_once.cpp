#include "net/connect.hpp"
#include "net/socket.hpp"

#include <iostream>
#include <vector>

using namespace net;

int main()
{
    std::cout << "Connecting" << std::endl;
    auto socket = connect_local("socket");
    std::cout << "Sending" << std::endl;
    socket.send("one");
    std::cout << "Receiving" << std::endl;
    socket.recv();
        
    return 0;
}
