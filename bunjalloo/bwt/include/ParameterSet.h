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
#ifndef ParameterSet_h_seen
#define ParameterSet_h_seen

#include <map>
#include <string>
#include "util/classhelper.h"

typedef std::map<std::string, std::string> KeyValueMap;

// Private class
class PSImpl
{
  friend class ParameterSet;
  public:
    PSImpl(const std::string & kvs, char sep):
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
    std::string::const_iterator m_position;
    std::string::const_iterator m_end;
    char m_sep;
    char m_value;
    KeyValueMap m_keyValueMap;
    std::string m_paramName;
    std::string m_paramValue;

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

class ParameterSet
{
  public:

    /** Create a parameter set based on a semi-colon (;) seperated list of key, value pairs.
     * @param keyValueString the list of key, value pairs seperated by @a sep
     * @param sep the seperator for the @a keyValueString argument. ';' is the default seperator.
     */
    ParameterSet(const std::string & keyValueString, char sep=';');

    ~ParameterSet();

    /** Check if this set has the requested parameter.
     * @param key the name of the parameter to check.
     * @return true if the parameter is in the set.
     */
    bool hasParameter(const std::string & key) const;
    /** Fetch a parameter value. Convenience function to save having to retrieve and traverse the map each time.
     * @param key the parameter name.
     * @param value the value is returned in this para.
     * @return true if the parameter is set, false otherwise.
     */
    bool parameter(const std::string & key, std::string & value) const;
    /** Fetch a map of parameter - value pairs.
     * @return a standard map of key,value pairs. The keys are the parameter names.
     */
    const KeyValueMap & keyValueMap() const;

  private:
    PSImpl m_psImpl;

    DISALLOW_COPY_AND_ASSIGN(ParameterSet);
};

#endif
