/*
  Copyright (C) 2008 Richard Quirk

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
#include "File.h"
#include "HtmlAnchorElement.h"
#include "HtmlConstants.h"
#include "HtmlElement.h"
#include "HtmlMetaElement.h"
#include "NodeDumper.h"

NodeDumper::NodeDumper(const std::string & name):
  m_file(*(new nds::File))
{
  m_file.open(name.c_str(), "w");
}

NodeDumper::~NodeDumper()
{
  delete &m_file;
}

void NodeDumper::addAttribute(HtmlElement & element, const std::string & name)
{
  if (not m_file.is_open()) return;

  const std::string & attr(element.attribute(name));
  if (not attr.empty())
  {
    m_file.write(name.c_str());
    m_file.write("=\"");
    m_file.write(attr.c_str());
    m_file.write("\" ");
  }
}

void NodeDumper::writeTag(const std::string & tag, TagType_t type)
{
  switch (type)
  {
    case OPEN_NO_END:
      m_file.write("<");
      m_file.write(tag.c_str());
      m_file.write(" ");
      break;
    case OPEN:
      m_file.write("<");
      m_file.write(tag.c_str());
      m_file.write(">");
      break;
    case CLOSE:
      m_file.write("</");
      m_file.write(tag.c_str());
      m_file.write(">\n");
      break;
  }
}


void NodeDumper::begin(HtmlAnchorElement & element)
{
  if (m_file.is_open())
  {
    writeTag(element.tagName(), OPEN_NO_END);
    addAttribute(element, "href");
    addAttribute(element, "name");
    m_file.write(">\n");
  }
}

void NodeDumper::end(HtmlAnchorElement & element)
{
  if (m_file.is_open())
  {
    writeTag(element.tagName(), CLOSE);
  }
}

bool NodeDumper::visit(HtmlElement & element)
{
  if (m_file.is_open() and element.isa(HtmlConstants::TEXT))
  {
    m_file.write(element.text().c_str());
  }
  return true;
}

void NodeDumper::begin(HtmlElement & element)
{
  if (m_file.is_open())
  {
    if (not element.isa(HtmlConstants::TEXT) and m_lastElement != element.tagName())
    {
      writeTag(element.tagName(), OPEN);
    }
    m_lastElement = element.tagName();
  }
}

void NodeDumper::end(HtmlElement & element)
{
  if (m_file.is_open())
  {
    if (not element.isa(HtmlConstants::TEXT) and not element.isa(HtmlConstants::BR_TAG))
    {
      writeTag(element.tagName(), CLOSE);
    }
  }
}

void NodeDumper::begin(HtmlMetaElement & element)
{
  if (m_file.is_open())
  {
    writeTag(element.tagName(), OPEN_NO_END);
    addAttribute(element, "http-equiv");
    addAttribute(element, "content");
    m_file.write(">\n");
  }
}
