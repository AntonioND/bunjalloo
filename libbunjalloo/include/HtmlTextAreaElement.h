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
#ifndef HtmlTextAreaElement_h_seen
#define HtmlTextAreaElement_h_seen

#include "HtmlElement.h"
class HtmlTextAreaElement:public HtmlElement
{
  public:
    HtmlTextAreaElement(const std::string & tagName)
      : HtmlElement(tagName), m_rows(0), m_cols(0)
    {}

    virtual void setAttribute(const std::string & name,
        const std::string & value);
    int rows() const;
    int cols() const;

    virtual void appendText(unsigned int value);

    DEFINE_ACCEPT();
  protected:
    int m_rows;
    int m_cols;
    std::string m_name;
    const std::string * attributePtr(const std::string & name) const;
};
#endif
