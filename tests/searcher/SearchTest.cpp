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
#include "SearchEntry.h"
#include <gtest/gtest.h>

using namespace std;

class SearchTest : public testing::Test
{
  protected:
    SearchEntry * m_search;

    void TearDown() {
      delete m_search;
    }

    void SetUp() {
      m_search = new SearchEntry("input/search.cfg");
    }
};

TEST_F(SearchTest, 0)
{
  string result;
  bool check(m_search->checkSearch("g bunjalloo", result));
  EXPECT_TRUE(check);
  string expected = "http://www.google.com/search?q=bunjalloo&btnG=Google+Search";
  EXPECT_EQ(expected, result);
}

TEST_F(SearchTest, Invalid)
{
  string result;
  bool valid(m_search->checkSearch("x bunjalloo", result));
  // should not be valid
  EXPECT_TRUE(not valid);
}

TEST_F(SearchTest, Percentage)
{
  // tests for %s at the end of a line and with a %20 in the url
  string result;
  bool valid(m_search->checkSearch("s bunjalloo", result));
  EXPECT_TRUE(valid);
  string expected = "http://checkpercentage/%20/bunjalloo";
  EXPECT_EQ(expected, result);
}
