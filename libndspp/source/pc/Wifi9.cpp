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
#include "Wifi9.h"

using namespace nds;

Wifi9::Wifi9()
{
  initialise();
}
Wifi9 & Wifi9::instance()
{
  static Wifi9 s_wifi9;
  return s_wifi9;
}

bool Wifi9::connected() const
{
  return status() == ASSOCIATED;
}

void Wifi9::initialise()
{ 
  // wifi init complete - wifi lib can now be used!
  m_connected = true;
}

void Wifi9::connect()
{ 
  // nop
  m_connected = true;
} 

Wifi9::WifiStatus Wifi9::status() const
{
  if (m_connected)
  {
    return ASSOCIATED;
  }
  else
  {
    return DISCONNECTED;
  }
}

void Wifi9::disconnect()
{
  m_connected = false;
}

int Wifi9::signalStrength() const
{
  if (not m_connected)
    return 0;

  // ??? 
  return 10;
}
