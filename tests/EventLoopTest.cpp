#include "potato/net/EventLoop.h"
#include "potato/log/Logger.h"
#include "potato/net/Acceptor.h"
#include "potato/net/TcpSocket.h"
#include <unordered_set>

potato::EventLoop loop(true);

std::unordered_set<potato::TcpSocketPtr> connections;

void disconnection(potato::TcpSocketPtr conn) {
  LOG_INFO("close connection:%s", conn->peerAddr().IpPort().c_str());
  connections.erase(conn);
}

void newConnection(potato::TcpSocketPtr sock) {
  LOG_INFO("new connection:%s", sock->peerAddr().IpPort().c_str());
  sock->attachToLoop(&loop);
  sock->setOnDisconnectedCallback(disconnection);
  sock->setOnReadyReadCallback([](potato::TcpSocketPtr conn) {
    do {
      char buf[1024];
      auto n = conn->read(buf, sizeof(buf));
      if (n > 0) {
        buf[n] = 0;
        LOG_INFO("received:%s from : %s", buf,
                 conn->peerAddr().IpPort().c_str());
        conn->write(buf, static_cast<size_t>(n));
      } else if (n == 0) {
        conn->close();
        break;
      } else {
        if (perrno != PAGAIN) {
          conn->close();
        }
        break;
      }
    } while (loop.etMode());
  });
  connections.insert(sock);
}

int main() {
  potato::IpAddress addr(8888, true);
  potato::Acceptor acceptor(&loop, addr);
  acceptor.setNewConnectionCallback(newConnection);
  acceptor.listen();
  loop.startLoop();
}