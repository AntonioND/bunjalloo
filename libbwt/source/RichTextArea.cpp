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
#include "Delete.h"
#include "LinkListener.h"
#include "Palette.h"
#include "Rectangle.h"
#include "RichTextArea.h"
#include "Stylus.h"
#include "utf8.h"
#include "string_utils.h"
#include "WidgetColors.h"
using namespace std;
using namespace nds;

static const int NO_INDEX(-1);
static const int POPUP_MENU_COUNT(20);

RichTextArea::RichTextArea(Font * font) :
  TextArea(font),
  m_documentSize(0),
  m_state(Link::STATE_PLAIN),
  m_linkListener(0),
  m_centred(false),
  m_outlined(false),
  m_underLine(false),
  m_linkTouched(0),
  m_downCount(0)
{
}

RichTextArea::~RichTextArea()
{
  removeClickables();
}

void RichTextArea::removeClickables()
{
  for_each(m_links.begin(), m_links.end(), delete_ptr());
  m_links.clear();
  m_state = Link::STATE_PLAIN;
}

static bool isEmpty(const std::string & line)
{
  std::string::const_iterator it(line.begin());
  for (; it != line.end(); ++it)
  {
    if (not isWhitespace(*it))
    {
      return false;
    }
  }
  return true;
}

void RichTextArea::appendText(const std::string & unicodeString)
{
  if (m_document.empty()) {
    m_documentSize = 0;
  }
  if (m_document.size() > 1
      and isEmpty(unicodeString)
      and isEmpty(currentLine())
      and isEmpty(m_document[m_document.size()-2]))
  {
    // avoid duplicate empty lines
    return;
  }
  TextArea::appendText(unicodeString);
  // recalculate document size
  m_documentSize += utf8::distance(unicodeString.begin(), unicodeString.end());
  if (m_centred)
  {
    m_preferredWidth = m_bounds.w;
  }
}

void RichTextArea::addLink(const std::string & href, bool visited)
{
  if (m_state == Link::STATE_LINK)
    return;
  Link * link = new Link(href);
  link->setTextStart(totalCharacters());
  m_links.push_back(link);
  m_state = Link::STATE_LINK;
  if (visited)
  {
    link->setColor(WidgetColors::LINK_VISITED);
  }
}

void RichTextArea::endLink()
{
  if (m_state == Link::STATE_LINK)
  {
    m_state = Link::STATE_PLAIN;
    Link & link(*m_links.back());
    link.setTextEnd(totalCharacters());
    if (link.textEnd() != link.textStart())
    {
      int lastChar = (int) ( currentLine()[currentLine().length()-1] );
      if (isWhitespace(lastChar))
      {
        link.setTextEnd(totalCharacters()-1);
      }
    }
  }
}

void RichTextArea::addImage(const std::string & src)
{
  if (m_state == Link::STATE_LINK)
  {
    // adjust the last link to be this color
    // e.g. an image that is a link. => green text [like this] but also a link.
    string href = m_links.back()->href();
    endLink();
    addLink(href);
    m_links.back()->setColor(WidgetColors::LINK_IMAGE);
  }
  else
  {
    Link * link = new Link(WidgetColors::LINK_IMAGE);
    link->setTextStart(totalCharacters());
    m_links.push_back(link);
    m_state = Link::STATE_COLOR;
  }
  m_links.back()->setSrc(src);
}

void RichTextArea::endImage()
{
  if (m_state == Link::STATE_COLOR)
  {
    m_state = Link::STATE_PLAIN;
    Link & link(*m_links.back());
    link.setTextEnd(totalCharacters());
  }
  else if (m_state == Link::STATE_LINK)
  {
    string href = m_links.back()->href();
    endLink();
    addLink(href);
  }
}

void RichTextArea::incrLine()
{
  if (underline()) {
    endUnderline();
    TextArea::incrLine();
    startUnderline();
  } else {
    TextArea::incrLine();
  }
  m_lineNumber++;
}

