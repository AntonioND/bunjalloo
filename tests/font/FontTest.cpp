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
      m_font = FontFactory::create(reinterpret_cast<const unsigned char*>(_binary_sans_set_bin_start),
          reinterpret_cast<const unsigned char*>(_binary_sans_map_bin_start));
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
  EXPECT_EQ(12, result);
}

TEST_F(FontTest, findEnd_no_limits)
{
  /*
    For 11 point serif, these are the sizes of abcd:
    a 1679
    b 1803
    c 1577
    d 1803
    total = 6862
   */
  unsigned int result;
  EXPECT_TRUE(m_font->findEnd("abcd", 0, 0, &result, 0));
  EXPECT_EQ(6862U, result);
}

TEST_F(FontTest, findEnd_char_limit)
{
  /*
    For 11 point serif, these are the sizes of that string:
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
  /*
    For 11 point serif, these are the sizes of that string:
      < 2360
      = 2360
      > 2360
      n 1814
      o 1695
      p 1803
      q 1803
      Ø 2309
      Ù 2373

      2373*2 + 2309*2 + 2360*2 + 1814 + 1695 + 1803 + 1803 + 2360*3 + 2309*2 + 2373*2
      = 39997 (= 156 pixels, more or less)
   */
  unsigned int result;
  EXPECT_TRUE(m_font->findEnd(test, 0, 0, &result, 0));
  EXPECT_EQ(40003, (int)result);

  // check that 90 pixels in it stops
  // 90 << 8 == 23040
  // 2373*2 + 2309*2 + 2360*3 + 1814 + 1695 + 1803 = 21756, add the q and it goes over
  unsigned int lastChar;
  EXPECT_FALSE(m_font->findEnd(test, 90, 0, &result, &lastChar));
  EXPECT_EQ(21756, (int)result);
  EXPECT_EQ(10U, lastChar);
}

TEST_F(FontTest, findEnd_both_limit)
{
  const char test[] = "abcd abcd abcd";
  /*
    For 11 point serif, these are the sizes of that string:
    ' ' = 895
    a 1679
    b 1803
    c 1577
    d 1803

    total = 6862 * 3 + 895 * 2
    = 22376 = 87
   */
  unsigned int result;
  EXPECT_TRUE(m_font->findEnd(test, 0, 0, &result, 0));
  EXPECT_EQ(6862 * 3 + 895 * 2, (int)result);

  unsigned int lastChar;
  // abcd a
  // 6862 + 895 + 1679 = 9436
  EXPECT_FALSE(m_font->findEnd(test, 37, 9, &result, &lastChar));
  EXPECT_EQ(9436, (int)result);
  EXPECT_EQ(6U, lastChar);
}

TEST_F(FontTest, test_valueToIndex) {
  TestCodePointToIndex(3, ' ');
  TestCodePointToIndex(36, 'A');
  TestCodePointToIndex(245, 190);
  TestCodePointToIndex(186, 255);
  TestCodePointToIndex(226, 322);
  TestCodePointToIndex(155, 960);
  TestCodePointToIndex(178, 8211);
  TestCodePointToIndex(156, 8747);
  TestCodePointToIndex(185, 9674);
  TestCodePointToIndex(192, 64257);
  TestCodePointToIndex(193, 64258);
  TestCodePointToIndex(-1, 1);
  TestCodePointToIndex(-1, 256);
  TestCodePointToIndex(-1, 264);
  TestCodePointToIndex(-1, 64259);
}

TEST_F(FontTest, nonvalid) {
  t_prerenderedGlyph *space = m_font->glyph(' ');
  t_prerenderedGlyph *result = m_font->glyph(1);
  EXPECT_TRUE(space == result);
}
