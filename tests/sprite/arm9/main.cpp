/*
  Copyright (C) 2008 Richard Quirk

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
#include <vector>
#include "libnds.h"
#include "Canvas.h"
#include "Video.h"
#include "File.h"
#include "Image.h"
#include "Sprite.h"
#include "Palette.h"
static const int screen = 1;
using nds::Image;

static void loadImage(Image & image, unsigned short * oamData)
{
  //sprite.setAffine(256, 0, 0, 256);
  //sprite.rotateScale(0);
  nds::ObjectPalette p(screen);
  p.load(image.palette(), image.paletteSize()*2);

  const unsigned short * data = image.data();
  // need to load in 16x16 tile order

  // 0  1  2  3  4  5  6  7
  // 8  9  10 11
  //
  // load order 0,1,4,5  2,3,6,7
  // tileIndex = tileX + tileY*tilesX
  //
  // for (metaTile = 0; metaTile < metaTileCound; metaTile++)
  //   for (y = 0; y < 2; y++)  
  //     for (x = 0; x < 2; x++)  
  //       tileIndex = metaTile*2 +  y*tilesX + x;
  //
  //
  //int tilesX = image.width() / 8;
  //int tilesY = image.height() / 8;
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
          //int tileIndex = y*tilesX + meta*2 + x;
          // int tileIndex = tileY*tilesX + tileX;
          unsigned short * oamSlot = &oamData[oamIndex*8*8/2];
          oamIndex++;
          // const unsigned char * src = data + (tileIndex*8*bytesPerRow);
          const unsigned short * src = data + (tileY*8*bytesPerRow) + (tileX*8);
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
}

int main(void) {

  Image image("open.png", true);
  nds::Palette p(0);
  p[0] = nds::Color(31, 31, 31);
  nds::Canvas & canvas = nds::Canvas::instance();
  nds::Sprite bigSprite(screen, 64, 32, 0, 256);
  bigSprite.setX(32);
  bigSprite.setY(128);
  std::vector<nds::Sprite*> sprites;
  for (int i = 0; i < 12; i++)
  {
    // would be 4 tiles per sprite (16x16 = 4*8x8) 
    // but the tile count is for 4bpp tiles, 256 color 8bpp require twice as much vram.
    nds::Sprite * sprite(new nds::Sprite(screen, 16, 16, i*8, 256));
    sprite->setX(i*16);
    sprite->setY(i*16);
    sprites.push_back(sprite);

  }
  if (image.isValid())
  {
    const unsigned short * data = image.data();
    if (data)
    {
      unsigned short * oamData = sprites.front()->oamData();
      loadImage(image, oamData);

      canvas.fillRectangle(0, 0, canvas.width(), canvas.height(), nds::Color(21,21,21));
      canvas.endPaint();

      int size = sprites.size();
      for ( int i = 0; i < size ; ++i)
      {
        nds::Sprite * sprite(sprites[i]);
        sprite->setEnabled();
        sprite->update();
      }
      bigSprite.setEnabled();
      bigSprite.update();
    }
  }
  nds::Sprite * white(sprites[0]);
  white->setTranslucent(true);
  white->update();
  nds::Video & main(nds::Video::instance(screen));
  main.blend(nds::Video::BLDMOD_OBJECT,
      0,
      nds::Video::BLDMOD_BG3|nds::Video::BLDMOD_BG2 | nds::Video::BLDMOD_BG1 | nds::Video::BLDMOD_BG0 | nds::Video::BLDMOD_BD
      );

  int affine = 0;
  int fadeLvl = 0;
  while(1)
  {
    affine += 182;
    // Compute sin and cos
    nds::Sprite * spinner(sprites[7]);
    spinner->setRotateScale(true);
    spinner->setRotate(1);
    u16 cosAng = cosLerp(affine) / 16;
    u16 sinAng = sinLerp(affine) / 16;
    spinner->setAffine(cosAng, sinAng, -sinAng, cosAng);
    fadeLvl++;
    fadeLvl&=0xf;
    main.setBlendAB(fadeLvl^0xf,fadeLvl);
    for (int i = 0; i < 12; ++i)
    {
      nds::Sprite * s(sprites[i]);
      int y = s->y();
      if (y == SCREEN_HEIGHT-16)
      {
        y = 0;
      }
      else
      {
        y++;
      }
      s->setY(y);
      s->update();
    }
    swiWaitForVBlank();
  }

  return 0;
}
