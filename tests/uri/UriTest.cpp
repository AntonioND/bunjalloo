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
#include "URI.h"

using namespace std;

TEST(UriTest, SimpleHttp)
{
  URI uri("http://test");
  string expected("test");
  EXPECT_TRUE( uri.isValid());
  EXPECT_EQ( URI::HTTP_PROTOCOL, uri.protocol());
  EXPECT_EQ( expected, uri.server());
  expected = "/";
  EXPECT_EQ( expected, uri.fileName());
}


TEST(UriTest, Broken)
{
  URI uri("htp:test", false);
  EXPECT_TRUE( not uri.isValid());
}

TEST(UriTest, Port)
{
  URI uri("http://server/file");
  int expectedPort = 80;
  string expectedServer("server");
  string expectedFile("/file");
  EXPECT_EQ( expectedServer, uri.server());
  EXPECT_EQ( expectedPort, uri.port());
  EXPECT_EQ( expectedFile, uri.fileName());

  uri.setUri("http://server:1500");
  expectedPort = 1500;
  expectedFile = "/";
  EXPECT_EQ( expectedServer, uri.server());
  EXPECT_EQ( expectedPort, uri.port());
  EXPECT_EQ( expectedFile, uri.fileName());

  uri.setUri("http://server:90/wiki/Documents:Main");
  expectedPort = 90;
  expectedFile = "/wiki/Documents:Main";
  EXPECT_EQ( expectedServer, uri.server());
  EXPECT_EQ( expectedPort, uri.port());
  EXPECT_EQ( expectedFile, uri.fileName());

  uri.setUri("http://server:");
  expectedPort = 80;
  expectedFile = "/";
  EXPECT_EQ( expectedServer, uri.server());
  EXPECT_EQ( expectedPort, uri.port());
  EXPECT_EQ( expectedFile, uri.fileName());

  uri.setUri("http://server/wiki/Documents:Main");
  expectedPort = 80;
  expectedFile = "/wiki/Documents:Main";
  EXPECT_EQ( expectedServer, uri.server());
  EXPECT_EQ( expectedPort, uri.port());
  EXPECT_EQ( expectedFile, uri.fileName());

}

TEST(UriTest, Navigate)
{
  URI uri("http://server/file");
  string expectedServer("server");
  string expectedFile("/file");
  EXPECT_EQ( expectedServer, uri.server());
  EXPECT_EQ( expectedFile, uri.fileName());

  uri = uri.navigateTo("newFile");
  expectedFile = "/newFile";
  EXPECT_EQ( expectedServer, uri.server());
  EXPECT_EQ( expectedFile, uri.fileName());

  uri = uri.navigateTo("newPath/newfile.html");
  expectedFile = "/newPath/newfile.html";
  EXPECT_EQ( expectedServer, uri.server());
  EXPECT_EQ( expectedFile, uri.fileName());

  uri = uri.navigateTo("index.html");
  expectedFile = "/newPath/index.html";
  EXPECT_EQ( expectedServer, uri.server());
  EXPECT_EQ( expectedFile, uri.fileName());

  uri = uri.navigateTo("/index.html");
  expectedFile = "/index.html";
  EXPECT_EQ( expectedServer, uri.server());
  EXPECT_EQ( expectedFile, uri.fileName());

  uri = uri.navigateTo("/directory/");
  expectedFile = "/directory/";
  expectedServer = "server";
  EXPECT_EQ( expectedServer, uri.server());
  EXPECT_EQ( expectedFile, uri.fileName());
  EXPECT_EQ( expectedFile, uri.fileName());
}

TEST(UriTest, ToString)
{
  URI uri("http://server/file");
  string expectedServer("server");
  string expectedFile("/file");
  EXPECT_EQ( expectedServer, uri.server());
  EXPECT_EQ( expectedFile, uri.fileName());
  uri = uri.navigateTo("newPath/newfile.html");
  expectedFile = "/newPath/newfile.html";
  EXPECT_EQ( expectedServer, uri.server());
  EXPECT_EQ( expectedFile, uri.fileName());
  string expectedURI = "http://server/newPath/newfile.html";
  EXPECT_EQ( expectedURI, uri.asString());

}

