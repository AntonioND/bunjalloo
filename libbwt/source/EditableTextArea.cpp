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
#include "Canvas.h"
#include "EditableTextArea.h"
#include "Palette.h"
#include "ScrollPane.h"
#include "Stylus.h"
#include "TextListener.h"
#include "utf8.h"
#include "WidgetColors.h"
#include "string_utils.h"
#include "password_mask.h"
using nds::Canvas;
using nds::Color;

EditableTextArea::EditableTextArea(Font * font) :
  TextArea(font),
  m_caretLine(-1),
  m_caretChar(-1),
  m_caretPixelX(-1),
  m_appendedNewLine(false),
  m_echoText(true),
  //m_layingOut(false),
  m_scrollPane(0)
{
}

void EditableTextArea::printu(const std::string & unicodeString)
{
  if (echoText())
  {
    TextArea::printu(unicodeString);
  }
  else
  {
    TextArea::printu(createPasswordMask(
          utf8::distance(unicodeString.begin(), unicodeString.end())));
  }
}

void EditableTextArea::paint(const nds::Rectangle & clip)
{
  TextArea::paint(clip);
  Canvas::instance().horizontalLine(clip.x, clip.top(), clip.w, WidgetColors::BUTTON_SHADOW);
  Canvas::instance().verticalLine(clip.left(), clip.top(), clip.h, WidgetColors::BUTTON_SHADOW);
  Canvas::instance().horizontalLine(clip.x, clip.bottom()-1, clip.w, WidgetColors::SCROLLBAR_ARROW);
  Canvas::instance().verticalLine(clip.right(), clip.top(), clip.h, WidgetColors::SCROLLBAR_ARROW);
  // now draw the caret too
  if (m_caretLine != -1)
  {
    // see if line is visible
    int caretLinePos(m_bounds.top() + m_caretLine*font().height());
    if (caretLinePos <= m_bounds.bottom())
    {
      // draw
      if (m_caretPixelX == -1)
      {
        m_caretPixelX = 0;
        const std::string & line(
            echoText() ? m_document[m_caretLine] :
            createPasswordMask(characters(m_caretLine)));
        unsigned int pix;
        font().findEnd(line, 0, m_caretChar, &pix, 0);
        m_caretPixelX = pix >> 8;
      }
      Canvas::instance().verticalLine(m_caretPixelX + 2, caretLinePos, font().height(), Color(31,0,0));
    }

  }
}

void EditableTextArea::text(std::string & returnString) const
{
  document(returnString);
}

void EditableTextArea::deleteChar()
{
  // delete a single character
  if (m_caretLine == -1)
  {
    std::string & line(currentLine());
    if (not line.empty())
    {
      removeLastCharacter(line);
    }
  }
  else
  {
    if (m_caretChar == 0) {
      if (m_caretLine) {
        m_caretLine--;
        m_caretChar = characters(m_caretLine);
      }
      else
      {
        return;
      }
    }

    m_caretChar -= 1;
    if (m_caretChar == -1)
    {
      m_document.erase(m_document.begin()+m_caretLine);
    }
    else
    {
      removeOneCharacter(m_document[m_caretLine], m_caretChar);
    }
    recalculateCaret();
  }
  resizeParent();
}

void EditableTextArea::recalculateCaret()
{
  m_caretPixelX = -1;
  int oldLength = characters(m_caretLine);
  int oldPrevLength = -1;
  if (m_caretLine > 0)
  {
    oldPrevLength = characters(m_caretLine-1);
  }
  // redo the text layout
  layoutText();
  if (m_appendedNewLine)
  {
    m_caretChar = 0;
    m_caretLine++;
    m_appendedNewLine = false;
    return;
  }
  if (m_caretLine == (int)m_document.size())
  {
    m_caretLine--;
    m_caretChar = characters(m_caretLine);
    m_caretPixelX = -1;
  }
  else
  {
    int newLength = characters(m_caretLine);

    // work out if the caret should overflow
    // also, what if it "underflows"?
    if (newLength < oldLength)
    {
      // word has wrapped to next line. To calculate where we are, work out the
      // difference from previous to now
      // Also, if we add a space to separate a word, the word may go to the *previous* line.
      int diff = oldLength - newLength;
      if (m_caretChar >= newLength)
      {
        m_caretLine++;
        // fix Issue 19 - off by one on wrap around new line.
        m_caretChar = diff;
        m_caretPixelX = -1;
      }
    }
    // check for underflow condition
    int newPrevLength = -1;
    if (m_caretLine > 0)
      newPrevLength = characters(m_caretLine-1);
    if (oldPrevLength != -1 and newPrevLength != -1 and oldPrevLength < newPrevLength)
    {
      // the previous line has eaten some of our text
      m_caretChar = 0;
      m_caretPixelX = -1;
    }
  }
  // ensure m_document is never empty.
  currentLine();
}

