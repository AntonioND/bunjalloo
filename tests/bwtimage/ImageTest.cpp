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
#include "Image.h"
#include <gtest/gtest.h>

using namespace std;
using nds::Image;

class ImageTest : public testing::Test
{
  protected:
    Image * m_image;

    void SetUp() {
      m_image = 0;
    }

    void TearDown() {
      delete m_image;
    }
};

// ----------------------------------------------

TEST_F(ImageTest, SmallJpg)
{
  m_image = new Image("inputs/small.jpg");
  EXPECT_TRUE(m_image->isValid());
  int expected = 200;
  int result = m_image->width();
  EXPECT_EQ(expected, result);
  expected = 170;
  result = m_image->height();
  EXPECT_EQ(expected, result);
}

TEST_F(ImageTest, SmallPng)
{
  m_image = new Image("inputs/small.png");
  EXPECT_TRUE(m_image->isValid());
  int expected = 200;
  int result = m_image->width();
  EXPECT_EQ(expected, result);
  expected = 170;
  result = m_image->height();
  EXPECT_EQ(expected, result);
}

TEST_F(ImageTest, SmallGif)
{
  m_image = new Image("inputs/small.gif");
  EXPECT_TRUE(m_image->isValid());
  int expected = 200;
  int result = m_image->width();
  EXPECT_EQ(expected, result);
  expected = 170;
  result = m_image->height();
  EXPECT_EQ(expected, result);
}

TEST_F(ImageTest, SmallBmp)
{
  m_image = new Image("inputs/small.bmp");
  EXPECT_TRUE(m_image->isValid());
  int expected = 200;
  int result = m_image->width();
  EXPECT_EQ(expected, result);
  expected = 170;
  result = m_image->height();
  EXPECT_EQ(expected, result);
}

// ----------------------------------------------

TEST_F(ImageTest, WideJpg)
{
  m_image = new Image("inputs/wide.jpg");
  EXPECT_TRUE(m_image->isValid());
  int expected = 249;
  int result = m_image->width();
  EXPECT_EQ(expected, result);
}

TEST_F(ImageTest, WidePng)
{
  m_image = new Image("inputs/wide.png");
  EXPECT_TRUE(m_image->isValid());
  int expected = 249;
  int result = m_image->width();
  EXPECT_EQ(expected, result);
}

TEST_F(ImageTest, WideGif)
{
  m_image = new Image("inputs/wide.gif");
  EXPECT_TRUE(m_image->isValid());
  int expected = 249;
  int result = m_image->width();
  EXPECT_EQ(expected, result);
}

TEST_F(ImageTest, WideBmp)
{
  m_image = new Image("inputs/wide.bmp");
  EXPECT_TRUE(m_image->isValid());
  int expected = 249;
  int result = m_image->width();
  EXPECT_EQ(expected, result);
}

// ----------------------------------------------

TEST_F(ImageTest, TallJpg)
{
  m_image = new Image("inputs/tall.jpg");
  EXPECT_TRUE(m_image->isValid());
  int expected = 382;
  int result = m_image->height();
  EXPECT_EQ(expected, result);
}

TEST_F(ImageTest, TallPng)
{
  m_image = new Image("inputs/tall.png");
  EXPECT_TRUE(m_image->isValid());
  int expected = 382;
  int result = m_image->height();
  EXPECT_EQ(expected, result);
}

TEST_F(ImageTest, TallGif)
{
  m_image = new Image("inputs/tall.gif");
  EXPECT_TRUE(m_image->isValid());
  int expected = 382;
  int result = m_image->height();
  EXPECT_EQ(expected, result);
}

TEST_F(ImageTest, TallBmp)
{
  m_image = new Image("inputs/tall.bmp");
  EXPECT_TRUE(m_image->isValid());
  int expected = 382;
  int result = m_image->height();
  EXPECT_EQ(expected, result);
}

// ----------------------------------------------

// Images that are too wide and too tall with different aspect ratios

TEST_F(ImageTest, BigTallPng)
{
  m_image = new Image("inputs/big_tall.png");
  EXPECT_TRUE(m_image->isValid());
  EXPECT_LE(m_image->height(), Image::MAX_HEIGHT);
  EXPECT_LE(m_image->width(), Image::MAX_WIDTH);

  // Check ratio
  int margin_w = Image::MAX_WIDTH - m_image->width();
  int margin_h = Image::MAX_HEIGHT - m_image->height();
  EXPECT_LE(margin_h, margin_w);
}

TEST_F(ImageTest, BigWidePng)
{
  m_image = new Image("inputs/big_wide.png");
  EXPECT_TRUE(m_image->isValid());
  EXPECT_LE(m_image->height(), Image::MAX_HEIGHT);
  EXPECT_LE(m_image->width(), Image::MAX_WIDTH);

  // Check ratio
  int margin_w = Image::MAX_WIDTH - m_image->width();
  int margin_h = Image::MAX_HEIGHT - m_image->height();
  EXPECT_LE(margin_w, margin_h);
}
