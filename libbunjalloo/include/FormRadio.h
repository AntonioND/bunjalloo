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
#ifndef FormRadio_h_seen
#define FormRadio_h_seen
#include <map>
#include "ButtonGroup.h"

class ButtonI;
class HtmlElement;
class FormRadio: public ButtonGroup
{
  public:
    void add(ButtonI * button, HtmlElement * element);

    void pressed(ButtonI * button);

  private:
    typedef std::map<ButtonI*, HtmlElement *> ButtonMap;
    ButtonMap m_buttonElements;
};

#endif
