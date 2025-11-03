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
#include "BoxLayout.h"

#include <gtest/gtest.h>

class BoxLayoutTest : public testing::Test
{
  protected:
    BoxLayout *m_layout;
    void SetUp();
    void TearDown();
};

class MockComponent: public Component
{
  public:
    void setPreferredSize(int w, int h)
    {
      m_preferredWidth = w;
      m_preferredHeight = h;
    }

    virtual void paint(const nds::Rectangle & clip) { }

    virtual bool stylusUp(const Stylus * stylus) { return false; }
    virtual bool stylusDownFirst(const Stylus * stylus) { return false; }
    virtual bool stylusDownRepeat(const Stylus * stylus) { return false; }
    virtual bool stylusDown(const Stylus * stylus) { return false; }
};

void BoxLayoutTest::SetUp()
{
  m_layout = new BoxLayout();
  m_layout->setSize(255, 192*2);
  m_layout->setLocation(0, 0);
}

void BoxLayoutTest::TearDown()
{
  delete m_layout;
}

TEST_F(BoxLayoutTest, Simple)
{
  /*
   * Test that the simple case works:
   *
   * comp1 - wide component with lowish height
   * comp2 - wide component with lowish height
   *   +----------+
   *   |  comp1   |
   *   +----------+
   *   |  comp2   |
   *   +----------+
   * comp1 should be at 0,0
   * comp2 should be at 0,comp1.bottom
   */
  MockComponent *comp1 = new MockComponent;
  MockComponent *comp2 = new MockComponent;

  using nds::Rectangle;
  Rectangle r1(0, 0, 250, 14);
  comp1->setSize(r1.w, r1.h);
  comp2->setSize(r1.w, r1.h);

  m_layout->add(comp1);
  m_layout->add(comp2);

  Rectangle r2 = r1;
  r2.y = r1.bottom();

  EXPECT_TRUE(r1 == comp1->bounds()) << "Component 1 bounds";
  EXPECT_TRUE(r2 == comp2->bounds()) << "Component 2 bounds";
  EXPECT_EQ(r2.bottom(), m_layout->height());
  EXPECT_EQ(2U, m_layout->boxCount());
}

TEST_F(BoxLayoutTest, Packed)
{
  /*
   * Test that packing 2 in one box works
   *
   * comp1 - small component with lowish height
   * comp2 - small component with lowish height
   *   +-------------+
   *   |comp1 | comp2|
   *   +-------------+
   * comp1 should be at 0,0
   * comp2 should be at comp1.right,0
   */
  MockComponent *comp1 = new MockComponent;
  MockComponent *comp2 = new MockComponent;

  using nds::Rectangle;
  Rectangle r1(0, 0, 60, 14);
  comp1->setSize(r1.w, r1.h);
  comp2->setSize(r1.w, r1.h);

  m_layout->add(comp1);
  m_layout->add(comp2);

  Rectangle r2 = r1;
  r2.x = r1.right();

  EXPECT_TRUE(r1 == comp1->bounds()) << "Component 1 bounds";
  EXPECT_TRUE(r2 == comp2->bounds()) << "Component 2 bounds";
  EXPECT_EQ(1U, m_layout->boxCount());
}

TEST_F(BoxLayoutTest, Packed2)
{
  /*
   * Test that packing 3 in one box, with odd heights, works
   *
   * comp1 - small component with lowish height
   * comp2 - small component with taller height
   * comp3 - small component with lowish height
   *   +------------------+
   *   |comp1 |comp2|comp3|
   *   +------{comp2}-----+
   *          +-----+
   * comp1 should be at 0,0
   * comp2 should be at comp1.right,0
   * comp3 should be at comp2.right,0
   */
  MockComponent *comp1 = new MockComponent;
  MockComponent *comp2 = new MockComponent;
  MockComponent *comp3 = new MockComponent;

  using nds::Rectangle;
  Rectangle r1(0, 0, 60, 14);
  Rectangle r2(0, 0, 60, 40);
  comp1->setSize(r1.w, r1.h);
  comp2->setSize(r2.w, r2.h);
  comp3->setSize(r1.w, r1.h);

  m_layout->add(comp1);
  m_layout->add(comp2);
  m_layout->add(comp3);

  // r1 is as expected
  // r2 should be at r1.right
  r2.x = r1.right();
  // r3 is same as r1, but to the right of r2
  Rectangle r3 = r1;
  r3.x = r2.right();

  EXPECT_TRUE(r1 == comp1->bounds()) << "Component 1 bounds";
  EXPECT_TRUE(r2 == comp2->bounds()) << "Component 2 bounds";
  EXPECT_TRUE(r3 == comp3->bounds()) << "Component 2 bounds";
  EXPECT_EQ(1U, m_layout->boxCount());
}

