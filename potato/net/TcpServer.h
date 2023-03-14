#ifndef POTATO_POTATO_NET_TCPSERVER_H_
#define POTATO_POTATO_NET_TCPSERVER_H_

#include "potato/net/Acceptor.h"
#include "potato/net/EventLoop.h"
#include "potato/net/IpAddress.h"
#include "potato/thread/Mutex.h"
#include "potato/utils/NonCopyable.h"
#include <unordered_set>
#include <vector>

namespace potato {

class TcpConnection;

class TcpServer : NonCopyable {
public:
  using Callback = std::function<void(TcpConnection *)>;
  TcpServer(const IpAddress &addr, bool etMode = false);
  ~TcpServer() = default;
  void start();

  void setConnectionCallback(Callback cb) {
    connectionCallback_ = std::move(cb);
  }
  void setMessageCallback(Callback cb) { messageCallback_ = std::move(cb); }
  void setWriteCompleteCallback(Callback cb) {
    writeCompleteCallback_ = std::move(cb);
  }

private:
  using Vector = std::vector<std::unique_ptr<TcpConnection>>;
  void newConnection(Socket::SocketPtr socket);
  void destroyConnection(TcpConnection *conn);
  TcpConnection *borrowConnection();
  void returnConnection(TcpConnection *);
  EventLoop loop_;
  Acceptor acceptor_;
  SpinLock lock_;
  Vector connectionPool_;
  std::unordered_set<int> availIndex_ GUARDED_BY(lock_);
  Callback connectionCallback_;
  Callback messageCallback_;
  Callback writeCompleteCallback_;
};

} // namespace potato

#endif // POTATO_POTATO_NET_TCPSERVER_H_
