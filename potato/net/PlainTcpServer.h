#ifndef POTATO_POTATO_NET_PLAINTCPSERVER_H_
#define POTATO_POTATO_NET_PLAINTCPSERVER_H_

#include "potato/net/Acceptor.h"
#include "potato/net/EventLoop.h"
#include "potato/utils/NonCopyable.h"
#include <functional>
#include <unordered_set>

namespace potato {

class EventLoop;

class PlainTcpServer : NonCopyable {
public:
  using TcpSocketCallback = std::function<void(TcpSocketPtr)>;

  explicit PlainTcpServer(const IpAddress &hostAddress, bool etMode = false);
  ~PlainTcpServer();

  void setConnectedCallback(TcpSocketCallback cb) {
    connectedCallback_ = std::move(cb);
  }

  void setDisconnectedCallback(TcpSocketCallback cb) {
    disconnectedCallback_ = std::move(cb);
  }

  void start();
  void stop();

  const IpAddress &hostAddress() const { return acceptor_.hostAddress(); }

private:
  void handleNewConnection(TcpSocketPtr conn);
  void handleClosed(TcpSocketPtr conn);
  EventLoop loop_;
  Acceptor acceptor_;
  std::unordered_set<TcpSocketPtr> connections_;
  TcpSocketCallback connectedCallback_;
  TcpSocketCallback disconnectedCallback_;
};

} // namespace potato

#endif // POTATO_POTATO_NET_PLAINTCPSERVER_H_
