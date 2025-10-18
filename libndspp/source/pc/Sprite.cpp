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
#include "Sprite.h"
#include "HardwareHandler.cpp"
#include "SpriteHandler.h"
#include "SDLhandler.h"
#include <stdio.h>

using namespace nds;
typedef unsigned short uint16;
static SpriteEntry OAM[128];
static SpriteEntry OAM_SUB[128];

static SpriteEntry s_sprites[2][128];
static pSpriteRotation s_affine = (pSpriteRotation)(void*)(&s_sprites[0]);
bool Sprite::s_used[2][128];

Sprite::Sprite(int screen):
  m_screen(screen),
  m_index(-1),
  m_used(s_used[screen==0?screen:1]),
  m_OAM(screen==0?(unsigned short*)OAM:(unsigned short*)OAM_SUB)
{
  initialise();
}

Sprite::Sprite(int screen, int w, int h, int tile, int colors):
  m_screen(screen),
  m_index(-1),
  m_used(s_used[screen==0?screen:1]),
  m_OAM(screen==0?(unsigned short*)OAM:(unsigned short*)OAM_SUB)
{
  initialise();
  this->setColors(colors);
  this->setTile(tile);
  this->setSize(w,h);
}

Sprite::~Sprite()
{
  // make sure sprite is not drawn
  setDoubleSize(true);
  setRotateScale(false);
  update();
  m_used[m_index] = false;
  SpriteHandler::remove(this);
}

int Sprite::y() const
{
  if (!valid()) return 0;
  // unpack y
  return OBJ_Y(s_sprites[m_screen][m_index].attribute[0]);
}

void Sprite::setY(int y)
{ 
  if (!valid()) return;
  // pack y
  SpriteEntry & sp = s_sprites[m_screen][m_index];
  sp.attribute[0] &= 0xFF00;
  sp.attribute[0] |= y;
}


bool Sprite::rotateScale() const
{ 
  if (!valid()) return false;
  return s_sprites[m_screen][m_index].attribute[0]&ATTR0_ROTSCALE;
}

void Sprite::setAttrBit(unsigned int attrn, unsigned int bits, bool set, unsigned int clear)
{
  SpriteEntry & sp = s_sprites[m_screen][m_index];
  unsigned short & attr = ( attrn==0 ? sp.attribute[0]: ( attrn==1 ? sp.attribute[1] : sp.attribute[2] ) );
  if (clear == 0) {
    clear = ~bits;
  } else {
    clear = ~clear;
  }
  attr &= clear;
  attr |= set?bits:0;
}

void Sprite::setRotateScale(bool s)
{ 
  if (!valid()) return;
  setAttrBit(0,ATTR0_ROTSCALE,s);
}

bool Sprite::doubleSize() const
{ 
  if (!valid()) return false;
  return s_sprites[m_screen][m_index].attribute[0]&ATTR0_DISABLED;
}

void Sprite::setDoubleSize(bool s)
{ 
  if (!valid()) return;
  setAttrBit(0,ATTR0_DISABLED,s);
}

unsigned int Sprite::mode() const
{ 
  if (!valid()) return 0;
  return ((s_sprites[m_screen][m_index].attribute[0]&(ATTR0_BMP))>>10);
}

void Sprite::setMode(unsigned int mode)
{
  if (!valid()) return;
  if (mode>3)
    return;
  setAttrBit(0,(mode)<<10,mode,ATTR0_BMP);
}

bool Sprite::mosaic() const
{ 
  if (!valid()) return false;
  return s_sprites[m_screen][m_index].attribute[0]&ATTR0_MOSAIC;
}

void Sprite::setMosaic(bool m)
{ 
  if (!valid()) return;
  setAttrBit(0,ATTR0_MOSAIC,m);
}
unsigned int Sprite::colors() const
{ 
  if (!valid()) return 0;
  return (s_sprites[m_screen][m_index].attribute[0]&ATTR0_COLOR_256)?256:16;
}

