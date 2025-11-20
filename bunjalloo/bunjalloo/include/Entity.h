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
#ifndef Entity_h_seen
#define Entity_h_seen

#include <string>

/** An HTML entity, for use with "&NNN;" type values.*/
struct Entity {
  //! The name of the entity.
  const char * const name;
  //! The equivalent Unicode character value.
  const unsigned int value;
};

//! Total number of entities.
#define ENTITY_COUNT 259
//! A static array of Entity values.
extern const Entity s_entity[ENTITY_COUNT];

#endif
