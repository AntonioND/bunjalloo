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
#include <nds/arm9/videoGL.h>

void nds::VideoGL::glMatrixMode(GL_MATRIX_MODE_ENUM mode)
{
  ::glMatrixMode((::GL_MATRIX_MODE_ENUM)mode);
}
void nds::VideoGL::glBegin(GL_GLBEGIN_ENUM b)
{
  ::glBegin((::GL_GLBEGIN_ENUM)b);
}


void nds::VideoGL::glInit()
{
  ::glInit();
}

void nds::VideoGL::glPopMatrix(int i)
{
  ::glPopMatrix(i);
}

void nds::VideoGL::glFlush(int i)
{
  ::glFlush(i);
}

void nds::VideoGL::glViewport(int x, int y, int w, int h)
{
  ::glViewport(x, y, w, h);
}

void nds::VideoGL::glClearPolyID(int id)
{
  ::glClearPolyID(id);
}

void nds::VideoGL::gluPerspectivef32(int fovy, int aspect, int zNear, int zFar)
{
  ::gluPerspectivef32(fovy, aspect, zNear, zFar);
}

void nds::VideoGL::gluLookAtf32(int eyex, int eyey, int eyez, int lookAtx, int lookAty, int lookAtz, int upx, int upy, int upz)
{
  ::gluLookAtf32(eyex, eyey, eyez, lookAtx, lookAty, lookAtz, upx, upy, upz);
}

void nds::VideoGL::glPolyFmt(int params)
{
  ::glPolyFmt(params);
}

void nds::VideoGL::glTranslate3f32(int x, int y, int z)
{
  ::glTranslate3f32(x, y, z);
}

void nds::VideoGL::glRotatef32i(int angle, int x, int y, int z)
{
  ::glRotatef32i(angle, x, y, z);
}

void nds::VideoGL::glRotateXi(int angle)
{
  ::glRotateXi(angle);
}
void nds::VideoGL::glRotateYi(int angle)
{
  ::glRotateYi(angle);
}
void nds::VideoGL::glRotateZi(int angle)
{
  ::glRotateZi(angle);
}

void nds::VideoGL::glVertex3v16(short x, short y, short z)
{
  ::glVertex3v16(x, y, z);
}
void nds::VideoGL::glTexCoord2f32(int u, int v)
{
  ::glTexCoord2f32(u, v);
}

int nds::VideoGL::POLY_ALPHA(int i)
{
  return ::POLY_ALPHA(i);
}

void nds::VideoGL::glEnable(int i)
{
  ::glEnable(i);
}

static int pixelToDSWidth(int size)
{
  switch (size)
  {
    case 8:
      return TEXTURE_SIZE_8;
    case 16:
      return TEXTURE_SIZE_16;
    case 32:
      return TEXTURE_SIZE_32;
    case 64:
      return TEXTURE_SIZE_64;
    case 128:
      return TEXTURE_SIZE_128;
    case 256:
      return TEXTURE_SIZE_256;
    case 512:
      return TEXTURE_SIZE_512;
    case 1024:
      return TEXTURE_SIZE_1024;
  }
  return -1;
}

void nds::VideoGL::glTexImage2D(int target, GL_TEXTURE_TYPE_ENUM type, int w, int h, int param, const void * data)
{
  int ds_w=pixelToDSWidth(w), ds_h=pixelToDSWidth(h);
  if (ds_w == -1 or ds_h == -1)
  {
    return;
  }

  ::glTexImage2D(target, 0, (::GL_TEXTURE_TYPE_ENUM)type, ds_w, ds_h, 0, param, (const uint8*)data);
}
void nds::VideoGL::glColor3b(int r, int g, int b)
{
  ::glColor3b(r, g, b);
}

void nds::VideoGL::glClearDepth(int depth)
{
  ::glClearDepth( depth );
}

void nds::VideoGL::glClearColor(int red, int green, int blue, int alpha)
{
  ::glClearColor(red, green, blue, alpha);
}

