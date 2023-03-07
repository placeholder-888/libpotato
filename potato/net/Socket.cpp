#include "potato/net/Socket.h"
#include "potato/log/Logger.h"
#include "potato/net/IpAddress.h"

using potato::ListenSocket;
using potato::Socket;

Socket::~Socket() {
  LOG_DEBUG("Socket::~Socket()");
  close();
}

void Socket::close() {
  if (isOpen_) {
    potato::close(socket_);
    LOG_TRACE("Socket::close()");
    isOpen_ = false;
  }
}

Socket::Socket(Socket &&socket) noexcept
    : socket_(socket.socket_), isOpen_(socket.isOpen_) {
  socket.isOpen_ = false;
}

Socket &Socket::operator=(Socket &&socket) noexcept {
  if (this == &socket)
    return *this;
  close();
  socket_ = socket.socket_;
  isOpen_ = socket.isOpen_;
  socket.isOpen_ = false;
  return *this;
}

ssize_t Socket::write(const void *buf, size_t len) const {
  assert(isOpen_);
  return potato::write(socket_, buf, len);
}

ssize_t Socket::read(void *buf, size_t len) const {
  assert(isOpen_);
  return potato::read(socket_, buf, len);
}

void Socket::bind(const IpAddress &address) {
  assert(isOpen_);
  socklen_t addrLen =
      address.ipv6() ? sizeof(struct sockaddr_in6) : sizeof(struct sockaddr_in);
  if (potato::bind(socket_, address.getSockAddr(), addrLen) < 0) {
    LOG_FATAL("Socket::bind() addr:%s", address.IpPort().c_str());
    abort();
  }
  address_ = address;
}

void Socket::setReuseAddr(bool on) const {
  assert(isOpen_);
  if (potato::setReuseAddr(socket_, on) < 0) {
    LOG_ERROR("Socket::setReuseAddr()");
  }
}

void Socket::setReusePort(bool on) const {
  assert(isOpen_);
  if (potato::setReusePort(socket_, on) < 0) {
    LOG_ERROR("Socket::setReuseAddr()");
  }
}

int Socket::getSocketError() const { return potato::getSocketError(socket_); }

std::string Socket::getSocketErrorStr() const {
  return potato::getSocketErrorStr(socket_);
}

SOCKET createListenSocket() {
  SOCKET listenSock = potato::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (listenSock == INVALID_SOCKET) {
    LOG_FATAL("createListenSocket()");
    abort();
  }
  potato::setNonBlock(listenSock);
  return listenSock;
}

ListenSocket::ListenSocket() : Socket(createListenSocket()), listening_(false) {
  setReuseAddr(true);
  setReusePort(true);
}

ListenSocket::ListenSocket(ListenSocket &&socket) noexcept
    : Socket(std::move(socket)), listening_(socket.listening_) {
  socket.listening_ = false;
}

ListenSocket &ListenSocket::operator=(ListenSocket &&socket) noexcept {
  if (this == &socket)
    return *this;
  listening_ = socket.listening_;
  socket.listening_ = false;
  Socket::operator=(std::move(socket));
  return *this;
}

void ListenSocket::listen() {
  assert(isOpen_);
  if (potato::listen(socket_) < 0) {
    LOG_FATAL("ListenSocket::listen()");
    abort();
  }
  LOG_DEBUG("ListenSocket::listen() success");
  listening_ = true;
}

std::pair<SOCKET, potato::IpAddress> ListenSocket::accept() {
  assert(isOpen_);
  assert(listening_);
  struct sockaddr_in6 addr {};
  socklen_t addrLen = sizeof(addr);
  SOCKET sock = potato::accept(socket_, &addr, &addrLen);
  LOG_DEBUG("ListenSocket::accept() errorHappened:%s",
            sock == INVALID_SOCKET ? "true" : "false");
  IpAddress address(addr);
  return std::make_pair(sock, address);
}
