/*
  This file incorporates work covered by the following copyright and
  permission notice:

    Copyright 2006, 2007 Pedro J. Estébanez.

    Okiwi is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 3 of the License, or
    (at your option) any later version.

    Foobar is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/


#include <stdio.h>
#include <unistd.h>
#include <getopt.h>
#include <assert.h>
#include <stdint.h>
#include <ft2build.h>
#include FT_FREETYPE_H
#include <freetype/ftglyph.h>
#include <freetype/ftbitmap.h>

#include "file_wrapper.h"

unsigned int from_fix8_round(unsigned int x)
{
  int overflow(((x&0xff) > 128) ? 1: 0);
  return (x>>8) + overflow;
}

typedef unsigned char uint8;
typedef unsigned short uint16;
typedef signed char sint8;

struct t_glyphBitmap {
  uint8 width, height;
  uint8 *bitmap;
};

struct t_charMapEntry {
  unsigned int charCode;
  unsigned int glyphIndex;
};

struct t_charMap {
  t_charMapEntry *entries;
  uint16 size;
};

struct t_prerenderedGlyph {
  t_glyphBitmap image;
  sint8 deltaX, deltaY;
  uint16 advanceX;			// 24.8
};

struct t_prerenderedSet {
  // Set identification
  uint8 face;
  uint8 size;

  // Vertical metrics
  int minAscender, maxDescender;

  // Prerendered images
  t_prerenderedGlyph *glyphs;
  int numGlyphs;
};

// on one side we have the glyph data:
//
// [A], [B], ..., [Z], [ñ], etc
//
// then we have the map data that points to the index:
//
// ['A', 0], ['B', 1]... ['ñ', 99], etc
//
// When we pass in an "A", how do we find the right entry in the char map?
// naïve O(n) search :-/

void convertFTBitmapToOwnGlyphBitmap(FT_Bitmap ftBitmap, t_glyphBitmap *gfxBitmap)
{
  int x, y;

  // Initialize pointers to both bitmaps
  uint8 *gfxbm = gfxBitmap->bitmap;
  unsigned char * ftbm = ftBitmap.buffer;

  // Transform bitmaps

  for (y = 0; y < ftBitmap.rows; y ++) {

    x = 0;
    while (x < ftBitmap.width) {

      uint8 chunk, color;

      // First pixel in output byte
      if (*ftbm % 64 >= 32) { color = ((*ftbm) >> 6) + 1; } else { color = (*ftbm) >> 6; }
      if (color > 0x3) color = 0x3;
      chunk = color << 6;
      ftbm ++;
      x ++;

      // Second pixel in output byte
      if (x < ftBitmap.width) {
        if (*ftbm % 64 >= 32) { color = ((*ftbm) >> 6) + 1; } else { color = (*ftbm) >> 6; }
        if (color > 0x3) color = 0x3;
        chunk = chunk | (color << 4);
        ftbm ++;
        x ++;

        // Third pixel in output byte
        if (x < ftBitmap.width) {
          if (*ftbm % 64 >= 32) { color = ((*ftbm) >> 6) + 1; } else { color = (*ftbm) >> 6; }
          if (color > 0x3) color = 0x3;
          chunk = chunk | (color << 2);
          ftbm ++;
          x ++;

          // Fourth pixel in output byte
          if (x < ftBitmap.width) {
            if (*ftbm % 64 >= 32) { color = ((*ftbm) >> 6) + 1; } else { color = (*ftbm) >> 6; }
            if (color > 0x3) color = 0x3;
            chunk = chunk | color;
            ftbm ++;
            x ++;
          }
        }
      }

      *gfxbm++ = chunk;
    }

    // Bypass FreeType bitmap padding
    ftbm += ftBitmap.pitch - ftBitmap.width;
  }
}

int prerenderSet(FT_Library ftlib, const char *sourceFaceFilename, int face, int size, t_prerenderedSet *prerenderedSet)
{
  FT_Face ftFace;
  // Let FreeType load source font

  FT_Open_Args ftoa;
  memset(&ftoa, 0, sizeof(FT_Open_Args));
  ftoa.pathname = const_cast<char*>(sourceFaceFilename);
  ftoa.flags = FT_OPEN_PATHNAME;

  if (FT_Open_Face(ftlib, &ftoa, 0, &ftFace) != 0) return 0;

  // Record some information
  prerenderedSet->face = face;
  prerenderedSet->size = size;
  printf("Font size %d\n", size);

  prerenderedSet->numGlyphs = ftFace->num_glyphs;
  printf("Number of glyphs: %d\n", prerenderedSet->numGlyphs);

  prerenderedSet->maxDescender  = -ftFace->bbox.yMin * prerenderedSet->size / ftFace->units_per_EM;
  prerenderedSet->minAscender   = -ftFace->bbox.yMax * prerenderedSet->size / ftFace->units_per_EM;

  printf("Size of glyph data: %d bytes\n", prerenderedSet->numGlyphs * sizeof(t_prerenderedGlyph));
  // Allocate memory for glyph vector
  if ((prerenderedSet->glyphs =
        static_cast<t_prerenderedGlyph*>(malloc(prerenderedSet->numGlyphs *
            sizeof(t_prerenderedGlyph)))) == NULL) {
    return 0;
  }

  // Set font size
  FT_Set_Pixel_Sizes(ftFace, 0, prerenderedSet->size);

  // Render each glyph
  int i;
  t_prerenderedGlyph *prerenderedGlyph = prerenderedSet->glyphs;
  FT_Glyph glyph;
  unsigned int allocated = 0;

  for (i = 0; i < prerenderedSet->numGlyphs; i ++) {

    // Load glyph
    if (FT_Load_Glyph(ftFace, i, FT_LOAD_DEFAULT) != 0) {
      return 0;
    }

    glyph = NULL;

    if (FT_Get_Glyph(ftFace->glyph, &glyph) != 0) {
      return 0;
    }

    // Render glyph
    if (FT_Glyph_To_Bitmap(&glyph, FT_RENDER_MODE_NORMAL, 0, 1) != 0) {
      return 0;
    }

    // Prepare output bitmap

    FT_BitmapGlyph bitmapGlyph = (FT_BitmapGlyph)glyph;

    t_glyphBitmap *image = &prerenderedGlyph->image;

    assert(bitmapGlyph->bitmap.width >= 0);
    assert(bitmapGlyph->bitmap.width < 256);
    image->width  = bitmapGlyph->bitmap.width;

    // Make width multiple of 4
    if ((image->width & 0x3) != 0) {
      image->width += 4 - (image->width & 0x3);
    }

    assert(bitmapGlyph->bitmap.rows >= 0);
    assert(bitmapGlyph->bitmap.rows < 256);
    image->height = bitmapGlyph->bitmap.rows;

    if (image->width > 0 && image->height > 0) {
      if ((image->bitmap = static_cast<uint8*>(malloc((image->width * image->height) / 4))) == NULL) {
        return 0;
      }
      allocated += (image->width * image->height) / 4;

      // Convert FreeType bitmap to our own format
      convertFTBitmapToOwnGlyphBitmap(bitmapGlyph->bitmap, image);

    } else {
      image->bitmap = NULL;
    }

    // Fill some glyph fields
    assert(bitmapGlyph->left >= -127);
    assert(bitmapGlyph->left < 128);
    assert(bitmapGlyph->top >= -127);
    assert(bitmapGlyph->top < 128);
    prerenderedGlyph->deltaX	= bitmapGlyph->left;
    prerenderedGlyph->deltaY	= -bitmapGlyph->top;
    assert(from_fix8_round(ftFace->glyph->linearHoriAdvance) >= 0);
    assert(from_fix8_round(ftFace->glyph->linearHoriAdvance) <= 0xffff);
    prerenderedGlyph->advanceX	= from_fix8_round(ftFace->glyph->linearHoriAdvance);

    // Dispose FreeType glyph
    FT_Done_Glyph(glyph);

    // Go to next output glyph
    prerenderedGlyph++;
  }
  printf("image mem allocated: %d bytes\n", allocated);

  // Dispose FreeType font
  FT_Done_Face(ftFace);

  return 1;
}

int saveSetToFile(const char *filename, const t_prerenderedSet *prerenderedSet)
{
  FileWrapper fw;
  fw.open(filename, "w");

  if (not fw.is_open())
    return 0;

  // Write header
  fw.write8(prerenderedSet->face);
  fw.write8(prerenderedSet->size);
  fw.write32(prerenderedSet->minAscender);
  fw.write32(prerenderedSet->maxDescender);
  fw.write32(prerenderedSet->numGlyphs);

  // Write glyphs
  t_prerenderedGlyph *glyph = prerenderedSet->glyphs;

  for (int i = 0; i < prerenderedSet->numGlyphs; ++i, ++glyph) {
    // t_prerenderedGlyph
    fw.write8(glyph->deltaX);
    fw.write8(glyph->deltaY);
    fw.write16(glyph->advanceX);
//    printf("advanceX %d\n", glyph->advanceX);
    // write bitmap data
    fw.write8(glyph->image.width);
    fw.write8(glyph->image.height);
    // write data:
    fw.writeData(glyph->image.bitmap, (glyph->image.width * glyph->image.height) / 4);
  }
  return 1;
}

int gatherCharmap(FT_Library ftlib, const char *sourceFaceFilename, t_charMap *charMap)
{
  FT_Face ftFace;

  // Let FreeType load source font

  FT_Open_Args ftoa;
  memset(&ftoa, 0, sizeof(FT_Open_Args));
  ftoa.pathname = const_cast<char*>(sourceFaceFilename);
  ftoa.flags = FT_OPEN_PATHNAME;

  if (FT_Open_Face(ftlib, &ftoa, 0, &ftFace) != 0) return 0;

  // Initialize output charmap
  charMap->entries = NULL;
  charMap->size = 0;

  // Process all character codes
  FT_ULong charCode;
  FT_UInt gIndex;

  // see how many chars there are
  charCode = FT_Get_First_Char(ftFace, &gIndex);
  unsigned int size(0);
  while (gIndex != 0) {
    size++;
    charCode = FT_Get_Next_Char(ftFace, charCode, &gIndex);
  }
  assert(size < 0x10000);
  charMap->size = size;
  printf("There are this many chars: %d\n", charMap->size);
  charMap->entries = static_cast<t_charMapEntry*>(malloc(charMap->size * sizeof(t_charMapEntry)));

  t_charMapEntry *charMapEntry = charMap->entries;
  t_charMapEntry last = {0, 0};
  charCode = FT_Get_First_Char(ftFace, &gIndex);
  while (gIndex != 0) {
    charMapEntry->charCode   = charCode;
    charMapEntry->glyphIndex = gIndex;
    // are they in order?
    assert(charMapEntry->charCode > last.charCode);
    charMapEntry++;
    last.charCode = charCode;
    last.glyphIndex = gIndex;
    charCode = FT_Get_Next_Char(ftFace, charCode, &gIndex);
  }

  // Dispose FreeType font
  FT_Done_Face(ftFace);

  return 1;
}

void saveCharmap(const char *filename, t_charMap *charMap)
{
  printf("Save to %s\n", filename);
  FileWrapper fw;
  fw.open(filename, "w");
  if (not fw.is_open())
    return;

  fw.write16(charMap->size);

  // Write data
  t_charMapEntry *entry(charMap->entries);
  for (int i = 0; i < charMap->size; ++i, ++entry) {
    fw.write32(entry->charCode);
    fw.write32(entry->glyphIndex);
  }

  return;
}

void check_freetype(const char *filename, const char *mapname, const char *setname, int font_size)
{
  FT_Library ftlib;
  if (FT_Init_FreeType(&ftlib) != 0) return;

  int face = 0;
  t_prerenderedSet set;
  prerenderSet(ftlib, filename, face, font_size, &set);
  saveSetToFile(setname, &set);

  t_charMap charMap;
  gatherCharmap(ftlib, filename, &charMap);

  saveCharmap(mapname, &charMap);

  FT_Done_FreeType(ftlib);
}

void usage(const char *arg0)
{
  printf("Usage: %s [OPTIONS] FONT_FILE\n\n", arg0);
  printf("OPTIONS:\n");
  printf(" --map=FILE,-m       map file name (a.map)\n");
  printf(" --set=FILE,-s       set file name (a.set)\n");
  printf(" --size=SIZE,-p      font point size (12)\n");
  printf(" --help,-h           help\n");
  exit(0);
}

int main(int argc, char* const *argv)
{
  int c;
  int font_size(12);
  const char *map(0);
  const char *set(0);
  const char *font(0);
  while (1) {
    int option_index = 0;
    static struct option long_options[] = {
      {"map", 1, 0, 0},
      {"set", 1, 0, 0},
      {"size", 1, 0, 0},
      {"help", 1, 0, 0},
      {0, 0, 0, 0}
    };
    c = getopt_long(argc, argv, "m:s:p:h", long_options, &option_index);
    if (c == -1)
      break;
    switch (c)
    {
      case 0:
        {
          if (strcmp(long_options[option_index].name, "map") == 0) {
            map = optarg;
          }
          if (strcmp(long_options[option_index].name, "set") == 0) {
            set = optarg;
          }
          if (strcmp(long_options[option_index].name, "size") == 0) {
            if (sscanf(optarg, "%d", &font_size) != 1)
              usage(argv[0]);
          }
          if (strcmp(long_options[option_index].name, "help") == 0) {
            usage(argv[0]);
          }
          break;
        }
      case 'h':
        usage(argv[0]);
        break;
      case 'm':
        map = optarg;
        break;
      case 's':
        set = optarg;
        break;
      case 'p':
        {
          if (sscanf(optarg, "%d", &font_size) != 1)
            usage(argv[0]);
        }
        break;
      default:
        printf ("?? getopt returned character code 0%o ??\n", c);
        break;
    }
  }
  if (optind < argc) {
    font = argv[optind];
  }

  if (!font) {
    printf("Error: expecting freetype filename as first argument\n");
    usage(argv[0]);
  }
  if (!map) {
    map = "a.map";
  }
  if (!set) {
    set = "a.set";
  }
  printf("Set: %s, Map: %s\n", set, map);
  check_freetype(font, map, set, font_size);
  return 0;
}
