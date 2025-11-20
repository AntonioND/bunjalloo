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
#ifndef ButtonGroup_h_seen
#define ButtonGroup_h_seen

#include <vector>
#include "ButtonListener.h"
#include "util/classhelper.h"

/** Class to handle single selection across a group of buttons. */
class ButtonGroup: public ButtonListener
{
  public:

    ButtonGroup() {}

    /** Add a ButtonI to the group.
     * @param button the button to add.
     */
    void add(ButtonI * button);
    /** Remove a ButtonI from the group.
     * @param button the button to remove.
     */
    void remove(ButtonI * button);

    /** Called by a ButtonI when it is touched. Handles the exclusiveness of
     * presses amongst the button peers.
     * @param button the button that has been pressed.
     */
    virtual void pressed(ButtonI * button);

  private:
    std::vector<ButtonI*> m_group;
    DISALLOW_COPY_AND_ASSIGN(ButtonGroup);
};

#endif
