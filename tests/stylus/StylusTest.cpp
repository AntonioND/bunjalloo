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
#include "Stylus.h"
#include <gtest/gtest.h>

TEST(StylusTest, Click)
{
  Stylus stylus;
  // unclick...
  stylus.update(Stylus::NOTHING, false, 1, 1);

  // press once
  stylus.update(Stylus::DOWN, false, 1, 1);
  EXPECT_EQ(Stylus::DOWN, stylus.touchType());
  EXPECT_TRUE(stylus.down());
  EXPECT_TRUE(not stylus.isRepeat());

  // release
  stylus.update(Stylus::UP, false, 1, 1);
  EXPECT_EQ(Stylus::UP, stylus.touchType());
  EXPECT_TRUE(stylus.up());
  EXPECT_TRUE(not stylus.isRepeat());
}

TEST(StylusTest, Hold)
{
  Stylus stylus;
  // unclick...
  stylus.update(Stylus::NOTHING, false, 1, 1);

  // press...
  stylus.update(Stylus::DOWN, false, 1, 1);
  EXPECT_EQ(Stylus::DOWN, stylus.touchType());

  stylus.update(Stylus::DOWN, true, 1, 1);
  // hold..
  EXPECT_EQ(Stylus::DOWN, stylus.touchType());
  EXPECT_TRUE(stylus.down());
  EXPECT_TRUE(stylus.isRepeat());

  // release
  stylus.update(Stylus::UP, true, 1, 1);
  EXPECT_EQ(Stylus::UP, stylus.touchType());
  EXPECT_TRUE(not stylus.isRepeat());
}

TEST(StylusTest, HoldAndMove)
{
  Stylus stylus;
  // unclick...
  stylus.update(Stylus::NOTHING, false, 1, 1);

  // press...
  stylus.update(Stylus::DOWN, false, 1, 1);
  EXPECT_EQ(Stylus::DOWN, stylus.touchType());
  int x, y;
  stylus.startPoint(x, y);
  EXPECT_EQ(1, x);
  EXPECT_EQ(1, y);

  // hold..
  for (int i = 0; i < 200; ++i)
  {
    stylus.update(Stylus::DOWN, i&1, i, i);
  }
  stylus.update(Stylus::DOWN, true, 99, 27);
  stylus.startPoint(x, y);
  EXPECT_EQ(1, x);
  EXPECT_EQ(1, y);

  stylus.lastPoint(x, y);
  EXPECT_EQ(99, x);
  EXPECT_EQ(99, stylus.lastX());
  EXPECT_EQ(27, y);
  EXPECT_EQ(27, stylus.lastY());
  EXPECT_EQ(Stylus::DOWN, stylus.touchType());
  EXPECT_TRUE(stylus.isRepeat());

  // release
  stylus.update(Stylus::UP, false, 1, 1);
  EXPECT_EQ(Stylus::UP, stylus.touchType());
  stylus.lastPoint(x, y);
  EXPECT_EQ(99, x);
  EXPECT_EQ(27, y);
}
