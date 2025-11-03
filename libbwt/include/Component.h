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
#ifndef Component_h_seen
#define Component_h_seen

#include <vector>
#include "Rectangle.h"
#include "StylusListener.h"
#include "util/classhelper.h"

//! Helper define for calling stylus callbacks on children
#define FOR_EACH_CHILD(fn) callStylusFunction(&Component::fn, stylus)

/** The base class for all GUI Widgets.*/
class Component: public StylusListener
{
  public:
    /** Set up the Component.*/
    Component();

    /** End of the Component life - removes (deletes) all its child
     * Components.*/
    virtual ~Component();

    /** Paint the widget, clipping it to the given area.
     * @param clip the area to clip painting to.
     */
    virtual void paint(const nds::Rectangle & clip) = 0;

    /** Add a child Component.
     * @param child the child Component to add.
     */
    void add(Component * child);

    /** Remove and delete all child Components. */
    void removeChildren();

    /** Set the absolute on screen location.
     * @param x the horizontal x position.
     * @param y the vertical y position.
     */
    virtual void setLocation(int x, int y);

    /** Set the size in pixels.
     * @param w the width.
     * @param h the height.
     */
    virtual void setSize(unsigned int w, unsigned int h);
    /** Get the position and size.
     * @return a Rectangle that holds the horizontal and vertical coordinates
     * as well as the width and height of the Component.
     */
    nds::Rectangle bounds() const;

    /** Get the preferred size. This is the "ideal" size that the Component would like to be if it were allowed.
     * @return a Rectangle that holds the width and height of the preferred size. The x,y coordinates are meaningless.
     */
    nds::Rectangle preferredSize() const;

    /** Get the current width.
     * @return the width in pixels.
     */
    inline int width() const;

    /** Get the current height.
     * @return the height in pixels.
     */
    inline int height() const;

    /** Get the current horizontal x position.
     * @return the absolute x position.
     */
    inline int x() const;

    /** Get the current vertical y position.
     * @return the absolute y position.
     */
    inline int y() const;

    /** Get the current visibility.
     * @return true if visible, false otherwise.
     */
    inline bool visible() const;

    /** Set the visibility.
     * @param visible true if this Component is visible.
     */
    void setVisible(bool visible=true);

    /** Get the number of children that this Component has.
     * @return the number of child components
     */
    unsigned int childCount() const;

    /** Get the child Component at the given index.
     * @param i the index of the child.
     * @return the Child component at @a i or 0 if not present.
     */
    const Component * childAt(unsigned int i) const;

    /** @return true if the Component needs repainting. */
    virtual bool dirty() const;

    bool callStylusFunction(bool (StylusListener::*fun)(const Stylus*), const Stylus *stylus);
  protected:
    /** Holds the position and size. */
    nds::Rectangle m_bounds;

    /** The width that this component would like to be. */
    int m_preferredWidth;

    /** The height that this component would like to be. */
    int m_preferredHeight;

    /** The visibility. */
    bool m_visible;

    /** The child Components. */
    std::vector<Component *> m_children;

    bool m_dirty;

  private:
    DISALLOW_COPY_AND_ASSIGN(Component);
};

int Component::width() const
{
  return m_bounds.w;
}
int Component::height() const
{
  return m_bounds.h;
}

int Component::x() const
{
  return m_bounds.x;
}
int Component::y() const
{
  return m_bounds.y;
}

bool Component::visible() const
{
  return m_visible;
}

#endif
