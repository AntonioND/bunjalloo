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
#include <gtest/gtest.h>
#include "ParameterSet.h"

using namespace std;

TEST(ParamSetTest, Basic)
{
  string header = "a=b\r\n";
  ParameterSet paramSet(header);
  string expected = "b";
  string key = "a";
  string result;
  paramSet.parameter(key, result);
  EXPECT_TRUE(paramSet.hasParameter(key));
  EXPECT_EQ(expected, result);
}

TEST(ParamSetTest, Quote)
{
  string header = "key= 'my value'\n";
  ParameterSet paramSet(header);
  string expected = "my value";
  string key = "key";
  string result;
  paramSet.parameter(key, result);
  EXPECT_TRUE(paramSet.hasParameter(key));
  EXPECT_EQ(expected, result);

  string refresh="0; url='redirected.html'";
  ParameterSet paramSet2(refresh);
  expected = "redirected.html";
  key = "url";
  result = "";
  EXPECT_TRUE(paramSet2.hasParameter(key));
  paramSet2.parameter(key, result);
  EXPECT_EQ(expected, result);
}

TEST(ParamSetTest, DoubleQuote)
{
  string header = "key = \"another	value\"\r\n";
  ParameterSet paramSet(header);
  string expected = "another	value";
  string key = "key";
  string result;
  paramSet.parameter(key, result);
  EXPECT_TRUE(paramSet.hasParameter(key));
  EXPECT_EQ(expected, result);
}

TEST(ParamSetTest, Amphersand)
{
  string header = "proxy=234.56.78.90&proxyOn=no";
  ParameterSet paramSet(header, '&');
  string expected = "234.56.78.90";
  string key = "proxy";
  string result;
  paramSet.parameter(key, result);
  EXPECT_TRUE(paramSet.hasParameter(key));
  EXPECT_EQ(expected, result);
  key = "proxyOn";
  expected = "no";
  paramSet.parameter(key, result);
  EXPECT_TRUE(paramSet.hasParameter(key));
  EXPECT_EQ(expected, result);
}
