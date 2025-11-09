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
#ifndef Rectangle_h_seen
#define Rectangle_h_seen

namespace nds
{
  struct Rectangle
  {
    int x; //!< x position
    int y; //!< y position
    int w; //!< width
    int h; //!< height

    Rectangle();

    Rectangle(int x, int y, int w, int h);

    /** Hit test.
     * @param x x position to check
     * @param y y position to check
     * @return true if the given position is in the rectangle.
     */
    bool hit(int x, int y) const;

    /** @return the right edge. */
    int right() const;

    /** @return the left edge. */
    int left() const;

    /** @return the top edge. */
    int top() const;

    /** @return the bottom edge. */
    int bottom() const;

    /** Get the intersection with another Rectangle.
     * @param r2 the rectangle to intersect with. 
     * @return the intersection of this Rectangle with teh otehr one. */
    Rectangle intersect(const Rectangle & r2) const;

    /** Comparison operator for is-equal-to.
     * @param other the other Rectangle to compare to
     * @return true if the other rectangle and this rectangle are the same dimenstions
     */
    bool operator==(const Rectangle & other);

    /** Comparison operator for is-not-equal-to.
     * @param other the other Rectangle to compare to
     * @return true if the other rectangle and this rectangle are not the same dimenstions
     */
    bool operator!=(const Rectangle & other);

    Rectangle& operator=(const Rectangle &rhs);
  };
}

#endif
