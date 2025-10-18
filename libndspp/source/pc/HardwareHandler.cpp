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
#include "HardwareHandler.h"

using namespace nds;
using namespace std;

//! Add a renderable object.
template <class T> void HardwareHandler<T>::add(T * p)
{
  HardwareHandler::instance().m_vector.push_back(p);
};

//! remove the object.
template <class T> void HardwareHandler<T>::remove(T * p)
{
  VectorT & vectorT = HardwareHandler<T>::instance().m_vector;
  VectorT_iterator iter = find(vectorT.begin(), vectorT.end() , p);
  if (iter != vectorT.end())
  {
    HardwareHandler<T>::instance().m_vector.erase(iter);
  }
}

//! Render the background objects.
template <class T> void HardwareHandler<T>::render()
{
  VectorT & vectorT = HardwareHandler::instance().m_vector;
  sort(vectorT.begin(), vectorT.end(), HardwareHandler::compare);
  for_each(vectorT.begin(), vectorT.end(), mem_fun(&T::render));
}

template <class T> HardwareHandler<T>& HardwareHandler<T>::instance()
{
  static HardwareHandler<T> s_handler;
  return s_handler;
}

template <class T> bool HardwareHandler<T>::compare(const T * x, const T * y)
{
  if (!x)
    return true;
  if (!y)
    return false;
  return ( *x < *y );
}
