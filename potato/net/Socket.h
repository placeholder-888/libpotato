#ifndef POTATO_POTATO_NET_SOCKET_H_
#define POTATO_POTATO_NET_SOCKET_H_

#include "potato/net/IpAddress.h"
#include "potato/utils/NonCopyable.h"
#include <atomic>
#include <memory>

namespace potato {

class IpAddress;

class Socket : NonCopyable, std::enable_shared_from_this<Socket> {
public:
  using SocketPtr = std::shared_ptr<Socket>;
  enum SockType { kTcpSocket, kTcpSocket6, kUdpSocket, kUdpSocket6 };

  Socket();
  explicit Socket(SockType type, bool abortOnErr = false);
  explicit Socket(socket_t socket, const IpAddress &addr);
  ~Socket();

  static SocketPtr createTcpSocket(bool abortOnErr = false);
  static SocketPtr createTcpSocket6(bool abortOnErr = false);
  static SocketPtr createUdpSocket(bool abortOnErr = false);
  static SocketPtr createUdpSocket6(bool abortOnErr = false);

  static SocketPtr adopt(socket_t socket, const IpAddress &localAddr);

  bool valid() const { return socket_ != INVALID_SOCKET; }
  bool isConnected() const { return connected_; }

  void setNonBlock() const;
  ssize_t write(const void *buf, size_t len) const;
  ssize_t read(void *buf, size_t len) const;

  const IpAddress &ipAddress() const { return address_; }

  int getSocketError() const;
  std::string getSocketErrorStr() const;

  socket_t getPlatformSocket() const { return socket_; }
  bool connect(const IpAddress &address);
  void bind(const IpAddress &address);
  void close();
  void listen() const;
  SocketPtr accept() const;

  void setReuseAddr(bool on) const;
  void setReusePort(bool on) const;
  void setTcpNodeLay(bool on) const;
  void setKeepAlive(bool on) const;

protected:
  socket_t socket_;
  std::atomic_bool connected_;
  IpAddress address_;
};

} // namespace potato

#endif // POTATO_POTATO_NET_SOCKET_H_
