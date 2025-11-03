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
#ifndef ComboBox_h_seen
#define ComboBox_h_seen
#include "Component.h"
#include "ButtonListener.h"
#include <string>

class ScrollPane;
class Button;
/** A ComboBox is an item that shows a button with the initial value, and a
 * drop down list with potential values. When the button is clicked, the menu
 * is shown.*/
class ComboBox: public Component,
                private ButtonListener
{
  public:
    /** Create an empty ComboBox.*/
    ComboBox();

    /** End of the objects existence.*/
    //~ComboBox();

    /** Add an item to the list. If it is the first item, it is selected.
     * @param item the text to add.
     */
    void addItem(const std::string &item);

    /** A button on the drop down list has been pressed. Set the text of the
     * main button to the selected item.
     * @param button the button that was pressed.
     */
    void pressed(ButtonI * button);

    int selectedIndex() const;
    const std::string & selectedItem() const;
    void setSelectedIndex(int select);
    unsigned int items() const;

    // implemented from Component.
    virtual void paint(const nds::Rectangle & clip);
    virtual void setLocation(int x, int y);
    virtual void setSize(unsigned int w, unsigned int h);

    virtual bool stylusUp(const Stylus * stylus);
    virtual bool stylusDownFirst(const Stylus * stylus);
    virtual bool stylusDownRepeat(const Stylus * stylus);
    virtual bool stylusDown(const Stylus * stylus);

  protected:
    inline ScrollPane * scrollPane();
    inline const ScrollPane * scrollPane() const;
    inline Button * button();

  private:
    int m_items;
    int m_selectedIndex;
    bool m_open;
    DISALLOW_COPY_AND_ASSIGN(ComboBox);

};

ScrollPane * ComboBox::scrollPane() {
  return (ScrollPane*)m_children.front();
}

const ScrollPane * ComboBox::scrollPane() const {
  return (const ScrollPane*)m_children.front();
}

#endif
