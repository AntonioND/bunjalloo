/*
  Copyright (C) 2007,2008 Richard Quirk

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "string_utils.h"
#include "ParameterSet.h"

using namespace std;

class PSImpl
{
  friend class ParameterSet;
  public:
    PSImpl(const string & kvs, char sep):
      m_state(BEFORE_NAME),
      m_position(kvs.begin()),
      m_end(kvs.end()),
      m_sep(sep)
    {
      if (m_position != m_end) {
        next();
        doParse();
      }
    }

  private:
    enum ParseState {
      BEFORE_NAME,
      IN_NAME,
      AFTER_NAME,
      BEFORE_VALUE,
      VALUE_DOUBLE_QUOTE,
      VALUE_SINGLE_QUOTE,
      VALUE_UNQUOTED
    };
    ParseState m_state;
    string::const_iterator m_position;
    string::const_iterator m_end;
    char m_sep;
    char m_value;
    KeyValueMap m_keyValueMap;
    string m_paramName;
    string m_paramValue;

    inline void next()
    {
      m_value = *m_position++;
    }
    void doParse();
    void cleanup();
    void addCurrentParam();

    void beforeName();
    void inName();
    void afterName();
    void beforeValue();
    void inValue();
    void handleState();

};

void PSImpl::beforeName()
{
  if (m_value != m_sep and not isWhitespace(m_value))
  {
    m_paramName += m_value;
    m_state = IN_NAME;
  }
}
void PSImpl::inName()
{
  if (not isWhitespace(m_value) and m_value != '=' and m_value != m_sep)
  {
    m_paramName += m_value;
  }
  else if (m_value == '=')
  {
    m_state = BEFORE_VALUE;
  }
  else if (m_value == m_sep)
  {
    addCurrentParam();
  }
  else
  {
    m_state = AFTER_NAME;
  }
}

void PSImpl::afterName()
{
  if (m_value == '=')
  {
    m_state = BEFORE_VALUE;
  }
  else if (m_value == m_sep)
  {
    addCurrentParam();
  }
}

void PSImpl::beforeValue()
{
  if (not isWhitespace(m_value))
  {
    if (m_value == '"')
    {
      m_state = VALUE_DOUBLE_QUOTE;
    }
    else if (m_value == '\'')
    {
      m_state = VALUE_SINGLE_QUOTE;
    }
    else
    {
      m_paramValue += m_value;
      m_state = VALUE_UNQUOTED;
    }
  }
}

void PSImpl::addCurrentParam()
{
  if (not m_paramName.empty()) {
    m_keyValueMap[m_paramName] = m_paramValue;
  }
  m_paramName.clear();
  m_paramValue.clear();
  m_state = BEFORE_NAME;
}

void PSImpl::inValue()
{
  if (m_value == m_sep)
  {
    addCurrentParam();
  }
  else if (not isWhitespace(m_value))
  {
    // TODO - check escaped quotes
    char quote = m_state==VALUE_DOUBLE_QUOTE?'"':(m_state==VALUE_SINGLE_QUOTE?'\'':0);
    if (m_value == quote)
    {
      addCurrentParam();
    }
    else
    {
      m_paramValue += m_value;
      m_value = 0;
    }
  }
  else
  {
    // whitespace
    /*if (m_state == VALUE_UNQUOTED)
    {
      addCurrentParam();
    }
    else
    {
    */
      m_paramValue += m_value;
      m_value = 0;
    //}
  }
}

void PSImpl::doParse()
{
  for (;m_position != m_end; next())
  {
    if (m_value == '\r' or m_value == '\n')
    {
      m_value = 0;
      break;
    }
    handleState();
  }
  cleanup();
}

void PSImpl::handleState()
{
  switch (m_state)
  {
    case BEFORE_NAME:
      beforeName();
      break;
    case IN_NAME:
      inName();
      break;
    case AFTER_NAME:
      afterName();
      break;
    case BEFORE_VALUE:
      beforeValue();
      break;
    case VALUE_DOUBLE_QUOTE:
    case VALUE_SINGLE_QUOTE:
    case VALUE_UNQUOTED:
      inValue();
      break;
  }
}

void PSImpl::cleanup()
{
  if ( not m_paramName.empty() ) {
    if (m_value != 0)
    {
      handleState();
    }
    addCurrentParam();
  }
}

ParameterSet::ParameterSet(const std::string & keyValueString, char sep):
  m_psImpl(new PSImpl(keyValueString, sep))
{
}

ParameterSet::~ParameterSet()
{
  delete m_psImpl;
}

bool ParameterSet::hasParameter(const std::string & key) const
{
  KeyValueMap::const_iterator it(m_psImpl->m_keyValueMap.find(key));
  KeyValueMap::const_iterator end(m_psImpl->m_keyValueMap.end());
  return (it != end);
}

bool ParameterSet::parameter(const std::string & key, std::string & value) const
{
  KeyValueMap::const_iterator it(m_psImpl->m_keyValueMap.find(key));
  KeyValueMap::const_iterator end(m_psImpl->m_keyValueMap.end());
  if (it != end)
  {
    value = it->second;
  }
  return (it != end);
}

const KeyValueMap & ParameterSet::keyValueMap() const
{
  return m_psImpl->m_keyValueMap;
}
