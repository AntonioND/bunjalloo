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
#include "Background.h"
#include "HardwareHandler.cpp"
#include "BackgroundHandler.h"
#include "SDLhandler.h"
#include <stdio.h>

using namespace nds;
static unsigned int SUB_DISPLAY_CR(0);
static unsigned int DISPLAY_CR(0);

#define BG_SIZE(m)		((m<<14))
Background::Background(
    int screen,
    int number,
    int charBlock,
    int screenBlock,
    int priority):
  m_DISPCNT(screen?SUB_DISPLAY_CR:DISPLAY_CR)
{
  m_bg.screen = screen;
  m_bg.number = number;
  m_bg.charBaseBlock = charBlock;
  m_bg.screenBaseBlock = screenBlock;
  m_bg.flags = BG_SIZE(0) | BG_16_COLOR | priority;
  updateTileMapData();
  BackgroundHandler::add(this);
}
Background::~Background()
{
  setEnabled(false);
  BackgroundHandler::remove(this);
}

void Background::updateTileMapData()
{
  if (m_bg.screen) {
    m_bg.tileData = SDLhandler::instance().vramSub( m_bg.charBaseBlock * 0x400);
    m_bg.mapData =  SDLhandler::instance().vramSub( m_bg.screenBaseBlock * 0x800);
  } else {
    m_bg.tileData = SDLhandler::instance().vramMain( m_bg.charBaseBlock * 0x400 );
    m_bg.mapData =  SDLhandler::instance().vramMain( m_bg.screenBaseBlock * 0x800 );
  }
}

unsigned char Background::charBaseBlock() const
{
  return m_bg.charBaseBlock;
};
void Background::setCharBaseBlock(unsigned char var)
{
  if (var > 31) return;
  m_bg.charBaseBlock = var;
  updateTileMapData();
};
int Background::color() const
{
  return (m_bg.flags & BG_256_COLOR)?256:16;
};
void Background::setColor(int var)
{
  switch(var) {
    case 16:
      m_bg.flags &= ~BG_256_COLOR;
      m_bg.flags |= BG_16_COLOR;
      break;
    case 256:
      m_bg.flags &= ~BG_16_COLOR;
      m_bg.flags |= BG_256_COLOR;
      break;
    default:
      break;
  } 
};

static inline unsigned long BG_ON(int n) {
  return (1 << (8+n));
};

void Background::setEnabled(bool enable)
{
  if (enable)
  {
    //unsigned short flags =  m_bg.flags;
    // this depends on if the BG is Sub or or regular BG.
    //REG_DISPCNT |= BG_ON(m_bg.number);
    m_DISPCNT |= BG_ON(m_bg.number);

    /*int bg_ctrl = BG_MAP_BASE(m_bg.screenBaseBlock) | BG_TILE_BASE(m_bg.charBaseBlock) | flags;
    if (m_bg.screen) {
      BGCTRL_SUB[m_bg.number] = bg_ctrl;
    } else {
      BGCTRL[m_bg.number] = bg_ctrl;
    }*/
  }
  else
  {
    m_DISPCNT &= ~BG_ON(m_bg.number);
  }
}

bool Background::enabled() const
{
  return (m_DISPCNT & BG_ON(m_bg.number));
}

unsigned short Background::flags() const
{
  return m_bg.flags;
};
void Background::setFlags(unsigned short var)
{
  m_bg.flags = var;// &(3|BG_MOSAIC_ON|BG_16_COLOR|BG_256_COLOR|BG_WRAP_ON|);
};
unsigned char Background::number() const
{
  return m_bg.number;
};
void Background::setNumber(unsigned char var)
{
  if (var > 3) return;
  m_bg.number = var;
};
unsigned char Background::screenBaseBlock() const
{
  return m_bg.screenBaseBlock;
};
void Background::setScreenBaseBlock(unsigned char var)
{
  if (var > 31) return;
  m_bg.screenBaseBlock = var;
  updateTileMapData();
};
Background::SIZE Background::size() const
{
  return static_cast<Background::SIZE>((m_bg.flags&BG_SIZE(3))>>14);
};
void Background::setSize(Background::SIZE var)
{
  unsigned int i = static_cast<unsigned int>(var);
  if (i > 3)
    return;
  m_bg.flags &= ~BG_SIZE(3);
  m_bg.flags |= BG_SIZE(i);
};

