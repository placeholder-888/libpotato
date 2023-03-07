#include "potato/log/Logger.h"
#include "potato/net/SocketCommon.h"
#include "potato/utils/Endian.h"

#include <iostream>
#include <string>

/*
int main() {
  SOCKET socket = potato::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  struct sockaddr_in addr {};
  addr.sin_family = AF_INET;
  addr.sin_port = potato::endian::hostToNetwork16(8888);
  addr.sin_addr.s_addr = potato::endian::hostToNetwork32(INADDR_LOOPBACK);
  auto ret = potato::connect(socket, reinterpret_cast<const sockaddr *>(&addr),
                  sizeof(addr));
  if (ret != 0) {
    LOG_ERROR("connect error! msg:%s",potato::strError(perrno).c_str());
    return 1;
  }
  while (true) {
    std::string str;
    std::getline(std::cin, str);
    potato::write(socket, str.c_str(), str.size());
    char buf[4096];
    auto size = potato::read(socket, buf, sizeof buf);
    if (size < 0) {
      LOG_ERROR("read error %s", potato::strError(perrno).c_str());
      break;
    }else if(size == 0){
      LOG_INFO("peek close connection");
      break;
    }else{
      buf[size] = 0;
      LOG_INFO("receive:%s",buf);
    }
  }
  potato::close(socket);
}
 */

int main() {}