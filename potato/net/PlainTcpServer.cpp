#include "potato/net/PlainTcpServer.h"
#include "potato/log/Logger.h"
#include "potato/net/TcpSocket.h"

using potato::PlainTcpServer;

PlainTcpServer::PlainTcpServer(const IpAddress &hostAddress, bool etMode)
    : loop_(etMode), acceptor_(&loop_, hostAddress) {
  acceptor_.setNewConnectionCallback(
      [this](TcpSocketPtr conn) { handleNewConnection(std::move(conn)); });
}

PlainTcpServer::~PlainTcpServer() = default;

void PlainTcpServer::start() {
  acceptor_.listen();
  loop_.startLoop();
}

void PlainTcpServer::stop() { loop_.stopLoop(); }

void PlainTcpServer::handleNewConnection(TcpSocketPtr conn) {
  connections_.insert(conn);
  conn->setOnDisconnectedCallback(
      [this](TcpSocketPtr sock) { handleClosed(std::move(sock)); });
  conn->setOnConnectedCallback(connectedCallback_);
  conn->setIpAddress(hostAddress());
  conn->attachToLoop(&loop_);
}

// 在close过后才会调用 因此handleClosed以及callback不应该再出现close
void PlainTcpServer::handleClosed(TcpSocketPtr conn) {
  connections_.erase(conn);
  if (disconnectedCallback_) {
    disconnectedCallback_(conn);
  }
}