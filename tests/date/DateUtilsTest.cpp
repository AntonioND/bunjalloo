#include <gtest/gtest.h>
#include "DateUtils.h"

TEST(DateUtils, parse_date)
{
  /*
     from time import strftime, strptime
     strftime('%s', strptime("Sun, 05 Jul 2009 19:11:28 GMT", "%a, %d %b %Y %H:%M:%S GMT"))
  */
  time_t expected(1246817488);
  time_t result = DateUtils::parseDate("Sun, 05 Jul 2009 19:11:28 GMT");

  EXPECT_EQ(expected, result);
}

TEST(DateUtils, parse_date_with_dashes)
{
  // with dashes should work too!
  time_t expected(1246817488);
  time_t result = DateUtils::parseDate("Sun, 05-Jul-2009 19:11:28 GMT");

  EXPECT_EQ(expected, result);
}

TEST(DateUtils, zero_time)
{
  time_t result = DateUtils::parseDate("Thu, 01 Jan 1970 00:00:00 GMT");
  EXPECT_EQ(timezone, result);
}


TEST(DateUtils, parse_bogus)
{
  time_t expected(0);
  time_t result = DateUtils::parseDate("KKKKK");

  EXPECT_EQ(expected, result);
}

TEST(DateUtils, format_time)
{
  time_t tzero(0 + timezone);
  std::string result = DateUtils::formatTime(tzero);
  EXPECT_EQ("Thu, 01 Jan 1970 00:00:00 GMT", result);
}

TEST(DateUtils, parse_time)
{
  time_t result = DateUtils::parseTime("100");
  EXPECT_EQ(100, result);

  result = DateUtils::parseTime("-1");
  EXPECT_EQ(0, result);

  result = DateUtils::parseTime("not a number");
  EXPECT_EQ(0, result);
}