TEST_F(BoxLayoutTest, Packed3)
{
  /*
   * Test that packing 3 in one box, with odd heights,
   * then packing one more large comp all works
   *
   * comp1 - small component with lowish height
   * comp2 - small component with taller height
   * comp3 - small component with lowish height
   * comp4 - large component with med height
   *
   *   +------------------+
   *   |comp1 |comp2|comp3|
   *   +------{comp2}-----+
   *   +------+-----+-----+
   *   |    comp4         |
   *   +-------------------
   * comp1 should be at 0,0
   * comp2 should be at comp1.right,0
   * comp3 should be at comp2.right,0
   * comp4 should be at 0,comp2.height
   */
  MockComponent *comp1 = new MockComponent;
  MockComponent *comp2 = new MockComponent;
  MockComponent *comp3 = new MockComponent;
  MockComponent *comp4 = new MockComponent;

  using nds::Rectangle;
  Rectangle r1(0, 0, 60, 14);
  Rectangle r2(0, 0, 60, 40);
  Rectangle larger(0, 0, 230, 20);
  comp1->setSize(r1.w, r1.h);
  comp2->setSize(r2.w, r2.h);
  comp3->setSize(r1.w, r1.h);
  comp4->setSize(larger.w, larger.h);

  m_layout->add(comp1);
  m_layout->add(comp2);
  m_layout->add(comp3);
  m_layout->add(comp4);

  // r1 is as expected
  // r2 should be at r1.right
  r2.x = r1.right();
  // r3 is same as r1, but to the right of r2
  Rectangle r3 = r1;
  r3.x = r2.right();
  // r4 is same as larger, but at r2.bottom
  Rectangle r4 = larger;
  r4.y = r2.bottom();

  EXPECT_TRUE(r1 == comp1->bounds()) << "Component 1 bounds";
  EXPECT_TRUE(r2 == comp2->bounds()) << "Component 2 bounds";
  EXPECT_TRUE(r3 == comp3->bounds()) << "Component 3 bounds";
  EXPECT_TRUE(r4 == comp4->bounds()) << "Component 4 bounds";
  EXPECT_EQ(2U, m_layout->boxCount());
}

TEST_F(BoxLayoutTest, Resize)
{
  /*
   * Test that the simple resizing case works:
   *
   * comp1 - wide component with lowish height
   * comp2 - small component initially, then resized
   *   +----------+--+
   *   |  comp1   |c2|
   *   +----------+--+
   * After resize:
   *   +----------+
   *   |  comp1   |
   *   +----------+
   *   |  comp2   |
   *   +----------+
   * comp1 should be at 0,0
   * comp2 should be at 0,comp1.bottom
   */
  MockComponent *comp1 = new MockComponent;
  MockComponent *comp2 = new MockComponent;

  using nds::Rectangle;
  Rectangle r1(0, 0, 250, 14);
  Rectangle small;
  comp1->setSize(r1.w, r1.h);
  comp2->setSize(small.w, small.h);

  m_layout->add(comp1);
  m_layout->add(comp2);

  small.x = r1.right();
  EXPECT_TRUE(r1 == comp1->bounds()) << "Component 1 bounds pre-resize";
  EXPECT_TRUE(small == comp2->bounds()) << "Component 2 bounds pre-resize";
  EXPECT_EQ(1U, m_layout->boxCount());

  // now resize..
  comp2->setSize(r1.w, r1.h);

  m_layout->doLayout();

  Rectangle r2 = r1;
  r2.y = r1.bottom();

  EXPECT_TRUE(r1 == comp1->bounds()) << "Component 1 bounds";
  EXPECT_TRUE(r2 == comp2->bounds()) << "Component 2 bounds";
  EXPECT_EQ(2U, m_layout->boxCount());
}

