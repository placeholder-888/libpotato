#ifndef POTATO_POTATO_NET_LOGGER_H_
#define POTATO_POTATO_NET_LOGGER_H_

#include <chrono>
#include <cstdio>
#include <ctime>
#include <string>

#define LOG_TRACE(format, ...)                                                     \
  printf("[TRACE] " format " (%s:%d)\n",  ##__VA_ARGS__,__FILE__, __LINE__)
#define LOG_DEBUG(format, ...)                                                     \
  printf("[DEBUG] " format " (%s:%d)\n",  ##__VA_ARGS__,__FILE__, __LINE__)
#define LOG_INFO(format, ...)                                                     \
  printf("[INFO] " format " (%s:%d)\n",  ##__VA_ARGS__,__FILE__, __LINE__)
#define LOG_WARN(format, ...)                                                     \
  printf("[WARN] " format " (%s:%d)\n",  ##__VA_ARGS__,__FILE__, __LINE__)
#define LOG_ERROR(format, ...)                                                     \
  printf("[ERROR] " format " (%s:%d)\n",  ##__VA_ARGS__,__FILE__, __LINE__)
#define LOG_FATAL(format, ...)                                                     \
  printf("[FATAL] " format " (%s:%d)\n",  ##__VA_ARGS__,__FILE__, __LINE__)

#endif // POTATO_POTATO_NET_LOGGER_H_
