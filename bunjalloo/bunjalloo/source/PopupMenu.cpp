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
#include "Button.h"
#include "Canvas.h"
#include "ScrollPane.h"
#include "Stylus.h"
#include "PopupMenu.h"

PopupMenu::PopupMenu()
{
  setVisible(false);
}

PopupMenu::~PopupMenu()
{
  Stylus::instance()->unregisterListener(this);
}

void PopupMenu::addMenuItem(const std::string &name, Callback cb, void * data)
{
  addItem(name);
  using std::pair;
  m_callbacks.push_back(pair<Callback, void*>(cb, data));
}

void PopupMenu::pressed(ButtonI * button)
{
  // A button on the scroll bar is pressed. See which it was...
  int children = scrollPane()->childCount();
  for (int i = 0; i < children; ++i)
  {
    if (button == ((Button*)scrollPane()->childAt(i)))
    {
      m_callbacks[i].first(m_callbacks[i].second);
      break;
    }
  }

  // make sure the selection is reset.
  for (int i = 0; i < children; ++i)
  {
    ((Button*)scrollPane()->childAt(i))->setSelected(false);
  }
  setVisible(false);
}

void PopupMenu::paint(const nds::Rectangle & clip)
{
  if ( not m_visible)
  {
    return;
  }
  scrollPane()->setLocation(m_bounds.x, m_bounds.top());
  if (scrollPane()->bounds().bottom() > nds::Canvas::instance().height())
  {
    scrollPane()->setLocation(m_bounds.x, m_bounds.top() - scrollPane()->height());
  }

  if (scrollPane()->bounds().right() > nds::Canvas::instance().width())
  {
    scrollPane()->setLocation(m_bounds.x - scrollPane()->width(), scrollPane()->bounds().top());
  }

  nds::Rectangle scrollPaneClip(scrollPane()->bounds());
  nds::Canvas::instance().fillRectangle(scrollPaneClip.x,
      scrollPaneClip.y, scrollPaneClip.w, scrollPaneClip.h, scrollPane()->backgroundColor());
  scrollPane()->paint(scrollPane()->bounds());
}

bool PopupMenu::stylusUp(const Stylus * stylus)
{
  if (not visible())
  {
    return false;
  }
  if (FOR_EACH_CHILD(stylusUp)) {
    m_dirty = true;
    return true;
  }
  return false;
}

bool PopupMenu::stylusDownFirst(const Stylus * stylus)
{
  if (not visible())
  {
    return false;
  }
  if (FOR_EACH_CHILD(stylusDownFirst)) {
    m_dirty = true;
    return true;
  }
  // nothing hit? hide me then
  setVisible(false);
  return false;
}

void PopupMenu::updateLayout()
{
  m_preferredHeight = scrollPane()->height();
  m_preferredWidth = scrollPane()->width();
  setSize(m_preferredWidth, m_preferredHeight);
  Stylus::instance()->registerListener(this);
}
