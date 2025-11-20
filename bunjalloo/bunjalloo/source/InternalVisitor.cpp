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
#include "InternalVisitor.h"
#include "HtmlAnchorElement.h"

InternalVisitor::InternalVisitor(const std::string & name)
  : m_index(-1), m_found(false), m_name(name)
{
}

bool InternalVisitor::visit(HtmlAnchorElement & element)
{
  if (not m_found)
  {
    m_index++;
    const std::string &name(element.attribute("name"));
    const std::string &id(element.attribute("id"));
    if (id == m_name or name == m_name)
    {
      m_found = true;
    }
  }
  return true;
}
