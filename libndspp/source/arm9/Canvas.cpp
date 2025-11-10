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
#include <string.h>
#include "nds.h"
#include "Canvas.h"
#include "Background.h"
#include "Video.h"

using namespace nds;

static const int BUFFER_SIZE = 256*256;

Canvas & Canvas::instance()
{
  static Canvas s_canvas;
  return s_canvas;
}

Canvas::~Canvas(){}

// static unsigned short BG_MEM_BUFFER[192*256];

Canvas::Canvas()
{
  m_frontMain = (unsigned short*)BG_BMP_RAM(0);
  m_backMain = m_frontMain + BUFFER_SIZE;
  m_frontSub = (unsigned short*)BG_BMP_RAM_SUB(0);
  //m_backSub = (unsigned short*)BG_MEM_BUFFER;
  m_backSub = (unsigned short*)BG_BMP_RAM(16);

  init();
}

void Canvas::init()
{
  defaultExceptionHandler();

  //consoleDebugInit(DebugDevice_NOCASH);

  // set up background.
  Video & main(Video::instance());
  Video & sub(Video::instance(1));
  sub.setMode(5);
  main.setMode(5);
  main.setObjectsEnabled();
  sub.setObjectsEnabled();

  // redo the banks - we want enough VRAM for the main and sub screen
  // Can't use hardware double buffering on sub screen, have to fudge it.
  // Use bank D for the back buffer of the sub screen.
  vramSetPrimaryBanks(VRAM_A_MAIN_BG_0x06000000,
                      VRAM_B_MAIN_BG_0x06020000,
                      VRAM_C_SUB_BG,
                      VRAM_D_MAIN_BG_0x06040000);
  vramSetBankI(VRAM_I_SUB_SPRITE);
  vramSetBankE(VRAM_E_MAIN_SPRITE);

  m_bgMain.setEnabled();
  m_bgSub.setEnabled();

  m_bgMain.update();
  m_bgSub.update();

  main.setToBottom();
  m_clip.x = m_clip.y = 0;
  m_clip.w = width();
  m_clip.h = height();
}

int Canvas::height() const
{
  // from nds/arm9/video.h
  return SCREEN_HEIGHT*2;
}
int Canvas::width() const
{
  // from nds/arm9/video.h
  return SCREEN_WIDTH;
}

void Canvas::drawPixel(int x, int y, int colour)
{
  if (not m_clip.hit(x, y))
    return;
  uint16 * gfx( vram(y));
  if (y >= 192)
    y -= 192;
  gfx[x+y*256] = colour|BIT(15);
}

unsigned short * Canvas::vram(int y)
{
  return (y < 192) ? m_backSub:m_backMain;
}

void Canvas::endPaint()
{
  // swap main screen
  unsigned short* temp = m_frontMain;
  m_frontMain = m_backMain;
  m_backMain = temp;
  // flip
  // base is 16KB and screen size is 256x256x2 (128KB)
  REG_BG3CNT ^= BG_BMP_BASE( 128 / 16 );

  // copy sub screen - buffer size * 2 since is 16 bit
  memcpy(m_frontSub, m_backSub, SCREEN_WIDTH*SCREEN_HEIGHT*2);
}

unsigned short * Canvas::frontVram(int y)
{
  int layer(y<192?1:0);
  if (layer) {
    return m_frontSub;
  }
  return m_frontMain;
}
