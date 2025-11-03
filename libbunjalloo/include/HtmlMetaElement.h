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
#ifndef HtmlMetaElement_h_seen
#define HtmlMetaElement_h_seen
#include "HtmlElement.h"

class HtmlMetaElement : public HtmlElement
{
  public:

    HtmlMetaElement(const std::string & tagName);
    HtmlElement * clone() const;
    DEFINE_ACCEPT();
  protected:
    std::string m_httpEquiv;
    std::string m_content;
    const std::string * attributePtr(const std::string & name) const;
    void copyAttributes(HtmlElement * copyTo) const;
};
#endif
