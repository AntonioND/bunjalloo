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
#include "HtmlOptionElement.h"
#include "Visitor.h"
IMPLEMENT_ACCEPT(HtmlOptionElement);

const std::string * HtmlOptionElement::attributePtr(const std::string & name) const
{
  const std::string * p = HtmlElement::attributePtr(name);
  if (p) {
    return p;
  }

  if (name == "value")
  {
    return &m_value;
  }
  if (name == "label")
  {
    return &m_label;
  }
  return 0;
}

void HtmlOptionElement::setAttribute(const std::string & name, const std::string & value)
{
  if (name == "selected")
  {
    m_selected = true;
  }
  HtmlElement::setAttribute(name, value);
}

bool HtmlOptionElement::selected() const
{
  return m_selected;
}