void Sprite::setColors(unsigned int col)
{ 
  if (!valid()) return;
  switch (col)
  {
    case 16:
      setAttrBit(0,ATTR0_COLOR_16,true,ATTR0_COLOR_256);
      break;
    case 256:
      setAttrBit(0,ATTR0_COLOR_256,true);
      break;
    default:
      break;
  }
}

// attribute[1]
unsigned int Sprite::x() const
{
  if (!valid()) return 0;
  return OBJ_X(s_sprites[m_screen][m_index].attribute[1]);
}

void Sprite::setX(unsigned int x)
{ 
  if (!valid()) return;
  SpriteEntry & sp = s_sprites[m_screen][m_index];
  sp.attribute[1] &= 0xFE00;
  sp.attribute[1] |= x&0x1FF;
}

// from gba_sprites.h
// attribute 0
#define OBJ_SHAPE(m)		((m)<<14)
// attribute 1
#define OBJ_SIZE(m)		((m)<<14)
// attribute 2
#define OBJ_CHAR(m)		((m)&0x03ff)
enum SPRITE_SIZECODE {        // v   hex   bits atr0 atr1
		Sprite_8x8,   // 0    0    0000  SQ  8 
		Sprite_16x16, // 1    1    0001  SQ  16
		Sprite_32x32, // 2    2    0010  SQ  32
		Sprite_64x64, // 3    3    0011  SQ  64
		Sprite_16x8,  // 4    4    0100  WI  8
		Sprite_32x8,  // 5    5    0101  WI  16
		Sprite_32x16, // 6    6    0110  WI  32
		Sprite_64x32, // 7    7    0111  WI  64
		Sprite_8x16,  // 8    8    1000  TA  8
		Sprite_8x32,  // 9    9    1001  TA  16
		Sprite_16x32, // 10   A    1010  TA  32
		Sprite_32x64  // 11   B    1011  TA  64
};


// shape is SQUARE TALL WIDE 
Sprite::SPRITE_SHAPE Sprite::shape() const
{ 
  if (!valid()) return NOT_VALID;
  return static_cast<Sprite::SPRITE_SHAPE>((s_sprites[m_screen][m_index].attribute[0]&OBJ_SHAPE(3))>>14);
}

void Sprite::setShape(Sprite::SPRITE_SHAPE shape)
{ 
  if (!valid()) return;
  unsigned int s = static_cast<unsigned int>(shape);
  if (s > 3)
    return;
  setAttrBit(0,OBJ_SHAPE(s),true,OBJ_SHAPE(3));
}

unsigned int Sprite::rotate() const
{ 
  if (!valid()) return 0;
  return (s_sprites[m_screen][m_index].attribute[1]>>9)&15;
}

void Sprite::setRotate(unsigned int rotate)
{ 
  if (!valid()) return;
  if (rotate > 15)
    return;
  setAttrBit(1,ATTR1_ROTDATA(rotate),true,ATTR1_ROTDATA(15));
}

bool Sprite::hflip() const
{ 
  if (!valid()) return false;
  return (bool)(s_sprites[m_screen][m_index].attribute[1]&ATTR1_FLIP_X);
}

void Sprite::setHflip(bool b)
{ 
  if (!valid()) return;
  setAttrBit(1,ATTR1_FLIP_X,b);
}

bool Sprite::vflip() const
{ 
  if (!valid()) return false;
  return (s_sprites[m_screen][m_index].attribute[1]&ATTR1_FLIP_Y);
}

void Sprite::setVflip(bool b)
{
  if (!valid()) return;
  setAttrBit(1,ATTR1_FLIP_Y,b);
}

