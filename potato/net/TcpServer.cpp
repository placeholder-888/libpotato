#include "potato/net/TcpServer.h"
#include "potato/log/Logger.h"
#include "potato/net/TcpConnection.h"

using potato::TcpServer;

TcpServer::TcpServer(const IpAddress &addr, bool etMode)
    : loop_(etMode), acceptor_(&loop_, addr) {
  acceptor_.setNewConnectionCallback(
      [this](Socket::SocketPtr socket) { newConnection(std::move(socket)); });
  for (int i = 0; i < 200; ++i) {
    connectionPool_.emplace_back(new TcpConnection(i, acceptor_.hostAddress()));
    availIndex_.insert(i);
  }
}

void TcpServer::start() {
  acceptor_.listen();
  loop_.startLoop();
}

void TcpServer::newConnection(Socket::SocketPtr socket) {
  auto conn = borrowConnection();
  conn->setConnectionCallback(connectionCallback_);
  conn->setDestroyCallback([this](TcpConnection *c) { destroyConnection(c); });
  conn->setMessageCallback(messageCallback_);
  conn->setWriteCompleteCallback(writeCompleteCallback_);
  conn->bindSocket(std::move(socket), &loop_);
}

void TcpServer::destroyConnection(TcpConnection *conn) {
  auto loop = conn->getLoop();
  assert(loop);
  loop->runInLoop([this, conn]() {
    conn->reset();
    returnConnection(conn);
  });
}

potato::TcpConnection *TcpServer::borrowConnection() {
  if (availIndex_.empty()) {
    SpinLockGuard guard(lock_);
    if (availIndex_.empty()) {
      int index = static_cast<int>(connectionPool_.size());
      for (int i = 0; i < 200; ++i, ++index) {
        connectionPool_.emplace_back(
            new TcpConnection(index, acceptor_.hostAddress()));
        availIndex_.insert(index);
      }
    }
  }
  int index = -1;
  {
    SpinLockGuard guard(lock_);
    auto it = availIndex_.begin();
    index = *it;
    availIndex_.erase(it);
  }
  assert(index >= 0);
  LOG_DEBUG("borrow connection: {}", index);
  return connectionPool_[static_cast<size_t>(index)].get();
}

void TcpServer::returnConnection(TcpConnection *conn) {
  int index = conn->index_;
  SpinLockGuard guard(lock_);
  availIndex_.insert(index);
  LOG_DEBUG("return connection: {}", index);
}
