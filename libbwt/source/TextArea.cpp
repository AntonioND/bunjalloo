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
#include <assert.h>
#include "utf8.h"
#include "libnds.h"
#include "ndspp.h"
#include "File.h"
#include "Font.h"
#include "string_utils.h"
#include "Canvas.h"
#include "Link.h"
#include "Palette.h"
#include "TextArea.h"
#include "utf8.h"

using namespace nds;
using namespace std;
const static unsigned char NEWLINE('\n');
static int INITIAL_POSITION(2<<8);

TextArea::TextArea(Font * font) :
  m_font(0),
  m_appendPosition(INITIAL_POSITION),
  m_parseNewline(true)
{
  setFont(font);
  setDefaultColor();
  m_preferredHeight = m_font->height();
  m_preferredWidth = SCREEN_WIDTH;
  m_preferredWidthFixed = m_preferredWidth << 8;
  m_bounds.w = SCREEN_WIDTH;
}

void TextArea::setFont(Font * font)
{
  m_font = font;
}

void TextArea::document(std::string & returnString) const
{
  std::vector<std::string>::const_iterator it(m_document.begin());
  for (; it != m_document.end(); ++it)
  {
    returnString.append(*it);
  }
}

void TextArea::clearText()
{
  // Could do the following to completely free the memory, but there
  // is a fair chance it will be reused anyway... speed vs memory again :-/
  std::vector<std::string> tmp;
  m_document.swap(tmp);
  m_appendPosition = INITIAL_POSITION;
  m_preferredWidth = -1;
  m_preferredWidthFixed = 0;
  m_preferredHeight = m_font->height();
  currentLine();
}

void TextArea::appendText(const std::string &unicodeString)
{
  if (m_document.empty())
  {
    m_preferredWidth = 0;
    m_preferredWidthFixed = 0;
  }
  // append text, adding in new lines as needed to wrap.
  // int currPosition = 0;
  // find the next space character
  std::string::const_iterator it = unicodeString.begin();
  std::string::const_iterator end_it = unicodeString.end();
  while (it != end_it)
  {

    std::string::const_iterator backup_it(it);
    std::string word(nextWordAdvanceWord(&it, end_it));
    int size = textSize(word);
    if ((size>>8) > width() and word.size() > 1)
    {
      it = backup_it;
      word = m_font->shorterWordFromLong(&it, end_it, width(), &size);
    }

    // if the word ends with a new line, then increment the height.
    // otherwise, if we go off the end of the line, increment the height.
    if (((m_appendPosition + size)>>8) > width())
    {
      // trim spaces from the end of the line
      // this word overflows the line - make a new line to hold the text.
      m_document.push_back(std::string());
      m_appendPosition = INITIAL_POSITION;
      m_preferredHeight += m_font->height();
    }
    if (m_preferredWidth < 0) {
      m_preferredWidth = 0;
      m_preferredWidthFixed = INITIAL_POSITION;
    }
    m_preferredWidthFixed += size;
    m_preferredWidth = (m_preferredWidthFixed >> 8);
    size_t last = word.length() - 1;
    if (not m_parseNewline and word[last] == NEWLINE) {
      // skip empty new lines
      word[last] = ' ';
    }
    currentLine().append(word);
    m_appendPosition += size;
    // if the word ended in a NEWLINE, then go onto the next line.
    if (m_parseNewline and word[last] == NEWLINE)
    {
      m_appendPosition = INITIAL_POSITION;
      m_document.push_back(std::string());
      m_preferredHeight += font().height();
    }
  }
}

void TextArea::layoutText()
{
  // need to shuffle the document about... this requires a new copy of it.
  std::string tmp;
  bool pnl = parseNewline();
  setParseNewline();
  document(tmp);
  clearText();
  if (not tmp.empty())
    appendText(tmp);
  if (m_preferredHeight == 0)
    m_preferredHeight = m_font->height();
  setParseNewline(pnl);
}

