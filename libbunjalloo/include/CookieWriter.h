#ifndef CookieWriter_h_seen
#define CookieWriter_h_seen

#include <time.h>

class Cookie;

class CookieWriter
{
  public:
    CookieWriter(time_t now);

    void operator()(Cookie *cookie);

    void remove(const char *domain);
  private:
    time_t m_now;
};
#endif
