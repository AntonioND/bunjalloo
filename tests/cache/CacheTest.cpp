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
#include <fstream>
#include "Cache.h"
#include "Document.h"
#include "URI.h"
#include "File.h"
#include <gtest/gtest.h>

using namespace std;

class CacheTest : public testing::Test
{
  protected:
    Cache * m_cache;
    Document * m_document;
    char * m_data;
    int m_length;

    virtual void SetUp();
    virtual void TearDown();
    void readFile(const char * fileName);
};

void CacheTest::readFile(const char * fileName)
{
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

void CacheTest::TearDown()
{
  delete m_cache;
  delete m_document;
  delete [] m_data;
  nds::File::rmrf("data");
}

void CacheTest::SetUp()
{
  m_data = 0;
  m_document = new Document();
  m_cache = new Cache(*m_document, true);
}

TEST_F(CacheTest, 0)
{
  bool contains(m_cache->contains(URI("http://foo.com")));
  EXPECT_TRUE(!contains);
  EXPECT_TRUE(m_cache->useCache());
}

TEST_F(CacheTest, LoadCheck)
{
  URI uri("http://www.google.com");
  bool contains(m_cache->contains(uri));
  EXPECT_FALSE(contains);
  bool result = m_cache->load(uri);
  EXPECT_FALSE(result);

  // now that it is in the cache, contains should be true
  contains = m_cache->contains(uri);
  EXPECT_TRUE(contains);

  // now navigate to a new page
  URI next = uri.navigateTo("/index.html");
  result = m_cache->load(next);
  EXPECT_TRUE(!result);

  // now that it is in the cache, contains should be true
  contains = m_cache->contains(next);
  EXPECT_TRUE(contains);

  // magically load the data from a file and then
  // check that a new load returns "true", loaded from cache
  readFile("test.txt");
  m_document->appendData(m_data, m_length);
  m_document->setStatus(Document::LOADED_HTML);
  result = m_cache->load(next);
  EXPECT_TRUE(result);

  // now navigate to an internal link
  next = uri.navigateTo("/index.html#pos1");
  // keep internal links, cache will not contain as it is a "new" link.
  contains = m_cache->contains(next, false);
  EXPECT_TRUE(!contains);
  contains = m_cache->contains(next);
  EXPECT_TRUE(contains);
  // should load from cache though, as it is the same base page.
  result = m_cache->load(next);
  EXPECT_TRUE(result);

}

TEST_F(CacheTest, Local)
{
  URI uri("10.0.0.2/~rich/cgi-bin/test.py#hello");
  bool result = m_cache->load(uri);
  EXPECT_TRUE(!result);
  string cachedFile = nds::File::base(m_cache->fileName(uri).c_str());
  URI uriNoint("10.0.0.2/~rich/cgi-bin/test.py");
  string expected = uriNoint.crc32();
  EXPECT_EQ(expected,  cachedFile);

}
