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
#ifndef HtmlInputElement_h_seen
#define HtmlInputElement_h_seen
#include "HtmlElement.h"

class HtmlInputElement:public HtmlElement
{
  public:
    HtmlInputElement(const std::string & tagName)
      : HtmlElement(tagName), m_checked(false), m_hasAltText(false)
    {}

    enum InputType
    {
      SUBMIT,
      IMAGE,
      TEXT,
      PASSWORD,
      RADIO,
      CHECKBOX,
      HIDDEN
    };
    InputType inputType() const;

    virtual void setAttribute(const std::string & name,
        const std::string & value);
    bool checked() const;
    void setChecked(bool checked);
    bool hasAltText() const;

    DEFINE_ACCEPT();
  protected:
    std::string m_type;
    std::string m_value;
    std::string m_name;
    std::string m_alt;
    std::string m_src;
    std::string m_size;
    bool m_checked;
    bool m_hasAltText;
    const std::string * attributePtr(const std::string & name) const;
};
#endif
