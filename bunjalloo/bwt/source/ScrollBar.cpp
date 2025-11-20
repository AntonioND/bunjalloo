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
#include "Canvas.h"
#include "Palette.h"
#include "ScrollBar.h"
#include "ScrollPane.h"
#include "Stylus.h"
#include "WidgetColors.h"
using nds::Canvas;

const static int ARROW_HEIGHT(10);
const static int MIN_HANDLE_SIZE(5);
const static int HANDLE_NOT_HELD(-1);

ScrollBar::ScrollBar():Component(),
  m_total(0),
  m_value(0),
  m_handleSize(0),
  m_handlePosition(0),
  m_handleHeld(HANDLE_NOT_HELD),
  m_hitOnRepeat(false),
  m_touchedMe(NO_TOUCH),
  m_scrollPane(0)
{
}

void ScrollBar::setTotal(int total)
{
  m_total = total;
}

void ScrollBar::setValue(int value)
{
  m_value = value;
}

int ScrollBar::total() const
{
  return m_total;
}

int ScrollBar::value() const
{
  return m_value;
}

void ScrollBar::setScrollable(ScrollPane * scrollPane)
{
  m_scrollPane = scrollPane;
}

bool ScrollBar::hitTopArrow(int y)
{
   return (y < (m_bounds.y + ARROW_HEIGHT));
}

bool ScrollBar::hitBottomArrow(int y)
{
  return (y > (m_bounds.y + m_bounds.h - ARROW_HEIGHT));
}

bool ScrollBar::hitHandle(int x, int y)
{
  // check if clicking on the handle:
  nds::Rectangle handle(m_bounds.x, m_handlePosition, m_bounds.w, m_handleSize);
  if (handle.hit(x, y))
  {
    return true;
  }
  return false;
}

void ScrollBar::doScroll(int x, int y)
{
  if (m_scrollPane)
  {
    // Check if clicking in the arrows:
    if (hitTopArrow(y))
    {
      m_scrollPane->up();
      m_handleHeld = HANDLE_NOT_HELD;
      return;
    }
    if (hitBottomArrow(y))
    {
      m_scrollPane->down();
      m_handleHeld = HANDLE_NOT_HELD;
      return;
    }
    // check if clicking on the handle:
    if (m_handleHeld == HANDLE_NOT_HELD)
    {
      if (hitHandle(x, y))
      {
        m_handleHeld = y;
        return;
      }
    }
    else if (m_handleHeld != HANDLE_NOT_HELD)
    {
      // scroll to y...
      m_scrollPane->scrollToPercent( ((y - ARROW_HEIGHT - m_bounds.top()) * 256 ) / visibleRange());
      m_handleHeld = y;
      return;
    }
    // check if above or below the bounds
    if (y < m_handlePosition)
    {
      m_scrollPane->upBlock();
      m_handleHeld = HANDLE_NOT_HELD;
      return;
    }
    if (y > m_handlePosition)
    {
      m_scrollPane->downBlock();
      m_handleHeld = HANDLE_NOT_HELD;
      return;
    }
  }
  else
  {
    // ??
    m_handleHeld = HANDLE_NOT_HELD;
  }
}

bool ScrollBar::stylusUp(const Stylus * stylus)
{
  // only pay attention to touches that start on this widget
  if (not m_bounds.hit(stylus->startX(), stylus->startY()))
  {
    return false;
  }

  if (not m_hitOnRepeat)
  {
    // do actual scroll
    doScroll(stylus->startX(), stylus->startY());
  }
  m_touchedMe = NO_TOUCH;
  m_dirty = true;
  return true;
}

bool ScrollBar::stylusDownFirst(const Stylus * stylus)
{
  int x = stylus->startX();
  int y = stylus->startY();
  if (not m_bounds.hit(x, y))
  {
    m_dirty = m_handleHeld != HANDLE_NOT_HELD;
    m_handleHeld = HANDLE_NOT_HELD;
    return false;
  }
  /*printf("Hit ScrollBar %d %d in %d %d w %d h %d\n",
      stylus->startX(), stylus->startY(),
      m_bounds.x, m_bounds.y,
      m_bounds.w, m_bounds.h);*/

  if (hitHandle(x, y))
  {
    m_touchedMe = TOUCH_HANDLE;
  }
  else if (hitTopArrow(y))
  {
    m_touchedMe = TOUCH_TOP_ARROW;
  }
  else if (hitBottomArrow(y))
  {
    m_touchedMe = TOUCH_BOT_ARROW;
  }
  else
  {
    m_touchedMe = TOUCH_SOMETHING;
  }

  m_hitOnRepeat = false;
  m_dirty = true;
  return true;
}

bool ScrollBar::stylusDownRepeat(const Stylus * stylus)
{
  if (not m_bounds.hit(stylus->startX(), stylus->startY()))
  {
    return false;
  }
  // do actual scrolling
  doScroll(stylus->startX(), stylus->lastY());
  m_hitOnRepeat = true;
  m_dirty = true;
  return true;
}

