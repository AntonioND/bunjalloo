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
#ifndef HtmlOptionElement_h_seen
#define HtmlOptionElement_h_seen
#include "HtmlElement.h"

class HtmlOptionElement : public HtmlElement
{
  public:
    HtmlOptionElement(const std::string & tagName)
      : HtmlElement(tagName), m_selected(false) { }

    virtual void setAttribute(const std::string & name, const std::string & value);

    bool selected() const;
    DEFINE_ACCEPT();
  private:
    bool m_selected;
    std::string m_value;
    std::string m_label;

    const std::string * attributePtr(const std::string & name) const;
};
#endif
