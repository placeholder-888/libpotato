#ifndef POTATO_POTATO_NET_IPADDRESS_H_
#define POTATO_POTATO_NET_IPADDRESS_H_

#include "SocketCommon.h"
#include "potato/utils/Endian.h"
#include "potato/utils/StringSlice.h"

namespace potato {

class IpAddress {
public:
  explicit IpAddress(uint16_t port = 0, bool loopBack = false,
                     bool ipv6 = false);
  explicit IpAddress(StringSlice ip, uint16_t port, bool ipv6 = false);

  explicit IpAddress(const struct sockaddr_in &addr) : plainAddr_(addr) {}
  explicit IpAddress(const struct sockaddr_in6 &addr) : plainAddr6_(addr) {}

  int family() const { return plainAddr_.sin_family; }
  bool ipv6() const { return family() == AF_INET6; }

  const struct sockaddr *getSockAddr() const {
    return ipv6() ? reinterpret_cast<const sockaddr *>(&plainAddr_)
                  : reinterpret_cast<const sockaddr *>(&plainAddr6_);
  }

  std::string Ip() const;
  uint16_t Port() const { return endian::networkToHost16(plainAddr_.sin_port); }
  std::string IpPort() const;

private:
  union {
    struct sockaddr_in plainAddr_;
    struct sockaddr_in6 plainAddr6_ {};
  };
};

} // namespace potato

#endif // POTATO_POTATO_NET_IPADDRESS_H_
