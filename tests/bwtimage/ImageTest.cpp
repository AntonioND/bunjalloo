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

TEST_F(ImageTest, Wide)
{
  m_image = new Image("inputs/wide.jpg");
  EXPECT_TRUE(m_image->isValid());
  int expected = 249;
  int result = m_image->width();
  EXPECT_EQ(expected, result);
}

TEST_F(ImageTest, Tall)
{
  m_image = new Image("inputs/tall.jpg");
  EXPECT_TRUE(m_image->isValid());
  int expected = 382;
  int result = m_image->height();
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

TEST_F(ImageTest, TallPng)
{
  m_image = new Image("inputs/tall.png");
  EXPECT_TRUE(m_image->isValid());
  int expected = 382;
  int result = m_image->height();
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

TEST_F(ImageTest, TallGif)
{
  m_image = new Image("inputs/tall.gif");
  EXPECT_TRUE(m_image->isValid());
  int expected = 382;
  int result = m_image->height();
  EXPECT_EQ(expected, result);
}
