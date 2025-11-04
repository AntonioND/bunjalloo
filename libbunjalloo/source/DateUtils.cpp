#include <time.h>

#include <string>
#include <algorithm>
#include "DateUtils.h"
#include "config_defs.h"
#include <cstring>

// picolibc doesn't implement %Z
static const char FORMAT[] = "%a, %d %b %Y %H:%M:%S GMT";

static char tospace(char c)
{
  if (c == '-')
    return ' ';
  return c;
}

time_t DateUtils::parseDate(const char *date)
{
  std::string spaces(date);
  transform(spaces.begin(), spaces.end(), spaces.begin(), tospace);

  struct tm val = { 0 };
  if (strptime(spaces.c_str(), FORMAT, &val) == 0)
    return 0;

  val.tm_isdst = 0;
  return timegm(&val);
}

std::string DateUtils::formatTime(time_t t)
{
  char buffer[140];
  memset(buffer, 0, sizeof(buffer));
  struct tm *val = gmtime(&t);
  strftime(buffer, sizeof(buffer), FORMAT, val);
  return buffer;
}

time_t DateUtils::parseTime(const char *time)
{
  time_t result = strtol(time, 0, 0);
  if (result < 0)
    return 0;
  return result;
}
