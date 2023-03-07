#include "potato/net/SocketCommon.h"
#include "potato/log/Logger.h"

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
  IgnoreSigPipe() {
    if (::signal(SIGPIPE, SIG_IGN) == SIG_ERR) {
      LOG_FATAL("cannot ignore SIGPIPE");
      abort();
    }
  }
};
static IgnoreSigPipe _;
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
  (void)socket, (void)on;
  return 0;
#endif
}

int setKeepAlive(SOCKET socket, bool on) {
  int option = on ? 1 : 0;
  return setSockOpt(socket, SOL_SOCKET, SO_KEEPALIVE, &option, sizeof(option));
}

std::string strError(int err) {
#ifdef PLATFORM_WINDOWS
  switch (WSAGetLastError()) {
  case 0:
    return "No error";
  case WSAEACCES:
    return "Permission denied";
  case WSAEWOULDBLOCK:
    return "Resource temporarily unavailable";
  case WSAENOPROTOOPT:
    return "Bad protocol option";
  case WSAESOCKTNOSUPPORT:
    return "Socket type not supported";

  case WSAEADDRINUSE:
    return "Address already in use";
  case WSAECONNABORTED:
    return "Software caused connection abort";
  case WSAECONNREFUSED:
    return "Connection refused";
  case WSAECONNRESET:
    return "Connection reset by peer";
  case WSAEDESTADDRREQ:
    return "Destination address required";
  case WSAEHOSTUNREACH:
    return "No route to host";
  case WSAEMFILE:
    return "Too many open files";
  case WSAENETDOWN:
    return " Network is down";
  case WSAENETRESET:
    return " Network dropped connection";
  case WSAENOBUFS:
    return " No buffer space available";
  case WSAENETUNREACH:
    return " Network is unreachable";
  case WSAETIMEDOUT:
    return " Connection timed out";
  case WSAHOST_NOT_FOUND:
    return " Host not found";
  case WSASYSNOTREADY:
    return " Network sub-system is unavailable";
  case WSANOTINITIALISED:
    return " WSAStartup() not performed";
  case WSANO_DATA:
    return " Valid name, no data of that type";
  case WSANO_RECOVERY:
    return " Non-recoverable query error";
  case WSATRY_AGAIN:
    return " Non-authoritative host found";
  case WSAVERNOTSUPPORTED:
    return " Wrong WinSock DLL version ";
  case WSAEAFNOSUPPORT:
    return "Address family not supported by protocol family";
  case WSAEALREADY:
    return "Operation already in progress";
  case WSAEINPROGRESS:
    return "Operation now in progress";
  case WSAEINTR:
    return "Interrupted function call";
  case WSAEINVAL:
    return "Invalid argument";
  case WSAEISCONN:
    return "Socket is already connected";
  case WSAEMSGSIZE:
    return "Message too long";
  case WSAENOTCONN:
    return "Socket is not connected";
  case WSAENOTSOCK:
    return "Socket operation on nonsocket";
  case WSAEOPNOTSUPP:
    return "Operation not supported";
  case WSAESHUTDOWN:
    return "Cannot send after socket shutdown";
  case WSAEFAULT:
    return "Bad address";
  case WSAEPROTONOSUPPORT:
    return "Protocol not supported";
  case WSAEPROTOTYPE:
    return "Protocol wrong type for socket";
  case WSAEADDRNOTAVAIL:
    return "Cannot assign requested address";
  case WSAEHOSTDOWN:
    return "Host is down";
  default:
    return "Unknown error";
  }
#else
  char buf[256]{};
  return ::strerror_r(err, buf, sizeof buf);
#endif
}

int getSockOpt(SOCKET socket, int level, int name, void *option,
               socklen_t *len) {
#ifdef PLATFORM_WINDOWS
  return ::getsockopt(socket, level, name, reinterpret_cast<char *>(option),
                      len);
#else
  return ::getsockopt(socket, level, name, option, len);
#endif
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

SOCKET accept(SOCKET socket, struct sockaddr_in6 *addr, socklen_t *len) {
  return ::accept(socket, reinterpret_cast<struct sockaddr *>(addr), len);
}

int getSocketError(SOCKET socket) {
  int sockErrno = 0;
  socklen_t len = sizeof(sockErrno);
  if (getSockOpt(socket, SOL_SOCKET, SO_ERROR, &sockErrno, &len) < 0) {
    LOG_ERROR("error at getSocketError");
  }
  return sockErrno;
}

std::string getSocketErrorStr(SOCKET socket) {
  int err = getSocketError(socket);
  return strError(err);
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

int connect(SOCKET socket, const struct sockaddr *addr, socklen_t len) {
#ifdef PLATFORM_WINDOWS
  return ::connect(socket, addr, len);
#else
  return ::connect(socket, addr, len);
#endif
}

} // namespace potato