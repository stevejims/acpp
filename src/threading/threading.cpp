#include "common/table.hpp"
#include "net/bind.hpp"
#include "net/socket.hpp"
#include <unistd.h>
#include <thread>

using namespace net;
using std::move;
using std::thread;

template <typename F>
void spawn(F f)
{
    auto t = thread(move(f));
    t.detach();
}

int main()
{
  const auto t = table();
  auto sock = bind_local("socket");
  sock.listen();

  spawn([&t, sock=move(sock)]
        () mutable {
    for (;;) {
      auto conn = sock.accept();
      
      spawn([&t, conn=move(conn)]
            () mutable {
        while (auto req = conn.recv()) {
          auto result = t.lookup(*req);
          conn.send(result);
        }
      });
    }
  });
  
  pause();
}
