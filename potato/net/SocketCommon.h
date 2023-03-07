#ifndef POTATO_POTATO_NET_SOCKETCOMMON_H_
#define POTATO_POTATO_NET_SOCKETCOMMON_H_

#include <cerrno>
#include <cstring>
#include <string>

#if defined(linux) || defined(__linux) || defined(__linux__)
#include <arpa/inet.h>
#include <csignal>
#include <fcntl.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <sys/socket.h>
#include <unistd.h>
#define SOCKET int
#define INVALID_SOCKET (-1)
#define PLATFORM_LINUX
#define perrno errno
#define PAGIN EAGAIN
#elif defined(WIN64) || defined(_WIN64) || defined(__WIN64__) ||               \
    defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
#define PLATFORM_WINDOWS
#include <WS2tcpip.h>
#include <WinSock2.h>
#define perrno (WSAGetLastError())
#define PAGAIN (WSAEWOULDBLOCK)
#endif

namespace potato {

SOCKET socket(int family, int type, int protocol);
int close(SOCKET socket);
int setNonBlock(SOCKET socket);
int setSockOpt(SOCKET socket, int level, int name, const void *option,
               socklen_t len);
std::string strError(int err);
int getSockOpt(SOCKET socket, int level, int name, void *option,
               socklen_t *len);
int setReuseAddr(SOCKET socket, bool on);
int setReusePort(SOCKET socket, bool on);
int setKeepAlive(SOCKET socket, bool on);
int setTcpNoDelay(SOCKET socket, bool on);
int bind(SOCKET socket, const struct sockaddr *addr, socklen_t len);
SOCKET accept(SOCKET socket, struct sockaddr *addr, socklen_t *len);
int listen(SOCKET socket);
int getSocketError(SOCKET socket);
std::string getSocketErrorStr(SOCKET socket);
int connect(SOCKET socket, const struct sockaddr *addr, socklen_t len);
ssize_t read(SOCKET socket, void *buf, size_t size);
ssize_t write(SOCKET socket, const void *buf, size_t size);

} // end namespace potato

#endif // POTATO_POTATO_NET_SOCKETCOMMON_H_
