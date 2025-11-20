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
#include "HtmlInputElement.h"
#include "Visitor.h"
IMPLEMENT_ACCEPT(HtmlInputElement);

const std::string * HtmlInputElement::attributePtr(const std::string & name) const
{
  const std::string * p = HtmlElement::attributePtr(name);
  if (p) {
    return p;
  }

  if (name == "type")
  {
    return &m_type;
  }
  if (name == "value")
  {
    return &m_value;
  }
  if (name == "src")
  {
    return &m_src;
  }
  if (name == "alt")
  {
    return &m_alt;
  }
  if (name == "name")
  {
    return &m_name;
  }
  if (name == "size")
  {
    return &m_size;
  }
  return 0;

}

HtmlInputElement::InputType HtmlInputElement::inputType() const
{
  const std::string &type(m_type);
  if (type == "submit") return SUBMIT;
  if (type == "image") return IMAGE;
  if (type == "button") return SUBMIT;
  if (type.empty() or type == "text") return TEXT;
  if (type == "password") return PASSWORD;
  if (type == "checkbox") return CHECKBOX;
  if (type == "radio") return RADIO;
  return HIDDEN;
}

bool HtmlInputElement::checked() const
{
  return m_checked;
}

void HtmlInputElement::setChecked(bool checked)
{
  m_checked = checked;
}

void HtmlInputElement::setAttribute(const std::string & name,
        const std::string & value)
{
  HtmlElement::setAttribute(name, value);
  if (name == "checked")
  {
    m_checked = true;

  }
  else if (name == "alt")
  {
    m_hasAltText = true;
  }
}

bool HtmlInputElement::hasAltText() const
{
  return m_hasAltText;
}
