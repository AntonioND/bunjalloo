/*
  Copyright (C) 2008 Richard Quirk

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
#include "Language.h"
#include "ConfigParser.h"
#include "Config.h"

using std::string;

void ConfigParser::parseMiddle(std::string & middle)
{
  // can be of the form:
  //   <empty>
  //   Word
  //   <something>=<something>?<value>[:<value>]
  if (middle.empty())
  {
    // trivial
    return;
  }
  // look for a ?
  size_t pos = middle.find('?');
  if (pos == string::npos)
  {
    // check for cfg: value
    if (middle.find("cfg:")==0)
    {
      string resource = middle.substr(4, middle.length()-4);
      string value;
      middle.clear();
      if ( m_config.resource(resource, value) )
      {
        middle = value;
      }
    }
    return;
  }
  else
  {
    // found - now split if part from then/else part
    string condition(middle.substr(0,pos));
    ParameterSet set(condition, '|');
    const KeyValueMap & kv(set.keyValueMap());
    if (kv.empty())
    {
      return;
    }
    bool isTrue(false);
    // else see if true - can use | to separate conditions
    for (KeyValueMap::const_iterator it(kv.begin()); it != kv.end(); ++it)
    {
      string tmp;
      if (m_config.resource(it->first, tmp) and tmp == it->second)
      {
        // condition fulfilled
        isTrue = true;
        break;
      }
    }
    // now get the values - if no else exists, then set middle to ""
    string values(middle.substr(pos+1, middle.length()-pos));
    size_t sep(values.find(':'));
    string trueVal, falseVal;
    if (sep != string::npos)
    {
      trueVal = values.substr(0,sep);
      falseVal = values.substr(sep+1, values.length()-sep);
    }
    else
    {
      trueVal = values;
      falseVal.clear();
    }

    if (isTrue)
    {
      middle = trueVal;
    }
    else
    {
      middle = falseVal;
    }
  }
}

void ConfigParser::replaceMarkers(std::string & line, const char marker)
{
  // find %Foo% and replace it with its localised value, or Foo if none found.
  bool found(true);
  while (found)
  {
    found = false;
    size_t pos = line.find(marker);
    if (pos != string::npos)
    {
      // look for the end of it
      size_t endpos = line.find(marker, pos+1);
      if (endpos != string::npos)
      {
        // subs pos->endpos with the bit in the middle
        string a = line.substr(0, pos);
        string middle = line.substr(pos+1, endpos-pos-1);
        // see if the middle is simple, or if it needs computing
        parseMiddle(middle);
        // FIXME - need to escape unicodes to HTML %3C type values
        // Fix this when utf8 is working
        a += T(middle).c_str();
        a += line.substr(endpos+1, line.length()-endpos-1);
        line = a;
        found = true;
      }
    }
  }
}

ConfigParser::ConfigParser(Config & config)
  : m_config(config)
{}
