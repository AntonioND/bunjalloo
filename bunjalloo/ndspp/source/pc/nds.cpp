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
#include "EnvHelpers.h"

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

void cothread_yield_irq(uint32_t flag)
{
  SDLhandler::instance().waitVsync();
}

void libndsCrash(const char *message)
{
  fprintf(stderr, "%s\n", message);
  exit(1);
}

// ---------------------

static uint16_t *slot2MemData;
static size_t slot2MemSize;

bool peripheralSlot2InitDefault(void)
{
  slot2MemSize = getenv_int("BUNJALLOO_SLOT2_RAM_SIZE",
                            8 * 1024 * 1024, // Default to 8 MB
                            0,
                            16 * 1024 * 1024); // Max is 16 MB
  if (slot2MemSize == 0)
    return false;

  slot2MemData = (uint16_t *)malloc(slot2MemSize);
  if (slot2MemData == NULL)
  {
    slot2MemSize = 0;
    return false;
  }

  //fprintf(stderr, "Slot 2 RAM: %zu bytes\n", slot2MemSize);

  return true;
}

uint16_t *peripheralSlot2RamStart(void)
{
  return slot2MemData;
}

uint32_t peripheralSlot2RamSize(void)
{
  return slot2MemSize;
}

// ---------------------

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
