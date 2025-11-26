#include <gtest/gtest.h>
#define private public
#include "Font.h"
#include "FontFactory.h"
#include "libnds.h"
#include "sans.h"

class FontTest: public testing::Test
{
  protected:
    Font *m_font;
    void SetUp()
    {
      m_font = FontFactory::create(reinterpret_cast<const unsigned char*>(_binary_sans_set_start),
          reinterpret_cast<const unsigned char*>(_binary_sans_map_start));
    }

    void TearDown()
    {
      delete m_font;
    }

    void TestCodePointToIndex(int expected, unsigned int codepoint) {
      int result = m_font->valueToIndex(codepoint);
      EXPECT_EQ(expected, result);
    }
};

TEST_F(FontTest, check_height_is_overall_max)
{
  int result = m_font->height();
  EXPECT_EQ(14, result);
}

TEST_F(FontTest, findEnd_no_limits)
{
  /*
    For the current font, these are the sizes of abcd:
    a 1726
    b 1787
    c 1548
    d 1787
    total = 6848
   */
  unsigned int result;
  EXPECT_TRUE(m_font->findEnd("abcd", 0, 0, &result, 0));
  EXPECT_EQ(6848U, result);
}

TEST_F(FontTest, findEnd_char_limit)
{
  /*
    For the current font, these are the sizes of that string:
    (C) = 2816
    ' ' = 895
    1 = 1792
    9 = 1792
    2 = 1792
    total = 2816 + 895 + 1792 * 4 = 10879
   */
  unsigned int result;
  EXPECT_TRUE(m_font->findEnd("© 1997", 0, 0, &result, 0));
  EXPECT_EQ(10879U, result);

  EXPECT_FALSE(m_font->findEnd("© 1997", 0, 3, &result, 0));
  EXPECT_EQ(2816 + 895 + 1792, (int)result);
}

TEST_F(FontTest, findEnd_size_limit)
{
  const char test[] = "ÙÙØØ<=>nopq<=>ØØÙÙ";
  // For the current font, this is the size of the string.
  unsigned int result;
  EXPECT_TRUE(m_font->findEnd(test, 0, 0, &result, 0));
  EXPECT_EQ(38344, (int)result);

  // Check that 90 pixels in it stops (90 << 8 == 23040)
  // The current string doesn't fit in that space
  unsigned int lastChar;
  EXPECT_FALSE(m_font->findEnd(test, 90, 0, &result, &lastChar));
  EXPECT_EQ(22713, (int)result);
  EXPECT_EQ(11U, lastChar);
}

TEST_F(FontTest, findEnd_both_limit)
{
  const char test[] = "abcd abcd abcd";
  // For the current font, these are the sizes of that string:
  unsigned int result;
  EXPECT_TRUE(m_font->findEnd(test, 0, 0, &result, 0));
  EXPECT_EQ(22334, (int)result);

  unsigned int lastChar;
  // abcd a
  // 6862 + 895 + 1679 = 9436
  EXPECT_FALSE(m_font->findEnd(test, 37, 9, &result, &lastChar));
  EXPECT_EQ(9469, (int)result);
  EXPECT_EQ(6U, lastChar);
}

TEST_F(FontTest, test_valueToIndex) {
  TestCodePointToIndex(3, ' ');
  TestCodePointToIndex(36, 'A');
  TestCodePointToIndex(128, 190);
  TestCodePointToIndex(193, 255);
  TestCodePointToIndex(260, 322);
  TestCodePointToIndex(853, 960);
  TestCodePointToIndex(2806, 8211);
  TestCodePointToIndex(3254, 8747);
  TestCodePointToIndex(3746, 9674);
  TestCodePointToIndex(5042, 64257);
  TestCodePointToIndex(5043, 64258);
  TestCodePointToIndex(-1, 1);
  TestCodePointToIndex(194, 256);
  TestCodePointToIndex(202, 264);
  TestCodePointToIndex(5044, 64259);
}

TEST_F(FontTest, nonvalid) {
  t_prerenderedGlyph *space = m_font->glyph(' ');
  t_prerenderedGlyph *result = m_font->glyph(1);
  EXPECT_TRUE(space == result);
}