TEST_F(BoxLayoutTest, SetLocation)
{
  /*
   * Test that setting the location of the BoxLayout moves all the children but
   * doesn't alter the box count
   *
   * comp1 - wide component with lowish height
   * comp2 - wide component with lowish height
   *   +----------+
   *   |  comp1   |
   *   +----------+
   *   |  comp2   |
   *   +----------+
   * comp1 should be at 0,0
   * comp2 should be at 0,comp1.bottom
   *
   * Then setLocation(0,-10):
   * comp1 should be at 0,-10
   * comp2 should be at 0,comp1.bottom
   */
  MockComponent *comp1 = new MockComponent;
  MockComponent *comp2 = new MockComponent;

  using nds::Rectangle;
  Rectangle r1(0, 0, 250, 14 );
  comp1->setSize(r1.w, r1.h);
  comp2->setSize(r1.w, r1.h);

  m_layout->add(comp1);
  m_layout->add(comp2);

  Rectangle r2 = r1;
  r2.y = r1.bottom();

  EXPECT_TRUE(r1 == comp1->bounds()) << "Component 1 bounds";
  EXPECT_TRUE(r2 == comp2->bounds()) << "Component 2 bounds";
  EXPECT_EQ(2U, m_layout->boxCount());

  m_layout->setLocation(0, -10);
  r1.y = -10;
  r2.y = r1.bottom();
  EXPECT_TRUE(r1 == comp1->bounds()) << "Component 1 bounds after setLocation";
  EXPECT_TRUE(r2 == comp2->bounds()) << "Component 2 bounds after setLocation";
  EXPECT_EQ(2U, m_layout->boxCount());
}

TEST_F(BoxLayoutTest, NastySize)
{
  /*
   * Test that the simple case works:
   *
   * comp1 - wide component with lowish height
   * comp2 - wide component with lowish height
   *   +----------+
   *   |  comp1   |
   *   +----------+
   *   |  comp2   |
   *   +----------+
   * comp1 should be at 0,0
   * comp2 should be at 0,comp1.bottom
   */
  MockComponent *comp1 = new MockComponent;
  MockComponent *comp2 = new MockComponent;

  using nds::Rectangle;
  Rectangle r1(0, 0, 230, 14 );
  comp1->setSize(r1.w, r1.h);
  comp2->setSize(r1.w, r1.h);

  m_layout->setSize(256, 13);
  comp1->setPreferredSize(256, 14);
  comp2->setPreferredSize(256, 14);

  m_layout->add(comp1);
  m_layout->add(comp2);

  // size should be max size of m_layout
  r1.w = m_layout->width();
  Rectangle r2 = r1;
  r2.y = r1.bottom();

  EXPECT_TRUE(r1 == comp1->bounds()) << "Component 1 bounds";
  EXPECT_TRUE(r2 == comp2->bounds()) << "Component 2 bounds";
  EXPECT_EQ(r2.bottom(), m_layout->height());
  EXPECT_EQ(2U, m_layout->boxCount());
}