void RichTextArea::printu(const std::string & unicodeString)
{
  static const std::string delimeter(" \r\n\t");
  unsigned int lastPosition = findLastNotOf(unicodeString, delimeter);
  unsigned int i = 0;
  if (m_centred) {
    // find size of line, change m_cursorx accordingly
    int w = textSize(unicodeString.substr(0, lastPosition));
    m_cursorx = ((m_bounds.x << 8) + (((m_bounds.w << 8) - w)/2));
  }

  std::string::const_iterator it(unicodeString.begin());
  std::string::const_iterator end_it(unicodeString.end());
  while (it != end_it)
  {
    if (m_nextEvent == m_paintPosition)
    {
      handleNextEvent();
    }
    ++m_paintPosition;
    unsigned int value(utf8::next(it, end_it));
    if (i > lastPosition) {
      continue;
    }
    if (doSingleChar(value))
      break;
    ++i;
  }
}

void RichTextArea::handleNextEvent()
{
  Link * currentLink(*m_currentLink);

  if (currentLink->textEnd() == currentLink->textStart())
  {
    // skip it
    m_nextEventType = Link::STATE_PLAIN;
  }

  switch (m_nextEventType)
  {
    case Link::STATE_LINK:
      startUnderline();
      m_nextEvent = currentLink->textEnd();
      m_nextEventType = Link::STATE_PLAIN;
      break;
    case Link::STATE_PLAIN:
      endUnderline();
      if (m_currentLink != m_links.end())
        ++m_currentLink;
      if (m_currentLink != m_links.end())
      {
        currentLink = *m_currentLink;
        m_nextEvent = currentLink->textStart();
        m_nextEventType = currentLink->eventType();
        if (m_nextEvent == m_paintPosition)
        {
          handleNextEvent();
        }
      } else {
        m_nextEvent = totalCharacters();
        m_nextEventType = Link::STATE_PLAIN;
      }
      break;
    case Link::STATE_COLOR:
      if (currentLink->clicked())
      {
        setTextColor(WidgetColors::LINK_CLICKED);
      }
      else
      {
        setTextColor(currentLink->color());
      }
      m_nextEvent = currentLink->textEnd();
      m_nextEventType = Link::STATE_PLAIN;
      setUnderline(false);
      break;
  }
}

unsigned int RichTextArea::charIndexToLine(unsigned int charIndex) const
{
  // given a cursor position, find the equivalent line/ypos
  unsigned int line = 0;
  unsigned int total = 0;
  int fontHeight = font().height();
  std::vector<std::string>::const_iterator it(m_document.begin());
  for (; it != m_document.end() and total < charIndex; ++it)
  {
    total += utf8::distance(it->begin(), it->end());
    line++;
  }
  return line*fontHeight;
}

unsigned int RichTextArea::totalCharacters()
{
  if (m_documentSize == 0)
  {
    m_documentSize = documentSize(NO_INDEX);
  }
  return m_documentSize;
}

unsigned int RichTextArea::documentSize(int endLine) const
{
  unsigned int total = 0;
  if (endLine == NO_INDEX)
  {
    endLine = m_document.size();
  }
  std::vector<std::string>::const_iterator it(m_document.begin());
  std::vector<std::string>::const_iterator end(m_document.end());
  for (int i = 0; it != end and i != endLine; ++it, ++i)
  {
    total += utf8::distance(it->begin(), it->end());
  }
  return total;
}

