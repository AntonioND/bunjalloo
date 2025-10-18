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
#include <nds/bios.h>
#include <nds/cothread.h>
#include <nds/interrupts.h>
#include <nds/memory.h>
extern "C" {
#include <nds/system.h>
}
#include <nds/arm9/video.h>
#include "Video.h"
#include "Sprite.h"

using namespace nds;

Video::Video(int screen):
  m_DISPCNT(screen?REG_DISPCNT_SUB:REG_DISPCNT),
  m_screen(screen)
{
  powerOn(POWER_ALL_2D);
  m_DISPCNT = 0;
  setObjectsEnabled();
  setObjectMapDimensions(1);
  setBanks();
  clear();
}

void Video::setBanks()
{
  //FIXME - too hard coded?
  if (m_screen)
  {
    vramSetBankC(VRAM_C_SUB_BG);
    vramSetBankD(VRAM_D_SUB_SPRITE);
  } else {
    vramSetBankA(VRAM_A_MAIN_BG);
    vramSetBankF(VRAM_F_MAIN_SPRITE);
    vramSetBankG(VRAM_G_MAIN_SPRITE);
  }
}

Video& Video::instance(int screen)
{
  static Video s_intance0(0);
  static Video s_intance1(1);
  return screen?s_intance1:s_intance0;
}

#define BLDMOD_MODE(n)    (n<<6)
void Video::blend(BLDMOD_MODE_t mode, int first, int second)
{
  int blnd((first)|((second)<<8)|(BLDMOD_MODE(mode)));
  if (m_screen)
    REG_BLDCNT_SUB = blnd;
  else
    REG_BLDCNT = blnd;
}

void Video::clear()
{
  u16 *bgpal = BG_PALETTE;
  u16 *sppal = SPRITE_PALETTE;
  u16 *gfx = BG_GFX;
  if (m_screen) {
    bgpal = BG_PALETTE_SUB;
    sppal = SPRITE_PALETTE_SUB;
    gfx = BG_GFX_SUB;
  }

  for (int i = 0; i < 256; ++i) {
    bgpal[i] = 0;
    sppal[i] = 0;
  }
  /* Clear VRAM */
  for (int i = 0; i < 0x40000; ++i) {
    gfx[i] = 0;
  }

  /* Clear all sprites */
  Sprite::disableAll(m_screen);
}

void Video::setObjectsEnabled(bool enable)
{
  m_DISPCNT &= ~DISPLAY_SPR_ACTIVE;
  m_DISPCNT |= (enable?DISPLAY_SPR_ACTIVE:0);
}
void Video::fade(bool fadeout, unsigned int speed)
{
  for (int loop = 0; loop < 17; loop++) {
    for (unsigned int i = 0; i < speed; ++i) {
      cothread_yield_irq(IRQ_VBLANK);
    }
    if (fadeout)
      setFade(loop);
    else
      setFade(16-loop);
  }
}

bool Video::objectsEnabled() const
{
  return (m_DISPCNT&DISPLAY_SPR_ACTIVE);
}

void Video::setFade(int level)
{
  if (level > 16)
    level = 16;
  if (level < 0)
    level = 0;
  blend( BLDMOD_FADE,
         BLDMOD_BG0 | BLDMOD_BG1 | BLDMOD_BG2| BLDMOD_OBJ ,
         BLDMOD_BD );
  if (m_screen) {
    REG_BLDY_SUB = level;
  }
  else {
    REG_BLDY = level;
  }

}

void Video::setBlendAB(int A, int B)
{
  A = A>0x1f?0x1f:A<0?0:A;
  B = B>0x1f?0x1f:B<0?0:B;
  if (m_screen) {
    REG_BLDALPHA_SUB = (A)|((B)<<8);
  }
  else {
    REG_BLDALPHA = (A)|((B)<<8);
  }
}

void Video::setMode(unsigned int mode)
{
  if (mode > 6)
    mode = 6;
  // MODE_0_2D is 0x10000, but no need to unset then set 0x10000
  // i.e. always have to have SOME mode, even if it is just mode 0
  m_DISPCNT &= ~(1|2|3|4|5|6);
  m_DISPCNT |= MODE_0_2D|mode;
}

unsigned int Video::mode() const
{
  // could have &7, but this is clearer and does the same job
  return m_DISPCNT & (1|2|3|4|5|6);
}

