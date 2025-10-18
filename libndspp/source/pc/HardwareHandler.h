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
#ifndef HardwareHandler_h_seen
#define HardwareHandler_h_seen
#include <vector>

namespace nds
{
  template <class T>
  class HardwareHandler
  {
    public:
      //! Create a background implementation.
      static void add(T * p );
      //! Destroy a background implementation.
      static void remove(T * );

      //! Render the background objects.
      static void render();

      static bool compare(const T * x, const T * y);
      
    private:
      //! Keep track of the objects created.
      typedef typename std::vector<T*> VectorT;
      typedef typename std::vector<T*>::iterator VectorT_iterator;
      VectorT m_vector;

      static HardwareHandler& instance();
      
      HardwareHandler() {};
      ~HardwareHandler() {};
      
  };
};

#endif