static bool size_to_sprite_sizecode(unsigned int w, unsigned int h, SPRITE_SIZECODE & size)
{
  // given w and h, return size
  bool ok = false;
  if (w == 8) {
    switch (h)
    {
      case 8:
        size = Sprite_8x8;
        ok = true;
        break;
      case 16:
        size = Sprite_8x16;
        ok = true;
        break;
      case 32:
        size = Sprite_8x32;
        ok = true;
        break;
    }
  } else if (w == 16) {
    switch (h)
    {
      case 8:
        size = Sprite_16x8;
        ok = true;
        break;
      case 16:
        size = Sprite_16x16;
        ok = true;
        break;
      case 32:
        size = Sprite_16x32;
        ok = true;
        break;
    }
  } else if (w == 32) {
    switch (h)
    {
      case 8:
        size = Sprite_32x8;
        ok = true;
        break;
      case 16:
        size = Sprite_32x16;
        ok = true;
        break;
      case 32:
        size = Sprite_32x32;
        ok = true;
        break;
      case 64:
        size = Sprite_32x64;
        ok = true;
        break;
    }
  } else if (w == 64) {
    switch (h)
    {
      case 32:
        size = Sprite_64x32;
        ok = true;
        break;
      case 64:
        size = Sprite_64x64;
        ok = true;
        break;
    }
  }
  return ok;
}

#define SPRITE_SIZECODE_TO_SIZE(n,m) case Sprite_##n##x##m:\
{\
  w = n; h = m; \
  break; \
}

static void sprite_sizecode_to_size(SPRITE_SIZECODE size, unsigned int & w, unsigned int & h)
{
  switch (size)
  {
    SPRITE_SIZECODE_TO_SIZE(8,8);
    SPRITE_SIZECODE_TO_SIZE(16,16);
    SPRITE_SIZECODE_TO_SIZE(32,32);
    SPRITE_SIZECODE_TO_SIZE(64,64);
    SPRITE_SIZECODE_TO_SIZE(16,8);
    SPRITE_SIZECODE_TO_SIZE(32,8);
    SPRITE_SIZECODE_TO_SIZE(32,16);
    SPRITE_SIZECODE_TO_SIZE(64,32);
    SPRITE_SIZECODE_TO_SIZE(8,16);
    SPRITE_SIZECODE_TO_SIZE(8,32);
    SPRITE_SIZECODE_TO_SIZE(16,32);
    SPRITE_SIZECODE_TO_SIZE(32,64);
  }
}

void Sprite::getSize(unsigned int &w, unsigned int &h) const
{
  if (!valid()) return;
  unsigned int shapeFlag = (s_sprites[m_screen][m_index].attribute[0]>>14);
  unsigned int sizeFlag  = (s_sprites[m_screen][m_index].attribute[1]>>14);
  SPRITE_SIZECODE size = static_cast<SPRITE_SIZECODE>( sizeFlag | (shapeFlag<<2) );
  sprite_sizecode_to_size(size, w, h);
}

void Sprite::setSize(unsigned int w, unsigned int h)
{
  if (!valid()) return;
  SPRITE_SIZECODE size;
  bool success = size_to_sprite_sizecode(w, h, size);
  if (success) {
    unsigned int shapeFlag = (size& 0xC)>>2;
    unsigned int sizeFlag = (size & 0x3);
    setAttrBit(0,OBJ_SHAPE(shapeFlag),true,OBJ_SHAPE(3));
    setAttrBit(1,OBJ_SIZE(sizeFlag),true,OBJ_SIZE(3));
  }
}



// SPRITE_SIZECODE size OBJ_SIZE Sprite_32x8 etc
unsigned int Sprite::width() const
{ 
  if (!valid()) return 0;
  unsigned int w, h;
  getSize(w, h);
  return w;
}

void Sprite::setWidth(unsigned int width)
{ 
  if (!valid()) return;
  unsigned int w, h;
  getSize(w, h);
  setSize(width, h);
}

unsigned int Sprite::height() const
{
  if (!valid()) return 0;
  unsigned int w, h;
  getSize(w, h);
  return h;
}

void Sprite::setHeight(unsigned int height)
{ 
  if (!valid()) return;
  unsigned int w, h;
  getSize(w, h);
  setSize(w, height);
}


// attribute[2]
unsigned int Sprite::tile() const
{
  if (!valid()) return 0;
  return OBJ_CHAR(s_sprites[m_screen][m_index].attribute[2]);
}

void Sprite::setTile(unsigned int t)
{ 
  if (!valid()) return;
  setAttrBit(2,OBJ_CHAR(t),true,OBJ_CHAR(0xFFFF));
}

