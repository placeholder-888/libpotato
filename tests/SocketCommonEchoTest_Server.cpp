#include "potato/log/Logger.h"
#include "potato/net/SocketCommon.h"
#include "potato/utils/Endian.h"
#include <cassert>
#include <thread>
#include <vector>

int main() {
  std::vector<std::unique_ptr<std::thread>> threads_;
  SOCKET socket = potato::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (socket == INVALID_SOCKET) {
    LOG_FATAL("cannot create socket");
    abort();
  }
  struct sockaddr_in addr {};
  addr.sin_family = AF_INET;
  addr.sin_port = potato::endian::hostToNetwork16(8888);
  addr.sin_addr.s_addr = INADDR_ANY;
  assert(potato::setReusePort(socket, true) == 0);
  assert(potato::setReuseAddr(socket, true) == 0);
  if (potato::bind(socket, reinterpret_cast<const sockaddr *>(&addr),
                   sizeof(addr)) < 0) {
    LOG_FATAL("bind() %s", potato::strError(perrno).c_str());
    abort();
  }
  if (potato::listen(socket) < 0) {
    LOG_FATAL("cannot create socket");
    abort();
  }
  for (int i = 0; i < 100; ++i) {
    struct sockaddr_in6 peerAddr {};
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
    if (potato::setTcpNoDelay(conn, true) != 0) {
      LOG_ERROR("setTcpNoDelay()");
    } else {
      LOG_INFO("setTcpNoDelay() success");
    }
    if (potato::setKeepAlive(conn, true) != 0) {
      LOG_ERROR("setKeepAlive()");
    } else {
      LOG_INFO("setKeepAlive() success");
    }
    auto thread = new std::thread([conn]() {
      while (true) {
        char buf[1024];
        auto size = potato::read(conn, buf, sizeof buf);
        if (size < 0) {
          LOG_ERROR("read error %s", potato::strError(perrno).c_str());
          break;
        } else if (size == 0) {
          LOG_INFO("the peer closed the connection");
          break;
        } else {
          buf[size] = 0;
          LOG_INFO("received:%s", buf);
          potato::write(conn, buf, static_cast<size_t>(size));
        }
      }
      potato::close(conn);
    });
    threads_.emplace_back(thread);
  }
  for (auto &thread : threads_) {
    thread->join();
  }
}
