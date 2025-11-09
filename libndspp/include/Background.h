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
#ifndef Background_h_seen
#define Background_h_seen

#include <stdint.h>

namespace nds {

  //! Tile flipping.
  enum MAP_DATA_t {
    TILE_FLIP_HORZ=0x400, //!< Horizontal tile flip
    TILE_FLIP_VERT=0x800  //!< Vertical tile flip
  };
  /*! @brief Background wraps the HW backgrounds.
   *
   *  Up to 4 background layers are available depending on the mode. 
   *  To set the mode see the Video class. @see Video,Video::mode(unsigned int).
   */
  class Background {
    public:
      /*! 
       * Creates a background object.
       * @param screen number of the screen, 0 for the main screen, 1 for the sub screen
       * @param number the background layer
       * @param charBlock character block (0-4)
       * @param screenBlock screen block (0-31)
       * @param priority layer priority. Allowed values 0-3. 0 on top, 3 underneath.
       */
      Background(
          int screen=0,
          int number=0,
          int charBlock=0,
          int screenBlock=0,
          int priority=0);

      /*! Destroy the Background object. 
       * The corresponding layer is switched off on the hardware too.
       */
      ~Background();

      //! update the hardware with the current scroll and enable values.
      void update();

      /** Get the screen that the background is rendered on.
       * @return the screen number
       */
      inline int screen() const
      {
        return m_bg.screen;
      }

      //! Map size
      typedef enum {
        SIZE_0, //!< Map size 256x256, 32x32 tiles, 16x16 in rot mode
        SIZE_1, //!< Map size 512x256, 64x32 tiles, 32x32 in rot mode
        SIZE_2, //!< Map size 256x512, 32x64 tiles, 64x64 in rot mode
        SIZE_3  //!< Map size 512x512, 64x64 tiles, 128x128 in rot mode
      } SIZE;

      /*!@brief enable the background layer on the hardware
       * @param enabled true to enable false to disable
       */
      void setEnabled(bool enabled=true);

      /*!@brief Fetch the enabled status of this layer
       * @returns true if the background layer is enabled, false if disabled
       */
      bool enabled() const;

      /*! @brief Get the special feature flags.
      * @returns a subset of flags. Priority, mosaic, colors, wrapping
      */
      unsigned short flags() const;
      /*! @brief Set a subset of special feature flags. Priority, mosaic,
       * colors, wrapping. Requires knowledge of bits, so is for experts only.
       * @param f flags to set.
       */
      void setFlags(unsigned short f);

      /*! @brief Get the number of colors used. 
       * @returns Number of colors used, either 16 or 256.
       */
      int color() const;
      /*! @brief Set number of colors used. Only 16 and 256 are valid.
       * @param c number of colors. 256 or 16.
       */
      void setColor(int c);

      /*! @brief Get the layer. 
       * @returns the layer number
       */
      unsigned char number() const;
      //!set the layer number
      void setNumber(unsigned char );
      //! @returns character base block
      unsigned char charBaseBlock() const;
      //! set character base block
      void setCharBaseBlock(unsigned char );
      //! @returns screen base block
      unsigned char screenBaseBlock() const;
      //! set screen base block
      void setScreenBaseBlock(unsigned char );
      //! @returns true if the wraparound flag is set.
      bool wraparound() const;
      //! set or unset the wraparound flag.
      void setWraparound(bool );
      //! @returns true if the mosaic flag is set.
      bool mosaic() const;
      //! set or unset the mosaic flag.
      void setMosaic(bool );

      //! @returns the map size
      SIZE size() const;
      //! set the map size
      void setSize(SIZE );
      //! @returns the horizontal scroll value
      signed short xScroll() const;
      //! set the horizontal scroll
      void setXScroll(signed short );
      //! @returns the vertical scroll value
      signed short yScroll() const;
      //! set m_bg.yScroll
      void setYScroll(signed short );
      //! @returns x displacement (rotation backgrounds)
      signed int DX() const;
      //! set x displacement (rotation backgrounds)
      void setDX(signed int );
      //! @returns y displacement (rotation backgrounds)
      signed int DY() const;
      //! set y displacement (rotation backgrounds)
      void setDY(signed int );
      //! @returns affine matrix PA (rotation backgrounds)
      signed short PA() const;
      //! set affine matrix PA (rotation backgrounds)
      void setPA(signed short );
      //! @returns affine matrix PB (rotation backgrounds)
      signed short PB() const;
      //! set affine matrix PB (rotation backgrounds)
      void setPB(signed short );
      //! @returns affine matrix PC (rotation backgrounds)
      signed short PC() const;
      //! set affine matrix PC (rotation backgrounds)
      void setPC(signed short );
      //! @returns affine matrix PD (rotation backgrounds)
      signed short PD() const;
      //! set affine matrix PD (rotation backgrounds)
      void setPD(signed short );

      /*! @brief Fetches a pointer to the map data
       * @returns pointer to the 16 bit map data in VRAM.
       */
      unsigned short * mapData() const;

      /*! @brief Fetches a pointer to the tile data
       * @returns pointer to the 16 bit tile data in VRAM.
       */
      unsigned short * tileData() const;

#if !defined(ARM9) && !defined(ARM7)
      void render();
      bool operator<(const Background & other) const;
      bool operator>(const Background & other) const;
#endif
    private:
      //! configuration of the background layer
      typedef struct {
        unsigned short* tileData;        //!< pointer to tile data in vram
        unsigned short * mapData;        //!< pointer to map data in vram
        unsigned short flags;            //!< mosaic, priority, colors, wrapping flags
        unsigned char number;            //!< background number (0-3)
        unsigned char charBaseBlock;     //!< location of character data
        unsigned char screenBaseBlock;   //!< location of screen block data
        unsigned char screen;            //!< sub screen flag
        signed short xScroll;            //!< x scroll for text backgrounds
        signed short yScroll;            //!< y scroll for text backgrounds
        signed long DX,                  //!< x offset for rotation backgrounds
            DY;                          //!< y offset for rotation backgrounds
        signed short PA,                 //!< affine value PA
            PB,                          //!< affine value PB
            PC,                          //!< affine value PC
            PD;                          //!< affine value PD
      } background_t;

      //! the background register to use
      volatile uint32_t & m_DISPCNT;
      //! the current hardware state for this background
      background_t m_bg;

      void setRotateFlags(volatile unsigned short * BG_REG);
      void updateTileMapData(void);
      void draw8x8Tile(int x, int y, unsigned char * gfx, int flags);
  };

};

#endif
