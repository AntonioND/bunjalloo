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
#ifndef System_h_seen
#define System_h_seen

namespace nds
{
  class System
  {

    public:
      /** Get the system name. */
      static const char * uname();

      // Supported:
      //
      //   0 = Japanese
      //   1 = English
      //   2 = French
      //   3 = German
      //   4 = Italian
      //   5 = Spanish
      //
      // Not supported:
      //
      //   6 = Chinese(?)
      //   7 = Unknown/Reserved
      static int language();
  };
}

#endif
