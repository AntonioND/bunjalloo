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
#ifndef Palette_h_seen
#define Palette_h_seen

namespace nds
{
  //! Representation of color
  class Color
  {
    public:
      /*! @brief Create a color using red, green, blue
       * @param red component, allowed values 0-31.
       * @param green component, allowed values 0-31.
       * @param blue component, allowed values 0-31.
       */
      Color(int red, int green, int blue);

      /*! @brief Create a color using a NDS format short
      * @param c 15 bit color value. Red value is stored in the lowest 5
      *          bits, then green then blue. The bits are like this: RRRRRGGGGGBBBBB.
      */
      Color(unsigned short c);

      /*! @brief Set the red, green and blue components.
       * @param red component
       * @param green component
       * @param blue component
       */
      void set(int red, int green, int blue);

      /*! @brief operator to cast to an unsigned short
      * @returns NDS representation of color.
      */
      inline operator unsigned short () const;
      /*! @brief operator to cast to an unsigned short
      * @returns NDS representation of color.
      */
      inline int toInt() const
      {
        return this->operator unsigned short ();
      }

      //! get the red component of the color
      inline unsigned int red() const;
      //! set the red component of the color
      inline void red(unsigned int);
      //! get the green component of the color
      inline unsigned int green() const;
      //! set the green component of the color
      inline void green(unsigned int);
      //! get the blue component of the color
      inline unsigned int blue() const;
      //! set the blue component of the color
      inline void blue(unsigned int);

    private:
      //! NDS format color
      unsigned short m_c;
  };

  /*! @brief Wraps the Background palette memory 
   */
  class Palette
  {
    public:
      /*! @brief Default palette (256 colors).
       * @param screen the screen for the palette. 0 for main screen, 1 for sub screen
       */
      Palette(int screen);

      /*! @brief 16 color palette.
       * @param screen the screen for the palette. 0 for main screen, 1 for sub screen
       * @param palette index of the 16 color palette (0-15)
       */
      Palette(int screen, int palette);

      /*! @brief get reference to color at an index
       * @param i color index
       * @returns unsigned short representation of the NDS color.
       */
      inline unsigned short & operator[] (int i);
      /*! @brief get the color at an index.
       * @param i color index.
       * @returns Color representation of the NDS color.
       */
      inline const Color operator[] (int i) const;

      /*! @brief Sets all the colors in the palette to black (0,0,0)
       */
      void clear();

      /*! @brief Load palette data.
       * @param palette pointer to palette data
       * @param length number of colors to load. If 0 then will load however
       * many are required to fill this palette.
       */
      void load(const unsigned short * palette, unsigned int length=0);

    protected:
      //! pointer to palette memory
      unsigned short *m_p;

    private:
      //! is 16 color palette or not
      bool m_16;
  };

  //! Object (sprite) palette
  class ObjectPalette: public Palette
  {
    public:
      /*! @brief Default object/sprite palette (256 colors).
       * @param screen the screen for the palette. 0 for main screen, 1 for sub screen.
       */
      ObjectPalette(int screen);

      /*! @brief 16 color object/sprite palette.
       * @param screen the screen for the palette. 0 for main screen, 1 for sub screen.
       * @param palette index of the 16 color palette (0-15).
       */
      ObjectPalette(int screen, int palette);
  };
  #include "Palette-inl.h"
};
#endif
