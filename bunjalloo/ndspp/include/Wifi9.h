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
#ifndef Wifi9_h_seen
#define Wifi9_h_seen

#ifdef ARM7
#error Wifi9 is only available on the ARM9
#endif
#include "util/classhelper.h"

namespace nds
{
  /*! @brief Wifi9 is a helper class for the arm9 dswifi code.
   *
   *  This class initialises the DS wifi hardware and handles connections and disconnections.
   */
  class Wifi9
  {
    public:

      /*! @brief use this instead of a constructor to create an instance of the class.
       * @returns an initialised, but not connected, wifi class for the arm9 processor.
       */
      static Wifi9 & instance();

      /** Connect to the AP using the built in WFC settings. */
      void connect();

      //! Status from dswifi, saves including dswifi header.
      enum WifiStatus
      {
        DISCONNECTED,   //!< Not trying to connect
        SEARCHING,      //!< Looking for AP
        AUTHENTICATING, //!< Connecting.
        ASSOCIATING,    //!< Connecting.
        ACQUIRINGDHCP,  //!< Connected to AP, getting IP
        ASSOCIATED,     //!< Connected
        CANNOTCONNECT,  //!< Error, unable to connect
      };

      /** Get the wifi connection status.
       * @returns the current Wifi status
       */
      WifiStatus status() const;

      /*! @brief fetch the status of the connection.
       * @returns true if connected, false otherwise.
       */
      bool connected() const;

      /*! @brief disconnect from the AP.
       * This disconnects and also tells the arm7 code to do the same.
       */
      void disconnect();

      /** Get the signal strength.
       * @return the signal strength.
       */
      int signalStrength() const;

    private:
      bool m_connected;
      //! construct the class, calls initialise
      Wifi9();
      //! initialise the DS wifi hardware but do not connect.
      void initialise();

      DISALLOW_COPY_AND_ASSIGN(Wifi9);
  };
};
#endif
