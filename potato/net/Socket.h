#ifndef POTATO_POTATO_NET_SOCKET_H_
#define POTATO_POTATO_NET_SOCKET_H_

#include "potato/net/IpAddress.h"
#include "potato/utils/NonCopyable.h"

namespace potato {

class IpAddress;

class Socket : NonCopyable {
public:
  explicit Socket(socket_t socket) : socket_(socket), isOpen_(true) {}
  Socket(Socket &&socket) noexcept;
  Socket &operator=(Socket &&socket) noexcept;
  virtual ~Socket();
  bool isOpen() const { return isOpen_; }
  ssize_t write(const void *buf, size_t len) const;
  ssize_t read(void *buf, size_t len) const;
  const IpAddress &ipAddress() const { return address_; }
  int getSocketError() const;
  std::string getSocketErrorStr() const;
  socket_t getPlatformSocket() const { return socket_; }
  bool connect(const IpAddress &address);
  void setReuseAddr(bool on) const;
  void setReusePort(bool on) const;
  void bind(const IpAddress &address);
  void setIpAddress(const IpAddress &address) { address_ = address; }
  virtual void close();

protected:
  socket_t socket_;
  bool isOpen_;
  IpAddress address_;
};

class ListenSocket : public Socket {
public:
  explicit ListenSocket(bool ipv6 = false);
  ListenSocket(ListenSocket &&socket) noexcept;
  ListenSocket &operator=(ListenSocket &&socket) noexcept;
  ~ListenSocket() override = default;
  void listen();
  std::pair<socket_t, IpAddress> accept();
  bool listening() const { return listening_; }

private:
  bool listening_;
};

} // namespace potato

#endif // POTATO_POTATO_NET_SOCKET_H_
