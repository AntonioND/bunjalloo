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
#include <algorithm>
#include <functional>
#include "Component.h"
#include "Stylus.h"
#include "Delete.h"

static const int NO_PREFERRED_SIZE(-1);

Component::Component() :
  m_bounds(),
  m_preferredWidth(NO_PREFERRED_SIZE),
  m_preferredHeight(NO_PREFERRED_SIZE),
  m_visible(true),
  m_dirty(true)
{ }

Component::~Component()
{
  removeChildren();
}

void Component::removeChildren()
{
  for_each(m_children.begin(), m_children.end(), delete_ptr());
  m_children.clear();
}

void Component::add(Component * child)
{
  // check if not already added
  std::vector<Component*>::const_iterator it = std::find(m_children.begin(), m_children.end(), child);
  if (it == m_children.end())
  {
    m_children.push_back(child);
  }
}

void Component::setSize(unsigned int w, unsigned int h)
{
  m_bounds.w = w;
  m_bounds.h = h;
  m_dirty = true;
}

nds::Rectangle Component::bounds() const
{
  return m_bounds;
}

nds::Rectangle Component::preferredSize() const
{
  nds::Rectangle r(0, 0, m_preferredWidth, m_preferredHeight);
  if (m_preferredWidth == NO_PREFERRED_SIZE)
  {
    r.w = m_bounds.w;
  }
  if (m_preferredHeight == NO_PREFERRED_SIZE)
  {
    r.h = m_bounds.h;
  }
  return r;
}

void Component::setLocation(int x, int y)
{
  m_bounds.x = x;
  m_bounds.y = y;
  m_dirty = true;
}

unsigned int Component::childCount() const
{
  return m_children.size();
}

const Component * Component::childAt(unsigned int i) const
{
  if (not m_children.empty() and m_children.size() > i)
  {
    return m_children[i];
  }
  return 0;
}

bool Component::dirty() const
{
  return m_dirty;
}

void Component::setVisible(bool visible)
{
  if (visible != m_visible)
    m_dirty = true;
  m_visible = visible;
}

static bool is(bool v) { return v; }

bool Component::callStylusFunction(bool (StylusListener::*fun)(const Stylus*), const Stylus *stylus)
{
  std::vector<bool> results(m_children.size());
  transform(m_children.begin(), m_children.end(), results.begin(),
      std::bind2nd(std::mem_fun(fun), stylus));
  return results.end() != find_if(results.begin(), results.end(), is);
}
