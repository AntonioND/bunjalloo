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
#ifndef InternalVisitor_h_seen
#define InternalVisitor_h_seen
#include "DefaultVisitor.h"
#include <string>

class InternalVisitor: public DefaultVisitor
{
  public:
    InternalVisitor(const std::string & name);

    inline bool found() const
    {
      return m_found;
    }

    inline int index() const
    {
      return m_index;
    }

    bool visit(HtmlAnchorElement & element);

  private:
    int m_index;
    bool m_found;
    std::string m_name;
};
#endif
