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
#ifndef BoxLayout_h_seen
#define BoxLayout_h_seen

#include "Component.h"
#include <list>

/** Class to provide flow-like positional layout. */
class BoxLayout: public Component
{
  public:
    BoxLayout();
    ~BoxLayout();

    unsigned int boxCount() const;

    /** layout the child components again.
     * @param force force the relayout, e.g. if resize this component */
    void doLayout(bool force=false);

    /** Force there to be an end-of-line after the last component. */
    void insertNewline();

    // Reimplement Component methods.
    virtual void setLocation(int x, int y);
    virtual void setSize(unsigned int w, unsigned int h);
    virtual void add(Component *child);
    virtual void paint(const nds::Rectangle & clip);

    // Stylus handling. TODO
    virtual bool stylusUp(const Stylus * stylus);
    virtual bool stylusDownFirst(const Stylus * stylus);
    virtual bool stylusDownRepeat(const Stylus * stylus);
    virtual bool stylusDown(const Stylus * stylus);
  private:
    class Box;
    std::list<Box*> m_boxes;
    void addToLayout(Component *child);
    void initBoxes();
    DISALLOW_COPY_AND_ASSIGN(BoxLayout);
};
#endif
