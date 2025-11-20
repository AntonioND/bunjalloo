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
#ifndef RadioButton_h_seen
#define RadioButton_h_seen

#include "ToggleButton.h"
/** A typically round button that the user can toggle on or off. */
class RadioButton : public ToggleButton
{
  public:
    /** Sets up the new RadioButton.*/
    RadioButton();

  protected:
    virtual const unsigned char * getGfxData();
    virtual const unsigned short * getPaletteData();

};
#endif
