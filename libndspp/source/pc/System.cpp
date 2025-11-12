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
#include <stdlib.h>
#include "EnvHelpers.h"
#include "System.h"
#include "libnds.h"

const char * nds::System::uname()
{
  return "SDL";
}

int nds::System::language()
{
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

  // Default to English
  return getenv_int("BUNJALLOO_LANGUAGE", 1, 0, 5);
}
