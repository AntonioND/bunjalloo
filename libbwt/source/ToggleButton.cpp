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
#include "ToggleButton.h"
#include "ButtonListener.h"
#include "Canvas.h"
#include "WidgetColors.h"
#include "Stylus.h"

ToggleButton::ToggleButton():
  Component(), m_touched(false)
{}

void ToggleButton::paint(const nds::Rectangle & clip)
{
  const unsigned char * gfx = getGfxData();
  const unsigned short * palette = getPaletteData();

  nds::Canvas & canvas(nds::Canvas::instance());

  // paint gfx - a 16x16 graphic in sprite tile format (blocks of 8x8)
  for (int tile = 0; tile < 4; tile++)
  {
    int yi = m_bounds.y + ((tile/2)*8);
    int xi = m_bounds.x + ((tile&1)*8);
    for (int y = yi; y < yi+8; ++y)
    {
      for (int x = xi; x < xi+8; x++)
      {
        canvas.drawPixel(x, y, palette[*gfx++]);
      }
    }
  }
  m_dirty = false;
}

bool ToggleButton::stylusUp(const Stylus * stylus)
{
  int x = stylus->lastX();
  int y = stylus->lastY();
  if (m_touched and m_bounds.hit(x, y))
  {
    m_touched = false;
    if (listener())
    {
      listener()->pressed(this);
    }
    else
    {
      setSelected(not selected());
    }
    return true;
  }
  return false;
}

bool ToggleButton::stylusDownFirst(const Stylus * stylus)
{
  int x = stylus->lastX();
  int y = stylus->lastY();
  if (m_bounds.hit(x, y))
  {
    m_touched = true;
    return true;
  }
  return false;
}

bool ToggleButton::stylusDownRepeat(const Stylus * stylus)
{
  return false;
}

bool ToggleButton::stylusDown(const Stylus * stylus)
{
  int x = stylus->lastX();
  int y = stylus->lastY();
  if (not m_bounds.hit(x, y))
  {
    m_touched = false;
  }
  return false;
}

bool ToggleButton::touched() const
{
  return m_touched;
}
