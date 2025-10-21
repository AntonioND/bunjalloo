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
#include <algorithm>
#include <functional>
#include "libnds.h"
#include "config_defs.h"
#include "Config.h"
#include "Controller.h"
#include "Document.h"
#include "Delete.h"
#include "Image.h"
#include "Palette.h"
#include "Rectangle.h"
#include "Stylus.h"
#include "Toolbar.h"
#include "URI.h"
#include "Video.h"
#include "View.h"
#include "Wifi9.h"

using nds::Wifi9;

static const int TOOLBAR_Y(SCREEN_HEIGHT-17);
static const int TOOLBAR_Y_VERT(5);
const int Toolbar::TOOLBAR_X(18);
const int Toolbar::TOOLBAR_X_RIGHT(26);
const int Toolbar::TOOLBAR_X_LEFT(10);
const int Toolbar::TOOLBAR_SEP(20);
static const int TIMER_RESET(120);
static const int TOOLBAR_SCREEN(0);
static const int NO_INDEX(-1);

// number of VRAM slots used per icon
const int Toolbar::TILES_PER_ICON(8);

static void setBlendHelper(int a, int b)
{
  nds::Video & main(nds::Video::instance(0));
  main.blend(nds::Video::BLDMOD_OBJECT, 0, nds::Video::BLDMOD_BG3);
  main.setBlendAB(a, b);

}

//VRAM initialisation - done just the once.
static void initSpriteData(unsigned short * oamData)
{
  static bool s_haveInitialised(false);
  if (s_haveInitialised)
    return;

  setBlendHelper(8, 8);

  nds::Image image(DATADIR "/fonts/toolbar.png", true);
  if (image.isValid())
  {
    const unsigned char * data = (const unsigned char *)image.data();
    if (data)
    {
      // need to load in 16x16 tile order
      int bytesPerRow = image.width();
      int tiles16x = image.width() / 16;
      int tiles16y = image.height() / 16;
      int oamIndex = 0;
      for (int metay = 0; metay < tiles16y ; metay++)
      {
        for (int metax = 0; metax < tiles16x ; metax++)
        {
          // loop over the 8 by 8 tiles
          // startx = x + metax * 2;
          // starty = y + metay * 2;
          // tileIndex = starty * tilesX + startx;
          for (int y = 0; y < 2; y++)
          {
            for (int x = 0; x < 2; x++)
            {
              // copy tile at tileX, tileY to next oam slot
              int tileY = y + metay*2;
              int tileX = x + metax*2;
              // index into oam:
              unsigned short * oamSlot = &oamData[oamIndex*8*8/2];
              oamIndex++;
              const unsigned char * src = data + (tileY*8*bytesPerRow) + (tileX*8);
              for (int row = 0; row < 8; row++)
              {
                for (int col = 0; col < 4; col++, src+=2)
                {
                  unsigned short pixelPair = src[0]|(src[1]<<8);
                  *oamSlot++ = pixelPair;
                }
                src -= 8;
                src += bytesPerRow;
              }

            }
          }
        }
      }
      nds::ObjectPalette p(TOOLBAR_SCREEN);
      p.load(image.palette(), image.paletteSize()*2);
    }
  }
  s_haveInitialised = true;
}

Toolbar::Toolbar(View & view, int entries):
  m_view(view),
  m_visible(false),
  m_touchedIndex(NO_INDEX),
  m_position(BOTTOM)
{
  m_view.document().registerView(this);
  for (int i = 0; i < entries; i++)
  {
    // would be 4 tiles per sprite (16x16 = 4*8x8)
    // but the tile count is for 4bpp tiles, 256 color 8bpp require twice as much vram.
    nds::Sprite * sprite(new nds::Sprite(TOOLBAR_SCREEN, 16, 16, i*TILES_PER_ICON, 256));
    m_sprites.push_back(sprite);
  }
  initSpriteData(m_sprites.front()->oamData());
  layout();
  Stylus::instance()->registerListener(this);
}

void Toolbar::layout()
{
  for (unsigned int i = 0; i < m_sprites.size(); i++)
  {
    int x, y;
    switch (m_position)
    {
      case LEFT:
        y = TOOLBAR_Y_VERT + (i*TOOLBAR_SEP);
        x = TOOLBAR_X_LEFT;
        break;

      case RIGHT:
        y = TOOLBAR_Y_VERT + (i*TOOLBAR_SEP);
        x = SCREEN_WIDTH - TOOLBAR_X_RIGHT;
        break;

      case TOP:
        x = TOOLBAR_X + (i*TOOLBAR_SEP);
        y = TOOLBAR_Y_VERT;
        break;

      default:
      case BOTTOM:
        x = TOOLBAR_X + (i*TOOLBAR_SEP);
        y = TOOLBAR_Y;
        break;
    }
    m_sprites[i]->setX(x);
    m_sprites[i]->setY(y);
  }
  setVisible(visible());
}