TEST_F(BoxLayoutTest, ResizeImages)
{
  /*
   * Test a "real life" resize case.
   *
   * c1 -> 0,0,249,13 (preferred height 26)
   * c2 -> 0,0,0,0
   * c3 -> 0,0,249,13 (preferred height 52)
   * c4 -> 0,0,0,0
   * c5 -> 0,0,249,13 (preferred height 26)
   *  
   * +----------++
   * |    c1    ||<c2
   * +----------++
   * |    c3    ||<c4
   * |          |
   * +----------+
   * |    c5    |
   * +----------+
   *
   * Then 
   * resize 1:
   *  c2 resize to -> 250 94
   *  doLayout()
   *  
   * resize 2:
   *  c4 resize to -> 250 232
   *  doLayout()
   *  
   */
  MockComponent *c1 = new MockComponent;
  MockComponent *c2 = new MockComponent;
  MockComponent *c3 = new MockComponent;
  MockComponent *c4 = new MockComponent;
  MockComponent *c5 = new MockComponent;

  using nds::Rectangle;
  Rectangle r1(0, 0, 249, 13 );
  Rectangle image;
  c1->setSize(r1.w, r1.h);
  c1->setPreferredSize(300, 26);
  c2->setSize(image.w, image.h);
  c3->setSize(r1.w, r1.h);
  c3->setPreferredSize(300, 52);
  c4->setSize(image.w, image.h);
  c5->setSize(r1.w, r1.h);
  c5->setPreferredSize(300, 26);

  m_layout->add(c1);
  m_layout->add(c2);
  m_layout->add(c3);
  m_layout->add(c4);
  m_layout->add(c5);

  m_layout->setSize(249, 26);
  {
    Rectangle ex1(0, 0, m_layout->width(), 26);
    Rectangle ex2(ex1.right(), 0, 0, 0);
    Rectangle ex3(0, 26, m_layout->width(), 52);
    Rectangle ex4(ex3.right(), 26, 0, 0);
    Rectangle ex5 = ex1; ex5.y = ex3.bottom();

    EXPECT_TRUE(ex1 == c1->bounds());
    EXPECT_TRUE(ex2 == c2->bounds());
    EXPECT_TRUE(ex3 == c3->bounds());
    EXPECT_TRUE(ex4 == c4->bounds());
    EXPECT_TRUE(ex5 == c5->bounds());
    EXPECT_EQ(3U, m_layout->boxCount());
  }

  c2->setSize(249, 94);
  /*
   * +----------+
   * |    c1    |
   * +----------+
   * |          |
   * |    c2    |
   * |          |
   * +----------+
   * |    c3    ||<c4
   * |          |
   * +----------+
   * |    c5    |
   * +----------+
   */
  // move down...
  m_layout->setLocation(0, 192);
  m_layout->doLayout();
  {
    Rectangle ex1(0, m_layout->y(), m_layout->width(), 26);
    Rectangle ex2(0, ex1.bottom(), 249, 94);
    Rectangle ex3(0, ex2.bottom(), m_layout->width(), 52);
    Rectangle ex4(ex3.right(), ex2.bottom(), 0, 0);
    Rectangle ex5 = ex1; ex5.y = ex3.bottom();

    EXPECT_TRUE(ex1 == c1->bounds());
    EXPECT_TRUE(ex2 == c2->bounds());
    EXPECT_TRUE(ex3 == c3->bounds());
    EXPECT_TRUE(ex4 == c4->bounds());
    EXPECT_TRUE(ex5 == c5->bounds());
    EXPECT_EQ(4U, m_layout->boxCount());
  }

  /*
   * +----------+
   * |    c1    |
   * +----------+
   * |          |
   * |    c2    |
   * |          |
   * +----------+
   * |    c3    |
   * |          |
   * +----------+
   * |          |
   * |    c4    |
   * |          |
   * +----------+
   * |    c5    |
   * +----------+
   */
  c4->setSize(249, 232);
  m_layout->doLayout();
  {
    Rectangle ex1(0, m_layout->y(), m_layout->width(), 26);
    Rectangle ex2(0, ex1.bottom(), 249, 94);
    Rectangle ex3(0, ex2.bottom(), m_layout->width(), 52);
    Rectangle ex4(0, ex3.bottom(), 249, 232);
    Rectangle ex5 = ex1; ex5.y = ex4.bottom();

    EXPECT_TRUE(ex1 == c1->bounds());
    EXPECT_TRUE(ex2 == c2->bounds());
    EXPECT_TRUE(ex3 == c3->bounds());
    EXPECT_TRUE(ex4 == c4->bounds());
    EXPECT_TRUE(ex5 == c5->bounds());
    EXPECT_EQ(5U, m_layout->boxCount());
    EXPECT_EQ(26+94+52+232+26, m_layout->height());
  }
}

TEST_F(BoxLayoutTest, Newline)
{
  /*
   * Test that adding a new line and resizing works
   *
   * c1 - small component
   * c2 - 0 component, resized later
   * -> newline
   * c3 - small component
   * c4 - small component
   *   +---+
   *   |c1|| < c2 is 0 size, but followed by a newline
   *   +-----+
   *   |c3|c4|
   *   +-----+
   * c1 at 0,0
   * c2 at 0,c1.right
   * c3 at 0, c1.bottom
   * c4 at c3.right, c1.bottom
   *
   * Note c1, c2 and c3 would all fit on one line normally
   */
  MockComponent *c1 = new MockComponent;
  MockComponent *c2 = new MockComponent;
  MockComponent *c3 = new MockComponent;
  MockComponent *c4 = new MockComponent;

  using nds::Rectangle;
  Rectangle r1(0, 0, 40, 14 );
  Rectangle r2;
  c1->setSize(r1.w, r1.h);
  c2->setSize(r2.w, r2.h);
  c3->setSize(r1.w, r1.h);
  c4->setSize(r1.w, r1.h);

  m_layout->add(c1);
  m_layout->add(c2);
  m_layout->insertNewline();
  m_layout->add(c3);
  m_layout->add(c4);

  c2->setSize(50, 18);
  m_layout->doLayout();
  r2.x = r1.right();
  r2.y = 0;
  r2.w = 50;
  r2.h = 18;

  Rectangle r3 = r1;
  r3.y = r2.bottom();
  Rectangle r4 = r3;
  r4.x = r3.right();

  EXPECT_TRUE(r1 == c1->bounds());
  EXPECT_TRUE(r2 == c2->bounds());
  EXPECT_TRUE(r3 == c3->bounds());
  EXPECT_TRUE(r4 == c4->bounds());
  EXPECT_EQ(r3.bottom(), m_layout->height());
  EXPECT_EQ(2U, m_layout->boxCount());
}
