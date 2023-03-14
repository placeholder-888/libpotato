#include "potato/time/Date.h"
#include "potato/log/Logger.h"
#include <cassert>

namespace {
struct tm LOCAL_TIME(const time_t *seconds) {
  struct tm tm_ {};
#ifdef PLATFORM_WINDOWS
  localtime_s(&tm_, seconds);
#else
  localtime_r(seconds, &tm_);
#endif
  return tm_;
}
} // namespace

using potato::Date;

bool Date::isLeapYear(int year) {
  return (year % 4 == 0 && year % 100 != 0) || year % 400 == 0;
}

int Date::getYearDays(int year) { return isLeapYear(year) ? 366 : 365; }

int Date::getDaysInMonth(int year, int month) {
  assert(month >= 1 && month <= 12);
  static const int daysInMonth[] = {31, 28, 31, 30, 31, 30,
                                    31, 31, 30, 31, 30, 31};
  if (month == 2 && isLeapYear(year))
    return 29;
  return daysInMonth[month - 1];
}

Date::Date() {
  seconds_ = time(nullptr);
  tm_ = LOCAL_TIME(&seconds_);
}

Date::Date(const Timestamp &timestamp) {
  if (!timestamp.valid()) {
    LOG_ERROR("Date::Date() invalid timestamp");
  }
  seconds_ = timestamp.toSeconds();
  tm_ = LOCAL_TIME(&seconds_);
}

Date::Date(time_t seconds) : Date(Timestamp::parseFromSeconds(seconds)) {}

Date::Date(int year, int month, int day, int hour, int minute, int second) {
  tm_.tm_year = year - 1900;
  tm_.tm_mon = month - 1;
  tm_.tm_mday = day;
  tm_.tm_hour = hour;
  tm_.tm_min = minute;
  tm_.tm_sec = second;
  seconds_ = mktime(&tm_);
  if (seconds_ < 0) {
    LOG_ERROR("Date::Date() invalid date");
  }
}

time_t Date::toSeconds() const { return seconds_; }

potato::Timestamp Date::toTimestamp() const {
  return Timestamp::parseFromSeconds(toSeconds());
}

bool Date::isLeapYear() const { return isLeapYear(year()); }

Date Date::addHours(int hours) const {
  return Date(toTimestamp().addHour(hours));
}

Date Date::addMinutes(int minutes) const {
  return Date(toTimestamp().addMinutes(minutes));
}

Date Date::addSeconds(int seconds) const {
  return Date(toTimestamp().addSeconds(seconds));
}

Date Date::addDays(int days) const { return Date(toTimestamp().addDay(days)); }

Date Date::addMonths(int months) const {
  return Date(year(), month() + months, day(), hour(), minute(), second());
}

Date Date::addYears(int years) const {
  return Date(year() + years, month(), day(), hour(), minute(), second());
}

std::string Date::toString() const {
  return fmt::format("{:04}-{:02}-{:02} {:02}:{:02}:{:02}", tm_.tm_year + 1900,
                     tm_.tm_mon + 1, tm_.tm_mday, tm_.tm_hour, tm_.tm_min,
                     tm_.tm_sec);
}

int Date::dayDistance(const Date &other) const {
  auto secondsDistance = std::abs(seconds_ - other.seconds_);
  return static_cast<int>(secondsDistance / (24 * 60 * 60));
}