unsigned int Sprite::priority() const
{
  if (!valid()) return 0;
  return (s_sprites[m_screen][m_index].attribute[2]&ATTR2_PRIORITY(3))>>10;
}

void Sprite::setPriority(unsigned int priority)
{ 
  if (!valid()) return;
  setAttrBit(2,ATTR2_PRIORITY(priority&3),true,ATTR2_PRIORITY(3));
}

unsigned int Sprite::palette() const
{ 
  if (!valid()) return 0;
  return (s_sprites[m_screen][m_index].attribute[2] & ATTR2_PALETTE(15))>>12;
}

void Sprite::setPalette(unsigned int palette)
{ 
  if (!valid()) return;
  setAttrBit(2,ATTR2_PALETTE(palette&15),true,ATTR2_PALETTE(15));
}

bool Sprite::translucent() const
{ 
  if (!valid()) return false;
  return s_sprites[m_screen][m_index].attribute[0]&ATTR0_TYPE_BLENDED;
}

void Sprite::setTranslucent(bool b)
{ 
  if (!valid()) return;
  setAttrBit(0,ATTR0_TYPE_BLENDED,b);
}

bool Sprite::window() const
{ 
  if (!valid()) return false;
  return s_sprites[m_screen][m_index].attribute[0]&ATTR0_TYPE_WINDOWED;
}

void Sprite::setWindow(bool b)
{ 
  if (!valid()) return;
  setAttrBit(0,ATTR0_TYPE_WINDOWED,b);
}


void Sprite::getAffine( 
    signed short & pa,
    signed short & pb,
    signed short & pc,
    signed short & pd) const
{
  if (!valid()) return;
  pa = s_affine[m_index].hdx;
  pb = s_affine[m_index].hdy;
  pc = s_affine[m_index].vdx;
  pd = s_affine[m_index].vdy;
}
void Sprite::setAffine( 
    signed short pa,
    signed short pb,
    signed short pc,
    signed short pd)
{
  if (!valid()) return;
   s_affine[m_index].hdx = pa;
   s_affine[m_index].hdy = pb;
   s_affine[m_index].vdx = pc;
   s_affine[m_index].vdy = pd;
}

void Sprite::update() const
{
  if (!valid()) return;
  SpriteEntry * oam = (SpriteEntry*)m_OAM;
  //DC_FlushRange(&s_sprites[m_screen][m_index],sizeof(SpriteEntry));
  if (memcmp(&s_sprites[m_screen][m_index], &oam[m_index], sizeof(SpriteEntry)) != 0)
  {
    dmaCopy(&s_sprites[m_screen][m_index], &oam[m_index], sizeof(SpriteEntry));
    SDLhandler::instance().setDirty();
  }
}

void Sprite::initialise(bool sync)
{
  if (!valid())
  {
    for (int i = 0; i < 128; i++)
    {
      if (!m_used[i]) {
        m_index = i;
        m_used[i] = true;
        s_sprites[m_screen][m_index].attribute[0] = 0;
        s_sprites[m_screen][m_index].attribute[1] = 0;
        s_sprites[m_screen][m_index].attribute[2] = 0;
        if (sync)
          update();
        SpriteHandler::add(this);
        break;
      }
    }
  }
}

void Sprite::copyOAM(int screen)
{
  if (screen == 0 || screen == 2) {
    //DC_FlushRange(s_sprites[0],128*sizeof(SpriteEntry));
    if (memcmp(s_sprites[0], OAM_SUB, 128 * sizeof(SpriteEntry)) != 0)
    {
      SDLhandler::instance().setDirty();
    }
    dmaCopy(s_sprites[0], OAM, 128 * sizeof(SpriteEntry));
  }
  if (screen == 1 || screen == 2) {
    //DC_FlushRange(s_sprites[1],128*sizeof(SpriteEntry));
    if (memcmp(s_sprites[1], OAM_SUB, 128 * sizeof(SpriteEntry)) != 0)
    {
      SDLhandler::instance().setDirty();
    }
    dmaCopy(s_sprites[1], OAM_SUB, 128 * sizeof(SpriteEntry));
  }
}

