#ifndef POTATO_POTATO_TIME_TIMESTAMP_H_
#define POTATO_POTATO_TIME_TIMESTAMP_H_

#if defined(linux) || defined(__linux) || defined(__linux__)
#define PLATFORM_LINUX
#include <sys/time.h>
#elif defined(WIN64) || defined(_WIN64) || defined(__WIN64__) ||               \
    defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
#define PLATFORM_WINDOWS
#include <sys/timeb.h>
#include <windows.h>
#endif

#include <cstdint>
#include <ctime>
#include <string>

namespace potato {

class Timestamp {
public:
  constexpr const static int64_t kMicroSecondsPerSecond = 1000 * 1000;

  Timestamp() : timestamp_(-1){};
  explicit Timestamp(int64_t microSeconds) : timestamp_(microSeconds) {}

  static Timestamp now();
  bool valid() const { return timestamp_ >= 0; }

  int64_t timestamp() const { return timestamp_; }

  time_t toSeconds() const {
    return static_cast<time_t>(timestamp_ / kMicroSecondsPerSecond);
  }

  std::string toString(bool withMilliSeconds = false) const;
  // 不会以\0结尾 在format的时候 注意预留一个字节
  size_t formatTo(char *buf, size_t size, bool withMilliSeconds = false) const;

  Timestamp addMicroSeconds(int64_t microSeconds) const {
    return Timestamp(timestamp_ + microSeconds);
  }
  Timestamp addMilliSeconds(int64_t milliSeconds) const {
    return Timestamp(timestamp_ + milliSeconds * 1000);
  }
  Timestamp addSeconds(int64_t seconds) const {
    return Timestamp(timestamp_ + seconds * kMicroSecondsPerSecond);
  }
  Timestamp addMinutes(int64_t minutes) const {
    return Timestamp(timestamp_ + minutes * kMicroSecondsPerSecond * 60);
  }
  Timestamp addHour(int64_t hour) const {
    return Timestamp(timestamp_ + hour * kMicroSecondsPerSecond * 60 * 60);
  }
  Timestamp addDay(int64_t day) const {
    return Timestamp(timestamp_ + day * kMicroSecondsPerSecond * 60 * 60 * 24);
  }

  static Timestamp parseFromSeconds(time_t seconds);

private:
  int64_t timestamp_;
};

inline bool operator==(const Timestamp &lhs, const Timestamp &rhs) {
  return lhs.timestamp() == rhs.timestamp();
}

inline bool operator!=(const Timestamp &lhs, const Timestamp &rhs) {
  return !(lhs == rhs);
}

inline bool operator<(const Timestamp &lhs, const Timestamp &rhs) {
  return lhs.timestamp() < rhs.timestamp();
}

inline bool operator>(const Timestamp &lhs, const Timestamp &rhs) {
  return rhs < lhs;
}

inline bool operator<=(const Timestamp &lhs, const Timestamp &rhs) {
  return !(lhs > rhs);
}

inline bool operator>=(const Timestamp &lhs, const Timestamp &rhs) {
  return !(lhs < rhs);
}

} // namespace potato

#endif // POTATO_POTATO_TIME_TIMESTAMP_H_
