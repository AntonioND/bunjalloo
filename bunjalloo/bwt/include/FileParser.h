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
#ifndef FileParser_h_seen
#define FileParser_h_seen
#include <string>
class ParameterSet;

class FileParser
{
  public:
    virtual ~FileParser() {}
    virtual void callback(const std::string & key, const std::string & value) = 0;
    void parseFile(const char * filename);

  private:
    void parseLine(const ParameterSet & set);
};
#endif
