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
#include "libnds.h"
#include "Font.h"
#include "Palette.h"
#include "utf8.h"
#include "ISO_8859_1.h"
#include "Canvas.h"
using namespace std;
using namespace nds;

struct t_prerenderedSet {
  // Set identification
  uint8_t face;
  uint8_t size;

  // Vertical metrics
  int32_t minAscender, maxDescender;

  // Prerendered images
  t_prerenderedGlyph *glyphs;
  uint32_t numGlyphs;
};

struct t_charMapEntry {
  uint32_t charCode;
  uint32_t glyphIndex;
};

struct t_charMap {
  t_charMapEntry *entries;
  uint16_t size;
};

struct t_glyphBitmap {
  uint8_t width, height;
  uint8_t *bitmap;
};

struct t_prerenderedGlyph {
  t_glyphBitmap image;
  int8_t deltaX, deltaY;
  uint16_t advanceX;
};

Font::Font():
  m_width(8),
  m_prerenderedSet(0),
  m_charMap(0)
{
}

#define READ_U8(value, from)  { value = from[index]; index += 1; }
#define READ_U16(value, from) { value = (from[index]<<8) |(from[index+1]); index += 2; }
#define READ_U32(value, from) { value = (from[index]<<24) |(from[index+1]<<16)|(from[index+2]<<8)|(from[index+3]); index+=4;}
void Font::init(const unsigned char * setData, const unsigned char * mapData)
{
  m_prerenderedSet = new t_prerenderedSet;
  m_charMap = new t_charMap;

  int index = 0;
  READ_U8(m_prerenderedSet->face, setData);
  READ_U8(m_prerenderedSet->size, setData);
  READ_U32(m_prerenderedSet->minAscender, setData);
  READ_U32(m_prerenderedSet->maxDescender, setData);
  READ_U32(m_prerenderedSet->numGlyphs, setData);

  m_prerenderedSet->glyphs =
    static_cast<t_prerenderedGlyph*>(malloc(m_prerenderedSet->numGlyphs *
          sizeof(t_prerenderedGlyph)));

  t_prerenderedGlyph *glyph(m_prerenderedSet->glyphs);
  for (uint32_t i = 0; i < m_prerenderedSet->numGlyphs; ++i, ++glyph) {
    // read t_prerenderedGlyph
    READ_U8(glyph->deltaX, setData);
    READ_U8(glyph->deltaY, setData);
    READ_U16(glyph->advanceX, setData);
    // read bitmap data
    READ_U8(glyph->image.width, setData);
    READ_U8(glyph->image.height, setData);
    // read data:
    int size((glyph->image.width * glyph->image.height) / 4);
    glyph->image.bitmap = static_cast<uint8_t*>(malloc(size));
    const uint8_t *src(&setData[index]);
    const uint8_t *end(&setData[index + size]);
    uint8_t *dest(glyph->image.bitmap);
    for (; src != end;) *dest++ = *src++;
    index += size;
  }

  // now read the glyph to code point map
  index = 0;
  READ_U16(m_charMap->size, mapData);

  m_charMap->entries = static_cast<t_charMapEntry*>(malloc(m_charMap->size *
        sizeof(t_charMapEntry)));
  // read data
  t_charMapEntry *entry(m_charMap->entries);
  for (int i = 0; i < m_charMap->size; ++i, ++entry) {
    READ_U32(entry->charCode, mapData);
    READ_U32(entry->glyphIndex, mapData);
  }
}

Font::~Font()
{
  // TODO: clean up the allocated font data
}

int Font::minGlyph() const
{
  // get the minimum glyph number
  return m_charMap->entries[0].glyphIndex;
}

int Font::valueToIndex(unsigned int codepoint) const
{
  //  use a binary search as charCode is ordered
  t_charMapEntry *entry(m_charMap->entries);
  uint32_t lower = 0;
  uint32_t upper = m_charMap->size;
  while (true) {
    uint32_t key = (upper + lower) / 2;
    if (entry[key].charCode > codepoint) {
      if (upper == key) {
        break;
      }
      upper = key;
    }
    else if (entry[key].charCode < codepoint) {
      if (lower == key) {
        break;
      }
      lower = key;
    }
    else {
      return entry[key].glyphIndex;
    }
  }
  return -1;
}

t_prerenderedGlyph *Font::glyph(unsigned int value) const
{
  int glyphIndex = valueToIndex(value);
  if (glyphIndex == -1)
  {
    int min = minGlyph();
    if (min == -1) {
      return &m_prerenderedSet->glyphs[0];
    }
    glyphIndex = min;
  }
  return &m_prerenderedSet->glyphs[glyphIndex];
}

void Font::textSize(const char * text, int amount, int & width, int & height, const std::string & encoding) const
{
  // length of text in bytes.
  bool utf8(encoding == "utf-8");
  width = 0;
  height = this->height();
  int maxWidth = 0;
  const char *end(text + amount);
  while (text != end)
  {
    unsigned int value;
    if (utf8) {
      value = utf8::next(text, end);
    } else {
      value = ISO_8859_1::decode(text[0]&0xff);
      text++;
    }
    if (value == 0xfffd) {
      value = '?';
    }
    if (value == '\n')
    {
      height += this->height();
      width = 0;
    } else {
      t_prerenderedGlyph *g(glyph(value));
      width += g->advanceX;
    }
    if (width > maxWidth) {
      maxWidth = width;
    }
  }
  width = maxWidth;
}

