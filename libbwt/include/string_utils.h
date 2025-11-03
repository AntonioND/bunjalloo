/*
  Copyright (C) 2007-2009 Richard Quirk

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
#ifndef string_utils_h_seen
#define string_utils_h_seen
#include <string>
#include <vector>

/** Check if the value is white space.
 * @param value the character value to check.
 * @return true if @a value is whitespace, false otherwise.
 */
bool isWhitespace(unsigned int value);

/** Strip the whitespace from the front and back of the given string.
 * @param modify the string that is stripped in place.
 */
void stripWhitespace(std::string & modify);

void tokenize(const std::string &str,
              std::vector<std::string> &tokens,
              const std::string &delimiters);

void split(const std::string &str,
              std::vector<std::string> &tokens,
              const std::string &delimiters);

std::string nextWordAdvanceWord(
    std::string::const_iterator *it,
    const std::string::const_iterator &end_it);

size_t findLastNotOf(const std::string &str, const std::string &delim);

/** Remove the last character from the string */
void removeLastCharacter(std::string &line);

/** Remove the character at the given position from the string */
void removeOneCharacter(std::string &line, int pos);
#endif