void RichTextArea::paint(const nds::Rectangle & clip)
{
  nds::Canvas::instance().setClip(clip);
  m_dirty = false;
  // ensure we switch off links
  setTextColor(0);
  setUnderline(false);
  // see where the first link is
  m_nextEvent = 0;
  if (not m_links.empty())
  {
    m_currentLink = m_links.begin();
    m_nextEvent = (*m_currentLink)->textStart();
    m_nextEventType = (*m_currentLink)->eventType();
  }
  else
  {
    m_currentLink = m_links.end();
    m_nextEvent = totalCharacters();
    m_nextEventType = Link::STATE_PLAIN;
  }
  m_paintPosition = 0;
  m_lineNumber = 0;
  setCursor(m_bounds.x + 2, m_bounds.y);
  if (m_outlined)
  {
    nds::Canvas::instance().
      drawRectangle(
            m_bounds.x,
            m_bounds.y,
            m_bounds.w-1,
            m_bounds.h,
            WidgetColors::BUTTON_SHADOW);
  }
  // work out what happens when we skip lines.
  std::vector<std::string>::const_iterator it(m_document.begin());
  if (m_cursory < clip.top()) {
    // cursor is above the top of the clip area
    unsigned int diff = (clip.top() - m_cursory) / font().height();
    if (diff > m_document.size())
      return;
    if (diff > 0) {
      checkSkippedLines(diff);
      it += diff;
      m_lineNumber += diff;
      m_cursory += diff * font().height();
    }
  }

  for (; it != m_document.end() and (m_cursory < m_bounds.bottom()) and (m_cursory < clip.bottom()); ++it)
  {
    printu(*it);
    incrLine();
  }
}

void RichTextArea::checkSkippedLines(int skipLines)
{
  // the lines are skipped - see which link applies to us, if any
  unsigned int total = documentSize(skipLines);

  // skipped "total" characters.
  m_paintPosition = total;
  // see which link applies
  bool handled(false);
  LinkList::const_iterator linkIt(m_links.begin());
  for (; linkIt != m_links.end(); ++linkIt)
  {
    Link * l(*linkIt);
    // if we are in the middle of a link:
    if (m_paintPosition >= l->textStart() and m_paintPosition <= l->textEnd())
    {
      m_nextEventType = l->eventType();
      m_currentLink = linkIt;
      handleNextEvent();
      handled = true;
      break;
    }
    // if we are before a link
    if (m_paintPosition <= l->textStart())
    {
      // this link is ours. (since m_links is in order)
      m_currentLink = linkIt;
      m_nextEvent = l->textStart();
      m_nextEventType = l->eventType();
      handled = true;
      break;
    }
  }
  if (not handled)
  {
    m_currentLink = m_links.end();
    m_nextEvent = totalCharacters();
    m_nextEventType = Link::STATE_PLAIN;
  }
}
int RichTextArea::lineAt(int y) const
{
  int fh = font().height();
  int bnds = y - m_bounds.y;
  int lineNum = bnds / fh;
  return lineNum;
}

int RichTextArea::pointToCharIndex(int x, int y) const
{
  if (x < m_bounds.x)
    return NO_INDEX;

  unsigned int lineNum = lineAt(y);
  if (lineNum >= m_document.size())
    return NO_INDEX;

  //unsigned int charNumber = documentSize(lineNum);
  const std::string & line(m_document[lineNum]);

  unsigned int caretChar, size;
  bool ended(font().findEnd(line, x - m_bounds.x, 0, &size, &caretChar));

  if (not ended and lineNum > 0)
  {
    int charsToLine = documentSize(lineNum);
    caretChar += charsToLine;
  }
  return caretChar;
}

int RichTextArea::linkPosition(int linkIndex) const
{
  LinkList::const_iterator it(m_links.begin());
  int i = 0;
  while (i != linkIndex)
  {
    ++it;
    ++i;
    if (it == m_links.end())
      break;
  }
  if (it != m_links.end())
  {
    // now know where it starts
    Link * l(*it);
    return m_bounds.top() + charIndexToLine(l->textStart());
  }
  return -1;
}

Link * RichTextArea::linkAt(int index)
{
  if (index != NO_INDEX)
  {
    LinkList::const_iterator linkIt(m_links.begin());
    for (; linkIt != m_links.end(); ++linkIt)
    {
      Link * l(*linkIt);
      if (    ((unsigned int)index) >= l->textStart()
          and ((unsigned int)index) <= l->textEnd())
      {
        return l;
      }
    }
  }
  return 0;
}

