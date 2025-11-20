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
#ifndef Video_h_seen
#define Video_h_seen
#include <stdint.h>
#include "util/classhelper.h"

namespace nds
{
  /*! @brief Controls the video hardware */
  class Video
  {
    public:

      /*! @brief use this instead of a constructor. 
       * @param screen Screen 0 is the main (top) screen, screen 1 is the bottom (sub) screen.
       * @returns the instance of the video system for screen 0 or 1
       */
      static Video& instance(int screen=0);

      /*! @brief clears video ram. */
      void clear();

      //! Defines the backgrounds to blend between
      typedef enum {
        BLDMOD_BG0=1,  //!< Layer 0
        BLDMOD_BG1=2,  //!< Layer 1
        BLDMOD_BG2=4,  //!< Layer 2
        BLDMOD_BG3=8,  //!< Layer 3
        BLDMOD_OBJ=16, //!< Layer 4
        BLDMOD_BD =32  //!< Backdrop layer
      } BLDMOD_BG_t;

      //! Defines the type of blending between layers
      typedef enum {
        BLDMOD_OBJECT=0, //!< Blend objects
        BLDMOD_ALPHA=1,  //!< Alpha blend layers
        BLDMOD_WHITE=2,  //!< White out
        BLDMOD_FADE=3    //!< Fade to black
      } BLDMOD_MODE_t;

      /*! @brief Perform hardware blending between layers.
       * @param first or top list of layers. e.g. BLDMOD_BG0|BLDMOD_BG1
       * @param second or bottom list of layers. e.g. BLDMOD_BD
       * @param mode the type of blending to perform
       */
      void blend(BLDMOD_MODE_t mode=BLDMOD_ALPHA,
          int first=BLDMOD_BG1 | BLDMOD_BG0 | BLDMOD_BG2 | BLDMOD_OBJ,
          int second=BLDMOD_BD);

      /*!@brief set the fade level (BLDMOD_FADE)
       * @param level the level of fading - from 0-16. 0 being bright, 16 being fully black.
       */
      void setFade(int level);

      /*!@brief set the white level (BLDMOD_WHITE)
       * @param level the level of white - from 0-16. 0 being normal, 16 being fully white.
       */
      void setWhite(int level);

      /** Set the blend mode for alpha or object mode.
       * @param A the A set of backgrounds
       * @param B the B set of backgrounds
       */
      void setBlendAB(int A, int B);

      /*!@brief fade from normal to black.
       * @param fadeout true to fade to black, false to fade back in
       * @param speed the speed of the fade out, higher value is slower
       */
      void fade(bool fadeout=true, unsigned int speed=1);
      /*!@brief fade from normal to white.
       * @param towhite true to brighten to white, false to fade back to normal
       * @param speed the speed of the whiteout, higher value is slower
       */
      void whiteout(bool towhite=true, unsigned int speed=1);

      /*! @brief screen capture
       */
      void capture() const;

      //! Switch the screens. By default the main screen is on top and the sub screen is on teh touch screen.
      static void swapScreens();

      //! Make this screen go to the top.
      void setToTop();
      //! Make this screen go to the bottom.
      void setToBottom();

      /** Is this screen on top?.
       * @return true if this is on top, false otherwise.
       */
      bool onTop() const;

      /*! @brief Set 3D mode
       * @param td if true sets 3D mode, if false does not
       */
      void setThreeD(bool td);
      /*! @brief Get 3D mode
       * @returns the current state of 3D. True if 3D is set false otherwise.
       */
      bool threeD() const;

      /*! @brief Set the video mode.
       * @param mode Mode value 0-6. Mode 6 is new on the DS.
       */
      void setMode(unsigned int mode);
      /*! @brief get the video mode.
       * @returns Mode value 0-6
       */
      unsigned int mode() const;

      /*! @brief Enable or disable objects (sprites).
       * @param enable true to enable objects, false to disable them
       */
      void setObjectsEnabled(bool enable=true);

      /*! @brief Get the status of the objects (sprites)
       * @returns true if objects enabled, false otherwise
       */
      bool objectsEnabled() const;

      /*! @brief Set the dimensions of the object map. May be 1D or 2D.
       * @param dimensions 1 or 2 for 1D or 2D.
       */
      void setObjectMapDimensions(int dimensions);

      /*! @brief get the dimensions of the object map. May be 1D or 2D.
       * @returns dimensions 1 or 2 for 1D or 2D.
       */
      int objectMapDimensions() const;
    private:
      //! the background register to use
      volatile uint32_t & m_DISPCNT;
      //! The screen used.
      int m_screen;

      /*! @brief Constructs a Video display for the main or sub screen.
       * @param screen 0 for the main screen, 1 for the sub screen.
       */
      Video(int screen);

      //! Set the default VRAM banks. Uses bank A for the main screen, bank C for the sub screen.
      void setBanks();

      DISALLOW_COPY_AND_ASSIGN(Video);
  };

}
#endif
