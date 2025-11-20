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
#ifndef PopupMenu_h_seen
#define PopupMenu_h_seen
#include "ComboBox.h"
#include <vector>

class PopupMenu: public ComboBox
{
  public:
    typedef void (*Callback)(void*);
    PopupMenu();

    ~PopupMenu();

    /** Add an option to the menu. This will typically be:
     * @code
     *   popup->addMenuItem(T("name"), MyClass::staticFunction, this);
     * @endcode
     * Adn the static callback function would then be:
     * @code
     *   void MyClass::staticFunction(void * self)
     *   {
     *     ((MyClass*)self)->doStuff();
     *   }
     * @endcode
     * @param name the name to show on the menu.
     * @param cb the callback function to use when the menu is activated.
     * @param data use defined data passed in to the @a cb.
     */
    void addMenuItem(const std::string & name, Callback cb, void * data);

    /** Layout the menu correctly once all the items are added. */
    void updateLayout();

    void pressed(ButtonI * button);
    void paint(const nds::Rectangle & clip);
    bool stylusUp(const Stylus * stylus);
    bool stylusDownFirst(const Stylus * stylus);
  private:
    typedef std::vector<std::pair<Callback , void*> > CallbackArray;
    CallbackArray m_callbacks;
};


#endif
