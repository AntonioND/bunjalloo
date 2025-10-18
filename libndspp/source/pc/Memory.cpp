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
#include <string.h>
#include "libnds.h"

void swiCopy(const void * source, void * dest, int flags)
{
  size_t amount(flags & 0xfffff);
  if (flags & COPY_MODE_WORD)
  {
    amount /= 2;
  }
  if (flags & COPY_MODE_FILL)
  {
    // ??
  }
  // void *memcpy(void *dest, const void *src, size_t n);
  ::memcpy(dest, source, amount*2);
}
