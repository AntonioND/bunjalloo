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
#include "utf8.h"
#include "HtmlParserDriver.h"
#include "ElementFactory.h"
#include "HtmlElement.h"
using namespace std;

void HtmlParserDriver::handleStartEndTag(const std::string & tag, const AttributeVector & attrs)
{
  handleStartTag(tag, attrs);
}

void HtmlParserDriver::handleStartTag(const std::string & tag, const AttributeVector & attrs)
{
  m_tags.push_back(tag);
  AttributeVector newVector;
  AttributeVector::const_iterator it(attrs.begin());
  for (; it != attrs.end(); ++it)
  {
    Attribute * at(*it);
    Attribute * newAttr = new Attribute(*at);
    newVector.push_back(newAttr);
  }

  HtmlElement * element = ElementFactory::create(tag, attrs);
  if (tag == "meta") { 
    checkMetaTagHttpEquiv(element); 
  }
  delete element;
  m_attributes.push_back(newVector);
}

void HtmlParserDriver::handleEndTag(const std::string & tag)
{
  m_tags.push_back(tag);
}

void HtmlParserDriver::handleData(unsigned int ucodeChar)
{
  if (ucodeChar == '\n')
    return;
  utf8::unchecked::append(ucodeChar, back_inserter(m_data));
}

void HtmlParserDriver::handleBinaryData(const void * data, unsigned int len)
{
  m_data += static_cast<const char*>(data);
}