bool ScrollBar::stylusDown(const Stylus * stylus)
{
  // do nothing
  // TODO: after repeat hits once, assume drag mode?
  return false;
}

void ScrollBar::paint(const nds::Rectangle & clip)
{
  m_dirty = false;
  // background
  Canvas::instance().fillRectangle(m_bounds.x+1,
                                        m_bounds.y+1,
                                        m_bounds.w-1,
                                        m_bounds.h-1,
                                        m_touchedMe==TOUCH_SOMETHING?WidgetColors::SCROLLBAR_BACKGROUND_TOUCH
                                        :WidgetColors::SCROLLBAR_BACKGROUND);

  // up arrow
  Canvas::instance().fillRectangle(m_bounds.x,
                                        m_bounds.y,
                                        m_bounds.w,
                                        ARROW_HEIGHT,
                                        m_touchedMe==TOUCH_TOP_ARROW?WidgetColors::SCROLLBAR_HANDLE_HELD:
                                        WidgetColors::SCROLLBAR_ARROW);
  // head.
  int headX = m_bounds.x + (m_bounds.w/2);
  int headY = m_bounds.y + (ARROW_HEIGHT/2) - 1;
  drawUpArrow(headX-1, headY);

  // down arrow
  Canvas::instance().fillRectangle(m_bounds.x,
                                        m_bounds.bottom()-ARROW_HEIGHT,
                                        m_bounds.w,
                                        ARROW_HEIGHT,
                                        m_touchedMe==TOUCH_BOT_ARROW?WidgetColors::SCROLLBAR_HANDLE_HELD:
                                        WidgetColors::SCROLLBAR_ARROW);
  headY = m_bounds.bottom() - (ARROW_HEIGHT/2);
  drawDownArrow(headX-1, headY);

  calculateHandle();
  unsigned short c(m_touchedMe==TOUCH_HANDLE?WidgetColors::SCROLLBAR_HANDLE_HELD:
      WidgetColors::SCROLLBAR_HANDLE_NOT_HELD);
  if (m_handleHeld != HANDLE_NOT_HELD)
  {
    c = WidgetColors::SCROLLBAR_HANDLE_HELD;
  }

  Canvas::instance().fillRectangle(m_bounds.x+1,
                                   m_handlePosition,
                                   m_bounds.w-2,
                                   m_handleSize,
                                   c);

  Canvas::instance().drawRectangle(m_bounds.x,
                                        m_bounds.y+ARROW_HEIGHT,
                                        m_bounds.w-1,
                                        m_bounds.h-1-2*ARROW_HEIGHT,
                                        WidgetColors::SCROLLBAR_BORDER);
}

void ScrollBar::calculateHandle()
{
  if (m_total <= 0) {
    return;
  }

  m_handleSize = (visibleRange() * visibleRange())/total();
  if (m_handleSize < MIN_HANDLE_SIZE)
    m_handleSize = MIN_HANDLE_SIZE;

  int scrollerHeight = visibleRange() - m_handleSize;

  if (m_total > m_bounds.h)
    m_handlePosition = ((m_value * scrollerHeight) / (m_total-m_bounds.h) );
  else
    m_handlePosition = 0;
  if (m_handlePosition < 0)
    m_handlePosition = 0;
  m_handlePosition += m_bounds.top() + ARROW_HEIGHT;
  if (m_handlePosition > (m_bounds.bottom() - ARROW_HEIGHT - m_handleSize))
  {
    m_handlePosition = m_bounds.bottom() - ARROW_HEIGHT - m_handleSize;
  }
}

int ScrollBar::visibleRange() const
{
  return m_bounds.h - ARROW_HEIGHT*2;
}

void ScrollBar::drawDownArrow(int x, int y)
{
  Canvas::instance().drawPixel(x, y+1, WidgetColors::SCROLLBAR_ARROW_HEAD);
  Canvas::instance().drawPixel(x-1, y, WidgetColors::SCROLLBAR_ARROW_HEAD);
  Canvas::instance().drawPixel(x, y, WidgetColors::SCROLLBAR_ARROW_HEAD);
  Canvas::instance().drawPixel(x+1, y, WidgetColors::SCROLLBAR_ARROW_HEAD);
}
void ScrollBar::drawUpArrow(int x, int y)
{
  Canvas::instance().drawPixel(x, y, WidgetColors::SCROLLBAR_ARROW_HEAD);
  Canvas::instance().drawPixel(x-1, y+1, WidgetColors::SCROLLBAR_ARROW_HEAD);
  Canvas::instance().drawPixel(x, y+1, WidgetColors::SCROLLBAR_ARROW_HEAD);
  Canvas::instance().drawPixel(x+1, y+1, WidgetColors::SCROLLBAR_ARROW_HEAD);
}
