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
#include "libnds.h"
#include "RadioButton.h"
#include "Palette.h"
#include "radiobutton_img.h"

RadioButton::RadioButton():ToggleButton()
{
  setSize(18, 18);
}

const unsigned char * RadioButton::getGfxData()
{
  int offset = 0;
  if (selected())
    offset |= 1;
  if (touched())
    offset |= 2;
  return (const u8*)&radiobutton_imgTiles[8*8*offset];
}

const unsigned short * RadioButton::getPaletteData()
{
  return radiobutton_imgPal;
}


