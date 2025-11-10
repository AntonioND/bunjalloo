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
#ifndef Canvas_h_seen
#define Canvas_h_seen

#include <cstddef>

#include "Background.h"
#include "Rectangle.h"
#include "util/classhelper.h"

namespace nds
{
  class Background;

  class Canvas
  {
    public:
      /** Get the static Canvas instance.*/
      static Canvas & instance();
      /** Draw a pixel at a given posiiton.
       * @param x the x position
       * @param y the y position. 0 is the tpo of the screen, 383 is the bottom
       * @param color the 15 bit RGB color.
       */
      void drawPixel(int x, int y, int color);

      void unsafeDrawPixel(int x, int y, int color);

      /** 
       *  Copy a block of vram at x1, y1 to the new area at x2, y2
       * @param x1 the absolute start coordinate in pixels
       * @param y1 the absolute start coordinate in pixels
       * @param w the width of the area to copy in pixels
       * @param h the height of the area to copy in pixels
       * @param x2 the absolute target coordinate in pixels
       * @param y2 the absolute target coordinate in pixels
       */
      void copyBlock(int x1, int y1, int w, int h,
                     int x2, int y2);

      /** Fill a rectangle at the given position with the given dimensions.
       * @param x the x position.
       * @param y the y position. 0 is the tpo of the screen, 383 is the bottom
       * @param w width in pixels.
       * @param h height in pixels.
       * @param color the 15 bit RGB color.
       */
      void fillRectangle(int x, int y, int w, int h, int color);
      /** Draw an outline rectangle at the given position with the given dimensions.
       * @param x the x position.
       * @param y the y position. 0 is the tpo of the screen, 383 is the bottom
       * @param w width in pixels.
       * @param h height in pixels.
       * @param color the 15 bit RGB color.
       */
      void drawRectangle(int x, int y, int w, int h, int color);
      /** Draw a vertical line.
       * @param x the x position.
       * @param y the y position. 0 is the tpo of the screen, 383 is the bottom
       * @param length the number of pixels in the line
       * @param color the 15 bit RGB color.
       */
      void verticalLine(int x, int y, int length, int colour);
      /** Draw a horizontal line.
       * @param x the x position.
       * @param y the y position. 0 is the tpo of the screen, 383 is the bottom
       * @param length the number of pixels in the line
       * @param color the 15 bit RGB color.
       */
      void horizontalLine(int x, int y, int length, int colour);
      /** Width of the canvas. */
      int width() const;
      /** Height of the canvas. */
      int height() const;
      /** Tell this Canvas that painting has finished. This will swap buffers
       * to refresh the display.*/
      void endPaint();

      /** Set the clipped region.
       * @param clip the area to clip to, anything outside this is not drawn.
       */
      void setClip(const nds::Rectangle & clip);
      /** Get the clipped region.
       * @return the area clipped to.
       */
      nds::Rectangle clip() const;

    private:
      Background m_bgMain { 0, 3, 0, 31 };
      Background m_bgSub { 1, 3, 0, 31 };

      unsigned short * m_frontMain { NULL };
      unsigned short * m_backMain { NULL };
      unsigned short * m_frontSub { NULL };
      unsigned short * m_backSub { NULL };
      Rectangle m_clip;

      Canvas();
      ~Canvas();

      unsigned short * vram();
      unsigned short * vram(int y);
      unsigned short * frontVram(int y);
      void init();

      DISALLOW_COPY_AND_ASSIGN(Canvas);
  };
}
#endif
