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
#include "Config.h"
#include "ConfigParser.h"

using namespace std;

class ConfigParseTest : public testing::Test
{
  protected:
    void SetUp();
    void TearDown();

    Config * m_config;
    ConfigParser * m_configParser;
};

void ConfigParseTest::SetUp()
{
  m_config = new Config;
  m_config->reload();
  m_configParser = new ConfigParser(*m_config);
}

void ConfigParseTest::TearDown()
{
  delete m_config;
  delete m_configParser;
}

TEST_F(ConfigParseTest, 0)
{
  string expected("Hello");
  string line("%Hello%");
  m_configParser->replaceMarkers(line);
  EXPECT_EQ( expected , line);

  expected = "";
  line = "%%";
  m_configParser->replaceMarkers(line);
  EXPECT_EQ( expected , line);

  expected = "This is empty: end";
  line = "This is empty:%% end";
  m_configParser->replaceMarkers(line);
  EXPECT_EQ( expected , line);

  expected = "This is full";
  line = "This is %full%";
  m_configParser->replaceMarkers(line);
  EXPECT_EQ( expected , line);

  expected = "This is has a percent% sign";
  line = "This is has a percent% sign";
  m_configParser->replaceMarkers(line);
  EXPECT_EQ( expected , line);
}

TEST_F(ConfigParseTest, 1)
{
  string expected = "This has two values";
  string line = "This has %two% %values%";
  m_configParser->replaceMarkers(line);
  EXPECT_EQ( expected , line);
}


TEST_F(ConfigParseTest, 2)
{
  string expected("selected=1");
  string line("%language=nl?selected=1%");
  m_configParser->replaceMarkers(line);
  EXPECT_EQ( expected , line);

  expected = "";
  line = "%%";
  m_configParser->replaceMarkers(line);
  EXPECT_EQ( expected , line);

}

TEST_F(ConfigParseTest, CfgParse)
{
  // collect config value
  string expected("somewhere");
  string line("%cfg:download%");
  m_configParser->replaceMarkers(line);
  EXPECT_EQ( expected , line);

  // non-config values are empty
  line = "%cfg:rubbish%";
  m_configParser->replaceMarkers(line);
  EXPECT_TRUE( line.empty() );

  // empty config string
  expected = "";
  line = "%cfg:%";
  m_configParser->replaceMarkers(line);
  EXPECT_EQ( expected , line);

}
