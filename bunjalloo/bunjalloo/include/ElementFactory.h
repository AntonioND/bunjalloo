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
#ifndef ElementFactory_h_seen
#define ElementFactory_h_seen

#include <string>
#include "Attribute.h"
class HtmlElement;

class ElementFactory
{
  public:
    static HtmlElement * create(const std::string & elementType);
    static HtmlElement * create(const std::string & elementType,
        const AttributeVector & attrs);

    static void remove(HtmlElement * element);

  private:
    ElementFactory();
    ~ElementFactory();
};
#endif
