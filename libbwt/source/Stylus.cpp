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
#include <functional>
#include <algorithm>
#include "Stylus.h"
#include "StylusListener.h"

Stylus * Stylus::s_stylus(0);

Stylus::Stylus() :
  m_startX(0),
  m_startY(0),
  m_lastX(0),
  m_lastY(0),
  m_repeat(false),
  m_state(NOTHING)
{
}

Stylus::~Stylus()
{
}

void Stylus::startPoint(int & x, int & y) const
{
  x = m_startX;
  y = m_startY;
}

void Stylus::lastPoint(int & x, int & y) const
{
  x = m_lastX;
  y = m_lastY;
}

int Stylus::startX() const
{
  return m_startX;
}
int Stylus::startY() const
{
  return m_startY;
}
int Stylus::lastX() const
{
  return m_lastX;
}
int Stylus::lastY() const
{
  return m_lastY;
}

#define FOR_EACH_LISTENER(fn) \
{  \
  StylusListenerVector::iterator first(m_listeners.begin()); \
  StylusListenerVector::iterator end(m_listeners.end()); \
  for (; first != end; ++first) \
  { \
    if ( (*first)->fn(this) ) { \
      break; \
    } \
  } \
}

void Stylus::notifyDownFirst()
{
  FOR_EACH_LISTENER(stylusDownFirst);
}
void Stylus::notifyDownRepeat()
{
  FOR_EACH_LISTENER(stylusDownRepeat);
}
void Stylus::notifyDown()
{
  FOR_EACH_LISTENER(stylusDown);
}
void Stylus::notifyUp()
{
  FOR_EACH_LISTENER(stylusUp);
}

void Stylus::update(TouchType touch, bool repeat, int x, int y)
{
  if (touch == DOWN)
  {
    // if this is first time through, store start pos
    if (m_state != DOWN)
    {
      m_startX = x;
      m_startY = y;
      m_state = DOWN;
      m_repeat = false;
      m_lastX = x;
      m_lastY = y;
      notifyDownFirst();
    }
    else
    {
      m_state = DOWN;
      m_repeat = repeat;
      m_lastX = x;
      m_lastY = y;
      if (repeat)
      {
        notifyDownRepeat();
      }
      else
      {
        notifyDown();
      }
    }
  }
  else
  {
    if (m_state == DOWN)
    {
      m_state = UP;
      notifyUp();
    }
    else
    {
      m_state = NOTHING;
    }
    m_repeat = false;
  }
}

void Stylus::registerListener(StylusListener * listener)
{
  StylusListenerVector::const_iterator it(find(m_listeners.begin(), m_listeners.end(), listener));
  if (it == m_listeners.end())
  {
    m_listeners.push_back(listener);
  }
}

void Stylus::unregisterListener(StylusListener * listener)
{
  StylusListenerVector::iterator it(find(m_listeners.begin(), m_listeners.end(), listener));
  if (it != m_listeners.end())
  {
    m_listeners.erase(it);
  }
}

Stylus::TouchType Stylus::keysToTouchType(bool held, bool up)
{
  if (held)
  {
    return DOWN;
  }
  if (up)
  {
    return UP;
  }
  return NOTHING;
}

void Stylus::registerStylus(Stylus * stylus)
{
  delete s_stylus;
  s_stylus = stylus;
}

void Stylus::deleteInstance()
{
  delete s_stylus;
  s_stylus = 0;
}

Stylus * Stylus::instance()
{
  if (s_stylus == 0)
  {
    registerStylus(new Stylus);
  }
  return s_stylus;
}

