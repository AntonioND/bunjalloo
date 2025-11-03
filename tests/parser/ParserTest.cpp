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
#include <vector>
#include <fstream>
#include "HeaderParser.h"
#include "HtmlParserDriver.h"

using namespace std;

class ParserTest : public testing::Test
{
  protected:
    HtmlParserDriver * m_htmlParser;
    HeaderParser * m_headerParser;
    char * m_data;
    unsigned int m_length;

    void TearDown() {
      delete m_headerParser;
      delete m_htmlParser;
      delete [] m_data;
    }

    void SetUp() {
      m_data = 0;
      m_length = 0;
      m_htmlParser = new HtmlParserDriver();
      m_headerParser = new HeaderParser(m_htmlParser, 0, 0);
    }

    void readFile(const char * fileName);
};

void ParserTest::readFile(const char * fileName) {
  ifstream testFile;
  string inputFileName("input/");
  inputFileName += fileName;
  testFile.open(inputFileName.c_str(), ios::in);
  if (testFile.is_open())
  {
    testFile.seekg(0, ios::end);
    m_length = testFile.tellg();
    m_data = new char[m_length+2];
    testFile.seekg(0, ios::beg);
    testFile.read(m_data, m_length);
    m_data[m_length] = 0;
    testFile.close();
  }
  EXPECT_TRUE(m_data != 0);
  EXPECT_TRUE(m_length != 0);
}

TEST_F(ParserTest, 0)
{
  readFile("test0.txt");
  m_headerParser->feed(m_data, m_length);
  int size = m_htmlParser->m_tags.size();
  EXPECT_EQ( 2 , size);
}

TEST_F(ParserTest, Redirect)
{
  readFile("redirect.txt");
  m_headerParser->feed(m_data, m_length);
  string result = m_headerParser->redirect();
  string expected("http://code.google.com/");
  EXPECT_EQ( expected , result);
}


TEST_F(ParserTest, NotFound)
{
  readFile("404.txt");
  m_headerParser->feed(m_data, m_length);
  unsigned int result = m_headerParser->httpStatusCode();
  unsigned int expected = 404;
  EXPECT_EQ( expected , result);
}

TEST_F(ParserTest, NotHtml)
{
  readFile("not-html.txt");
  m_headerParser->feed(m_data, m_length);
  int result = m_htmlParser->m_tags.size();
  int expected = 0;
  EXPECT_EQ( expected , result);
  result =  m_htmlParser->m_data.size();
  expected = 49;
  EXPECT_EQ( expected , result);
}

TEST_F(ParserTest, Iso)
{
  readFile("iso.txt");
  m_headerParser->feed(m_data, m_length);
  HtmlParser::Encoding result = m_htmlParser->encoding();
  HtmlParser::Encoding expected = HtmlParser::ISO_ENCODING;
  EXPECT_EQ( expected , result);
  int dataSize =  m_htmlParser->m_data.length();
  int expectedDataSize = 7;
  EXPECT_EQ( expectedDataSize , dataSize);
}

TEST_F(ParserTest, Utf8)
{
  readFile("utf8.txt");
  m_headerParser->feed(m_data, m_length);
  HtmlParser::Encoding result = m_htmlParser->encoding();
  HtmlParser::Encoding expected = HtmlParser::UTF8_ENCODING;
  EXPECT_EQ( expected , result);
  int dataSize =  m_htmlParser->m_data.length();
  int expectedDataSize = 7;
  EXPECT_EQ( expectedDataSize , dataSize);
}

TEST_F(ParserTest, Refresh)
{
  readFile("refresh.txt");
  m_headerParser->feed(m_data, m_length);
  std::string refresh;
  int refreshTime;
  m_htmlParser->refresh(refresh, refreshTime);
  std::string expected("refresh2.html");
  EXPECT_EQ( expected , refresh);
  int expectedTime = 6;
  EXPECT_EQ( expectedTime , refreshTime);
}

TEST_F(ParserTest, BogusDoctype)
{
  readFile("bogus-doctype.txt");
  m_headerParser->feed(m_data, m_length);
  int result = m_htmlParser->m_data.size();
  EXPECT_EQ( 0 , result);
}

TEST_F(ParserTest, MetaIso)
{
  readFile("metaiso.txt");
  m_headerParser->feed(m_data, m_length);
  HtmlParser::Encoding result = m_htmlParser->encoding();
  HtmlParser::Encoding expected = HtmlParser::ISO_ENCODING;
  EXPECT_EQ( expected , result);
}
TEST_F(ParserTest, Slashdot)
{
  readFile("slashdot.txt");
  m_headerParser->feed(m_data, m_length);
  int size = m_htmlParser->m_tags.size();
  EXPECT_EQ( 3 , size);
}

TEST_F(ParserTest, Numbers)
{
  readFile("numbers.txt");
  m_headerParser->feed(m_data, m_length);
  int size = m_htmlParser->m_tags.size();
  EXPECT_EQ( 1 , size);
}

TEST_F(ParserTest, Gzip)
{
  readFile("gzip.txt");
  // feed data in steps, to make sure we are initialising things correctly.
  m_headerParser->feed(m_data, m_length-20);
  m_headerParser->feed(m_data+m_length-20, 20);
  int size = m_htmlParser->m_tags.size();
  EXPECT_EQ( 4 , size);
}

TEST_F(ParserTest, Newline)
{
  readFile("newline.txt");
  // feed data in steps, to make sure we are initialising things correctly.
  m_headerParser->feed(m_data, m_length);
  int size = m_htmlParser->m_tags.size();
  EXPECT_EQ( 1 , size);
}

TEST_F(ParserTest, BogusComment)
{
  readFile("boguscomment.txt");
  // feed data in steps, to make sure we are initialising things correctly.
  m_headerParser->feed(m_data, m_length);
  int size = m_htmlParser->m_tags.size();
  EXPECT_EQ( 2 , size);
}

TEST_F(ParserTest, bogus_input_select)
{
  readFile("bogus_select.txt");
  m_headerParser->feed(m_data, m_length);
  int size = m_htmlParser->m_attributes.size();
  EXPECT_EQ(3 , size);

  const AttributeVector &inputAttrs(m_htmlParser->m_attributes[2]);
  EXPECT_EQ(2U , inputAttrs.size());

  std::string expectedInputValueUtf8("Iniciar Sesión");
  EXPECT_EQ(expectedInputValueUtf8 , inputAttrs[1]->value);
}

TEST_F(ParserTest, iso_entity_mix)
{
  // regression test for the above code - shouldn't double encode utf8
  readFile("iso_entity.txt");
  m_headerParser->feed(m_data, m_length);
  int size = m_htmlParser->m_attributes.size();
  EXPECT_EQ(1 , size);

  const std::string &text(m_htmlParser->m_data);
  EXPECT_EQ("€ óóo" , text);
}

TEST_F(ParserTest, utf_value)
{
  string data(
      "HTTP/1.1 200 OK\r\n"
      "Content-type: text/html; charset=utf-8\r\n"
      "\r\n"
      "<html><form><input value='señal'/></form>"
  );
  m_headerParser->feed(data.c_str(), data.size());
  int size = m_htmlParser->m_attributes.size();
  ASSERT_EQ(3, size);

  const AttributeVector &inputAttrs(m_htmlParser->m_attributes[2]);
  EXPECT_EQ(1U , inputAttrs.size());
  std::string expectedInputValueUtf8("señal");
  EXPECT_EQ(expectedInputValueUtf8 , inputAttrs[0]->value);
}
