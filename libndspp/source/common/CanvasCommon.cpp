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
#include <cstring>
#include "Canvas.h"
#include "Palette.h"
#include "libnds.h"
using namespace nds;

void Canvas::setClip(const Rectangle & clip)
{
  m_clip = clip;
  if (m_clip.x < 0)
  {
    m_clip.x = 0;
  }
  if (m_clip.y < 0)
  {
    m_clip.y = 0;
  }
  if (m_clip.w > width())
  {
    m_clip.w = width();
  }
  if (m_clip.h > height())
  {
    m_clip.h = height();
  }
}

Rectangle Canvas::clip() const
{
  return m_clip;
}

void Canvas::verticalLine(int x, int y, int length, int colour)
{
  if (length < 0)
    return;

  // TODO: This should use m_clip.w and m_clip.h instead of width() and height()
  // but some code that draws things doesn't seem to set the clipping rectangles
  // correctly so some things aren't drawn.
#if 1
  if ((x < m_clip.x) or (x > width()))
    return;

  int yend = y + length;

  if ((y > height()) or (yend < m_clip.y))
    return;

  if (y < m_clip.y)
    y = m_clip.y;

  if (yend > height())
    yend = height();
#else
  if ((x < m_clip.x) or (x > m_clip.w))
    return;

  int yend = y + length;

  if ((y > m_clip.h) or (yend < m_clip.y))
    return;

  if (y < m_clip.y)
    y = m_clip.y;

  if (yend > m_clip.h)
    yend = m_clip.h;
#endif

  length = yend - y;

  for (int i = 0 ; i < length; ++i)
  {
    drawPixel(x, y+i, colour);
  }
}

void Canvas::unsafeDrawPixel(int x, int y, int color)
{
  unsigned short *gfx(vram(y));
  if (y >= 192)
    y -= 192;
  unsigned short *dest = &gfx[x+y*SCREEN_WIDTH];
  *dest = color
#ifdef ARM9
      | (1<<15)
#endif
      ;
}

void Canvas::horizontalLine(int x, int y, int length, int colour)
{
  if (length < 0)
    return;

  // TODO: This should use m_clip.w and m_clip.h instead of width() and height()
  // but some code that draws things doesn't seem to set the clipping rectangles
  // correctly so some things aren't drawn.
#if 1
  if ((y < m_clip.y) or (y > height()))
    return;

  int xend = x + length;

  if ((x > width()) or (xend < m_clip.x))
    return;

  if (x < m_clip.x)
    x = m_clip.x;

  if (xend > width())
    xend = width();
#else
  if ((y < m_clip.y) or (y > m_clip.h)) // m_clip.bottom() ??
    return;

  int xend = x + length;

  if ((x > m_clip.w) or (xend < m_clip.x))
    return;

  if (x < m_clip.x)
    x = m_clip.x;

  if (xend > m_clip.w)
    xend = m_clip.w;
#endif

  length = xend - x;

  unsigned short * gfx(vram(y));
  if (y >= 192)
    y -= 192;
  unsigned short *dest = &gfx[x+y*width()];
  unsigned short *end = dest + length;

  while (dest != end)
  {
    // drawPixel(x+i, y, colour);
    *dest++ = colour
#ifdef ARM9
      | (1<<15)
#endif
      ;
  }
}

void Canvas::drawRectangle(int x, int y, int w, int h, int colour)
{
  horizontalLine(x,y,w,colour);
  horizontalLine(x,y+h,w,colour);
  verticalLine(x,y,h,colour);
  verticalLine(x+w,y,h+1,colour);
}

void Canvas::fillRectangle(int x, int y, int w, int h, int colour)
{
  if (y < 0) {
    h = h + y;
    y = 0;
  }
  if (y > (m_clip.y + m_clip.h))
    return;

  if ( (y < m_clip.y) and (y+h) > m_clip.y) {
    y = m_clip.y;
    //printf("y clipped to %d\n",y);
  }

  if ( ((y+h) > 0) and (y + h) > (m_clip.y+m_clip.h))
  {
    h = m_clip.y + m_clip.h - y;
    //printf("clip %d %d %d %d\n", m_clip.x, m_clip.y, m_clip.w, m_clip.h);
    //printf("fillR %d %d %d %d\n", x, y, w, h);
  }
  for (int j = 0; j < h; j++) {
    horizontalLine(x, y+j, w, colour);
  }
}

void Canvas::copyBlock(int x1, int y1, int w, int h,
                     int x2, int y2)
{
  // copy the data @ x1, y1 to x2, y2
  for (int i = 0; i < h; ++i) {
    int sourcey = y1 + i;
    int targety = y2 + i;
    if (sourcey < 0) {
      continue;
    }
    if (targety < 0) {
      continue;
    }
    if (targety > SCREEN_HEIGHT * 2) {
      break;
    }
    unsigned short *source(frontVram(sourcey));
    unsigned short *target(vram(targety));
    if (sourcey >= SCREEN_HEIGHT) {
      sourcey -= SCREEN_HEIGHT;
    }
    if (targety >= SCREEN_HEIGHT) {
      targety -= SCREEN_HEIGHT;
    }
    source = &source[x1 + sourcey*SCREEN_WIDTH];
    target = &target[x2 + targety*SCREEN_WIDTH];
    memcpy(target, source, w*2);
  }
}
