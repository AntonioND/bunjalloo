#include "Cookie.h"
#include "DateUtils.h"

std::string Cookie::asString() const
{
  std::string result(m_name + "=" + m_value);
  if (not m_domain.empty() and m_domain[0] == '.')
    result += ";domain="+m_domain;
  if (m_expires != -1)
    result += ";Expires=" + DateUtils::formatTime(m_expires);
  if (not m_path.empty())
    result += ";path=" + m_path;
  if (m_secure)
    result += ";secure";
  return result;
}
