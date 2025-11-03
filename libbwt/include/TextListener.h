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
#ifndef TextListener_h_seen
#define TextListener_h_seen

class TextEntryI;
/** Interface for a class that is interested in editing a TextEntryI.*/
class TextListener
{
  public:
    /** Destructor for this interface.*/
    virtual ~TextListener() { }

    /** This method is typically called by the TextEntryI when it is touched by
     * the user.
     * @param entry the TextEntryI that is to be edited.
     */
    virtual void editText(TextEntryI * entry) = 0;
};
#endif