static std::string insertAt(const std::string &line, int position, const std::string &extra)
{
  std::string::const_iterator it(line.begin());
  utf8::unchecked::advance(it, position);
  std::string start(line.begin(), it);
  start.append(extra);
  start.append(it, line.end());
  return start ;
}

void EditableTextArea::appendText(const std::string & unicodeString)
{
  /* FIXME - virtual TextArea::appendText
  if (m_layingOut)
  {
    TextArea::appendText(unicodeString);
    return;
  }
  */

  if (m_caretLine == -1)
  {
    TextArea::appendText(unicodeString);
    // select the end of the text
    if (not listener())
    {
      m_caretLine = m_document.size()-1;
      m_caretChar = characters(m_caretLine);
      m_caretPixelX = -1;
    }
  }
  else
  {
    // caret is on a line.
    // what if the line goes over the edge?
    // the m_caretLine should increase by one and reshuffle the line.
    std::string & line(m_document[m_caretLine]);
    // Check we don't walk off the edge, just in case...
    int end = characters(m_caretLine);
    if (m_caretChar > end)
    {
      m_caretChar = end;
    }
    if (line.empty())
    {
      line.append(unicodeString);
    }
    else {
      line = insertAt(line, m_caretChar, unicodeString);
    }
    if (unicodeString.length() == 1 and unicodeString[0] == '\n')
    {
      m_appendedNewLine = true;
    }
    else
    {
      m_appendedNewLine = false;
      m_caretChar += utf8::distance(unicodeString.begin(), unicodeString.end());
    }
    recalculateCaret();
  }
  resizeParent();
}

void EditableTextArea::clearText()
{
  TextArea::clearText();
  //m_layingOut = true;
  layoutText();
  //m_layingOut = false;
  resizeParent();
  m_caretLine = -1;
  m_caretChar = -1;
  m_caretPixelX = -1;
  m_appendedNewLine = false;
}

/*
void EditableTextArea::setSize(unsigned int w, unsigned int h)
{
  //m_layingOut = true;
  TextArea::setSize(w, h);
  m_layingOut = false;
}
*/

void EditableTextArea::resizeParent()
{
  if (m_scrollPane)
  {
    int height = std::max(preferredSize().h, m_scrollPane->height());
    setSize(width(), height);
    m_preferredHeight = height;
    m_scrollPane->setSize(m_scrollPane->width(), m_scrollPane->height());
    m_scrollPane->scrollToPercent(255);
  }
}

void EditableTextArea::setCaret(int x, int y)
{
  // set the caret position based on x, y position
  // first see if in bounds
  if (m_bounds.hit(x, y))
  {
    // next see if the caret can be set.
    m_caretLine = (y - m_bounds.y)/font().height();
    if (m_caretLine >= (int)m_document.size())
    {
      m_caretLine = -1;
      return;
    }
    m_caretChar = 0;
    m_caretPixelX = -1;
    const std::string & line(
        echoText() ? m_document[m_caretLine] :
        createPasswordMask(characters(m_caretLine)));
    unsigned int s, c;
    font().findEnd(line, x, 0, &s, &c);
    m_caretPixelX = s >> 8;
    m_caretChar = c;
  }
}


void EditableTextArea::setText(const std::string & text)
{
  clearText();
  appendText(text);
  resizeParent();
}

bool EditableTextArea::inBounds(int x, int y)
{
  return ( (m_scrollPane and m_scrollPane->bounds().hit(x, y) and m_bounds.hit(x, y) )
      or (not m_scrollPane and m_bounds.hit(x, y)));
}

bool EditableTextArea::stylusUp(const Stylus * stylus)
{
  if (inBounds(stylus->startX(), stylus->startY()) and
      inBounds(stylus->lastX(), stylus->lastY()))
  {
    if (listener())
    {
      listener()->editText(this);
    }
    else
    {
      setCaret(stylus->lastX(), stylus->lastY());
    }
  }
  return false;
}

bool EditableTextArea::stylusDownFirst(const Stylus * stylus)
{
  // if inside a scrollPane, check relative to the bounds of parent
  // otherwise check our bounds.
  return false;
}

bool EditableTextArea::stylusDownRepeat(const Stylus * stylus)
{
  // nothing
  return false;
}

bool EditableTextArea::stylusDown(const Stylus * stylus)
{
  // selections?
  return false;
}

int EditableTextArea::caretLine() const
{
  return m_caretLine;
}

bool EditableTextArea::isMultiLine() const
{
  return true;
}

void EditableTextArea::setParentScroller(ScrollPane * scrollPane)
{
  m_scrollPane = scrollPane;
}

bool EditableTextArea::caretVisible() const
{
  if (m_caretLine != -1)
  {
    int caretLinePos(m_bounds.top() + m_caretLine*font().height());
    if (caretLinePos <= m_bounds.bottom()
        and m_scrollPane
        and caretLinePos <= m_scrollPane->bounds().bottom()
        and caretLinePos > m_scrollPane->bounds().top())
    {
      return true;
    }
  }
  return false;
}
