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
#include <cstdlib>
#include "HtmlBodyElement.h"
#include "Visitor.h"
IMPLEMENT_ACCEPT(HtmlBodyElement);

const std::string * HtmlBodyElement::attributePtr(const std::string & name) const
{
  const std::string * p = HtmlElement::attributePtr(name);
  if (p) {
    return p;
  }

  if (name == "bgcolor")
  {
    return &m_bgcolor;
  }
  return 0;

}

void HtmlBodyElement::copyAttributes(HtmlElement * copyTo) const
{
  HtmlElement::copyAttributes(copyTo);
  ((HtmlBodyElement*)copyTo)->m_bgcolor = m_bgcolor;
}

HtmlElement * HtmlBodyElement::clone() const
{
  HtmlElement * theClone(new HtmlBodyElement(m_tagName));
  copyAttributes(theClone);
  return theClone;
}

struct ColorMap
{
  const char * name;
  unsigned int value;
};
const static ColorMap s_colorMap[] = {
  { "black",   0x000000},
  { "green",   0x008000},
  { "silver",  0xC0C0C0},
  { "lime",    0x00FF00},
  { "gray",    0x808080},
  { "olive",   0x808000},
  { "white",   0xFFFFFF},
  { "yellow",  0xFFFF00},
  { "maroon",  0x800000},
  { "navy",    0x000080},
  { "red",     0xFF0000},
  { "blue",    0x0000FF},
  { "purple",  0x800080},
  { "teal",    0x008080},
  { "fuchsia", 0xFF00FF},
  { "aqua",    0x00FFFF},
  { 0, 0 }
};

unsigned int HtmlBodyElement::bgColor() const
{
  // see if it is a #XXX or #FF0000 type colour
  std::string bgcol = m_bgcolor;
  if (bgcol[0] == '#')
  {
    // get next 6 letters
    std::string::const_iterator it(bgcol.begin());
    // skip #
    ++it;
    int count = 0;
    std::string color("0x");
    for (; it != bgcol.end() and count < 6; ++it,++count)
    {
      color += *it;
    }
    return strtol(color.c_str(), 0, 0);
  }
  else
  {
    int index(0);
    for (;;)
    {
      if (s_colorMap[index].name == 0)
      {
        break;
      }
      if (bgcol == std::string(s_colorMap[index].name))
      {
        return s_colorMap[index].value;
      }
      index++;
    }
  }
  return 0;
}
