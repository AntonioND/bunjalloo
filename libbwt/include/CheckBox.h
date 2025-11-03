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
#ifndef CheckBox_h_seen
#define CheckBox_h_seen

#include "ToggleButton.h"
/** A CheckBox is a button that can be selected or deselected, toggling its
 * state each time it is pressed.*/
class CheckBox : public ToggleButton
{
  public:
    /** Create an instance of this class. */
    CheckBox();

  protected:
    virtual const unsigned char * getGfxData();
    virtual const unsigned short * getPaletteData();

  private:
    DISALLOW_COPY_AND_ASSIGN(CheckBox);
};
#endif
