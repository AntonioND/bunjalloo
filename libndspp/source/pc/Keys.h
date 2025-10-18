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
#ifndef Keys_h_seen
#define Keys_h_seen

struct SDL_KeyboardEvent;
struct SDL_MouseButtonEvent;

#include <SDL/SDL_keyboard.h>

class Keys
{
  public:
    static Keys & instance();
    void handleKeyEvent(SDL_KeyboardEvent & event);
    void handleMouseEvent(SDL_MouseButtonEvent & event);
    void handleMouseMotion(int x, int y);
    void scan();
    int  down() const;
    int  up() const;
    int held();
    int repeat();
    void setRepeat(int setDelay, int setRepeat);
    int touchX() const;
    int touchY() const;
    int keysRealKeyboard() const;

  private:
    static const int KEY_COUNT = 13;
    static const SDLKey s_keyCodes[Keys::KEY_COUNT];

    unsigned int m_held;
    unsigned int m_heldLast;
    unsigned int m_heldRepeat;

    unsigned int m_repeat;
    unsigned int m_delay;
    unsigned int m_count;

    bool m_keys[KEY_COUNT];
    int m_touchX;
    int m_touchY;
    int m_keypress;

    unsigned int keyRegister();

    Keys();

};
#endif
