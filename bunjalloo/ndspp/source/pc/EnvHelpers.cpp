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

#include <string>
#include <stdlib.h>

// This function reads the value of an environment variable, makes sure that
// it's a valid integer, and restricts it to the range [min, max]. If the
// environment variable isn't set or it has an invalid value (not a valid
// integer) it returns the default value passed to the function.
int getenv_int(const char *var_name, int default_value, int min, int max)
{
  const char *value = getenv(var_name);
  if (value == NULL)
    return default_value;

  int user_value;

  try
  {
    user_value = std::stoi(std::string(value));
  }
  catch (...)
  {
    return default_value;
  }

  if (user_value < min)
    return min;
  if (user_value > max)
    return max;

  return user_value;
}
