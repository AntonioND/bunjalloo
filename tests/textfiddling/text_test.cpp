/*
  Copyright (C) 2009 Richard Quirk

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
#include <gtest/gtest.h>

TEST(TextTest, Simple)
{
  std::string s("this is a string");
  std::string::const_iterator it(s.begin());
  std::string::const_iterator end_it(s.end());

  std::string result = nextWordAdvanceWord(&it, end_it);
  EXPECT_EQ(std::string("this "), result);
  result = nextWordAdvanceWord(&it, end_it);
  EXPECT_EQ(std::string("is "), result);
  result = nextWordAdvanceWord(&it, end_it);
  EXPECT_EQ(std::string("a "), result);
  result = nextWordAdvanceWord(&it, end_it);
  EXPECT_EQ(std::string("string"), result);
}

TEST(TextTest, Unicode) {
  std::string s("thís ís à üniÇod€ strîñg");
  std::string::const_iterator it(s.begin());
  std::string::const_iterator end_it(s.end());

  std::string result = nextWordAdvanceWord(&it, end_it);
  EXPECT_EQ(std::string("thís "), result);
  result = nextWordAdvanceWord(&it, end_it);
  EXPECT_EQ(std::string("ís "), result);
  result = nextWordAdvanceWord(&it, end_it);
  EXPECT_EQ(std::string("à "), result);
  result = nextWordAdvanceWord(&it, end_it);
  EXPECT_EQ(std::string("üniÇod€ "), result);
  result = nextWordAdvanceWord(&it, end_it);
  EXPECT_EQ(std::string("strîñg"), result);
}

TEST(TextTest, Crap) {
  std::string s("\"«»\n");
  std::string::const_iterator it(s.begin());
  std::string::const_iterator end_it(s.end());
  std::string result = nextWordAdvanceWord(&it, end_it);
  EXPECT_EQ(s, result);
}

TEST(TextTest, FindLast) {
  std::string s(" ñÑaá$Ó");
  static const std::string delimeter(" \r\n\t");
  int lastPosition = findLastNotOf(s, delimeter);
  EXPECT_EQ(11, lastPosition);

  s = "Ña";
  lastPosition = findLastNotOf(s, delimeter);
  EXPECT_EQ(2, lastPosition);
}

TEST(TextTest, FindLast2) {
  std::string s(" Ñ  ");
  static const std::string delimeter(" \t");
  int lastPosition = findLastNotOf(s, delimeter);
  EXPECT_EQ(3, lastPosition);
}

TEST(TextTest, Newline) {
  std::string s("line with\nnew line");
  std::string::const_iterator it(s.begin());
  std::string::const_iterator end_it(s.end());
  std::string result = nextWordAdvanceWord(&it, end_it);
  EXPECT_EQ(std::string("line "), result);
  result = nextWordAdvanceWord(&it, end_it);
  EXPECT_EQ(std::string("with\n"), result);
  result = nextWordAdvanceWord(&it, end_it);
  EXPECT_EQ(std::string("new "), result);
  result = nextWordAdvanceWord(&it, end_it);
  EXPECT_EQ(std::string("line"), result);
}

TEST(TextTest, ReallyLongLine) {
  std::string s("[http://code.google.com/p/gdata-python-client gdata-python-client]");
  std::string::const_iterator it(s.begin());
  std::string::const_iterator end_it(s.end());
  std::string result = nextWordAdvanceWord(&it, end_it);
  EXPECT_EQ(std::string("[http://code.google.com/p/gdata-python-client "), result);
  result = nextWordAdvanceWord(&it, end_it);
  EXPECT_EQ(std::string("gdata-python-client]"), result);
}

TEST(TextTest, removes_last_character)
{
  std::string s("some utf8 texts");
  removeLastCharacter(s);
  EXPECT_EQ("some utf8 text", s);
}

TEST(TextTest, removes_last_character_unicode)
{
  std::string s("some utf8 text with ñ");
  removeLastCharacter(s);
  EXPECT_EQ("some utf8 text with ", s);
}

TEST(TextTest, removes_character_inline)
{
  std::string s("some utf8 text");
  removeOneCharacter(s, 2);
  EXPECT_EQ("soe utf8 text", s);
}

TEST(TextTest, removes_character_zeroth)
{
  std::string s("foo");
  removeOneCharacter(s, -1);
  EXPECT_EQ("foo", s);

  removeOneCharacter(s, 0);
  EXPECT_EQ("oo", s);
}

TEST(TextTest, removes_character_inline_unicode)
{
  std::string s("some ñ text");
  removeOneCharacter(s, 6);
  EXPECT_EQ("some ñtext", s);

  removeOneCharacter(s, 5);
  EXPECT_EQ("some text", s);
}

TEST(TextTest, test_stripWhitespace)
{
  std::string s("foo  \n");
  stripWhitespace(s);
  EXPECT_EQ("foo", s);
}
