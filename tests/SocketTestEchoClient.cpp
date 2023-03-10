#include "potato/log/Logger.h"
#include "potato/net/Socket.h"

#include <iostream>
#include <string>

int main() {
  socket_t s = potato::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  potato::Socket socket(s);
  potato::IpAddress hostAddr(8888, true);
  if (!socket.connect(hostAddr)) {
    LOG_ERROR("connect error! hostAddr:%s msg:%s", hostAddr.IpPort().c_str(),
              potato::strError(perrno).c_str());
    return 1;
  }
  // get the socket address
  LOG_INFO("connected Success! localAddr:%s peerAddr:%s",
           socket.ipAddress().IpPort().c_str(), hostAddr.IpPort().c_str());
  while (true) {
    std::string str;
    std::getline(std::cin, str);
    if (str == "quit")
      break;
    socket.write(str.c_str(), str.size());
    char buf[4096];
    auto size = socket.read(buf, sizeof buf);
    if (size < 0) {
      LOG_ERROR("read error %s", potato::strError(perrno).c_str());
      break;
    } else if (size == 0) {
      LOG_INFO("peek close connection");
      break;
    } else {
      buf[size] = 0;
      LOG_INFO("receive:%s", buf);
    }
  }
  return 0;
}