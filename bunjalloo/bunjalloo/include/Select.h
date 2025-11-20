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
#ifndef Select_h_seen
#define Select_h_seen

#include "ComboBox.h"

class HtmlElement;
/** A form select control. Shows a drop down list of items.
 */
class Select: public ComboBox
{
  public:
    Select(HtmlElement * select);

    void init();

    /** Overridden from ComboBox::pressed() in order to capture selection
     * events and set the HtmlElement value.
     * @param button the menu button pressed.
     */
    virtual void pressed(ButtonI * button);

  private:
    HtmlElement * m_element;

};

#endif
