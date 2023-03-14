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
#define socket_t int
#define INVALID_SOCKET (-1)
#define PLATFORM_LINUX
#define perrno errno
#define PAGAIN EAGAIN
#define processId_t pid_t
#elif defined(WIN64) || defined(_WIN64) || defined(__WIN64__) ||               \
    defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
#define PLATFORM_WINDOWS
#include <WS2tcpip.h>
#include <WinSock2.h>
#include <processthreadsapi.h>
#include <windows.h>

#define perrno (WSAGetLastError())
#define PAGAIN (WSAEWOULDBLOCK)
#define socket_t SOCKET
#define processId_t DWORD
#endif

namespace potato {

socket_t socket(int family, int type, int protocol);
int close(socket_t socket);
int setNonBlock(socket_t socket);
int setSockOpt(socket_t socket, int level, int name, const void *option,
               socklen_t len);
std::string strError(int err);
int getSockOpt(socket_t socket, int level, int name, void *option,
               socklen_t *len);
int setReuseAddr(socket_t socket, bool on);
int setReusePort(socket_t socket, bool on);
int setKeepAlive(socket_t socket, bool on);
int setTcpNoDelay(socket_t socket, bool on);
int bind(socket_t socket, const struct sockaddr *addr, socklen_t len);
socket_t accept(socket_t socket, struct sockaddr *addr, socklen_t *len);
socket_t accept(socket_t socket, struct sockaddr_in6 *addr, socklen_t *len);
int listen(socket_t socket);
int getSocketError(socket_t socket);
std::string getSocketErrorStr(socket_t socket);
int connect(socket_t socket, const struct sockaddr *addr, socklen_t len);
ssize_t read(socket_t socket, void *buf, size_t size);
ssize_t write(socket_t socket, const void *buf, size_t size);
// for EventLoop wakeup
int socketPair(socket_t socket[2]);
processId_t getProcessId();

} // end namespace potato

#endif // POTATO_POTATO_NET_SOCKETCOMMON_H_
