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
#include <nds.h>
#include <dswifi9.h>
#include "Wifi9.h"
#include "System.h"

using namespace nds;

Wifi9::Wifi9():m_connected(false)
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
  Wifi_InitDefault(WFC_CONNECT | WIFI_ATTEMPT_DSI_MODE);
}

void Wifi9::connect()
{
  m_connected = true;
#if 0
  // TODO: Let the user disconnect to save battery
  // if connected, you can use the berkley sockets interface to connect to the internet
  if (m_connected) {
    WifiStatus s = status();
    switch (s)
    {
      case SEARCHING:
      case AUTHENTICATING:
      case ASSOCIATING:
      case ACQUIRINGDHCP:
      case ASSOCIATED:
        /* In all these cases, do nothing */
        return;

      case DISCONNECTED:
      case CANNOTCONNECT:
        /* Try to reconnect */
        break;
    }
  }
  m_connected = true;
  // simple WFC connect:
  // request connect - uses the stored data to connect
  //Wifi_AutoConnect();
#endif
}

Wifi9::WifiStatus Wifi9::status() const
{
  // check status
  if (m_connected)
  {
    int s = Wifi_AssocStatus();
    if (s == ASSOCSTATUS_ASSOCIATED)
    {
      return ASSOCIATED;
    }
    if (s == ASSOCSTATUS_CANNOTCONNECT)
    {
      return CANNOTCONNECT;
    }
    return ASSOCIATING;
  }
  else
  {
    return DISCONNECTED;
  }
}

void Wifi9::disconnect()
{
#if 0
  // TODO: Let the user disconnect to save battery
  Wifi_DisconnectAP();
  Wifi_DisableWifi();
#endif
  m_connected = false;
}

int Wifi9::signalStrength() const
{
  if (not m_connected)
    return 0;

  Wifi_AccessPoint accessPoint;
  for (int ap = 0; ap < 3; ++ap)
  {
    int result = Wifi_GetAPData(ap, &accessPoint);
    if (result == WIFI_RETURN_OK)
    {
      return accessPoint.rssi;
    }
  }
  return 0;
}
