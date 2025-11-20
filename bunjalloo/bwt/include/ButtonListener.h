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
#ifndef ButtonListener_h_seen
#define ButtonListener_h_seen

class ButtonI;
/** Classes that want to register interest in button presses should inherit
 * from this interface.*/
class ButtonListener
{
  public:

    /** End of lifetime. */
    virtual ~ButtonListener() {};

    /** Callback for when a ButtonI is pressed.
     * @param button typically this is the button that was pressed.
     */
    virtual void pressed(ButtonI * button) = 0;
};
#endif
