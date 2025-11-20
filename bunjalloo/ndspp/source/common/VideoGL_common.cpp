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
#include "VideoGL.h"
#ifdef ARM9
#include <nds/arm9/videoGL.h>
#else
#include <GL/gl.h>
#endif

void nds::VideoGL::glLoadIdentity()
{
  ::glLoadIdentity();
}
void nds::VideoGL::glEnd()
{
  ::glEnd();
}

void nds::VideoGL::glPushMatrix()
{
  ::glPushMatrix();
}

void nds::VideoGL::glBindTexture2D(int texture)
{
  ::glBindTexture(
#ifdef GL_TEXTURE_2D
      GL_TEXTURE_2D,
#else
      0,
#endif
      texture);
}


int nds::VideoGL::glGenTexture()
{
#ifdef ARM9
  int i;
#else
  GLuint i;
#endif
  ::glGenTextures(1, &i);
  return i;
}
