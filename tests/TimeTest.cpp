#include "potato/log/Logger.h"
#include "potato/time/Date.h"
#include "potato/time/Timestamp.h"

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
  potato::Timestamp s;
  TEST(!s.valid());
  potato::Timestamp t = potato::Timestamp::now();
  TEST(t.valid());
  fmt::println("{:25} {}", "current time", t.toString());
  fmt::println("{:25} {}", "current time with milli", t.toString(true));
  fmt::println("{:25} {}", "add 5000 microseconds",
               t.addMicroSeconds(5000).toString(true));
  fmt::println("{:25} {}", "add 5000 milliseconds",
               t.addMilliSeconds(5000).toString(true));
  fmt::println("{:25} {}", "add 5 seconds", t.addSeconds(5).toString(true));
  TEST(t.addMilliSeconds(5000) == t.addSeconds(5));
  fmt::println("{:25} {}", "add 5 minutes", t.addMinutes(5).toString(true));
  TEST(t.addSeconds(60) == t.addMinutes(1));
  fmt::println("{:25} {}", "add 5 hours", t.addHour(5).toString(true));
  TEST(t.addMinutes(60) == t.addHour(1));
  fmt::println("{:25} {}", "add 5 days", t.addDay(5).toString(true));
  TEST(t.addHour(24) == t.addDay(1));
  TEST(t.addHour(8 * 24) == t.addDay(8));
}

void testCase2() {
  potato::Date d;
  fmt::println("{:25} {}", "date format", d.toString());
  TEST(d.addHours(24) == d.addDays(1));
  TEST(d.addHours(24 * 7) == d.addDays(7));
  TEST(d.addHours(24 * 30) == d.addDays(30));
  TEST(d.addHours(24 * 365) == d.addDays(365));
  TEST(d.addHours(-24) == d.addDays(-1));
  TEST(d.addHours(-24 * 7) == d.addDays(-7));
  TEST(d.addHours(-24 * 30) == d.addDays(-30));
  TEST(d.addHours(-24 * 365) == d.addDays(-365));
  fmt::println("{} {:20} {}", d.toString(), "after add 1 year",
               d.addYears(1).toString());
  fmt::println("{} {:20} {}", d.toString(), "after add 1 month",
               d.addMonths(1).toString());
  fmt::println("{} {:20} {}", d.toString(), "after add 1 day",
               d.addDays(1).toString());
  fmt::println("{} {:20} {}", d.toString(), "after add 1 hour",
               d.addHours(1).toString());
  fmt::println("{} {:20} {}", d.toString(), "after add 1 minute",
               d.addMinutes(1).toString());
  fmt::println("{} {:20} {}", d.toString(), "after add 1 second",
               d.addSeconds(1).toString());
  potato::Date d1(2000, 1, 1);
  TEST(d1.addYears(1) == potato::Date(2001, 1, 1));
  TEST(d1.addYears(2) == potato::Date(2002, 1, 1));
  TEST(d1.addMonths(3) == potato::Date(2000, 4, 1));
  TEST(d1.addMonths(4) == potato::Date(2000, 5, 1));
  TEST(d1.addDays(5) == potato::Date(2000, 1, 6));
  TEST(d1.addDays(31) == potato::Date(2000, 2, 1));
  TEST(d1.weekday() == 6);
  TEST(d1.addDays(1).weekday() == 7);
  TEST(d1.addDays(2).weekday() == 1);
  TEST(d1.isLeapYear());
  TEST(d1.dayInYear() == 1);
  TEST(d1.daysInMonth() == 31);
  TEST(d1.daysInYear() == 366);
  TEST(d1.getDaysInMonth(2001, 1) == 31);
  TEST(d1.getDaysInMonth(2001, 2) == 28);
  TEST(d1.getDaysInMonth(2000, 2) == 29);
  TEST(d1.isLeapYear(2001) == false);
  TEST(d1.addYears(20).addMonths(2).addDays(3) == potato::Date(2020, 3, 4));
  TEST(d1.dayDistance(d1.addYears(20).addMonths(2).addDays(3)) == 7368);
  potato::Date date(2020, 3, 4);
  TEST(date.year() == 2020);
  TEST(date.month() == 3);
  TEST(date.day() == 4);
  TEST(date.hour() == 0);
  TEST(date.minute() == 0);
  TEST(date.second() == 0);
  TEST(date.dayInYear() == 64); // 31 + 29 + 4
  TEST(date.weekday() == 3);
  TEST(date.daysInMonth() == 31);
  TEST(date.daysInYear() == 366);
  TEST(date.isLeapYear());
  TEST(date.getDaysInMonth(2020, 2) == 29);
  TEST(date.getDaysInMonth(2020, 3) == 31);
  potato::Date date1(2025, 7, 1, 6, 6, 6);
  TEST(date1.year() == 2025);
  TEST(date1.month() == 7);
  TEST(date1.day() == 1);
  TEST(date1.hour() == 6);
  TEST(date1.minute() == 6);
  TEST(date1.second() == 6);
  TEST(date1.dayInYear() == 182); // 31 + 28 + 31 + 30 + 31 + 30 + 1
  TEST(date1.weekday() == 2);
  TEST(date1.daysInMonth() == 31);
  TEST(date1.daysInYear() == 365);
  TEST(date1.isLeapYear() == false);
  TEST(date1.dayDistance(potato::Date(2005, 3, 8)) == 7420);
}

int main() {
  testCase1();
  testCase2();
  TEST_RESULT();
}