#ifndef POTATO_POTATO_NET_LOGGER_H_
#define POTATO_POTATO_NET_LOGGER_H_

#include "3rdparty/fmt/include/fmt/core.h"
#include <cstdio>
#include <cstring>
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

#define LOG_TRACE(format, args...)                                             \
  fmt::print("[TRACE] " format " ({}:{})\n", ##args,                           \
             SourceFile(__FILE__).data_, __LINE__)

#define LOG_DEBUG(format, args...)                                             \
  fmt::print("[DEBUG] " format " ({}:{})\n", ##args,                           \
             SourceFile(__FILE__).data_, __LINE__)

#define LOG_INFO(format, args...)                                              \
  fmt::print("[INFO] " format " ({}:{})\n", ##args,                            \
             SourceFile(__FILE__).data_, __LINE__)

#define LOG_WARN(format, args...)                                              \
  fmt::print("[WARN] " format " ({}:{})\n", ##args,                            \
             SourceFile(__FILE__).data_, __LINE__)

#define LOG_ERROR(format, args...)                                             \
  fmt::print("[ERROR] " format " ({}:{})\n", ##args,                           \
             SourceFile(__FILE__).data_, __LINE__)

#define LOG_FATAL(format, args...)                                             \
  fmt::print("[FATAL] " format " ({}:{})\n", ##args,                           \
             SourceFile(__FILE__).data_, __LINE__)

#endif // POTATO_POTATO_NET_LOGGER_H_
