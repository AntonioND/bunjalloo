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
#ifndef HtmlImageElement_h_seen
#define HtmlImageElement_h_seen
#include "HtmlElement.h"

class HtmlImageElement : public HtmlElement
{
  public:
    HtmlImageElement(const std::string & tagName)
      : HtmlElement(tagName),m_hasAltText(false)
    {}

    HtmlElement * clone() const;
    virtual void setAttribute(const std::string & name,
        const std::string & value);
    bool hasAltText() const;
    DEFINE_ACCEPT();
  protected:
    std::string m_src;
    std::string m_alt;
    std::string m_name;
    bool m_hasAltText;
    const std::string * attributePtr(const std::string & name) const;
    void copyAttributes(HtmlElement * copyTo) const;
};
#endif
