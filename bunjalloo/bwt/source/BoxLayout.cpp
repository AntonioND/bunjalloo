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

#include <set>
#include <algorithm>
#include <functional>
#include "libnds.h"
#include "Canvas.h"
#include "BoxLayout.h"
#include "Delete.h"
#include "Palette.h"

using nds::Rectangle;

/*
 * BoxLayout has a list of Box
 * Each Box has a set of BoundComponent
 *
 */

class BoxLayout::Box
{
  public:
    Box(BoxLayout * parent): m_parent(parent), m_forceEnd(false)
    {
      m_bounds.x = 0;
      m_bounds.y = 0;
      m_bounds.w = 0;
      m_bounds.h = 0;
    }

    ~Box()
    {
      for_each(m_children.begin(), m_children.end(), delete_ptr());
    }

    void setPosition(int x, int y)
    {
      m_bounds.x = x;
      m_bounds.y = y;
    }

    void setSize(int w, int h)
    {
      m_bounds.w = w;
      m_bounds.h = h;
    }

    Rectangle bounds() const
    {
      return m_bounds;
    }

    bool hasChanged()
    {
      // see if any child components need redoing
      BoundComponentSet::iterator it(find_if(m_children.begin(), m_children.end(), std::mem_fun(&BoundComponent::hasChanged)));
      return it != m_children.end();
    }

    void move(int dx, int dy)
    {
      for (BoundComponentSet::iterator it(m_children.begin());
          it != m_children.end(); ++it)
      {
        BoundComponent *c(*it);
        c->move(dx, dy);
      }
      m_bounds.x -= dx;
      m_bounds.y -= dy;
    }

    /** Force the end of the area. */
    void setForceEnd(bool forceEnd)
    {
      m_forceEnd = forceEnd;
    }

    bool forceEnd()
    {
      return m_forceEnd;
    }

    Component *lastComponent()
    {
      if (m_children.empty())
        return 0;
      BoundComponent *bc(*m_children.rbegin());
      return bc->component();
    }

    void addPrivate(Component *child)
    {
      BoundComponent * boundChild = new (std::nothrow) BoundComponent(child);
      if (boundChild == NULL)
        libndsCrash("addPrivate(): OOM");

      m_children.insert(boundChild);
      recalcSize();
    }

    int m_indentation { -1 };

  private:
    BoxLayout *m_parent;
    Rectangle m_bounds;
    class BoundComponent
    {
      public:
        BoundComponent(Component *c):
          m_component(c),m_bounds(c->bounds()) {}

        bool hasChanged() const
        {
          return m_component->bounds() != m_bounds;
        }

        void move(int dx, int dy)
        {
          m_bounds.x -= dx;
          m_bounds.y -= dy;
        }

        inline Component *component() const
        {
          return m_component;
        }

      private:
        Component *m_component;
        Rectangle m_bounds;
    };
    typedef std::set<BoundComponent*> BoundComponentSet;
    BoundComponentSet m_children;
    bool m_forceEnd;

    bool initialised() const
    {
      return m_bounds.h != 0;
    }

    // on adding a child, this recalculates the width and the height
    // that the box uses and stores it in m_bounds
    // width is total width of all components
    // height is height of tallest child component
    void recalcSize()
    {
      int w(0);
      int h(-1);
      for (BoundComponentSet::iterator it(m_children.begin());
          it != m_children.end();
          ++it) {
        BoundComponent *child(*it);
        w += child->component()->width();
        if (child->component()->height() > h)
          h = child->component()->height();
      }
      m_bounds.w = w;
      m_bounds.h = h;
    }
};

BoxLayout::BoxLayout()
{
  initBoxes();
}

void BoxLayout::initBoxes()
{
  Box *first(new (std::nothrow) Box(this));
  if (first == NULL)
    libndsCrash("initBoxes(): OOM");

  first->setPosition(m_bounds.x,m_bounds.y);
  m_boxes.push_front(first);
}

BoxLayout::~BoxLayout()
{
  for_each(m_boxes.begin(), m_boxes.end(), delete_ptr());
}

void BoxLayout::addToLayout(Component *child)
{
  // See where this component should be placed considering the location of the
  // previous components. It can only place components directly to the right of
  // the previous one or directly below all previous components. It can't place
  // it below a component that is already to the right of another component.

  // TODO: Add a "setSizeFixed()" method to prevent RichTextArea and
  // friends from resizing after being fixed into position?

  const Rectangle &bounds(child->preferredSize());

  // Calculate max allowed width considering the indentation of the element.
  int w(bounds.w);
  if (child->indentation() == -1)
  {
    if (w > m_bounds.w)
      w = m_bounds.w;
  }
  else
  {
    if (w > m_bounds.w + child->indentation())
      w = m_bounds.w - child->indentation();
  }
  child->setSize(w, bounds.h);

  // Get position of the previous component
  int lastBoxX = 0;
  int lastBoxY = 0;
  int lastBoxW = 0;
  //int lastBoxH = 0;
  bool lastBoxForceEnd = false;
  {
    Box *lastBox(m_boxes.front());
    if (lastBox)
    {
      lastBoxX = lastBox->bounds().x;
      lastBoxY = lastBox->bounds().y;
      lastBoxW = lastBox->bounds().w;
      //lastBoxH = lastBox->bounds().h;
      lastBoxForceEnd = lastBox->forceEnd();
    }
  }

  bool canBeAppended = false;

  if ((!lastBoxForceEnd) and (child->indentation() == -1))
  {
    // If a new line hasn't been forced and a new indentation hasn't been set,
    // try to add the new component to the right of the previous ones.

    if ((w + lastBoxX + lastBoxW) < width())
    {
      canBeAppended = true;
      child->setLocation(lastBoxX + lastBoxW, lastBoxY);
    }
  }

  if (!canBeAppended)
  {
    // If it can't be appended (it doesn't fit, or a new line/indentation has
    // been requested) add it below everything else.
    child->setLocation(child->indentation(), m_bounds.h);
  }

  Box * newBox = new (std::nothrow) Box(this);
  if (newBox == NULL)
    libndsCrash("addToLayout(): OOM");

  newBox->setPosition(child->x(), child->y());
  newBox->setSize(child->width(), child->height());

  newBox->addPrivate(child);
  m_boxes.push_front(newBox);

  // Add height of the last box, whatever it was to the bounds height
  int newLimitY = newBox->bounds().y + newBox->bounds().h;
  if (newLimitY > m_bounds.h)
    m_bounds.h = newLimitY;
}

