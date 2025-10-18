#include <string>
#include <algorithm>
#include "DateUtils.h"
#include "config_defs.h"
#include <cstring>
//#include <time.h>
#ifdef ARM9
// time.h doesn't have timezone on the DS.
static int timezone = 0;
#endif

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

  struct tm val;
  memset(&val, 0, sizeof(val));
  if (0) { //strptime(spaces.c_str(), FORMAT, &val) == 0) {
    return 0;
  }
  val.tm_isdst = 0;
  timezone = 0;
  return mktime(&val);
}

std::string DateUtils::formatTime(time_t t)
{
  char buffer[140];
  memset(buffer, 0, sizeof(buffer));
  t -= timezone;
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
