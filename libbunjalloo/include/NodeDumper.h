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
#ifndef NodeDumper_h_seen
#define NodeDumper_h_seen

namespace nds
{
  class File;
}
#include "DefaultVisitor.h"

class NodeDumper: public DefaultVisitor
{
  public:
    NodeDumper(const std::string & outfile);
    ~NodeDumper();

    virtual void begin(HtmlAnchorElement & element);
    virtual void end(HtmlAnchorElement & element);

    virtual bool visit(HtmlElement & element);
    virtual void begin(HtmlElement & element);
    virtual void end(HtmlElement & element);

    virtual void begin(HtmlMetaElement & element);

  private:
    nds::File & m_file;
    std::string m_lastElement;

    // add an attribute like this: name="value", value extracted from element
    void addAttribute(HtmlElement & element, const std::string & name);

    enum TagType_t
    {
      OPEN,
      OPEN_NO_END,
      CLOSE
    };
    void writeTag(const std::string & tagName, TagType_t type);
};
#endif
