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
#include <vector>
#include "string_utils.h"
#include "ParameterSet.h"
#include "File.h"
#include "FileParser.h"

using namespace std;
void FileParser::parseFile(const char * filename)
{
  nds::File theFile;
  theFile.open(filename);
  vector<string> lines;
  if (theFile.is_open())
  {
    theFile.readlines(lines);
    theFile.close();
    for (vector<string>::iterator it(lines.begin());
        it != lines.end();
        ++it)
    {
      string & line(*it);
      stripWhitespace(line);
      if (not line.empty() and line[0] != '#')
      {
        ParameterSet set(line);
        parseLine(set);
      }
    }
  }
}

void FileParser::parseLine(const ParameterSet & set)
{
  const KeyValueMap& keyValueMap(set.keyValueMap());
  for (KeyValueMap::const_iterator it(keyValueMap.begin());
      it != keyValueMap.end();
      ++it)
  {
    callback(it->first, it->second);
  }
}
