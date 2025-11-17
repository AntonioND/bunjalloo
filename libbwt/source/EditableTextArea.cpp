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
#include <cassert>
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
  TextArea(font)
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

  // Now draw the caret if the line has been specified
  if (m_caretLine != -1)
  {
    // See if the caret line is visible
    int caretLinePos(m_bounds.top() + m_caretLine*font().height());
    if (caretLinePos <= m_bounds.bottom())
    {
      // If the caret X coordinate is undefined, calculate it now
      if (m_caretPixelX == -1)
      {
        // Check if we're at the start of the line
        if (m_caretChar == 0)
        {
          m_caretPixelX = 0;
        }
        else
        {
          m_caretPixelX = 0;
          assert(m_caretLine < (int)m_document.size());
          const std::string & line(
              echoText() ? m_document[m_caretLine] :
              createPasswordMask(characters(m_caretLine)));
          unsigned int pix;
          font().findEnd(line, 0, m_caretChar, &pix, 0);
          m_caretPixelX = pix >> 8;
        }
      }

      // Draw caret
      Canvas::instance().verticalLine(m_caretPixelX + 2, caretLinePos, font().height(), Color(31,0,0));
    }

  }
}

void EditableTextArea::text(std::string & returnString) const
{
  document(returnString);
}

// Delete a single character left of the caret
void EditableTextArea::deleteChar()
{
  if (m_caretLine == -1)
  {
    // If the caret isn't set, delete the last character in the string
    std::string & line(currentLine());
    if (not line.empty())
    {
      removeLastCharacter(line);
    }
  }
  else
  {
    if (m_caretChar == 0)
    {
      // The caret is at the beginning of a line

      if (m_caretLine == 0)
      {
        // If this is the first line there's nothing to delete
        return;
      }

      // Move to the end of the previous line
      m_caretLine--;
      m_caretChar = characters(m_caretLine);

      // Erase the last character in that line
      m_caretChar--;
      removeOneCharacter(m_document[m_caretLine], m_caretChar);
    }
    else
    {
      // Erase the character before the caret
      m_caretChar--;
      removeOneCharacter(m_document[m_caretLine], m_caretChar);
    }

    // Redo the text layout
    layoutText();

    // ensure m_document is never empty.
    currentLine();

    // Recalculate caret coordinates
    recalculateCaret();
  }

  resizeParent();
}

void EditableTextArea::recalculateCaret()
{
  // The paint() method will update this the next time it's called
  m_caretPixelX = -1;
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
    // If the caret hasn't selected any line append text at the end
    TextArea::appendText(unicodeString);
    if (not listener())
    {
      m_caretLine = m_document.size()-1;
      m_caretChar = characters(m_caretLine);
      m_caretPixelX = -1;
    }
  }
  else
  {
    // Caret is on a line, inject it at the caret.

    size_t currentCursor = 0;
    for (int i = 0; i < (int)m_document.size(); i++)
    {
      if (m_caretLine == i)
      {
        currentCursor += m_caretChar;
        break;
      }
      currentCursor += (int)m_document[i].length();
    }

    size_t newCursor = currentCursor + unicodeString.length();

    std::string & line(m_document[m_caretLine]);

    if (line.empty())
    {
      line.append(unicodeString);
    }
    else
    {
      line = insertAt(line, m_caretChar, unicodeString);
    }

    // Reshuffle text
    layoutText();

    for (int i = 0; i < (int)m_document.size(); i++)
    {
      size_t lineLength = m_document[i].length();

      if (newCursor <= lineLength)
      {
        m_caretLine = i;
        m_caretChar = newCursor;
        break;
      }

      newCursor -= lineLength;
    }

    // If the user has added a new line don't stay in the current line, jump to
    // the next one.
    if (unicodeString.length() == 1 and unicodeString[0] == '\n')
    {
      m_caretLine++;
      m_caretChar = 0;
    }

    // Recalculate caret coordinates
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

    // If the user presses in the empty space after the end of the text (on the
    // right of the last line or below the last line) set the caret to the last
    // line.
    if (m_caretLine >= (int)m_document.size())
    {
      m_caretLine = (int)m_document.size() - 1;
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

    // If the last character in a line is a newline character, pick the previous
    // character instead.
    if (m_caretChar > 0)
    {
      if (m_document[m_caretLine][m_caretChar - 1] == '\n')
      {
        m_caretChar--;
        recalculateCaret();
      }
    }
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