void BoxLayout::add(Component *child)
{
  Component::add(child);
  // Fudge size vs preferredSize
  addToLayout(child);
}

void BoxLayout::insertNewline()
{
  // magically add something that says "this box has a new line"!
  if (m_boxes.size() > 1 or m_boxes.front()->lastComponent() != 0)
  {
    m_boxes.front()->setForceEnd(true);
  }
}

void BoxLayout::paint(const nds::Rectangle & clip)
{
  using nds::Rectangle;
  // paint all children.
  for (std::vector<Component*>::iterator it(m_children.begin());
      it != m_children.end();
      ++it)
  {
    Component * c(*it);
    Rectangle bounds(c->bounds());
    Rectangle thisClip(clip.intersect(bounds));
    if (thisClip.w == 0 and thisClip.h == 0)
      continue;
    c->paint(thisClip);
    nds::Canvas::instance().setClip(clip);

#if 0
    // Draw boxes around each Component to help debugging
    nds::Color col(31, 0, 0);
    nds::Canvas::instance().horizontalLine(thisClip.x, thisClip.top(), thisClip.w, col);
    nds::Canvas::instance().verticalLine(thisClip.left(), thisClip.top(), thisClip.h, col);
    nds::Canvas::instance().horizontalLine(thisClip.x, thisClip.bottom()-1, thisClip.w, col);
    nds::Canvas::instance().verticalLine(thisClip.right(), thisClip.top(), thisClip.h, col);
#endif
  }
}

void BoxLayout::setLocation(int x, int y)
{
  // work out dx, dy
  int dx = this->x() - x;
  int dy = this->y() - y;
  Component::setLocation(x, y);
  if (dx == 0 and dy == 0)
    return;

  for (std::vector<Component*>::iterator it(m_children.begin());
      it != m_children.end();
      ++it)
  {
    Component * child(*it);
    child->setLocation( child->x() - dx, child->y() - dy);
  }
  for (std::list<Box*>::iterator it(m_boxes.begin());
      it != m_boxes.end();
      ++it)
  {
    Box *box(*it);
    box->move(dx, dy);
  }
}

void BoxLayout::setSize(unsigned int w, unsigned int h)
{
  if (static_cast<int>(w) != m_bounds.w)
  {
    m_bounds.w = w;
    doLayout(true);
  }
}

unsigned int BoxLayout::boxCount() const
{
  return m_boxes.size();
}

void BoxLayout::doLayout(bool force)
{
  // The height needs to be set to 0 so that it is recalculated here. HOwever, X
  // and Y shouldn't be needed in principle. In practice, they also need to be
  // set to 0.
  m_bounds.x = 0;
  m_bounds.y = 0;
  m_bounds.h = 0;

  std::vector<bool> forceEndVector;

  // Delete all Boxes to avoid mem leaks
  for (std::list<Box*>::const_iterator it(m_boxes.begin()); it != m_boxes.end(); ++it)
  {
    Box *b(*it);
    forceEndVector.push_back(b->forceEnd());
    delete b;
  }
  m_boxes.clear();

  // Recreate list of Boxes
  initBoxes();
  int i = 0;
  for (std::vector<Component*>::iterator it(m_children.begin()); it != m_children.end(); ++it)
  {
    Component *c(*it);
    addToLayout(c);
    if (forceEndVector[i++])
      insertNewline();
  }
}

bool BoxLayout::stylusUp(const Stylus * stylus)
{
  if (not visible())
    return false;
  m_dirty = true;
  return FOR_EACH_CHILD(stylusUp);
}

bool BoxLayout::stylusDownFirst(const Stylus * stylus)
{
  if (not visible())
    return false;
  m_dirty = true;
  return FOR_EACH_CHILD(stylusDownFirst);
}

bool BoxLayout::stylusDownRepeat(const Stylus * stylus)
{
  if (not visible())
    return false;
  m_dirty = true;
  return FOR_EACH_CHILD(stylusDownRepeat);
}

bool BoxLayout::stylusDown(const Stylus * stylus)
{
  if (not visible())
    return false;
  m_dirty = true;
  return FOR_EACH_CHILD(stylusDown);
}

