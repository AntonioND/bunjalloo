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
#ifndef ToggleButton_h_seen
#define ToggleButton_h_seen

#include "ButtonI.h"
#include "Component.h"

/** A dual state button that toggles on and off as the user presses it.*/
class ToggleButton: public Component, public ButtonI
{
  public:
    /** Construct a new ToggleButton.*/
    ToggleButton();

    // from Component
    virtual void paint(const nds::Rectangle & clip);

    virtual bool stylusUp(const Stylus * stylus);
    virtual bool stylusDownFirst(const Stylus * stylus);
    virtual bool stylusDownRepeat(const Stylus * stylus);
    virtual bool stylusDown(const Stylus * stylus);

    bool touched() const;
  protected:
    /** Concrete classes that inherit from ToggleButton should return the
     * graphics to display.
     * @return the graphics data, in "sprite base" tile format. The implementer
     * should return the correct data for selected/unselected state by checking
     * selected()
     */
    virtual const unsigned char *getGfxData() = 0;

    /** Concrete classes that inherit from ToggleButton should return the
     * palette for the displayed graphics.
     * @return the palette data that corresponds to the value returned by getGfxData()
     */
    virtual const unsigned short *getPaletteData() = 0;

  private:
    bool m_touched;
    DISALLOW_COPY_AND_ASSIGN(ToggleButton);
};
#endif
