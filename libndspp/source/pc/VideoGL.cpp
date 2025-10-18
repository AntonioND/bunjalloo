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
#include <cstdio>
#include "VideoGL.h"
#include "Video.h"
#include <GL/gl.h>
#include <malloc.h>
#include "SDLhandler.h"
#include <SDL/SDL_video.h>

void nds::VideoGL::glInit()
{
  glColor3f(0.0f, 0.0f, 0.0f);
  ::glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
  ::glClearDepth(1.0);
  ::glEnable(GL_DEPTH_TEST);
}

void nds::VideoGL::glTexImage2D(int target, GL_TEXTURE_TYPE_ENUM type, int w, int h, int param, const void * data)
{
  // allocate pixels for data..
  // size is "size in half nibbles" (!) i.e. size*w*h / 4 gives size-in-bytes
  int pctype = GL_RGBA;
  switch ((int)type)
  {
    case 1:
      // 1 byte
      pctype = GL_RGB4;
      break;
    case 2:
      // half a nibble (2 bits)
      pctype = GL_RGB4;
      break;
    case 3:
      // a nibble (4 bits)
      pctype = GL_RGB16;
      break;
    case 4:
      // 1 byte, GL_RGB256
      break;
    case 5:
      pctype = GL_COMPRESSED_RGB;
      break;

    case 6: // 8 col, 5 alpha  -> 8 bit
      // ??
      break;

    case 7:
      // GL_RGBA -> 16 bit
      break;

    case 8:
      // GL_RGBA -> 16 bit
      break;
  }
  //unsigned char * pixels = (unsigned char*)malloc( (4*w*h));
  SDL_Surface * surface = SDL_CreateRGBSurface( SDL_SWSURFACE, w, h, 32,
#if SDL_BYTEORDER == SDL_LIL_ENDIAN // OpenGL RGBA masks
      0x000000FF,
      0x0000FF00,
      0x00FF0000,
      0xFF000000
#else
      0xFF000000,
      0x00FF0000,
      0x0000FF00,
      0x000000FF
#endif
      );
  if (pctype == GL_RGBA)
  {
    // sigh..
    const unsigned short * src((const unsigned short*)data);
    // unsigned char * dst(pixels);

    SDL_Rect rect = {0,0,1,1};
    for (int i = 0; i < w*h; ++i)
    {
      unsigned short rgb16 = *src++;
      int red = (rgb16&0x1f)*8;
      int blue = ((rgb16>>5)&0x1f)*8;
      int green = ((rgb16>>10)&0x1f)*8;
      int rgba = SDL_MapRGBA( surface->format, red, green, blue, 0 );
      SDL_FillRect (surface, &rect,  rgba);
      rect.x++;
      if (rect.x == surface->w)
      {
        rect.x = 0;
        rect.y++;
      }
    }
    ::glTexImage2D( GL_TEXTURE_2D,
        0,
        GL_RGBA,
        surface->w, surface->h,
        0,
        GL_RGBA,
        GL_UNSIGNED_BYTE,
        surface->pixels ) ;
  }
  SDL_FreeSurface(surface);
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR ) ;
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR ) ;
#if 1
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT ) ;
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT ) ;
#endif
//   free(pixels);
}

void nds::VideoGL::glPopMatrix(int i)
{
  ::glPopMatrix();
}

void nds::VideoGL::glFlush(int i)
{
  ::glFlush();
  SDL_GL_SwapBuffers();
  SDLhandler::instance().setDirty();
}

void nds::VideoGL::glViewport(int x, int y, int w, int h)
{
  if (nds::Video::instance(0).onTop())
  {
    // like graph axis, 0,0 is bottom left
    // increase 192 so it starts half way up the screen
    y += 192;
  }
  ::glViewport(x, y, w, h);
}