bool RichTextArea::stylusUp(const Stylus * stylus)
{
  bool consumed(false);
  m_downCount = 0;
  if (m_linkTouched and m_bounds.hit(stylus->lastX(), stylus->lastY()))
  {
    // was it the same link?
    int charClicked = pointToCharIndex(stylus->startX(), stylus->startY());
    Link * l(linkAt(charClicked));
    bool clicked(false);
    if (l == m_linkTouched)
    {
      // same link
      m_linkListener->linkClicked(l);
      clicked = true;
    }
    m_linkTouched->setClicked(clicked);
    consumed = true;
  }
  m_linkTouched = 0;
  if (not consumed)
    return FOR_EACH_CHILD(stylusUp);
  return consumed;
}

bool RichTextArea::stylusDownFirst(const Stylus * stylus)
{
  bool consumed(false);
  // see if it hit a link
  if (m_bounds.hit(stylus->startX(), stylus->startY()))
  {
    m_downCount = 0;
    m_linkTouched = 0;
    int charClicked = pointToCharIndex(stylus->startX(), stylus->startY());
    if (charClicked != NO_INDEX)
    {
      m_linkTouched = linkAt(charClicked);
      if (m_linkTouched)
      {
        m_linkTouched->setClicked();
        consumed = true;
      }
    }
    else
    {
      return FOR_EACH_CHILD(stylusDownFirst);
    }
  }
  return consumed;
}

bool RichTextArea::stylusDownRepeat(const Stylus * stylus)
{
  // do nothing to avoid accidental clicks
  // FOR_EACH_CHILD(stylusDownRepeat);
  return false;
}

bool RichTextArea::stylusDown(const Stylus * stylus)
{
  int x = stylus->lastX();
  int y = stylus->lastY();
  if (m_bounds.hit(x, y))
  {
    int charClicked = pointToCharIndex(x, y);
    Link * l(linkAt(charClicked));
    if (l != m_linkTouched and m_linkTouched)
    {
      m_linkTouched->setClicked(false);
      m_dirty = true;
      m_linkTouched = 0;
      m_downCount = 0;
    }
    else if (l and l == m_linkTouched)
    {
      m_downCount++;
      if (m_downCount == POPUP_MENU_COUNT)
      {
        // show a menu!
        m_downCount = 0;
        m_linkTouched->setClicked(false);
        if (m_linkListener)
        {
          m_linkListener->linkPopup(m_linkTouched);
        }
        m_linkTouched = 0;
        m_dirty = true;
      }
    }
    else
    {
      m_downCount = 0;
      return FOR_EACH_CHILD(stylusDown);
    }
  }
  else if (m_linkTouched)
  {
    m_linkTouched->setClicked(false);
    return true;
  }
  return false;
}

void RichTextArea::addLinkListener(LinkListener * linkListener)
{
  m_linkListener = linkListener;
}


void RichTextArea::insertNewline()
{
  bool pnl = parseNewline();
  setParseNewline();
  RichTextArea::appendText("\n");
  setParseNewline(pnl);
}

bool RichTextArea::centred() const
{
  return m_centred;
}

void RichTextArea::setCentred(bool centre)
{
  m_centred = centre;
}

bool RichTextArea::outlined() const
{
  return m_outlined;
}

void RichTextArea::setOutlined(bool outline)
{
  m_outlined = outline;
}

unsigned int RichTextArea::linkCount() const
{
  return m_links.size();
}

void RichTextArea::endUnderline()
{
  if (underline() and (m_cursorx > m_startUnderlineX)) {
    Canvas::instance().horizontalLine(
        m_startUnderlineX>>8,
        m_cursory + font().base(),
        (m_cursorx - m_startUnderlineX) >>8,
        foregroundColor());
  }
  setTextColor(0);
  setUnderline(false);
}

void RichTextArea::startUnderline()
{
  Link * currentLink(*m_currentLink);
  if (not currentLink->href().empty())
  {
    if (currentLink->clicked())
    {
      setTextColor(WidgetColors::LINK_CLICKED);
    }
    else
    {
      setTextColor(currentLink->color());
    }
    m_startUnderlineX = m_cursorx;
    m_startUnderlineY = m_cursory;
    setUnderline();
  }
}

void RichTextArea::setUnderline(bool underline)
{
  m_underLine = underline;
}
bool RichTextArea::underline() const
{
  return m_underLine;
}
