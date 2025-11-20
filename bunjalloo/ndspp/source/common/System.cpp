/*
  Copyright (C) 2025 Antonio Niño Díaz

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

#include "Slot2Allocator.h"
#include "System.h"

std::string nds::System::slot2meminfo()
{
  std::string info;

  size_t _used, _free, _total;
  Slot2Allocator::instance().get_usage(&_used, &_free, &_total);
  if (_total == 0)
  {
    info += std::string("Slot-2 RAM: Not detected");
  }
  else
  {
    info += std::string("Slot-2 RAM: ")
            + std::to_string(_free / 1024) + std::string(" / ")
            + std::to_string(_total / 1024) + std::string(" KiB");
  }

  return info;
}