void nds::VideoGL::glMatrixMode(GL_MATRIX_MODE_ENUM mode)
{
  int m = GL_MODELVIEW;
  switch (mode)
  {
    case 0: m = GL_PROJECTION; break;
    case 1: m = GL_POSITION; break;
    case 2: m = GL_MODELVIEW; break;
    case 3: m = GL_TEXTURE; break;
    default: break;
  };
  ::glMatrixMode(m);
}

void nds::VideoGL::glBegin(GL_GLBEGIN_ENUM b)
{
  int m = GL_TRIANGLES;
  switch (b)
  {
    case 0: m = GL_TRIANGLES; break;
    case 1: m = GL_QUADS; break;
    case 2: m = GL_TRIANGLE_STRIP; break;
    case 3: m = GL_QUAD_STRIP; break;
  }
  //printf("glBegin %d\n", m);
  ::glBegin(m);
}

void nds::VideoGL::glClearPolyID(int id)
{
  // ???
}

#include <GL/glu.h>
#define f32tofloat(n)        (((float)(n)) / (float)(1<<12)) /*!< \brief convert f32 to float */
void nds::VideoGL::gluPerspectivef32(int fovy, int aspect, int zNear, int zFar)
{
  ::gluPerspective( (fovy*360.) / 512., f32tofloat(aspect), f32tofloat(zNear), f32tofloat(zFar));
}

void nds::VideoGL::gluLookAtf32(int eyex, int eyey, int eyez, int lookAtx, int lookAty, int lookAtz, int upx, int upy, int upz)
{
  ::gluLookAt(f32tofloat(eyex),
      f32tofloat(eyey),
      f32tofloat(eyey),
      f32tofloat(lookAtx),
      f32tofloat(lookAty),
      f32tofloat(lookAtz),
      f32tofloat(upx),
      f32tofloat(upy),
      f32tofloat(upz));
}

void nds::VideoGL::glPolyFmt(int params)
{
}

void nds::VideoGL::glTranslate3f32(int x, int y, int z)
{
  ::glTranslatef(f32tofloat(x), f32tofloat(y), f32tofloat(z));
}

void nds::VideoGL::glRotatef32i(int angle, int x, int y, int z)
{
  ::glRotatef(angle*360.0 / 512., f32tofloat(x), f32tofloat(y), f32tofloat(z));
}

void nds::VideoGL::glRotateXi(int angle)
{
  ::glRotatef(angle*360.0 / 512., 1.0, 0., 0.);
}
void nds::VideoGL::glRotateYi(int angle)
{
  ::glRotatef(angle*360.0 / 512., 0., 1., 0.);
}
void nds::VideoGL::glRotateZi(int angle)
{
  ::glRotatef(angle*360.0 / 512., 0., 0., 1.);
}

void nds::VideoGL::glVertex3v16(short x, short y, short z)
{
  ::glVertex3f(f32tofloat(x), f32tofloat(y), f32tofloat(z));
}
void nds::VideoGL::glTexCoord2f32(int u, int v)
{
  ::glTexCoord2f(f32tofloat(u), f32tofloat(v));
}

int nds::VideoGL::POLY_ALPHA(int i)
{
  return 0;
}

void nds::VideoGL::glEnable(int i)
{
  int bits = 0;
  if (i & 1)
      bits |= GL_TEXTURE_2D;
  if (i & 4)
      bits |= GL_ALPHA_TEST;
  if (i & 8)
      bits |= GL_BLEND;
  if (i & 128)
      bits |= GL_FOG;
  ::glEnable(bits);
}

void nds::VideoGL::glColor3b(int r, int g, int b)
{
  ::glColor3f(((float)r)/255.0f, ((float)g)/255.0f, ((float)b)/255.0f);
}

void nds::VideoGL::glClearDepth(int depth)
{
  ::glClearDepth( (double)depth / 32767.);
}

void nds::VideoGL::glClearColor(int red, int green, int blue, int alpha)
{
  ::glClearColor(red/255., green/255., blue/255., alpha/255.);
}

