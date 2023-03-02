#include "SocketCommon.h"
#include "../log/Logger.h"
#include <valarray>

namespace potato {
#ifdef PLATFORM_WINDOWS
class WSAInitializer {
public:
  WSAInitializer() {
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
    }
  }
  ~WSAInitializer() { WSACleanup(); }
};
__attribute__((unused)) static WSAInitializer _;
#else
class IgnoreSigPipe {
public:
  IgnoreSigPipe() {}
};
#endif

SOCKET socket(int family, int type, int protocol) {
  return ::socket(family, type, protocol);
}

int close(SOCKET socket) {
#ifdef PLATFORM_WINDOWS
  return ::closesocket(socket);
#else
  return ::close(socket);
#endif
}

int setNonBlock(SOCKET socket) {
#ifdef PLATFORM_WINDOWS
  u_long nonblock = 1;
  return ::ioctlsocket(socket, static_cast<long>(FIONBIO), &nonblock);
#else
  int flags = ::fcntl(socket, F_GETFL, 0);
  return ::fcntl(socket, F_SETFL, flags | O_NONBLOCK);
#endif
}

int setSockOpt(SOCKET socket, int level, int name, const void *option,
               socklen_t len) {
#ifdef PLATFORM_WINDOWS
  return ::setsockopt(socket, level, name, static_cast<const char *>(option),
                      len);
#else
  return ::setsockopt(socket, level, name, option, len);
#endif
}

int setReuseAddr(SOCKET socket, bool on) {
  int option = on ? 1 : 0;
  return setSockOpt(socket, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option));
}

int setReusePort(SOCKET socket, bool on) {
#ifdef PLATFORM_LINUX
  int option = on ? 1 : 0;
  return setSockOpt(socket, SOL_SOCKET, SO_REUSEPORT, &option, sizeof(option));
#else
  (void)socket,(void)on;
  return 0;
#endif
}

int setKeepAlive(SOCKET socket, bool on) {
  int option = on ? 1 : 0;
  return setSockOpt(socket, SOL_SOCKET, SO_KEEPALIVE, &option, sizeof(option));
}

int setTcpNoDelay(SOCKET socket, bool on) {
  int option = on ? 1 : 0;
  return setSockOpt(socket, IPPROTO_TCP, TCP_NODELAY, &option, sizeof(option));
}

int bind(SOCKET socket, const struct sockaddr *addr, socklen_t len) {
  return ::bind(socket, addr, len);
}

int listen(SOCKET socket) { return ::listen(socket, SOMAXCONN); }

SOCKET accept(SOCKET socket, struct sockaddr *addr, socklen_t *len) {
  return ::accept(socket, addr, len);
}

int getSocketError(SOCKET socket) {
  int sockErrno = 0;
  socklen_t len = sizeof(sockErrno);
#ifdef PLATFORM_WINDOWS
  if (::getsockopt(socket, SOL_SOCKET, SO_ERROR,
                   reinterpret_cast<char *>(&sockErrno), &len) < 0) {
#else
  if (getSockOpt(socket, SOL_SOCKET, SO_ERROR, &sockErrno, &len) < 0) {
#endif
    LOG_ERROR("error at getSocketError");
  }
  return sockErrno;
}

std::string getSocketErrorStr(SOCKET socket) {
  int err = getSocketError(socket);
  char buf[256];
#ifdef PLATFORM_WINDOWS
  if (::strerror_s(buf, sizeof buf, err) < 0) {
    return "";
  }
  return buf;
#else
  return ::strerror_r(buf, sizeof buf, err);
#endif
}

ssize_t read(SOCKET socket, void *buf, size_t size) {
#ifdef PLATFORM_WINDOWS
  return ::recv(socket, static_cast<char *>(buf), static_cast<int>(size), 0);
#else
  return ::read(socket, buf, size);
#endif
}

ssize_t write(SOCKET socket, const void *buf, size_t size) {
#ifdef PLATFORM_WINDOWS
  return ::send(socket, static_cast<const char *>(buf), static_cast<int>(size),
                0);
#else
  return ::write(socket, buf, size);
#endif
}

} // namespace potato