TEST(UriTest, NavigateFile)
{
  URI uri("file:///path/file");

  string expectedFile = "/path/file";
  EXPECT_EQ( URI::FILE_PROTOCOL, uri.protocol());
  EXPECT_EQ( expectedFile, uri.fileName());

  uri = uri.navigateTo("newPath/newfile.html");
  expectedFile = "/path/newPath/newfile.html";
  EXPECT_EQ( expectedFile, uri.fileName());

  uri = uri.navigateTo("/root/index.txt");
  expectedFile = "/root/index.txt";
  EXPECT_EQ( expectedFile, uri.fileName());

}

TEST(UriTest, NavigateDots)
{
  URI uri("http://server/file");
  string expectedServer("server");
  string expectedFile("/file");
  EXPECT_EQ( expectedServer, uri.server());
  EXPECT_EQ( expectedFile, uri.fileName());

  uri = uri.navigateTo("../newfile.html");
  expectedFile = "/newfile.html";
  expectedServer = "server";
  EXPECT_EQ( expectedServer, uri.server());
  EXPECT_EQ( expectedFile, uri.fileName());
  string expectedURI = "http://server/newfile.html";
  EXPECT_EQ( expectedURI, uri.asString());

  // make sure we can't go higher than root dir
  uri = uri.navigateTo("../newDir/index.html");
  expectedFile = "/newDir/index.html";
  expectedServer = "server";
  EXPECT_EQ( expectedServer, uri.server());
  EXPECT_EQ( expectedFile, uri.fileName());
  expectedURI = "http://server/newDir/index.html";
  EXPECT_EQ( expectedURI, uri.asString());

  // dots and absolute == rubbish
  uri = uri.navigateTo("/../another/file.html");
  expectedFile = "/another/file.html";
  expectedServer = "server";
  expectedURI = "http://server/another/file.html";
  EXPECT_EQ( expectedServer, uri.server());
  EXPECT_EQ( expectedFile, uri.fileName());
  EXPECT_EQ( expectedURI, uri.asString());

  // and dots later on..
  uri = uri.navigateTo("/another/directory/sub/../file.html");
  expectedFile = "/another/directory/file.html";
  expectedServer = "server";
  expectedURI = "http://server/another/directory/file.html";
  EXPECT_EQ( expectedServer, uri.server());
  EXPECT_EQ( expectedFile, uri.fileName());
  EXPECT_EQ( expectedURI, uri.asString());


}


TEST(UriTest, HttpFromFile)
{
  URI uri("file:///some/file.txt");

  string expectedFile = "/some/file.txt";
  EXPECT_EQ( URI::FILE_PROTOCOL, uri.protocol());
  EXPECT_EQ( expectedFile, uri.fileName());

  uri = uri.navigateTo("http://myserver.com/newfile.html");
  expectedFile = "/newfile.html";
  string expectedServer = "myserver.com";
  EXPECT_EQ( URI::HTTP_PROTOCOL, uri.protocol());
  EXPECT_EQ( expectedFile, uri.fileName());
  EXPECT_EQ( expectedServer, uri.server());

}

TEST(UriTest, SecurityFile)
{
  // should not be able to link to local URIs from http.
  URI uri("http://someserver.com/file.html");
  EXPECT_EQ( URI::HTTP_PROTOCOL, uri.protocol());
  uri = uri.navigateTo("file:///my/secret/file.txt");
  EXPECT_EQ( URI::HTTP_PROTOCOL, uri.protocol());
  uri = uri.navigateTo("config://options.html?proxy=123");
  EXPECT_EQ( URI::HTTP_PROTOCOL, uri.protocol());
  EXPECT_EQ( URI::HTTP_PROTOCOL, uri.protocol());
}

