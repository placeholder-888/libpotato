#include "potato/net/Buffer.h"
#include <climits>

int totalTestCount = 0;
int testPassCount = 0;

#define TEST(condition)                                                        \
  do {                                                                         \
    ++totalTestCount;                                                          \
    if (condition) {                                                           \
      ++testPassCount;                                                         \
    } else {                                                                   \
      printf("test failed in %s:%d %s\n", __FILE__, __LINE__, __func__);       \
    }                                                                          \
  } while (0)

#define TEST_RESULT()                                                          \
  printf("[passed: %d] [failed:%d] [total:%d]\n", testPassCount,               \
         totalTestCount - testPassCount, totalTestCount)

void testCase1() {
  potato::Buffer buffer;
  TEST(buffer.readableBytes() == 0);
  TEST(buffer.writeableBytes() == 4096);
  buffer.write("hello world");
  TEST(buffer.readableBytes() == 11);
  TEST(buffer.writeableBytes() == 4096 - 11);
  TEST(buffer.readAsSlice() == "hello world");
  TEST(buffer.readableBytes() == 11);
  buffer.retrieve(11);
  TEST(buffer.readableBytes() == 0);
  TEST(buffer.writeableBytes() == 4096);
  std::string s(401, 'a');
  std::string content;
  for (size_t i = 1; i <= 10; ++i) {
    buffer.write(s);
    content += s;
    TEST(buffer.readableBytes() == i * s.size());
    TEST(buffer.writeableBytes() == 4096 - i * s.size());
    TEST(buffer.readAsSlice() == content);
    char buf[500];
    buffer.read(buf, s.size());
    TEST(potato::StringSlice(buf, s.size()) == s);
  }
  TEST(buffer.readableBytes() == 10 * s.size());
  TEST(buffer.writeableBytes() == 4096 - 10 * s.size());
  auto c = buffer.retrieveAllAsString();
  TEST(c == content);
}

void testCase2() {
  potato::Buffer buffer;
  buffer.writeInt32(8888);
  TEST(buffer.readableBytes() == sizeof(int32_t));
  TEST(buffer.writeableBytes() == 4096 - sizeof(int32_t));
  TEST(buffer.readInt32() == 8888);
  TEST(buffer.readableBytes() == sizeof(int32_t));
  TEST(buffer.retrieveInt32() == 8888);
  TEST(buffer.readableBytes() == 0);
  buffer.writeInt32(INT_MAX);
  buffer.writeInt32(INT_MIN);
  TEST(buffer.readableBytes() == 2 * sizeof(int32_t));
  TEST(buffer.writeableBytes() == 4096 - 2 * sizeof(int32_t));
  TEST(buffer.retrieveInt32() == INT_MAX);
  TEST(buffer.retrieveInt32() == INT_MIN);
  TEST(buffer.readableBytes() == 0);
  TEST(buffer.writeableBytes() == 4096);
  buffer.write(std::string(8000, 'a'));
  buffer.write(std::string(8888, 'b'));
  buffer.write(std::string(9999, 'c'));
  TEST(buffer.readableBytes() == 8000 + 8888 + 9999);
  std::string content =
      std::string(8000, 'a') + std::string(8888, 'b') + std::string(9999, 'c');
  TEST(buffer.readAsSlice() == content);
  TEST(buffer.readableBytes() == 8000 + 8888 + 9999);
  TEST(buffer.retrieveAllAsString() == content);
  TEST(buffer.readableBytes() == 0);
}

