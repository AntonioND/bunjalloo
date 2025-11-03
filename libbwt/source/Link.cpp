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
#include "Link.h"
#include "WidgetColors.h"

Link::Link(const std::string & a) :
  m_textStart(0),
  m_textEnd(0),
  m_color(WidgetColors::LINK_REGULAR),
  m_clicked(false),
  m_anchor(a),
  m_src("")
{ }

Link::Link(unsigned short color) :
  m_textStart(0),
  m_textEnd(0),
  m_color(color),
  m_clicked(false),
  m_anchor(""),
  m_src("")
{ }


std::string Link::src() const
{
  return m_src;
}

void Link::setSrc(const std::string & src)
{
  m_src = src;
}

std::string Link::href() const
{
  // FIXME - what about #?
  /*
  string_utils.href = m_anchor->attribute("href");
  // strip off the # part?
  // replace ' ' with %20?
  return unicode2string(href);
  */
  return m_anchor;
}

Link::EventType Link::eventType() const
{
  if (m_anchor.empty() and m_color != WidgetColors::LINK_REGULAR)
    return STATE_COLOR;
  return STATE_LINK;
}
