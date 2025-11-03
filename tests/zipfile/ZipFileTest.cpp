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
#include <unistd.h>
#include <gtest/gtest.h>
#include "File.h"
#include "ZipFile.h"

using namespace std;

class ZipFileTest : public testing::Test
{
  protected:
    void SetUp();
    void TearDown();

    ZipFile * m_zipfile;
    bool m_hasTestDir;
    void cdTestDir();
};

void ZipFileTest::SetUp()
{
  m_zipfile = new ZipFile(0);
  m_hasTestDir = false;
}
void ZipFileTest::cdTestDir()
{
  nds::File::mkdir("test");
  chdir("test");
  m_hasTestDir = true;
}
void ZipFileTest::TearDown()
{
  if (m_hasTestDir)
  {
    chdir("..");
    nds::File::rmrf("test");
  }
  delete m_zipfile;
}

static std::string readFile(const char * filename)
{
  nds::File f;
  f.open(filename);
  if (f.is_open())
  {
    int size = f.size();
    char * data = new char[size+1];
    f.read(data);
    data[size] = 0;
    string s(data);
    delete [] data;
    return s;
  }
  return "";
}

TEST_F(ZipFileTest, 0)
{
  string filename("data/test0.zip");
  m_zipfile->open(filename.c_str());
  //EXPECT_EQ( expected , line);
  EXPECT_TRUE( m_zipfile->is_open());
  vector<string> files;
  m_zipfile->list(files);
  size_t expected(2);
  EXPECT_EQ(expected, files.size());

  string expected1("file1.txt");
  EXPECT_EQ(expected1, files[0]);
  string expected2("file2.txt");
  EXPECT_EQ(expected2, files[1]);

  cdTestDir();
  // now unzip and check the files exists
  m_zipfile->extract();
  nds::File::FileType expectedType(nds::File::F_REG);
  EXPECT_EQ(expectedType, nds::File::exists("file1.txt"));
  EXPECT_EQ(expectedType, nds::File::exists("file2.txt"));
  expectedType = nds::File::F_NONE;
  EXPECT_EQ(expectedType, nds::File::exists("test0.zip"));

  string contents = readFile("file1.txt");
  string expectedContents("Wed Mar 19 19:54:44 CET 2008\n");
  EXPECT_EQ(expectedContents, contents);

  contents = readFile("file2.txt");
  expectedContents = "Linux rich-laptop 2.6.22-14-generic #1 SMP Tue Feb 12 07:42:25 UTC 2008 i686 GNU/Linux\n";
  EXPECT_EQ(expectedContents, contents);
}

TEST_F(ZipFileTest, 1)
{
  string filename("/data/test0.zip");
  m_zipfile->open(filename.c_str());
  //EXPECT_EQ( expected , line);
  EXPECT_TRUE( m_zipfile->is_open());

  cdTestDir();
  // now unzip and check the files exists
  m_zipfile->extract();
  nds::File::FileType expectedType(nds::File::F_REG);
  EXPECT_EQ(expectedType, nds::File::exists("file1.txt"));
  EXPECT_EQ(expectedType, nds::File::exists("file2.txt"));
}

class TestExtractListener: public ExtractListener
{
  public:
    TestExtractListener(const std::string & onlyExtract):
      m_onlyExtract(onlyExtract) { }
    virtual bool extract(const char * name)
    {
      return (name == m_onlyExtract);
    }
    virtual void before(const char * name) { }
    virtual void after(const char * name) { }
    std::string m_onlyExtract;

};


TEST_F(ZipFileTest, 2)
{
  string filename("/data/test2.zip");
  TestExtractListener extractListener("output/somefile.txt");
  m_zipfile->setListener(&extractListener);
  m_zipfile->open(filename.c_str());
  //EXPECT_EQ( expected , line);
  EXPECT_TRUE( m_zipfile->is_open());

  cdTestDir();
  // now unzip and check the files exists
  m_zipfile->extract();
  nds::File::FileType expectedType(nds::File::F_REG);
  EXPECT_EQ(expectedType, nds::File::exists("output/somefile.txt"));
  expectedType = nds::File::F_NONE;
  EXPECT_EQ(expectedType, nds::File::exists("output/anotherfile.txt"));
}

