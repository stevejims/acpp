#include "common/table.hpp"
#include "net/bind.hpp"
#include "net/socket.hpp"
using namespace net;

int main()
{
  const auto t = table();
  auto sock = bind_local("socket");
  sock.listen();

  for (;;) {
    auto conn = sock.accept();

    while (auto req = conn.recv()) {
      auto result = t.lookup(*req);
      conn.send(result);
    }
  }
}