void TextArea::setSize(unsigned int w, unsigned int h)
{
  if (m_bounds.w != (int)w) {
    Component::setSize(w, h);
    layoutText();
  } else {
    Component::setSize(w, h);
  }
}

void TextArea::setCursor(int x, int y)
{
  m_cursorx = x << 8;
  m_cursory = y;
  m_initialCursorx = x << 8;
}

void TextArea::incrLine()
{
  m_cursorx = m_initialCursorx;
  m_cursory += m_font->height();
}

bool TextArea::doSingleChar(int value)
{
  int advance = m_font->doSingleChar(
      value,
      m_cursorx,
      m_cursory,
      m_bounds.right() << 8,
      m_fgCol,
      m_bgCol);
  if (advance == -2) {
    // requires more space
    incrLine();
    advance = m_font->doSingleChar(
        value,
        m_cursorx,
        m_cursory,
        m_bounds.right() << 8,
        m_fgCol,
        m_bgCol);
    if (advance < 0)
      return false;
  }
  m_cursorx += advance;
  return m_cursory > m_bounds.bottom();
}

void TextArea::printu(const std::string & unicodeString)
{
  std::string::const_iterator it(unicodeString.begin());
  for (; it != unicodeString.end() and m_cursory < m_bounds.bottom(); )
  {
    uint32_t value = utf8::next(it, unicodeString.end());
    if (doSingleChar(value))
      break;
  }
}

int TextArea::textSize(const std::string &unicodeString) const
{
  int width(0);
  int height(0);
  m_font->textSize(unicodeString.c_str(), unicodeString.length(),
      width, height, "utf-8");

  return width;
}

void TextArea::setDefaultColor()
{
  m_bgCol = RGB5(31, 31, 31);
  m_fgCol = 0;
}

void TextArea::setBackgroundColor(unsigned short color)
{
  m_bgCol = color;
}

unsigned short TextArea::backgroundColor() const
{
  return m_bgCol;
}

void TextArea::setTextColor(unsigned short color)
{
  m_fgCol = color;
}

unsigned short TextArea::foregroundColor() const
{
  return m_fgCol;
}

TextArea::~TextArea()
{
}

/** Paint the text area. */
void TextArea::paint(const nds::Rectangle & clip)
{
  // TextArea wants to paint from 0,0 to 256,END_Y
  // ScrollView says "start at position X" instead... (the clip square)
  // TextArea then thinks "position X, that is really line N"
  // and draws there until it runs out of space.
  //
  // Theres "width" which is the possible width (wraps at)
  // then theres clip-width which is where it should draw to.
  int startPos = m_bounds.x + (INITIAL_POSITION>>8);
  setCursor(startPos, m_bounds.y);
  Canvas::instance().fillRectangle(clip.x, clip.y, clip.w, clip.h, m_bgCol);
  // work out the number of lines to skip
  std::vector<std::string>::const_iterator it(m_document.begin());
  if (m_cursory < clip.top()) {
    // cursor is above the top of the clip area
    unsigned int diff = (clip.top() - m_cursory) / font().height();
    if (diff >= m_document.size())
      return;
    if (diff > 0) {
      it += diff;
      m_cursory += diff * font().height();
    }
  }

  for (; it != m_document.end() and (m_cursory < m_bounds.bottom()) and (m_cursory < clip.bottom()); ++it)
  {
    printu(*it);
    incrLine();
  }
}

std::string TextArea::asString() const
{
  std::string returnString;
  bool needComma(false);
  std::vector<std::string>::const_iterator it(m_document.begin());
  for (; it != m_document.end(); ++it)
  {
    if (needComma)
      returnString += ",\n";
    returnString += "[\"";
    returnString.append(*it);
    returnString += "\"]";
    needComma = true;
  }

  return returnString;
}

size_t TextArea::characters(size_t line) const
{
  return utf8::distance(m_document[line].begin(),
      m_document[line].end());
}
