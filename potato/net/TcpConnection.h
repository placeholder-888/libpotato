#ifndef POTATO_POTATO_NET_TCPCONNECTION_H_
#define POTATO_POTATO_NET_TCPCONNECTION_H_

#include "potato/net/Buffer.h"
#include "potato/net/Socket.h"
#include "potato/utils/NonCopyable.h"
#include <atomic>
#include <functional>
#include <memory>

namespace potato {
class EventLoop;
class IOEvent;

class TcpConnection : public NonCopyable {
  friend class TcpServer;

public:
  using SocketPtr = std::shared_ptr<Socket>;
  using Callback = std::function<void(TcpConnection *)>;

  TcpConnection(int index, const IpAddress &local);
  ~TcpConnection();

  void setMessageCallback(Callback cb) { messageCallback_ = std::move(cb); }

  void setWriteCompleteCallback(Callback cb) {
    writeCompleteCallback_ = std::move(cb);
  }

  void setConnectionCallback(Callback cb) {
    connectionCallback_ = std::move(cb);
  }

  bool isConnected() const { return socket_ && socket_->isConnected(); }

  EventLoop *getLoop() const { return loop_; }

  void send(StringSlice msg);
  void send(const char *data, size_t len);
  void write(StringSlice msg);
  void write(const char *data, size_t len);

  Buffer &readBuffer() { return readBuffer_; }

  const IpAddress &localAddress() const { return localAddress_; }

  const IpAddress &peerAddress() const {
    assert(socket_);
    return socket_->ipAddress();
  }

  void close();

private:
  void handleReadEvent();
  void handleWriteEvent();
  void handleCloseEvent();
  void bindSocket(SocketPtr ptr, EventLoop *loop);
  void reset();

  void setDestroyCallback(Callback cb) { destroyCallback_ = std::move(cb); }

  int index_;
  IpAddress localAddress_;
  EventLoop *loop_;
  std::unique_ptr<IOEvent> ioEvent_;
  SocketPtr socket_;
  Buffer readBuffer_;
  Buffer writeBuffer_;
  Callback messageCallback_;
  Callback writeCompleteCallback_;
  Callback connectionCallback_;
  // only for tcpServer
  Callback destroyCallback_;
};

} // namespace potato

#endif // POTATO_POTATO_NET_TCPCONNECTION_H_
