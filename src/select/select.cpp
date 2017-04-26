#include "common/table.hpp"
#include "net/bind.hpp"
#include "net/select.hpp"
#include "net/socket.hpp"

#include <vector>

using namespace net;

int main()
{
  const auto t = table();
  auto sock = bind_local("socket");
  sock.listen();

  auto conns = std::vector<socket>();
  auto all_fds = fdset();
  all_fds.add(sock.fd());
  
  for (;;) {
    auto ready_fds = select(all_fds);

    if (ready_fds.readable(sock.fd())) {
      conns.emplace_back(sock.accept());
      all_fds.add(conns.back().fd());
    }
    for (auto it=conns.begin();
         it!=conns.end();) {
      if (ready_fds.readable(it->fd())) {
        if (auto req = it->recv()) {
          auto result = t.lookup(*req);
          it->send(result);
        }
        else {
          all_fds.del(it->fd());
          it = conns.erase(it); continue;
        }
      }
      ++it;
    }
  }
}