Toolbar::~Toolbar()
{
  for_each(m_sprites.begin(), m_sprites.end(), delete_ptr());
  m_view.document().unregisterView(this);
  Stylus::instance()->unregisterListener(this);
}

void Toolbar::notify()
{
  updateIcons();
}

bool Toolbar::visible() const
{
  return m_visible;
}

void Toolbar::setVisible(bool visible)
{
  m_visible = visible;
  for_each(m_sprites.begin(), m_sprites.end(), std::bind2nd(std::mem_fun(&nds::Sprite::setEnabled), visible));
  for_each(m_sprites.begin(), m_sprites.end(), std::mem_fun(&nds::Sprite::update));
}

int Toolbar::touchedIndex(int x, int y) const
{
  if (not visible())
  {
    return NO_INDEX;
  }

  nds::Sprite * first(m_sprites.front());
  nds::Sprite * last(m_sprites.back());
  nds::Rectangle touchZone(first->x(), first->y()+SCREEN_HEIGHT,
    last->x() - first->x() + first->width(),
    last->y() - first->y() + first->height());
  if (touchZone.hit(x, y))
  {
    int index = 0;
    for (SpriteVector::const_iterator it(m_sprites.begin());
        it != m_sprites.end();
        ++it, ++index)
    {
      nds::Sprite * sprite(*it);
      nds::Rectangle rect(sprite->x(), sprite->y()+SCREEN_HEIGHT, sprite->width(), sprite->height());
      if (sprite->enabled() and rect.hit(x, y))
      {
        return index;
      }
    }
  }
  return NO_INDEX;
}

bool Toolbar::stylusUp(const Stylus * stylus)
{
  if (not visible())
    return false;
  // are we touching something...
  if (m_touchedIndex != NO_INDEX)
  {
    int finalIndex = touchedIndex(stylus->lastX(), stylus->lastY());
    if (finalIndex == m_touchedIndex)
    {
      handlePress(m_touchedIndex);
    }
    if (m_touchedIndex >= 0)
      m_sprites[m_touchedIndex]->setTranslucent(false);
  }
  setBlendHelper(8, 8);
  return false;
}

bool Toolbar::stylusDownFirst(const Stylus * stylus)
{
  if (not visible())
    return false;

  bool consumed = false;
  m_touchedIndex = touchedIndex(stylus->startX(), stylus->startY());
  if (m_touchedIndex != NO_INDEX)
  {
    setBlendHelper(31, 5);
    m_sprites[m_touchedIndex]->setTranslucent(true);
    consumed = true;
  }
  return consumed;
}

bool Toolbar::stylusDownRepeat(const Stylus * stylus)
{
  return false;
}

bool Toolbar::stylusDown(const Stylus * stylus)
{
  if (not visible())
    return false;
  bool consumed = false;
  int newIndex = touchedIndex(stylus->lastX(), stylus->lastY());
  if (m_touchedIndex != NO_INDEX and newIndex != m_touchedIndex)
  {
    m_sprites[m_touchedIndex]->setTranslucent(false);
    setBlendHelper(8, 8);
    m_touchedIndex = NO_INDEX;
    consumed = true;
  }
  return consumed;
}

void Toolbar::cyclePosition()
{
  switch (m_position)
  {
    case TOP:
      m_position = RIGHT;
      break;
    case RIGHT:
      m_position = BOTTOM;
      break;
    case BOTTOM:
      m_position = LEFT;
      break;
    case LEFT:
      m_position = TOP;
      break;
    default:
      m_position = BOTTOM;
      break;
  }
  layout();
}

void Toolbar::showCursor(int x, int y, int cursorid)
{
  // plonk a cursor at the given position
  // coords in world space
  y -= 192;
  if (y < 0)
    return;
  m_cursorSprite->setTile(cursorid * TILES_PER_ICON);
  m_cursorSprite->setXY(x, y);
  m_cursorSprite->setEnabled();
  m_cursorSprite->update();
}

void Toolbar::hideCursor()
{
  m_cursorSprite->setEnabled(false);
  m_cursorSprite->update();
}

Toolbar::Position Toolbar::position() const
{
  return m_position;
}

