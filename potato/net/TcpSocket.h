#ifndef POTATO_POTATO_NET_TCPSOCKET_H_
#define POTATO_POTATO_NET_TCPSOCKET_H_

#include "potato/net/Socket.h"
#include <atomic>
#include <functional>
#include <memory>

namespace potato {

class EventLoop;
class IOEvent;
class TcpSocket;

using TcpSocketPtr = std::shared_ptr<TcpSocket>;

class TcpSocket : public Socket,
                  public std::enable_shared_from_this<TcpSocket> {
public:
  using Callback = std::function<void(TcpSocketPtr)>;
  TcpSocket(socket_t sock, const IpAddress &peer);
  ~TcpSocket() override;

  void operator=(TcpSocket &&) = delete;
  TcpSocket(TcpSocket &&) = delete;

  static TcpSocketPtr newTcpConnection(socket_t sock, const IpAddress &peer) {
    return std::make_shared<TcpSocket>(sock, peer);
  }

  void attachToLoop(EventLoop *loop);

  void expectWriting();
  void ignoreWriting();

  void setTcpNoDelay(bool on);
  void setKeepAlive(bool on);

  void setOnReadyReadCallback(Callback cb) {
    readyReadCallback_ = std::move(cb);
  }
  void setOnReadyWriteCallback(Callback cb) {
    readyWriteCallback_ = std::move(cb);
  }
  void setOnErrorOccurredCallback(Callback cb) {
    errorOccurredCallback_ = std::move(cb);
  }
  void setOnConnectedCallback(Callback cb) {
    connectedCallback_ = std::move(cb);
  }
  void setOnDisconnectedCallback(Callback cb) {
    disconnectedCallback_ = std::move(cb);
  }

  bool connected() const { return state_ == kConnected; }

  const IpAddress &peerAddr() const { return peer_; }

  void close() override;

private:
  void handleRead();
  void handleWrite();
  void handleError();
  void handleClose();

  void safeClose();

  enum ConnectionState { kConnected, kDisconnected };
  IpAddress peer_;
  std::atomic<ConnectionState> state_;
  EventLoop *ownerLoop_{nullptr};
  std::unique_ptr<IOEvent> event_;
  Callback readyReadCallback_;
  Callback readyWriteCallback_;
  Callback errorOccurredCallback_;
  Callback connectedCallback_;
  Callback disconnectedCallback_;
};

} // namespace potato

#endif // POTATO_POTATO_NET_TCPSOCKET_H_
