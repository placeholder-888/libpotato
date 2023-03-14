#ifndef POTATO_POTATO_TIME_DATE_H_
#define POTATO_POTATO_TIME_DATE_H_

#include "potato/time/Timestamp.h"
#include <ctime>
#include <vector>

namespace potato {
// 对tm的一个简单封装
class Date {
public:
  Date(); // 会生成当前时间
  explicit Date(const Timestamp &timestamp);
  explicit Date(time_t seconds);
  explicit Date(int year, int month, int day, int hour = 0, int minute = 0,
                int second = 0);

  int year() const { return tm_.tm_year + 1900; }
  int month() const { return tm_.tm_mon + 1; }
  int day() const { return tm_.tm_mday; }
  int hour() const { return tm_.tm_hour; }
  int minute() const { return tm_.tm_min; }
  int second() const { return tm_.tm_sec; }
  int weekday() const { return tm_.tm_wday == 0 ? 7 : tm_.tm_wday; }
  // 一年中的第几天
  int dayInYear() const { return tm_.tm_yday + 1; }
  // 一月中的天数
  int daysInMonth() const { return getDaysInMonth(year(), month()); }
  // 一年中的天数
  int daysInYear() const { return getYearDays(year()); }

  int dayDistance(const Date &other) const;

  bool isLeapYear() const;

  static int getDaysInMonth(int year, int month);
  static bool isLeapYear(int year);
  static int getYearDays(int year);

  Timestamp toTimestamp() const;
  time_t toSeconds() const;

  struct tm toTm() const { return tm_; }

  Date addDays(int days) const;
  Date addMonths(int months) const;
  Date addYears(int years) const;
  Date addHours(int hours) const;
  Date addMinutes(int minutes) const;
  Date addSeconds(int seconds) const;

  std::string toString() const;

private:
  time_t seconds_{0};
  struct tm tm_ {};
};

inline bool operator==(const Date &lhs, const Date &rhs) {
  return lhs.toTimestamp() == rhs.toTimestamp();
}

inline bool operator!=(const Date &lhs, const Date &rhs) {
  return !(lhs == rhs);
}

inline bool operator<(const Date &lhs, const Date &rhs) {
  return lhs.toTimestamp() < rhs.toTimestamp();
}

inline bool operator>(const Date &lhs, const Date &rhs) { return rhs < lhs; }

inline bool operator<=(const Date &lhs, const Date &rhs) {
  return !(lhs > rhs);
}

inline bool operator>=(const Date &lhs, const Date &rhs) {
  return !(lhs < rhs);
}

} // namespace potato

#endif // POTATO_POTATO_TIME_DATE_H_