static int round_down(int value) {
  return value >> 8;
}

static int round_up(int value) {
  return value << 8;
}

bool Font::findEnd(
    const std::string &text,
    unsigned int maxSize,
    unsigned int maxLastChar,
    unsigned int *sizeOut,
    unsigned int *lastCharOut) const
{
  std::string::const_iterator it(text.begin());
  std::string::const_iterator end(text.end());
  unsigned int size = 0;
  unsigned int character = 0;
  maxSize = round_up(maxSize);

  for (; it != end and (!maxSize or size < maxSize) and (!maxLastChar or character < maxLastChar); character++)
  {
    uint32_t value = utf8::next(it, end);
    if (value == 0xfffd)
      value = '?';
    t_prerenderedGlyph *g(glyph(value));
    size += g->advanceX;
    if (maxSize and size > maxSize) {
      size -= g->advanceX;
      break;
    }
  }
  if (lastCharOut) { *lastCharOut = character; }
  if (sizeOut) { *sizeOut = size; }
  return it == end;
}

static void putPixel(Canvas &canvas, int x, int y, int color)
{
  //int color = colors[pixel&3];
  canvas.unsafeDrawPixel(round_down(x), y, color);
}

inline int LC(int fg, int bg) {
  return ((3 * bg ) + fg) / 4;
}

inline int L(const Color &fg, const Color &bg)
{
  // 75 % background, 25 % fg
  int red = LC(fg.red(), bg.red());
  int green = LC(fg.green(), bg.green());
  int blue = LC(fg.blue(), bg.blue());
  return RGB5(red, green, blue);
}

inline int MC(int fg, int bg) {
  return (bg + fg) / 2;
}

inline int M(const Color &fg, const Color &bg)
{
  // 50/50
  int red = MC(fg.red(), bg.red());
  int green = MC(fg.green(), bg.green());
  int blue = MC(fg.blue(), bg.blue());
  return RGB5(red, green, blue);
}
static int colors[4] = { 0 };

void Font::printAt(t_prerenderedGlyph &g, int xPosition, int yPosition, int color, int bgcolor)
{
  xPosition += g.deltaX << 7;
  yPosition = yPosition + base() + g.deltaY;
  if ( (colors[0] != color) or (colors[3] != bgcolor)) {
    Color fg(color);
    Color bg(bgcolor);
    colors[0] = bg;
    colors[1] = L(fg, bg);
    colors[2] = M(fg, bg);
    colors[3] = fg;
  }
  Canvas &canvas(Canvas::instance());
  const nds::Rectangle &clip(canvas.clip());
  int xbounds = g.image.width;
  if ((xPosition + round_up(g.image.width)) > round_up(clip.right())) {
    xbounds = clip.right() - round_down(xPosition);
  }
  int ybounds = g.image.height;
  if ((yPosition + ybounds) > clip.bottom()) {
    ybounds = clip.bottom() - yPosition;
  }
  int ystart = 0;
  if (clip.top() > yPosition) {
    ystart = clip.top() - yPosition;
  }
  if (yPosition < 0) {
    if ((yPosition + ybounds) < 0) return;
    ystart = -yPosition;
  }
  const unsigned char *data = &g.image.bitmap[ystart * (g.image.width / 4)];
  for (int y = ystart; y < ybounds; ++y)
  {
    // width is not a multiple of 4 necessarily
    int x = 0;
    unsigned char pixelCuartet(0);
    while (x != xbounds)
    {
      if ((x&3) == 0)
        pixelCuartet = *data++;
      int pixel = pixelCuartet >> (6 - (2*(x&3)));
      int color = colors[pixel&3];
      putPixel(canvas, xPosition + round_up(x), yPosition + y, color);
      x++;
    }
  }
}

std::string Font::shorterWordFromLong(
    std::string::const_iterator *it,
    const std::string::const_iterator &end_it,
    int width,
    int *size) const
{
  // This is a very long word, split it up
  std::string shorterWord;
  *size = 0;
  width = round_up(width);
  while (*it != end_it)
  {
    // store the start of the unicode code point
    std::string::const_iterator a(*it);
    uint32_t value = utf8::next(*it, end_it);
    if (value == 0xfffd)
      value = '?';
    t_prerenderedGlyph *g(glyph(value));
    if ( (*size + g->advanceX) >= width) {
      // rewind to the start of that last code point
      *it = a;
      return shorterWord;
    }
    for (; a != *it; ++a) {
      shorterWord += *a;
    }
    *size += g->advanceX;
  }
  return shorterWord;
}

int Font::doSingleChar(unsigned int value, int cursorx, int cursory, int right, int color, int bgcolor)
{
  if (value == '\n')
    return -1;
  if (value == 0xfffd)
    value = '?';
  t_prerenderedGlyph *g(glyph(value));
  if (round_down(cursorx + g->advanceX) > round_down(right)) {
    return -2;
  }
  printAt(*g, cursorx, cursory, color, bgcolor);
  return g->advanceX;
}

int Font::height() const
{
  return m_prerenderedSet->maxDescender - m_prerenderedSet->minAscender;
}

int Font::base() const
{
  return 2 + ((height() * 5) / 7);
}
