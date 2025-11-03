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
#include <set>
#include <stdint.h>
#include "config_defs.h"
#include "string_utils.h"
#include "utf8.h"
using std::string;
using std::vector;

bool isWhitespace(unsigned int value)
{
  if (value == 0x00A0)
    return true;
  if (value > 0x0020)
    return false;
  return (
         value == 0x0020  // SPACE
      or value == 0x0009  // CHARACTER TABULATION
      or value == 0x000A  // LINE FEED
      or value == 0x000B  // LINE TABULATION
      or value == 0x000C  // FORM FEED
      or value == 0x000D  // FORM FEED
      );
}

void stripWhitespace(std::string & modify)
{
  if (modify.empty())
    return;
  static const std::string delimter(" \r\n	");
  size_t firstNonBlank = modify.find_first_not_of(delimter);
  size_t lastNonBlank = modify.find_last_not_of(delimter);
  if (firstNonBlank != std::string::npos and lastNonBlank != std::string::npos)
    modify = modify.substr(firstNonBlank, (lastNonBlank-firstNonBlank+1));
}

void tokenize(const std::string &str,
              std::vector<std::string> &tokens,
              const std::string &delimiters)
{
  // Skip delimiters at beginning.
  std::string::size_type lastPos = str.find_first_not_of(delimiters, 0);
  // Find first "non-delimiter".
  std::string::size_type pos     = str.find_first_of(delimiters, lastPos);

  while (std::string::npos != pos || std::string::npos != lastPos)
  {
    // Found a token, add it to the vector.
    tokens.push_back(str.substr(lastPos, pos - lastPos));
    // Skip delimiters.  Note the "not_of"
    lastPos = str.find_first_not_of(delimiters, pos);
    // Find next "non-delimiter"
    pos = str.find_first_of(delimiters, lastPos);
  }
}

void split(const std::string &str,
              std::vector<std::string> &tokens,
              const std::string &delimiters)
{
  // pythonic split, includes empties
  // 'this|string||is|split' -> ['this','string','','is','split']

  // Special case, delimiter at start
  std::string::size_type lastPos = 0;
  std::string::size_type pos = str.find_first_of(delimiters, lastPos);
  tokens.push_back(str.substr(lastPos, pos - lastPos));
  lastPos = pos;
  if (lastPos != std::string::npos)
    pos = str.find_first_of(delimiters, lastPos+1);

  while (lastPos != std::string::npos || pos != std::string::npos)
  {
    // skip delimeter (+1) subtract 1 for length. may add empty str
    tokens.push_back(str.substr(lastPos+1, pos - 1 - lastPos));
    lastPos = pos;
    if (lastPos != std::string::npos)
      pos = str.find_first_of(delimiters, lastPos+1);
  }
}

std::string nextWordAdvanceWord(
    std::string::const_iterator *it,
    const std::string::const_iterator &end_it)
{
  // find the end of the word
  std::string::const_iterator start(*it);
  while (*it != end_it) {
    uint32_t value = utf8::next(*it, end_it);
    if (isWhitespace(value)) {
      break;
    }
  }
  return string(start, *it);
}

size_t findLastNotOf(const std::string &str, const std::string &delim) {
  size_t last = str.find_last_not_of(delim);
  std::string::const_iterator it(str.begin());
  it += last;
  if (it == str.end()) {
    return last;
  }
  while (!utf8::is_valid(it, str.end())) {
    ++it;
    ++last;
  }
  return last;
}

void removeLastCharacter(std::string &line)
{
  std::string::iterator last(line.begin());
  std::string::iterator end(line.end());

  for (std::string::iterator it(line.begin());
      it != end; )
  {
    last = it;
    utf8::next(it, end);
  }
  line.erase(last, end);
}

void removeOneCharacter(std::string &line, int pos)
{
  if (pos < 0)
    return;
  std::string::iterator last(line.begin());
  std::string::iterator end(line.end());

  int i(0);
  std::string::iterator it(line.begin());
  for (;
      it != end; ++i)
  {
    last = it;
    utf8::next(it, end);
    if (i == pos) {
      break;
    }
  }
  line.erase(last, it);
}
