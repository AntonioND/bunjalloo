#include <cstdlib>
#include <string>
#include <vector>
#include "Version.h"
#include "string_utils.h"

Version::Version(const char * version):
  m_major(0),
  m_minor(0),
  m_point(0)
{
  // split out the version 
  using std::string;
  using std::vector;
  string v(version);
  vector<string> tokens;
  tokenize(v, tokens, string("."));
  if (tokens.size() > 0)
  {
    m_major = strtol(tokens[0].c_str(), 0, 10);
    if (tokens.size() > 1)
    {
      m_minor = strtol(tokens[1].c_str(), 0, 10);
      if (tokens.size() > 2)
      {
        m_point = strtol(tokens[2].c_str(), 0, 10);
      }
    }
  }
}

bool Version::operator >(const Version & other) const
{
  if (m_major < other.m_major)
    return false;
  else if (m_major > other.m_major)
    return true;
  else
  {
    // same major
    if (m_minor < other.m_minor)
      return false;
    else if (m_minor > other.m_minor)
      return true;
    else
    {
      if (m_point < other.m_point)
        return false;
      else if (m_point > other.m_point)
        return true;
    }
  }
  // same!
  return false;
}

bool Version::operator <(const Version & other) const
{
  if (other == *this)
  {
    return false;
  }
  else
  {
    return other > *this;
  }
}

bool Version::operator ==(const Version & other) const
{
  if (&other == this)
  {
    return true;
  }
  return m_point == other.m_point
    and m_minor == other.m_minor
    and m_major == other.m_major;
}

bool Version::operator !=(const Version & other) const
{
  return not ((*this) == other);
}
