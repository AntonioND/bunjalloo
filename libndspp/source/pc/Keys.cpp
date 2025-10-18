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
#include "Keys.h"
#include "SDL/SDL.h"

const SDLKey Keys::s_keyCodes[Keys::KEY_COUNT] =
{
  SDLK_z,      // KEY_A 	  
  SDLK_x,      // KEY_B 	  
  SDLK_SPACE,  // KEY_SELECT
  SDLK_RETURN, // KEY_START 
  SDLK_RIGHT,  // KEY_RIGHT 
  SDLK_LEFT,   // KEY_LEFT  
  SDLK_UP,     // KEY_UP 	  
  SDLK_DOWN,   // KEY_DOWN  
  SDLK_w,      // KEY_R	  
  SDLK_q,      // KEY_L 	  
  SDLK_a,      // KEY_X
  SDLK_s,      // KEY_Y
  SDLK_UNKNOWN, // mouse btn
};

Keys::Keys():
    m_repeat(30),
    m_delay(60),
    m_count(60),
    m_touchX(0),
    m_touchY(0)
{}

Keys & Keys::instance()
{
  static Keys s_instance;
  return s_instance;
}
int Keys::touchX() const
{
  return m_touchX;
}
int Keys::touchY() const
{
  return m_touchY;
}

void Keys::handleMouseEvent(SDL_MouseButtonEvent & event)
{
  if (event.type == SDL_MOUSEBUTTONDOWN)
  {
    m_keys[12] = true;
    m_touchX = event.x;
    m_touchY = event.y;
  }
  else if (event.type == SDL_MOUSEBUTTONUP)
  {
    m_keys[12] = false;
    m_touchX = 512;
    m_touchY = 512;
  }
}

void Keys::handleMouseMotion(int x, int y)
{
  m_touchX = x;
  m_touchY = y;
}

void Keys::handleKeyEvent(SDL_KeyboardEvent & event)
{
  if (event.type == SDL_KEYDOWN)
  {
    // set in key array  
    for (int i = 0; i < KEY_COUNT; i++) {
      if (s_keyCodes[i] == event.keysym.sym) {
        m_keys[i] = true;
      }
    }
    m_keypress = event.keysym.sym;
  }
  else if (event.type == SDL_KEYUP)
  {
    // unset in key array
    for (int i = 0; i < KEY_COUNT; i++) {
      if (s_keyCodes[i] == event.keysym.sym) {
        m_keys[i] = false;
      }
    }
    m_keypress = 0;
  }
}

unsigned int Keys::keyRegister()
{
  unsigned int keyReg = 0;
  for (int i = 0; i < KEY_COUNT; ++i)
  {
    if (m_keys[i]) {
      keyReg |= 1<<i;
    }
  }
  return keyReg;
}

void Keys::scan()
{
  m_heldLast = m_held;
  m_held = keyRegister();

  if ( m_delay != 0 )
  {
    if (m_held != m_heldLast)
    {
      m_count = m_delay;
      m_heldRepeat = down();
    }

    m_count--;

    if (m_count == 0)
    {
      m_count = m_repeat;
      m_heldRepeat = m_held;
    }
  }
}

int  Keys::down() const
{
  return (m_held ^ m_heldLast) & m_held;
}

int  Keys::up() const
{
  return (m_held ^ m_heldLast) & (~m_held);
}

int Keys::held()
{
  return m_held;
}

int Keys::repeat()
{
  int tmp = m_heldRepeat;
  m_heldRepeat = 0;
  return tmp;
}

void Keys::setRepeat(int setDelay, int setRepeat)
{
  m_delay  = setDelay;
  m_repeat = setRepeat;
  m_count  = setDelay;
  m_heldRepeat = 0;
}
int Keys::keysRealKeyboard() const
{
  return m_keypress;
}

// static functions that emulate libnds
void scanKeys()
{
  Keys::instance().scan();
}
u32 keysDown()
{
  return Keys::instance().down();
}
u32 keysUp()
{
  return Keys::instance().up();
}
u32 keysHeld()
{
  return Keys::instance().held();
}
u32 keysDownRepeat()
{
  return Keys::instance().repeat();
}
void keysSetRepeat( u8 setDelay, u8 setRepeat )
{
  Keys::instance().setRepeat(setDelay, setRepeat);
}

u32 keysRealKeyboard()
{
  return Keys::instance().keysRealKeyboard();
}

void touchRead(touchPosition *touch)
{
  touch->px = Keys::instance().touchX();
  touch->py = Keys::instance().touchY();
  touch->x = touch->px*100;
  touch->y = touch->py*100;
}
