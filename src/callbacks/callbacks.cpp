#include "common/table.hpp"
#include "net_callbacks.hpp"
#include <memory>

using namespace net_callbacks;
using std::make_shared;
using std::shared_ptr;
using std::experimental::optional;
using std::string;
using std::move;

struct on_recv {
  const table &t;
  shared_ptr<socket> conn;
  void operator()(optional<string> req) const {
    if (req) {
      auto result = t.lookup(*req);
      conn->send(result);
      conn->recv(on_recv{t, conn});
    }
  }
};
struct on_accept {
  const table &t;
  socket &sock;
  void operator()(socket s) const {
    auto conn = make_shared<socket>(move(s));
    conn->recv(on_recv{t, conn});
    sock.accept(on_accept{t, sock});
  }
};
int main()
{
  const auto t = table();
  auto sock = bind_local("socket");
  sock.listen();
  sock.accept(on_accept{t, sock});
  loop();
}
