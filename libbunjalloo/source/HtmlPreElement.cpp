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
#include "HtmlConstants.h"
#include "HtmlPreElement.h"
#include "Visitor.h"
#include "utf8.h"
IMPLEMENT_ACCEPT(HtmlPreElement);


void HtmlPreElement::appendText(unsigned int value)
{
  if (m_children.size())
  {
    if (m_children.back()->isa(HtmlConstants::TEXT))
    {
      utf8::unchecked::append(value, back_inserter(m_children.back()->text()));
      return;
    }
  }
  HtmlElement* textNode = new HtmlElement(HtmlConstants::TEXT);
  utf8::unchecked::append(value, back_inserter(textNode->text()));
  append(textNode);
}
