#include "potato/net/SocketCommon.h"
#include "potato/log/Logger.h"
#include "potato/utils/Endian.h"
#include <memory>
#include <thread>
#include <vector>

int main() {
  std::vector<std::unique_ptr<std::thread>> threads_;
  SOCKET socket = potato::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (socket == INVALID_SOCKET) {
    LOG_FATAL("cannot create socket");
    abort();
  }
  struct sockaddr_in addr;
  addr.sin_family = AF_INET;
  addr.sin_port = potato::endian::hostToNetwork16(8888);
  addr.sin_addr.s_addr = INADDR_ANY;
  if (potato::bind(socket, reinterpret_cast<const sockaddr *>(&addr),
                   sizeof(addr)) < 0) {
    LOG_FATAL("bind()");
    abort();
  }
  if (potato::listen(socket) < 0) {
    LOG_FATAL("cannot create socket");
    abort();
  }
  while (true) {
    struct sockaddr_in6 peerAddr;
    socklen_t len = sizeof(peerAddr);
    auto conn =
        potato::accept(socket, reinterpret_cast<sockaddr *>(&peerAddr), &len);
    if (peerAddr.sin6_family == AF_INET) {
      auto a = reinterpret_cast<struct sockaddr_in *>(&peerAddr);
      char buf[256];
      inet_ntop(AF_INET, &a->sin_addr, buf, sizeof buf);
      LOG_INFO("new Connection %s:%d", buf,
               potato::endian::networkToHost16(a->sin_port));
    }
    auto thread = new std::thread([conn]() {
      while (true) {
        char buf[1024];
        auto size = potato::read(conn, buf, sizeof buf);
        if (size < 0) {
          LOG_ERROR("read error");
        } else if (size == 0) {
          LOG_INFO("the peer closed the connection");
          break;
        } else {
          buf[size] = 0;
          LOG_INFO("received:%s", buf);
          potato::write(conn, buf, static_cast<size_t>(size));
        }
      }
    });
    threads_.emplace_back(thread);
  }
}