void Video::setObjectMapDimensions(int dimensions)
{
  unsigned short save = m_DISPCNT;
  m_DISPCNT &= ~DISPLAY_SPR_1D_LAYOUT;
  if (dimensions==1) {
    m_DISPCNT |= DISPLAY_SPR_1D_LAYOUT;
  }
  else if (dimensions != 2) {
    m_DISPCNT = save;
  }
}
int Video::objectMapDimensions() const
{
  return (m_DISPCNT & DISPLAY_SPR_1D_LAYOUT)?1:2;
}

void Video::setWhite(int level)
{
  if (level > 16)
    level = 16;
  if (level < 0)
    level = 0;
  blend( BLDMOD_WHITE,
        BLDMOD_BG2 | BLDMOD_BG1 | BLDMOD_BG0 | BLDMOD_BD | BLDMOD_OBJ,
         0 );
  if (m_screen)
    REG_BLDY_SUB = level;
  else
    REG_BLDY = level;
}

void Video::whiteout(bool towhite, unsigned int speed)
{
  for (int loop = 0; loop < 17; loop++) {
    for (unsigned int i = 0; i < speed; ++i) {
      cothread_yield_irq(IRQ_VBLANK);
    }
    if (towhite)
      setWhite(loop);
    else
      setWhite(16-loop);
  }
}

void Video::setThreeD(bool td)
{
  if (td) {
    // textures? should enable a vram bank for textures too
    powerOn(POWER_ALL);
    m_DISPCNT |= ENABLE_3D|DISPLAY_BG0_ACTIVE;
    vramSetBankA(VRAM_A_TEXTURE);
  } else {
    vramSetBankA(VRAM_A_MAIN_BG);
    powerOff((PM_Bits)(POWER_3D_CORE|POWER_MATRIX));
    m_DISPCNT &= ~(ENABLE_3D|DISPLAY_BG0_ACTIVE);
  }
}

bool Video::threeD() const
{
  return (m_DISPCNT & ENABLE_3D);
}

void Video::capture() const
{
  // can only capture on video 0 (main screen)
  if (m_screen)
    return;
  
  // allocate write bank to LCD.
  vramSetBankB(VRAM_B_LCD);
  const int mode   = 0;  // what are the sources?
  const int source = threeD()?1:0;
  const int dst    = 1;           // A = 0, B = 1, C = 2, D = 3
  REG_DISPCAPCNT = 
    DCAP_ENABLE      |  // Enable/Busy flag
    DCAP_MODE(mode)  |  // Capture mode    (0=Source A, 1=Source B, 2/3=Sources A+B blended)
    DCAP_OFFSET(0)   |  // VRAM Write Offset (0=00000h, 0=08000h, 0=10000h, 0=18000h)
    DCAP_SRC(source) |  // Source A          (0=Graphics Screen BG+3D+OBJ, 1=3D Screen
    DCAP_SIZE(3)     |  // Capture Size      (0=128x128, 1=256x64, 2=256x128, 3=256x192 dots)
    DCAP_OFFSET(0)   |  // VRAM Read Offset  (0=00000h, 0=08000h, 0=10000h, 0=18000h)
    DCAP_BANK(dst)   |  // VRAM Write Block  (0..3 = VRAM A..D) (VRAM must be allocated to LCDC)
    DCAP_A(16)       |  // Bland ignored if capture mode is 0 or 1
    DCAP_B(0);
  int loops = 4;
  while (REG_DISPCAPCNT & DCAP_ENABLE) {
    cothread_yield_irq(IRQ_VBLANK);
    // shouldn't block if the emulator is broken...
    if ((--loops)==0)
      break;
  }
  // and finally set to bg
  // where does DISP_MMEM_FIFO come into this?
}

void Video::swapScreens()
{
  lcdSwap();
}

void Video::setToTop()
{
  if (m_screen == 0) {
    // we are the main screen
    lcdMainOnTop();
  } else {
    // we are the sub screen
    lcdMainOnBottom();
  }
}

void Video::setToBottom()
{
  if (m_screen) {
    // we are the sub screen - so make the main one go to the top.
    lcdMainOnTop();
  } else {
    // we are the main screen.
    lcdMainOnBottom();
  }
}

bool Video::onTop() const
{
  bool mot = REG_POWERCNT & POWER_SWAP_LCDS;
  if (m_screen) {
    // we are the sub screen - on top if main is not
    return not mot;
  } else {
    // we are the main screen.
    return mot;
  }
}
