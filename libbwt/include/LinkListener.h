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
#ifndef LinkListener_h_seen
#define LinkListener_h_seen

class Link;
/** Classes that want to register interest in link clicking should inherit from this interface. */
class LinkListener
{
  public:
    virtual ~LinkListener() {}

    /** Called when a Link is clicked somehwere.
     * @param link the link that has been clicked.
     */
    virtual void linkClicked(Link * link) = 0;

    /** Called when a Link is hovered over for some seconds.
     * @param link the link that has been hovered over.
     */
    virtual void linkPopup(Link * link) = 0;

};
#endif
