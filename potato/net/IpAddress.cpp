#include "potato/net/IpAddress.h"
#include "potato/log/Logger.h"

using namespace potato;

IpAddress::IpAddress(uint16_t port, bool loopBack, bool ipv6) {
  plainAddr_.sin_family = ipv6 ? AF_INET6 : AF_INET;
  plainAddr_.sin_port = endian::hostToNetwork16(port);
  if (ipv6) {
    plainAddr6_.sin6_addr = loopBack ? in6addr_loopback : in6addr_any;
  } else {
    plainAddr_.sin_addr.s_addr =
        loopBack ? endian::hostToNetwork32(INADDR_LOOPBACK) : INADDR_ANY;
  }
}

IpAddress::IpAddress(StringSlice ip, uint16_t port, bool ipv6) {
  plainAddr_.sin_family = ipv6 ? AF_INET6 : AF_INET;
  plainAddr_.sin_port = endian::hostToNetwork16(port);
  int ret = 0;
  if (ipv6) {
    ret = inet_pton(AF_INET6, ip.data(), &plainAddr6_.sin6_addr);
  } else {
    ret = inet_pton(AF_INET, ip.data(), &plainAddr_.sin_addr);
  }
  if (ret < 0) {
    LOG_ERROR("IpAddress() ip:%s error:%s", ip.data(),
              strError(perrno).c_str());
  }
}

std::string IpAddress::Ip() const {
  char buf[128];
  if (!ipv6()) {
    inet_ntop(AF_INET, &plainAddr_.sin_addr, buf, sizeof buf);
  } else {
    inet_ntop(AF_INET6, &plainAddr6_.sin6_addr, buf, sizeof buf);
  }
  return buf;
}

std::string IpAddress::IpPort() const {
  char buf[128];
  if (!ipv6()) {
    inet_ntop(AF_INET, &plainAddr_.sin_addr, buf, sizeof buf);
  } else {
    buf[0] = '[';
    inet_ntop(AF_INET6, &plainAddr6_.sin6_addr, buf + 1, sizeof buf - 1);
  }
  auto size = ::strlen(buf);
  ::snprintf(buf + size, sizeof(buf) - size, "%s:%u", ipv6() ? "]" : "",
             Port());
  return buf;
}