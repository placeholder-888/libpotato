#include "potato/net/IpAddress.h"

int totalTestCount = 0;
int testPassCount = 0;

#define TEST(condition)                                                        \
  do {                                                                         \
    ++totalTestCount;                                                          \
    if (condition) {                                                           \
      ++testPassCount;                                                         \
    } else {                                                                   \
      printf("test failed in %s:%d\n", __FILE__, __LINE__);                    \
    }                                                                          \
  } while (0)

#define TEST_RESULT()                                                          \
  printf("[passed: %d] [failed:%d] [total:%d]\n", testPassCount,               \
         totalTestCount - testPassCount, totalTestCount)

void testCase1() {
  potato::IpAddress addr0;
  TEST(addr0.family() == AF_INET);
  TEST(addr0.ipv6() == false);
  TEST(addr0.Port() == 0);
  TEST(addr0.Ip() == "0.0.0.0");
  TEST(addr0.IpPort() == "0.0.0.0:0");
  potato::IpAddress addr1(80);
  TEST(addr1.family() == AF_INET);
  TEST(addr1.ipv6() == false);
  TEST(addr1.Port() == 80);
  TEST(addr1.Ip() == "0.0.0.0");
  TEST(addr1.IpPort() == "0.0.0.0:80");
  potato::IpAddress addr2(10086, true);
  TEST(addr2.family() == AF_INET);
  TEST(addr2.ipv6() == false);
  TEST(addr2.Port() == 10086);
  TEST(addr2.Ip() == "127.0.0.1");
  TEST(addr2.IpPort() == "127.0.0.1:10086");
  potato::IpAddress addr3("111.111.111.111", 8888);
  TEST(addr3.family() == AF_INET);
  TEST(addr3.ipv6() == false);
  TEST(addr3.Port() == 8888);
  TEST(addr3.Ip() == "111.111.111.111");
  TEST(addr3.IpPort() == "111.111.111.111:8888");
}

void testCase2() {
  potato::IpAddress addr0(0, false, true);
  TEST(addr0.family() == AF_INET6);
  TEST(addr0.ipv6() == true);
  TEST(addr0.Port() == 0);
  TEST(addr0.Ip() == "::");
  TEST(addr0.IpPort() == "[::]:0");
  potato::IpAddress addr1(80, false, true);
  TEST(addr1.family() == AF_INET6);
  TEST(addr1.ipv6() == true);
  TEST(addr1.Port() == 80);
  TEST(addr1.Ip() == "::");
  TEST(addr1.IpPort() == "[::]:80");
  potato::IpAddress addr2(10086, true, true);
  TEST(addr2.family() == AF_INET6);
  TEST(addr2.ipv6() == true);
  TEST(addr2.Port() == 10086);
  TEST(addr2.Ip() == "::1");
  TEST(addr2.IpPort() == "[::1]:10086");
  potato::IpAddress addr3("::ffff:192.168.89.9", 6666, true);
  TEST(addr3.family() == AF_INET6);
  TEST(addr3.ipv6() == true);
  TEST(addr3.Port() == 6666);
  TEST(addr3.Ip() == "::ffff:192.168.89.9");
  TEST(addr3.IpPort() == "[::ffff:192.168.89.9]:6666");
  potato::IpAddress addr4("cdcd:910a:2222:5498:8475:1111:3900:2020", 6666,
                          true);
  TEST(addr4.family() == AF_INET6);
  TEST(addr4.ipv6() == true);
  TEST(addr4.Port() == 6666);
  TEST(addr4.Ip() == "cdcd:910a:2222:5498:8475:1111:3900:2020");
  TEST(addr4.IpPort() == "[cdcd:910a:2222:5498:8475:1111:3900:2020]:6666");
}

int main() {
  testCase1();
  testCase2();
  TEST_RESULT();
}