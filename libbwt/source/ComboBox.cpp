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

#include "libnds.h"
#include "Palette.h"
#include "Canvas.h"
#include "ComboBox.h"
#include "ScrollBar.h"
#include "Stylus.h"
#include "WidgetColors.h"

// Space at the right of the ComboBox to draw a small arrow inside a rectangle
static const int COMBO_DD_BUTTON_WIDTH(3);
// An offset for the drop down bar.
static const int COMBO_DD_BAR_WIDTH(8);

ComboBox::ComboBox()
{
  add(&m_scrollPane, false);
  add(&m_button, false);

  scrollPane()->setBackgroundColor(WidgetColors::COMBOBOX_DROP_DOWN);
  scrollPane()->setStretchChildren();
  button()->setDecoration(false);
  button()->setBackgroundColor(WidgetColors::COMBOBOX_FOREGROUND);
  scrollPane()->setTopLevel(false);
}

void ComboBox::addItem(const std::string & item)
{
  // If the box is empty, add the first element and set the dimensions of the
  // ComboBox to the size of the first element.
  if (button()->text().empty())
  {
    button()->setText(item);
    if (m_bounds.w == 0)
    {
      m_bounds.w = button()->width();
      m_bounds.h = button()->height();
    }
  }

  // Try to allocate a new button
  Button * b = new (std::nothrow) Button(item);
  if (b == NULL)
    return;

  // Calculate the new size of the ComboBox. If this new Button is wider than
  // the previous ComboBox, expand it. We also need to fit the rectangle with
  // the small arrow, and a few extra pixels to separate the text from the
  // rectangle (so that it has a margin similar to the one on the left of the
  // text).
  if (b->preferredSize().w + COMBO_DD_BUTTON_WIDTH + 5 > m_bounds.w)
  {
     m_bounds.w = b->preferredSize().w + COMBO_DD_BUTTON_WIDTH + 5;
  }

  // Add callback to button so that the combobox is updated.
  b->setListener(this);
  b->setDecoration(false);
  b->setBackgroundColor(WidgetColors::COMBOBOX_DROP_DOWN);
  m_items++;
  scrollPane()->add(b); // The button will be freed by the destructor of Component()
  int idealHeight = (m_bounds.h+2)*m_items;
  if (idealHeight > (192/2))
  {
    idealHeight = 192/2;
  }

  scrollPane()->setSize(m_bounds.w, idealHeight);
  button()->setSize(m_bounds.w, m_bounds.h);
  scrollPane()->setScrollIncrement(m_bounds.h);
}

bool ComboBox::stylusUp(const Stylus * stylus)
{
  if (m_open and scrollPane()->stylusUp(stylus))
  {
    m_dirty = true;
    return true;
  }
  return false;
}

bool ComboBox::stylusDownFirst(const Stylus * stylus)
{
  int x = stylus->startX();
  int y = stylus->startY();
  if (m_bounds.hit(x, y))
  {
    m_open = not m_open;
    if (not m_open)
      button()->setBackgroundColor(WidgetColors::COMBOBOX_FOREGROUND);
    else
      button()->setBackgroundColor(WidgetColors::COMBOBOX_SELECTED);
    return true;
  }
  if (m_open and scrollPane()->stylusDownFirst(stylus))
  {
    m_dirty = true;
    return true;
  }
  m_open = false;
  button()->setBackgroundColor(WidgetColors::COMBOBOX_FOREGROUND);
  m_dirty = true;
  return false;
}

bool ComboBox::stylusDownRepeat(const Stylus * stylus) { return false; }
bool ComboBox::stylusDown(const Stylus * stylus) { return false; }

void ComboBox::setLocation(int x, int y)
{
  Component::setLocation(x, y);
  button()->setLocation(x, y);
}

void ComboBox::setSize(unsigned int w, unsigned int h)
{
  // The combo box needs a bit of extra space on the right to display the scroll
  // bar. If there is no space the scroll bar will be drawn outside of the
  // screen and it won't be possible to scroll the list of options.
  // TODO: Double-check that this max value is correct.
  unsigned int wMax = SCREEN_WIDTH-16-COMBO_DD_BAR_WIDTH;
  if (w > wMax)
    w = wMax;

  Component::setSize(w, h);

  scrollPane()->setSize(w+16, scrollPane()->preferredSize().h);
  m_preferredHeight = button()->preferredSize().h;
  m_preferredWidth = w+COMBO_DD_BAR_WIDTH;
}

void ComboBox::paint(const nds::Rectangle & clip)
{
  if (not m_items)
    return;

  button()->paint(clip);
  bool down(true);
  if (m_open)
  {
    scrollPane()->setLocation(m_bounds.x, m_bounds.bottom());
    if (scrollPane()->bounds().bottom() > nds::Canvas::instance().height())
    {
      scrollPane()->setLocation(m_bounds.x, m_bounds.top() - scrollPane()->height());
      down = false;
    }
    ScrollPane::setPopup(scrollPane());
  }
  else
  {
    ScrollPane::removePopup(scrollPane());
  }
  // draw the little drop down box thing.
  int headX = m_bounds.right() - COMBO_DD_BUTTON_WIDTH;
  int headY = m_bounds.top() + m_bounds.h/2-1;
  if (down)
    ScrollBar::drawDownArrow(headX, headY);
  else
    ScrollBar::drawUpArrow(headX, headY);
  nds::Canvas::instance().verticalLine(headX-COMBO_DD_BUTTON_WIDTH, m_bounds.top(),
                                       m_bounds.h, WidgetColors::BUTTON_SHADOW);
}

void ComboBox::pressed(ButtonI * pressed)
{
  Button * button = (Button*)pressed;
  this->button()->setText(button->text());

  for (unsigned int i = 0; i < scrollPane()->childCount(); ++i)
  {
    if (scrollPane()->childAt(i) == button)
    {
      m_selectedIndex = i;
      break;
    }
  }
  button->setSelected(false);
  m_open = false;
  this->button()->setBackgroundColor(WidgetColors::COMBOBOX_FOREGROUND);
}

const std::string & ComboBox::selectedItem() const
{
  return ((Button*)scrollPane()->childAt(m_selectedIndex))->text();
}

int ComboBox::selectedIndex() const
{
  return m_selectedIndex;
}

void ComboBox::setSelectedIndex(int select)
{
  Button * b((Button*)scrollPane()->childAt(select));
  if (b)
  {
    pressed(b);
  }
}

unsigned int ComboBox::items() const
{
  return scrollPane()->childCount();
}
