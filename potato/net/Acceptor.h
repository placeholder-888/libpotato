#ifndef POTATO_POTATO_NET_ACCEPTOR_H_
#define POTATO_POTATO_NET_ACCEPTOR_H_

#include "potato/net/Socket.h"
#include <functional>
#include <memory>

namespace potato {

class EventLoop;
class IOEvent;
class TcpSocket;

class Acceptor : NonCopyable {
public:
  using NewConnectionCallback = std::function<void(Socket::SocketPtr)>;

  Acceptor(EventLoop *loop, const IpAddress &address);
  ~Acceptor();

  void listen();

  bool listening() const { return listening_; }

  const IpAddress &hostAddress() const { return hostAddress_; }

  void setNewConnectionCallback(NewConnectionCallback cb) {
    newConnectionCallback_ = std::move(cb);
  }

private:
  void handleAccept();

  bool listening_{false};
  Socket listenSocket_;
  IpAddress hostAddress_;
  EventLoop *loop_;
  std::unique_ptr<IOEvent> ioEvent_;
  int idleFd_{-1}; // only for linux
  NewConnectionCallback newConnectionCallback_;
};

} // namespace potato

#endif // POTATO_POTATO_NET_ACCEPTOR_H_
