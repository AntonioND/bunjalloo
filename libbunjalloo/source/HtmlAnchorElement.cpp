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
#include "HtmlAnchorElement.h"
#include "Visitor.h"
IMPLEMENT_ACCEPT(HtmlAnchorElement);

const std::string * HtmlAnchorElement::attributePtr(const std::string & name) const
{
  const std::string * p = HtmlElement::attributePtr(name);
  if (p) {
    return p;
  }

  if (name == "href")
  {
    return &m_href;
  }
  if (name == "name")
  {
    return &m_name;
  }
  return 0;

}

void HtmlAnchorElement::copyAttributes(HtmlElement * copyTo) const
{
  HtmlElement::copyAttributes(copyTo);
  ((HtmlAnchorElement*)copyTo)->m_href = m_href;
  ((HtmlAnchorElement*)copyTo)->m_name = m_name;
}

HtmlElement * HtmlAnchorElement::clone() const
{
  HtmlElement * theClone(new HtmlAnchorElement(m_tagName));
  copyAttributes(theClone);
  return theClone;
}

