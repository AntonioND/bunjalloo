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
#ifndef BookmarkToolbar_h_seen
#define BookmarkToolbar_h_seen

#include "Toolbar.h"

/** A Toolbar that is shown when the user wants to bookmark a web site or list
 * the current bookmarks that she has saved.
 */
class BookmarkToolbar: public Toolbar
{
  public:
    /** Create a BookmarkToolbar.
     * @param view the View item that helps handle user input.
     */
    BookmarkToolbar(View & view);

    /** Update the icon glyphs based on the current state of the world. */
    virtual void updateIcons();

    virtual void tick();

  protected:
    void handlePress(int i);
    void layout();

};
#endif
