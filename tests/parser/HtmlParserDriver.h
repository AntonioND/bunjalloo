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
#ifndef HtmlParserDriver_h_seen
#define HtmlParserDriver_h_seen

#include "HtmlParser.h"
#include <vector>

class HtmlParserDriver : public HtmlParser
{

  public :

    std::string m_data;
    std::vector<std::string> m_tags;
    std::vector< AttributeVector > m_attributes;

  protected:
    virtual void handleStartEndTag(const std::string & tag, const AttributeVector & attrs);
    virtual void handleStartTag(const std::string & tag, const AttributeVector & attrs);
    virtual void handleEndTag(const std::string & tag);
    virtual void handleData(unsigned int ucodeChar);
    virtual void handleBinaryData(const void * data, unsigned int len);

};

#endif
