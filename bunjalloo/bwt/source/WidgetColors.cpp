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
#include "WidgetColors.h"

#define FG      RGB5(27,27,27)
#define SHAD    RGB5(21,21,21)
#define VDARK   RGB5(14,14,14)
#define PRESSED RGB5(24,24,24)
#define BRIGHT  RGB5(31,31,31)

const unsigned short WidgetColors::BUTTON_FOREGROUND = FG;
const unsigned short WidgetColors::BUTTON_PRESSED = PRESSED;
const unsigned short WidgetColors::BUTTON_SHADOW = SHAD;
const unsigned short WidgetColors::BUTTON_SHINE = BRIGHT;

const unsigned short WidgetColors::COMBOBOX_DROP_DOWN = FG;
const unsigned short WidgetColors::COMBOBOX_FOREGROUND = FG;
const unsigned short WidgetColors::COMBOBOX_SELECTED = PRESSED;

const unsigned short WidgetColors::SCROLLBAR_BACKGROUND = SHAD;
const unsigned short WidgetColors::SCROLLBAR_BACKGROUND_TOUCH = RGB5(5, 5, 28);
const unsigned short WidgetColors::SCROLLBAR_BORDER = VDARK;
const unsigned short WidgetColors::SCROLLBAR_ARROW = FG;
const unsigned short WidgetColors::SCROLLBAR_ARROW_HEAD = VDARK;
const unsigned short WidgetColors::SCROLLBAR_HANDLE_NOT_HELD = FG;
const unsigned short WidgetColors::SCROLLBAR_HANDLE_HELD = RGB5(28, 28, 11);

const unsigned short WidgetColors::TEXT_FIELD_TOUCH = RGB5(22, 22, 31);

const unsigned short WidgetColors::LINK_VISITED = RGB5(19, 0,22);
const unsigned short WidgetColors::LINK_CLICKED = RGB5(31, 0, 0);
const unsigned short WidgetColors::LINK_REGULAR = RGB5( 0, 0,31);
const unsigned short WidgetColors::LINK_IMAGE   = RGB5( 0,21, 0);