unsigned short * Sprite::oamData() const
{
  unsigned int thisTile = this->tile();
  /*if (this->color() == 256) {
    thisTile /= 2;
  }*/
  uint16 * oamRam = SDLhandler::instance().spriteGfx();
  if (m_screen) {
    oamRam = SDLhandler::instance().subSpriteGfx();
  }
  return &oamRam[thisTile*16];
}

// real worker funcions
void Sprite::loadTileData(const void * tileData, unsigned int length)
{
  uint16 * oamRam = this->oamData();
  dmaCopy(tileData, oamRam, length);
}
void Sprite::loadTileMapData(const void * t, const void * m, unsigned int length)
{
  // load data from tile data referencing the map - "emulates" the tile data in the bg
  const unsigned short* gfx = (const unsigned short*)t;
  const unsigned short* map = (const unsigned short*)m;
  unsigned int thisTile = this->tile();
  if (this->colors() == 256) {
    thisTile /= 2;
  }
  // ???
  thisTile *= 64;
  uint16 * oamRam = SDLhandler::instance().spriteGfx();
  if (m_screen) {
    oamRam = SDLhandler::instance().subSpriteGfx();
  }

  int amount = this->colors()==256?64:32;
  int stepSize = this->colors()==256?32:16;
  // get start address in screen
  for (unsigned int loop = 0; loop < length; loop++) {
    // id = <the tile id in the map file at this position>
    int id = map[loop];
    dmaCopy(&(gfx[id*stepSize]), &oamRam[loop*stepSize+thisTile],amount);
  }
}

void Sprite::disableAll(int screen)
{
  int start(screen==1?1:0);
  SpriteEntry * sprite = s_sprites[start];
  int end(screen==2?256:128);
  for (int i = 0; i < end; ++i)
    sprite[i].attribute[0] = ATTR0_DISABLED;
  copyOAM(screen);
}

unsigned short Sprite::attribute(int i) const
{
  return s_sprites[m_screen][m_index].attribute[i&3];
}

void Sprite::draw8x8Tile(int xPos, int yPos, int xi, int yi, const unsigned char * gfx)
{
  if (translucent())
  {
    SDLhandler::instance().setAlpha(128);
  }
  for (int j = 0; j < 8; j++) {
    for (int i = 0; i < 8; i++) {
      int x = i + xi*8;
      int y = j + yi*8;
      unsigned char pixelPair = *gfx++;
      int pix = pixelPair&0xf;
      if (colors() == 256)
      {
        pix = pixelPair;
      }

      if (hflip()) {
	// horizontal tiles start at the end and work back
        x = 7-i + xi*8;
      }
      if (pix) {
        SDLhandler::instance().drawPixel(xPos+x, yPos+y, 2+m_screen, pix);
      }
    }
  }
  if (translucent())
  {
    SDLhandler::instance().setAlpha(255);
  }
}
void Sprite::render()
{
  // draw the sprite...
  if (not valid())
    return;
  SpriteEntry & sprite = ((SpriteEntry*)m_OAM)[m_index];
  if ( (sprite.attribute[0] & ATTR0_DISABLED) )
    return;

  unsigned int w(0),h(0);
  getSize(w,h);
  uint16 * oamRam = this->oamData();
  int xPos = OBJ_X(sprite.attribute[1]);
  int yPos = OBJ_Y(sprite.attribute[0]);
  if (rotateScale() and doubleSize())
  {
    xPos += width()/2;
    yPos += width()/2;
  }
  int position = 0;
  for (unsigned int y = 0; y < h/8; ++y)
  {
    for (unsigned int x = 0; x < w/8; ++x)
    {
      int i = x;
      if (hflip())
      {
        i = (w/8) - 1 - x;
      }
      draw8x8Tile(xPos, yPos, i, y, (unsigned char*)&oamRam[position]);
      position += 32;
    }
  }
}

bool Sprite::operator<(const Sprite & other) const
{
  return other.m_index > m_index;
}
bool Sprite::operator>(const Sprite & other) const
{
  return not operator<(other);
}
