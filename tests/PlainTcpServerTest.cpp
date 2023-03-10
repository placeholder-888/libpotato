#include "potato/net/PlainTcpServer.h"
#include "potato/log/Logger.h"
#include "potato/net/TcpSocket.h"
#include <thread>

class EchoServer {
public:
  explicit EchoServer(const potato::IpAddress &hostAddress, bool etMode = false)
      : etMode_(etMode), server_(hostAddress, etMode) {
    server_.setConnectedCallback(
        [this](potato::TcpSocketPtr conn) { newConnection(std::move(conn)); });
    server_.setDisconnectedCallback(
        [this](potato::TcpSocketPtr conn) { disconnected(std::move(conn)); });
  }

  void start() { server_.start(); }
  void stop() { server_.stop(); }

private:
  void newConnection(potato::TcpSocketPtr conn) {
    LOG_INFO("new connection:%s", conn->peerAddr().IpPort().c_str());
    conn->setOnReadyReadCallback(
        [this](potato::TcpSocketPtr sock) { handleRead(std::move(sock)); });
  }

  void disconnected(potato::TcpSocketPtr conn) {
    LOG_INFO("close connection:%s", conn->peerAddr().IpPort().c_str());
  }

  void handleRead(potato::TcpSocketPtr conn) {
    LOG_INFO("do read");
    do {
      char buf[4096];
      auto n = conn->read(buf, sizeof(buf));
      if (n > 0) {
        conn->write(buf, static_cast<size_t>(n));
      } else if (n == 0) {
        conn->close();
        break;
      } else {
        if (perrno != PAGAIN) {
          LOG_ERROR("error occurred :%s %s", conn->peerAddr().IpPort().c_str(),
                    potato::strError(perrno).c_str());
          conn->close();
        }
        break;
      }
    } while (etMode_);
  }
  bool etMode_;
  potato::PlainTcpServer server_;
};

int main() {
  potato::IpAddress addr(8888, true);
  EchoServer server(addr, false);
  server.start();
}