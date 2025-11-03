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
#include <cstdio>
#include "Button.h"
#include "ButtonListener.h"
#include "Canvas.h"
#include "Palette.h"
#include "Rectangle.h"
#include "Stylus.h"
#include "TextAreaFactory.h"
#include "TextArea.h"
#include "TextContainer.h"
#include "WidgetColors.h"

using nds::Canvas;
using nds::Color;
using nds::Rectangle;

Button::Button() :
  TextContainer(),
  m_decoration(true)
{
  setBackgroundColor(WidgetColors::BUTTON_FOREGROUND);
}

Button::Button(const std::string & label) :
  TextContainer(label),
  m_decoration(true)
{

}

void Button::paint(const nds::Rectangle & clip)
{
  // print the button
  unsigned short hilight(WidgetColors::BUTTON_SHINE);
  unsigned short lowlight(WidgetColors::BUTTON_SHADOW);

  if (selected())
  {
    setBackgroundColor(WidgetColors::BUTTON_PRESSED);
    hilight = WidgetColors::BUTTON_SHADOW;
    lowlight = WidgetColors::BUTTON_SHINE;
  }
  else
  {
    setBackgroundColor(WidgetColors::BUTTON_FOREGROUND);
  }
  TextContainer::paint(clip);

  if (m_decoration)
  {
    if (clip.hit(m_bounds.x, m_bounds.top()))
    {
      Canvas::instance().horizontalLine(m_bounds.x, m_bounds.top(), m_bounds.w, hilight);
    }
    Canvas::instance().verticalLine(m_bounds.left(), m_bounds.top(), m_bounds.h, hilight);
    if (clip.hit(m_bounds.x, m_bounds.bottom()-1))
    {
      Canvas::instance().horizontalLine(m_bounds.x, m_bounds.bottom()-1, m_bounds.w, lowlight);
    }
    Canvas::instance().verticalLine(m_bounds.right()-1, m_bounds.top(), m_bounds.h, lowlight);
  }
  m_dirty = false;
}

bool Button::stylusUp(const Stylus * stylus)
{
  int x = stylus->lastX();
  int y = stylus->lastY();
  m_dirty = false;
  if (selected())
  {
    setSelected(false);
    m_dirty = true;
    if (m_bounds.hit(x,y) and listener())
    {
      listener()->pressed(this);
    }
  }
  return m_dirty;
}

bool Button::stylusDownFirst(const Stylus * stylus)
{
  int x = stylus->startX();
  int y = stylus->startY();
  m_dirty = false;
  if (m_bounds.hit(x,y))
  {
    setSelected(true);
    m_dirty = true;
  }
  else
  {
    setSelected(false);
  }
  return m_dirty;
}

bool Button::stylusDownRepeat(const Stylus * stylus)
{
  // nothing? perhaps add a flag for repeatable on/off
  return false;
}

bool Button::stylusDown(const Stylus * stylus)
{
  // scraping
  int x = stylus->lastX();
  int y = stylus->lastY();
  if (selected() and not m_bounds.hit(x,y))
  {
    setSelected(false);
    m_dirty = true;
  }
  return false;
}

void Button::setDecoration(bool decorate)
{
  m_decoration = decorate;
}