void Background::setRotateFlags(volatile unsigned short * BG_REG)
{
  /*
  s16 * src = &m_bg.PA;
  for (int i = 0 ; i < 4; ++i)
    BG_REG[i] = *src++;
  u32 * bg_reg = (u32*)&BG_REG[4];
  bg_reg[0] = m_bg.DX;
  bg_reg[1] = m_bg.DY;
  */
}

void Background::update()
{
  // update flags
  if (enabled())
    setEnabled();
}
bool Background::wraparound() const
{
  return (m_bg.flags&BG_WRAP_ON)!=0;
};
void Background::setWraparound(bool var)
{
  m_bg.flags &= ~BG_WRAP_ON;
  m_bg.flags |= var?BG_WRAP_ON:0;
};
bool Background::mosaic() const
{
  return (m_bg.flags&BG_MOSAIC_ON)!=0;
};
void Background::setMosaic(bool var)
{
  m_bg.flags &= ~BG_MOSAIC_ON;
  m_bg.flags |= var?BG_MOSAIC_ON:0;
};
signed short Background::xScroll() const
{
  return m_bg.xScroll;
};
void Background::setXScroll(signed short var)
{
  m_bg.xScroll = var;
};
signed short Background::yScroll() const
{
  return m_bg.yScroll;
};
void Background::setYScroll(signed short var)
{
  m_bg.yScroll = var;
};
signed int Background::DX() const
{
  return m_bg.DX;
};
void Background::setDX(signed int var)
{
  m_bg.DX = var;
};
signed int Background::DY() const
{
  return m_bg.DY;
};
void Background::setDY(signed int var)
{
  m_bg.DY = var;
};
signed short Background::PA() const
{
  return m_bg.PA;
};
void Background::setPA(signed short var)
{
  m_bg.PA = var;
};
signed short Background::PB() const
{
  return m_bg.PB;
};
void Background::setPB(signed short var)
{
  m_bg.PB = var;
};
signed short Background::PC() const
{
  return m_bg.PC;
};
void Background::setPC(signed short var)
{
  m_bg.PC = var;
};
signed short Background::PD() const
{
  return m_bg.PD;
};
void Background::setPD(signed short var)
{
  m_bg.PD = var;
};


unsigned short * Background::mapData() const
{
  return m_bg.mapData;
}
unsigned short * Background::tileData() const
{
  return m_bg.tileData;
}

void Background::draw8x8Tile(int xi, int yi, unsigned char * gfx, int flags)
{
  int palette = 0;
  if (color() == 16)
  {
    palette = (flags >> 12)&0xf;
    /*
    if (palette)
      printf("Palette %d %x %d,%d\n", palette, flags, xi, yi);
    */
  }

  for (int j = 0; j < 8; j++) {
    for (int i = 0; i < 4; i++) {
      unsigned char pixelPair = *gfx++;
      int x = i*2 + xi*8;
      int y(0);
      if (flags & TILE_FLIP_VERT) {
	y = (7-j) + yi*8;
      } else {
	y = j + yi*8;
      }
      int increment = 1;
      unsigned char pix1 = pixelPair&0xf;
      unsigned char pix2 = (pixelPair&0xf0)>>4;
      if (flags & TILE_FLIP_HORZ) {
	// horizontal tiles start at the end and work back
        x = (3-i)*2 + xi*8;
        increment = -1;
        pix1 = pix2;
        pix2 = pixelPair&0xf;
      }

      if (pix1) {
        SDLhandler::instance().drawPixel(x, y, m_bg.screen, palette*16+pix1);
      }
      x += increment;
      if (pix2) {
        SDLhandler::instance().drawPixel(x, y, m_bg.screen, palette*16+pix2);
      }
    }
  }
}

void Background::render()
{
  if (not ( m_DISPCNT & BG_ON(m_bg.number) ))
  {
    return;
  }
  for (int x = 0; x < 256/8; ++x)
  {
    for (int y = 0; y < 192/8; ++y)
    {
      int tileId = m_bg.mapData[x+y*32]&0x3FF;
      int flags = m_bg.mapData[x+y*32]&~0x3FF;
      draw8x8Tile(x, y, (unsigned char*)&m_bg.tileData[  tileId * 16 ] , flags );
    }
  }
}


bool Background::operator>(const Background & other) const
{
  if ( (other.m_bg.flags & 3) == (m_bg.flags&3))
    return other.m_bg.number > m_bg.number;
  return ( (other.m_bg.flags & 3) > (m_bg.flags&3));
}
bool Background::operator<(const Background & other) const
{
  return not operator>(other);
}
