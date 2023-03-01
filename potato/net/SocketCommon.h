#ifndef POTATO_POTATO_NET_SOCKETCOMMON_H_
#define POTATO_POTATO_NET_SOCKETCOMMON_H_

#if defined(linux) || defined(__linux) || defined(__linux__)
#include <arpa/inet.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#define SOCKET int
#elif defined(WIN64) || defined(_WIN64) || defined(__WIN64__) ||               \
    defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
#include <WS2tcpip.h>
#include <WinSock2.h>
#program comment(lib, "ws2_32.lib")
#endif

namespace potato {} // end namespace potato

#endif // POTATO_POTATO_NET_SOCKETCOMMON_H_
