#include "potato/log/Logger.h"
#include "potato/net/Socket.h"
#include "potato/net/SocketCommon.h"
#include "potato/utils/Endian.h"
#include <cassert>
#include <map>
#include <set>
#include <thread>
#include <vector>
/*
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
*/

int main() {
  std::vector<std::unique_ptr<std::thread>> threads;
  potato::ListenSocket listenSocket;
  potato::IpAddress hostAddress(8888);
  listenSocket.bind(hostAddress);
  listenSocket.listen();
  std::map<SOCKET, potato::Socket> sockets;
  std::set<SOCKET> activeSockets;
  fd_set allSet;
  FD_ZERO(&allSet);
  FD_SET(listenSocket.getPlatformSocket(), &allSet);
  while (true) {
    fd_set readSet = allSet;
    int nready = select(1024, &readSet, nullptr, nullptr, nullptr);
    if (nready < 0) {
      LOG_ERROR("select() %s", potato::strError(perrno).c_str());
      break;
    } else if (nready == 0) {
      continue;
    }
    if (FD_ISSET(listenSocket.getPlatformSocket(), &readSet)) {
      auto p = listenSocket.accept();
      if (p.first == INVALID_SOCKET) {
        LOG_ERROR("accept() %s", potato::strError(perrno).c_str());
        break;
      } else {
        LOG_INFO("new Connection %s", p.second.IpPort().c_str());
        sockets.emplace(p.first, potato::Socket(p.first));
        auto it = sockets.find(p.first);
        assert(it != sockets.end());
        it->second.setIpAddress(p.second);
        FD_SET(p.first, &allSet);
        activeSockets.insert(p.first);
      }
    } else {
      for (auto it = activeSockets.begin(); it != activeSockets.end(); ++it) {
        if (FD_ISSET(*it, &readSet)) {
          char buf[4096];
          auto sit = sockets.find(*it);
          assert(sit != sockets.end());
          auto size = sit->second.read(buf, sizeof buf);
          if (size <= 0) {
            if (size < 0)
              LOG_ERROR("read error %s", potato::strError(perrno).c_str());
            else
              LOG_INFO("the peer closed the connection addr:%s",
                       sit->second.ipAddress().IpPort().c_str());
            FD_CLR(*it, &allSet);
            sockets.erase(sit);
            activeSockets.erase(it);
            LOG_INFO("cur active connection Count:%d",
                     static_cast<int>(activeSockets.size()));
            break;
          } else {
            buf[size] = 0;
            LOG_INFO("received from %s content: %s",
                     sit->second.ipAddress().IpPort().c_str(), buf);
            sit->second.write(buf, static_cast<size_t>(size));
          }
        }
      }
    }
  }
  /*
  while (true) {
    auto p = listenSocket.accept();
    if (p.first == INVALID_SOCKET) {
      LOG_ERROR("accept() %s", potato::strError(perrno).c_str());
      break;
    } else {
      LOG_INFO("new Connection %s", p.second.IpPort().c_str());
      threads.emplace_back(new std::thread([p]() {
        potato::Socket socket(p.first);
        socket.setIpAddress(p.second);
        while (true) {
          char buf[4096];
          auto size = socket.read(buf, sizeof buf);
          if (size < 0) {
            LOG_ERROR("read error %s", potato::strError(perrno).c_str());
            break;
          } else if (size == 0) {
            LOG_INFO("the peer closed the connection addr:%s",
                     socket.ipAddress().IpPort().c_str());
            break;
          } else {
            buf[size] = 0;
            LOG_INFO("received from %s content: %s",
                     socket.ipAddress().IpPort().c_str(), buf);
            socket.write(buf, static_cast<size_t>(size));
          }
        }
      }));
    }
  }
  for (auto &thread : threads) {
    thread->join();
  }
   */
}