#include <stdio.h>
#include "config_defs.h"
#include "CacheControl.h"
#include "ParameterSet.h"


CacheControl::CacheControl()
: m_noCache(false),
  m_ageValue(-1),
  m_date(-1),
  m_expires(-1),
  m_maxAge(-1),
  m_lastModified(-1),
  m_requestTime(0),
  m_responseTime(0)
{}

void CacheControl::reset()
{
  if (!this) return;

  m_ageValue = -1;
  m_date = -1;
  m_expires = -1;
  m_maxAge = -1;
  m_lastModified = -1;
  m_requestTime = 0;
  m_responseTime = 0;
  m_noCache = false;
}

void CacheControl::setAge(time_t age)
{
  m_ageValue = age;
}

void CacheControl::setCacheControl(const std::string &value)
{
  if (!this)
    return;

  // split on commas
  ParameterSet paramSet(value, ',');
  std::string ma;
  if (paramSet.parameter("max-age", ma)) {
    // got
    int matches = sscanf_platform(ma.c_str(), "%d", &m_maxAge);
    if (matches != 1) {
      m_maxAge = 0;
    }
  }

  m_noCache = paramSet.hasParameter("no-cache") or paramSet.hasParameter("no-store");
}

void CacheControl::setDate(time_t date)
{
  m_date = date;
}

void CacheControl::setExpires(time_t expires)
{
  m_expires = expires;
}

void CacheControl::setRequestTime(time_t request)
{
  m_requestTime = request;
}

void CacheControl::setResponseTime(time_t response)
{
  m_responseTime = response;
}

int calculateCurrentAge(
    time_t responseTime,
    time_t date,
    time_t ageValue,
    time_t requestTime)
{
  // http://www.w3.org/Protocols/rfc2616/rfc2616-sec13.html#sec13.2
  time_t apparent_age = std::max(0L, (long)(responseTime - date));
  int corrected_received_age = std::max(apparent_age, ageValue);
  int response_delay = responseTime - requestTime;
  int corrected_initial_age = corrected_received_age + response_delay;
  return corrected_initial_age - responseTime;
}

bool CacheControl::shouldCache(time_t now) const
{
  if (m_noCache) {
    return false;
  }
  /*
  printf("arg now: %ld\n", now);
  printf("m_responseTime: %ld\n", m_responseTime);
  printf("m_date: %d\n", m_date);
  printf("m_ageValue: %d\n", m_ageValue);
  printf("m_requestTime: %ld\n", m_requestTime);
  printf("m_maxAge: %d\n", m_maxAge);
  printf("m_lastModified: %d\n", m_lastModified);
  */

  if (m_date != -1) {
    int current_age = calculateCurrentAge(m_responseTime, m_date, m_ageValue, m_requestTime) + now;
    int freshness_lifetime = -1;
    int maxAge = m_maxAge + now;
    if (m_maxAge == -1 and m_lastModified != -1) {
      maxAge = std::max(-1L, (long)(now + ((now - m_lastModified) / 8)));
    }
    if (maxAge != -1) {
      freshness_lifetime = maxAge;
    }
    else {
      if (m_expires == -1) {
        // no expires, so no way to know how long to keep for
        goto default_cache;
      }
      freshness_lifetime = m_expires - m_date;
    }
    //printf("freshness_lifetime %d vs current_age %d => %s\n",
    //  freshness_lifetime, current_age,
    //  freshness_lifetime > current_age ?"YES": "NO");
    return freshness_lifetime > current_age;
  }
  // else server didn't send the date header.

default_cache:
  // Cache for 2 minutes.
  return (m_responseTime + m_maxAge + 120) > now;
}

void CacheControl::setLastModified(time_t lastmodified)
{
  m_lastModified = lastmodified;
}
