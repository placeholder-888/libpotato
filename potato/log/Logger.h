#ifndef POTATO_POTATO_NET_LOGGER_H_
#define POTATO_POTATO_NET_LOGGER_H_

#include <chrono>
#include <cstdio>
#include <cstring>
#include <ctime>
#include <string>

class SourceFile {
public:
  template <int N> SourceFile(const char (&arr)[N]) : data_(arr), size_(N - 1) {
    const char *slash = strrchr(data_, '/');
    if (slash) {
      data_ = slash + 1;
      size_ -= static_cast<int>(data_ - arr);
    }
  }
  const char *data_;
  int size_;
};

#define LOG_TRACE(format, ...)                                                 \
  printf("[TRACE] " format " (%s:%d)\n", ##__VA_ARGS__,                        \
         SourceFile(__FILE__).data_, __LINE__)
#define LOG_DEBUG(format, ...)                                                 \
  printf("[DEBUG] " format " (%s:%d)\n", ##__VA_ARGS__,                        \
         SourceFile(__FILE__).data_, __LINE__)
#define LOG_INFO(format, ...)                                                  \
  printf("[INFO] " format " (%s:%d)\n", ##__VA_ARGS__,                         \
         SourceFile(__FILE__).data_, __LINE__)
#define LOG_WARN(format, ...)                                                  \
  printf("[WARN] " format " (%s:%d)\n", ##__VA_ARGS__,                         \
         SourceFile(__FILE__).data_, __LINE__)
#define LOG_ERROR(format, ...)                                                 \
  printf("[ERROR] " format " (%s:%d)\n", ##__VA_ARGS__,                        \
         SourceFile(__FILE__).data_, __LINE__)
#define LOG_FATAL(format, ...)                                                 \
  printf("[FATAL] " format " (%s:%d)\n", ##__VA_ARGS__,                        \
         SourceFile(__FILE__).data_, __LINE__)

#endif // POTATO_POTATO_NET_LOGGER_H_
