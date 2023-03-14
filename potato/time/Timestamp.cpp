#include "potato/time/Timestamp.h"
#include "potato/log/Logger.h"
#include <cassert>

using potato::Timestamp;

std::string Timestamp::toString(bool withMilliSeconds) const {
  char buf[32] = {0};
  size_t size = sizeof(buf);
  formatTo(buf, size, withMilliSeconds);
  return buf;
}

size_t Timestamp::formatTo(char *buf, size_t size,
                           bool withMilliSeconds) const {
  auto seconds = toSeconds();
  struct tm tm_;
  // rest microseconds
  auto rest = timestamp_ - seconds * kMicroSecondsPerSecond;
  // convert to milliSeconds
  auto milliSeconds = rest / 1000;
#ifdef PLATFORM_WINDOWS
  localtime_s(&tm_, &seconds);
#else
  localtime_r(&seconds, &tm_);
#endif
  if (withMilliSeconds) {
    auto ret =
        fmt::format_to_n(buf, size, "{:04}-{:02}-{:02} {:02}:{:02}:{:02}:{:03}",
                         tm_.tm_year + 1900, tm_.tm_mon + 1, tm_.tm_mday,
                         tm_.tm_hour, tm_.tm_min, tm_.tm_sec, milliSeconds);
    size = ret.size;
  } else {
    auto ret = fmt::format_to_n(
        buf, size, "{:04}-{:02}-{:02} {:02}:{:02}:{:02}", tm_.tm_year + 1900,
        tm_.tm_mon + 1, tm_.tm_mday, tm_.tm_hour, tm_.tm_min, tm_.tm_sec);
    size = ret.size;
  }
  return size;
}

Timestamp Timestamp::now() {
#ifdef PLATFORM_WINDOWS
  struct _timeb buffer;
  _ftime(&buffer);
  return Timestamp(buffer.time * kMicroSecondsPerSecond +
                   buffer.millitm * 1000);
#else
  struct timeval tv;
  gettimeofday(&tv, nullptr);
  return Timestamp(tv.tv_sec * kMicroSecondsPerSecond + tv.tv_usec);
#endif
}

Timestamp potato::Timestamp::parseFromSeconds(time_t seconds) {
  return Timestamp(seconds * kMicroSecondsPerSecond);
}
