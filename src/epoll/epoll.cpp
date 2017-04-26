#include "common/table.hpp"
#include "net/bind.hpp"
#include "net/epoll.hpp"
#include "net/socket.hpp"

#include <unordered_map>

using namespace net;
using std::move;
using std::unordered_map;

int main()
{
    const auto t = table();
    auto sock = bind_local("socket");
    sock.listen();

    auto conns = unordered_map<int, socket>();
    epoll poll;
    poll.add(sock.fd());

    for (;;) {
        const auto fd = poll.wait();

        if (fd == sock.fd()) {
            auto conn = sock.accept();
            poll.add(conn.fd());
            conns[conn.fd()] = move(conn);
        }
        else {
            auto &conn = conns.at(fd);
            if (auto req = conn.recv()) {
                auto result = t.lookup(*req);
                conn.send(result);
            }
            else {
                poll.del(fd);
                conns.erase(fd);
            }
        }
    }
}
