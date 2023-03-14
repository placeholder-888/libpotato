#include "potato/net/Socket.h"
#include "potato/log/Logger.h"
#include "potato/net/IpAddress.h"

using potato::Socket;

Socket::Socket(SockType type, bool abortOnErr) : connected_(false) {
  if (type == kTcpSocket) {
    socket_ = potato::socket(AF_INET, SOCK_STREAM, 0);
  } else if (type == kTcpSocket6) {
    socket_ = potato::socket(AF_INET6, SOCK_STREAM, 0);
  } else if (type == kUdpSocket) {
    socket_ = potato::socket(AF_INET, SOCK_DGRAM, 0);
  } else if (type == kUdpSocket6) {
    socket_ = potato::socket(AF_INET6, SOCK_DGRAM, 0);
  } else {
    LOG_FATAL("Socket::Socket() error: unknown type");
    abort();
  }
  if (socket_ == INVALID_SOCKET) {
    LOG_ERROR("Socket::Socket() error:%s", potato::strError(perrno).c_str());
    if (abortOnErr)
      abort();
  }
}

Socket::Socket() : socket_(INVALID_SOCKET), connected_(false) {}

Socket::Socket(socket_t socket, const IpAddress &addr)
    : socket_(socket), connected_(true), address_(addr) {
  assert(socket_ != INVALID_SOCKET);
}

Socket::~Socket() {
  LOG_INFO("Socket::~Socket()");
  close();
}

void Socket::setNonBlock() const {
  assert(valid());
  potato::setNonBlock(socket_);
}

void Socket::close() {
  connected_ = false;
  if (socket_ != INVALID_SOCKET) {
    potato::close(socket_);
    socket_ = INVALID_SOCKET;
  }
}

ssize_t Socket::write(const void *buf, size_t len) const {
  if (connected_) {
    return potato::write(socket_, buf, len);
  }
  return -1;
}

ssize_t Socket::read(void *buf, size_t len) const {
  if (connected_) {
    return potato::read(socket_, buf, len);
  }
  return -1;
}

void Socket::bind(const IpAddress &address) {
  assert(connected_ == false && valid());
  socklen_t addrLen =
      address.ipv6() ? sizeof(struct sockaddr_in6) : sizeof(struct sockaddr_in);
  if (potato::bind(socket_, address.getSockAddr(), addrLen) < 0) {
    LOG_FATAL("Socket::bind() addr:%s", address.IpPort().c_str());
    abort();
  }
  address_ = address;
}

void Socket::setReuseAddr(bool on) const {
  assert(connected_ == false && valid());
  if (potato::setReuseAddr(socket_, on) < 0) {
    LOG_ERROR("Socket::setReuseAddr()");
  }
}

void Socket::setReusePort(bool on) const {
  assert(connected_ == false && valid());
  if (potato::setReusePort(socket_, on) < 0) {
    LOG_ERROR("Socket::setReuseAddr()");
  }
}

int Socket::getSocketError() const { return potato::getSocketError(socket_); }

std::string Socket::getSocketErrorStr() const {
  return potato::getSocketErrorStr(socket_);
}

bool Socket::connect(const IpAddress &address) {
  assert(connected_ == false && valid());
  socklen_t addrLen =
      address.ipv6() ? sizeof(struct sockaddr_in6) : sizeof(struct sockaddr_in);
  if (potato::connect(socket_, address.getSockAddr(), addrLen) < 0) {
    return false;
  }
  struct sockaddr_in6 addr {};
  socklen_t len = sizeof(addr);
  ::getsockname(socket_, reinterpret_cast<sockaddr *>(&addr), &len);
  address_ = IpAddress(addr);
  connected_ = true;
  return true;
}

void Socket::listen() const {
  assert(connected_ == false && valid());
  if (potato::listen(socket_) < 0) {
    LOG_FATAL("Socket::listen()");
    abort();
  }
}

Socket::SocketPtr Socket::accept() const {
  assert(connected_ == false && valid());
  struct sockaddr_in6 addr {};
  socklen_t addrLen = sizeof(addr);
  socket_t sock = potato::accept(socket_, &addr, &addrLen);
  IpAddress address(addr);
  if (sock == INVALID_SOCKET) {
    return nullptr;
  }
  return std::make_shared<Socket>(sock, address);
}

void Socket::setTcpNodeLay(bool on) const {
  assert(valid());
  potato::setTcpNoDelay(socket_, on);
}

void Socket::setKeepAlive(bool on) const {
  assert(valid());
  potato::setKeepAlive(socket_, on);
}

Socket::SocketPtr Socket::createTcpSocket(bool abortOnErr) {
  return std::make_shared<Socket>(kTcpSocket, abortOnErr);
}

Socket::SocketPtr Socket::createTcpSocket6(bool abortOnErr) {
  return std::make_shared<Socket>(kTcpSocket6, abortOnErr);
}

Socket::SocketPtr Socket::createUdpSocket(bool abortOnErr) {
  return std::make_shared<Socket>(kUdpSocket, abortOnErr);
}

Socket::SocketPtr Socket::createUdpSocket6(bool abortOnErr) {
  return std::make_shared<Socket>(kUdpSocket6, abortOnErr);
}

Socket::SocketPtr Socket::adopt(socket_t socket, const IpAddress &localAddr) {
  return std::make_shared<Socket>(socket, localAddr);
}