TEST(UriTest, Port2)
{
  URI uri("123.56.78.90:8080");
  int expectedPort = 8080;
  string expectedServer = "123.56.78.90";
  EXPECT_EQ( expectedServer, uri.server());
  EXPECT_EQ( expectedPort, uri.port());

  // try breaking things
  URI uri2("123.56.78.90:");
  expectedPort = 80;
  expectedServer = "123.56.78.90";
  EXPECT_EQ( expectedServer, uri2.server());
  EXPECT_EQ( expectedPort, uri2.port());

  // try breaking things
  URI uri3("123.56.78.90/wherevr");
  expectedPort = 80;
  expectedServer = "123.56.78.90";
  EXPECT_EQ( expectedServer, uri3.server());
  EXPECT_EQ( expectedPort, uri3.port());
  URI breakage(uri3.navigateTo("/search?q=label:c"));
  string expectedFile("/search?q=label:c");
  EXPECT_EQ( expectedServer, breakage.server());

}

TEST(UriTest, Spaces)
{
  URI uri("http://server/some file with spaces.jpeg");
  string expectedFile = "/some%20file%20with%20spaces.jpeg";
  EXPECT_EQ( URI::HTTP_PROTOCOL, uri.protocol());
  EXPECT_EQ( expectedFile, uri.fileName());
}

TEST(UriTest, Https)
{
  URI uri("https://server/filename.html");
  string expectedFile = "/filename.html";
  EXPECT_EQ( URI::HTTPS_PROTOCOL, uri.protocol());
  EXPECT_EQ( expectedFile, uri.fileName());
}

TEST(UriTest, Internal)
{
  URI uri("http://server/filename.html");
  string expectedFile = "/filename.html";
  EXPECT_EQ( expectedFile, uri.fileName());
  const URI & internal(uri.navigateTo("#section1"));
  string expectedInternal("section1");
  EXPECT_EQ( expectedFile, internal.fileName());
  EXPECT_EQ( expectedInternal, internal.internalLink());
  const URI & internal2(internal.navigateTo("#section2"));
  expectedInternal = "section2";
  EXPECT_EQ( expectedInternal, internal2.internalLink());

}

TEST(UriTest, CRC)
{
  URI uri("http://server.com/");
  unsigned int crc = uri.crc32int();
  unsigned int expected(0x7fec9bc0);
  EXPECT_EQ(expected, crc);
}

TEST(UriTest, Escape)
{
  string result = URI::escape("/blah&#?=:@");
  string expected("%2Fblah%26%23%3F%3D%3A%40");
  EXPECT_EQ(expected, result);
  result = URI::unescape("%2Fblah%26%23%3F%3D%3A%40");
  expected = "/blah&#?=:@";
  EXPECT_EQ(expected, result);

  result = URI::unescape("%2Fblah%2%2");
  expected = "/blah%2%2";
  EXPECT_EQ(expected, result);

}

TEST(UriTest, TwoDots)
{
  URI uri("http://tbn0.example.com/images?q=tbn:xFaRtM:http://www.example.net/es/blabla/FOO.jpg");
  string expected("/images?q=tbn:xFaRtM:http://www.example.net/es/blabla/FOO.jpg");
  EXPECT_EQ(expected, uri.fileName());
  uri = uri.navigateTo(uri.fileName());
  EXPECT_EQ(expected, uri.fileName());

}

TEST(UriTest, Port3)
{
  URI uri("http://www.example.com:1234/index");
  uri = uri.navigateTo("/index2");
  int expected = 1234;
  EXPECT_EQ(expected, uri.port());
}

TEST(UriTest, relative_query)
{
  URI uri("http://www.example.com/test.php");
  uri = uri.navigateTo("?a");
  string expected("http://www.example.com/test.php?a");
  EXPECT_EQ(expected, uri.asString());
}

TEST(UriTest, relativeQueryInternal)
{
  URI uri("http://www.example.com/test.php#a");
  uri = uri.navigateTo("?c");
  string expected("http://www.example.com/test.php?c");
  EXPECT_EQ(expected, uri.asString());
}

TEST(UriTest, relative_item)
{
  URI uri("http://www.example.com/");
  uri = uri.navigateTo("item?id=123");
  string expected("http://www.example.com/item?id=123");
  EXPECT_EQ(expected, uri.asString());
}