void testCase3() {
  potato::Buffer buffer;
  std::string str(4000, 'a');
  buffer.write(str);
  TEST(buffer.readableBytes() == 4000);
  TEST(buffer.writeableBytes() == 4096 - 4000);
  TEST(buffer.readAsSlice() == str);
  char buf[4096]{};
  buffer.retrieve(buf, 3999);
  TEST(potato::StringSlice(buf, 3999) == potato::StringSlice(str.data(), 3999));
  TEST(buffer.readableBytes() == 1);
  TEST(buffer.writeableBytes() == 4096 - 4000);
  TEST(buffer.readAsSlice() == potato::StringSlice("a", 1));
  // 会进行移动 而不是扩容
  buffer.write(str.data(), str.size());
  TEST(buffer.readableBytes() == 4001);
  TEST(buffer.writeableBytes() == 4096 - 4001);
  TEST(buffer.readAsSlice() == str + "a");
  TEST(buffer.retrieveAllAsString() == str + "a");
  TEST(buffer.readableBytes() == 0);
  TEST(buffer.writeableBytes() == 4096);
  buffer.writeInt32(INT_MAX);
  buffer.writeInt32(INT_MIN);
  buffer.writeInt64(INT64_MAX);
  buffer.writeInt64(INT64_MIN);
  buffer.writeUint64(UINT64_MAX);
  buffer.writeUint64(0);
  TEST(buffer.readableBytes() ==
       2 * sizeof(int32_t) + 2 * sizeof(int64_t) + 2 * sizeof(uint64_t));
  TEST(buffer.writeableBytes() ==
       4096 - 2 * sizeof(int32_t) - 2 * sizeof(int64_t) - 2 * sizeof(uint64_t));
  TEST(buffer.retrieveInt32() == INT_MAX);
  TEST(buffer.retrieveInt32() == INT_MIN);
  TEST(buffer.retrieveInt64() == INT64_MAX);
  TEST(buffer.retrieveInt64() == INT64_MIN);
  TEST(buffer.retrieveUint64() == UINT64_MAX);
  TEST(buffer.retrieveUint64() == 0);
  TEST(buffer.readableBytes() == 0);
  TEST(buffer.writeableBytes() == 4096);
}

void testCase4() {
  potato::Buffer buffer;
  buffer.write(std::string(100, 'o'));
  TEST(buffer.readableBytes() == 100);
  TEST(buffer.writeableBytes() == 4096 - 100);
  buffer.retrieve(10);
  TEST(buffer.readableBytes() == 90);
  TEST(buffer.writeableBytes() == 4096 - 100);
  buffer.hasWritten(3333);
  TEST(buffer.readableBytes() == 90 + 3333);
  TEST(buffer.writeableBytes() == 4096 - 100 - 3333);
  char buf[101]{};
  buffer.read(buf, 100);
  TEST(buffer.readableBytes() == 90 + 3333);
  TEST(buffer.writeableBytes() == 4096 - 100 - 3333);
  TEST(potato::StringSlice(buf, 100) ==
       std::string(90, 'o') + std::string(10, 0));
  buffer.retrieve(90);
  TEST(buffer.readableBytes() == 3333);
  TEST(buffer.writeableBytes() == 4096 - 100 - 3333);
  auto str = buffer.retrieveAllAsString();
  TEST(str.size() == 3333);
  TEST(str == std::string(3333, 0));
  TEST(buffer.readableBytes() == 0);
  TEST(buffer.writeableBytes() == 4096);
}

void testCase5() {
  potato::Buffer buffer;
  buffer.write(std::string(3000, 'o'));
  buffer.write("hello");
  TEST(buffer.readableBytes() == 3005);
  TEST(buffer.writeableBytes() == 4096 - 3005);
  auto slice = buffer.readAsSlice();
  auto ptr = slice.find("hello");
  TEST(ptr != nullptr);
  TEST(ptr - slice.data() == 3000);
  TEST(ptr - buffer.peek() == 3000);
  TEST(buffer.readableBytes() == 3005);
  TEST(buffer.writeableBytes() == 4096 - 3005);
  buffer.retrieveUntil(ptr);
  TEST(buffer.readableBytes() == 5);
  TEST(buffer.writeableBytes() == 4096 - 3005);
  TEST(buffer.readAsSlice() == "hello");
  TEST(*buffer.peek() == 'h');
  TEST(buffer.retrieveAllAsString() == "hello");
  TEST(buffer.readableBytes() == 0);
  TEST(buffer.writeableBytes() == 4096);
  std::string bigContent(4096, 'a');
  for (size_t i = 1; i <= 1024; ++i) {
    buffer.write(bigContent);
    TEST(buffer.readableBytes() == i * 4096);
  }
  TEST(buffer.readableBytes() == 1024 * 4096);
  TEST(buffer.readAsSlice().split('a').size() == 1024 * 4096 + 1);
  TEST(buffer.readableBytes() == 1024 * 4096);
  for (size_t i = 1; i <= 1024; ++i) {
    char buf[4096]{};
    buffer.retrieve(buf, 4096);
    TEST(potato::StringSlice(buf, 4096) == bigContent);
  }
  TEST(buffer.readableBytes() == 0);
  TEST(buffer.writeableBytes() >= 4096 * 1024);
}

int main() {
  testCase1();
  testCase2();
  testCase3();
  testCase4();
  testCase5();
  TEST_RESULT();
}