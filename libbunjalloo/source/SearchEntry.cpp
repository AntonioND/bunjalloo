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
#include <vector>
#include "File.h"
#include "SearchEntry.h"
#include "string_utils.h"

using namespace std;

static bool split(const string & line, string & shortcut, string & leftover)
{
  size_t gap(line.find_first_of(" "));
  if (gap == string::npos)
  {
    return false;
  }
  shortcut = line.substr(0, gap);
  if (gap+1 < line.length())
  {
    leftover = line.substr(gap+1, line.length());
    return true;
  }
  return false;
}

SearchEntry::SearchEntry(const std::string & searchFile)
{
  nds::File file;
  file.open(searchFile.c_str());
  if (file.is_open())
  {
    vector<string> lines;
    file.readlines(lines);
    for (vector<string>::iterator it(lines.begin());
        it != lines.end();
        ++it)
    {
      string & line(*it);
      stripWhitespace(line);
      if (not line.empty() and line[0] != '#')
      {
        string shortcut, leftover;
        bool isValid(split(line, shortcut, leftover));
        if (isValid)
        {
          m_searches[shortcut] = leftover;
        }
      }
    }
  }
}

bool SearchEntry::checkSearch(const std::string & userEntry, std::string & searchResult)
{
  // see if we can find %s and replace it with the user entry!
  string shortcut, leftover;
  bool isValid(split(userEntry, shortcut, leftover));
  if (not isValid)
    return false;

  KeyValueMap::const_iterator it(m_searches.find(shortcut));
  if (it == m_searches.end())
  {
    return false;
  }
  string search(it->second);
  size_t replace(search.find("%s"));
  if (replace == string::npos)
  {
    return false;
  }
  search.erase(replace,2);
  search.insert( replace, leftover.c_str());
  searchResult = search;
  return true;
}


