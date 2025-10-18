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
#include "libnds.h"
#include "SDL/SDL.h"
#include <string.h>
#include <iostream>
#include "SDLhandler.h"

short cosLerp(short angle) {
  return 0;
}
short sinLerp(short angle) {
  return 0;
}

void dmaCopy(const void * source, void * destination, unsigned int length)
{
  ::memcpy(destination, source, length);
}
using namespace std;

void powerOn()
{
}
void irqInit()
{
}
void irqSet(int, VoidFunctionPointer fn)
{
  SDLhandler::instance().enableVblank(fn);
}

void swiWaitForVBlank() 
{
  SDLhandler::instance().waitVsync();
}

#if 0
void glReset(void)
{
  SDL_GL_SwapBuffers( );
  glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity( );
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity( );

}
#endif
