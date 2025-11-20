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
#ifndef Sprite_h_seen
#define Sprite_h_seen
#include "util/classhelper.h"

namespace nds {

  /*! @brief Object to represent hardware sprite.
   * @see Video,Video::enableObjects
   * */
  class Sprite
  {
    public:
      /*! @brief Create a sprite object with detailed parameters. Sprite is
       * initially disabled.
       * @param screen 0 for the main screen, 1 for the sub screen.
       * @param w width in pixels
       * @param h height int pixels
       * @param tile Index of the tile in sprite memory to use
       * @param colors number of colo(u)rs to use. may be 16 or 256.
       */
      Sprite(int screen, int w, int h, int tile=0, int colors=16);

      /*! @brief Creates a sprite object on the given screen. Sprite is
       * initially disabled.
       * @param screen 0 for the main screen, 1 for the sub screen.
       */
      Sprite(int screen);


      //! Destroy the sprite and hide it
      ~Sprite();

      /*! @brief Disable all sprites for a given screen
       * @param screen the screen to disable the sprites on. 0 is main screen, 1 is sub screen, 2 is both.
       */
      static void disableAll(int screen);

      /*!@brief Move the sprite to the given X, Y position
       * @param x the x position in pixels
       * @param y the y position in pixels
       */
      inline void setXY(unsigned int x, unsigned int y)
      {
        this->setX(x); this->setY(y);
      }

      /*!@brief get the current X, Y position
       * @param x the x position in pixels
       * @param y the y position in pixels
       * @returns x, y position
       */
      inline void getXY(unsigned int & x, unsigned int & y)
      {
        x = this->x();
        y = this->y();
      }

      /*! shape is SQUARE TALL WIDE.
       * @see shape(),shape(SPRITE_SHAPE)
       */
      typedef enum {
        NOT_VALID=-1, //!< Value returned if the shape is not valid
        SQUARE=0,     //!< Height and width are the same
        WIDE=1,       //!< Height is less than width
        TALL=2        //!< Width is less than height
      } SPRITE_SHAPE;

      /*! retrieve the size.
       * @param w width one of 8,16,32,64
       * @param h height one of 8,16,32,64
       * @see setSize,height,width
       */
      void getSize(unsigned int &w, unsigned int &h) const;
      /*! Set the size.
       * @param w width one of 8,16,32,64
       * @param h height one of 8,16,32,64
       * @see getSize,height,width
       */
      void setSize(unsigned int w, unsigned int h);

      /*! @brief Fetch the underlying attributes.
       * @param i Index of the attribute. Values allowed 0-2.
       * @returns Attribute value.
       */
      unsigned short attribute(int i) const;

      /*! basic affine support.
       *  returns the values of the affine matrix.
       *  @param pa affine entry PA
       *  @param pb affine entry PB
       *  @param pc affine entry PC
       *  @param pd affine entry PD
       *  @see setAffine
       */
      void getAffine( 
          signed short & pa,
          signed short & pb,
          signed short & pc,
          signed short & pd) const;
      /*! basic affine support.
       *  set the values of the affine matrix.
       *  @param pa affine entry PA
       *  @param pb affine entry PB
       *  @param pc affine entry PC
       *  @param pd affine entry PD
       *  @see getAffine
       */
      void setAffine( 
          signed short pa,
          signed short pb,
          signed short pc,
          signed short pd);

      // misc functions
      //! synchronise this sprite with hardware video memory
      void update() const;

      /*! @brief copy all sprites to hardware video memory.
       * @param screen the screen to copy. 0 = main, 1 = sub, 2 = both main and sub
       */
      static void copyOAM(int screen=2);
      /*! returns true if valid. 
       * Only 128 Sprites are available in hardware. Any more are invalid.
       */
      inline bool valid() const
      {
        return m_index >= 0;
      }

      /*! Reinitialise the sprite (reset index). Does nothing if already valid.
       * @param sync if set to true will copy state to hardware
       */
      void initialise(bool sync=false);

      //! get the current y position.
      int y() const;
      //! set the current y position.
      void setY(int);

      //! returns true if the rotation-scale flag is set.
      bool rotateScale() const;
      //! set or unset the rotation-scale flag.
      void setRotateScale(bool);
      //! returns true if the double size flag is set.
      bool doubleSize() const;
      //! set or unset the double size flag set.
      void setDoubleSize(bool);

      /*! @brief Enable or disable a sprite using the doubleSize trick.
       * @param e enable flag. true to enable the sprite, false to disable it
       */
      inline void setEnabled(bool e=true) {
        setDoubleSize(!e);
        setRotateScale(false);
      }

      /** Get the enabled status based on internal flags.
       * @return true if enabled.
       */
      inline bool enabled() const
      {
        if ( doubleSize() and not rotateScale())
          return false;
        return true;
      }

      //! returns the OR of the mode flags. 1 for translucent, 2 for obj-window.
      unsigned int mode() const;
      /*! @brief Sets the special mode. Can be translucent, object-window or bitmap.
       *  @param m Can have the value 0-3. 0 removes the current mode.
       *  1 is translucent.
       *  2 is object-window.
       *  3 is bitmap.
       */
      void setMode(unsigned int m);
      //! returns true if the mosaic flag is set.
      bool mosaic() const;
      //! set or unset the mosaic flag.
      void setMosaic(bool);
      //! returns the number of colors used. Either 16 or 256.
      unsigned int colors() const;
      //! set the number of colors used. Only 16 and 256 are valid.
      void setColors(unsigned int);

      // attr1
      //! returns the current x position.
      unsigned int x() const;
      //! set the current x position.
      void setX(unsigned int);

      //! @brief fetch the current shape.
      //! @returns the current shape.
      SPRITE_SHAPE shape() const;
      //! set the current shape.
      //! @param s the shape to use
      void setShape(SPRITE_SHAPE s);
      //! returns the rotation matrix index (0-15). @see getAffine,setAffine
      unsigned int rotate() const;
      //! set the rotation matrix to use (0-15). @see getAffine,setAffine
      void setRotate(unsigned int);
      //! returns the state of the horizontal flip flag.
      bool hflip() const;
      //! set or unset the horizontal flip flag.
      void setHflip(bool);
      //! returns the state of the vertical flip flag.
      bool vflip() const;
      //! set or unset the vertical flip flag.
      void setVflip(bool);

      // SPRITE_SIZECODE size; OBJ_SIZE Sprite_32x8 etc
      //! returns the actual width in pixels. @see getSize,height
      unsigned int width() const;
      /*! set the width.
       * @param w must be one of 8,16,32,64.
       * @see setSize,height
       */
      void setWidth(unsigned int w);
      //! returns the actual height in pixels.  @see getSize,width
      unsigned int height() const;
      /*! set the height.
       * @param h must be one of 8,16,32,64
       * @see setSize,width
       */
      void setHeight(unsigned int h);
      // attr2
      //! returns the tile entry
      unsigned int tile() const;
      //! set the tile entry
      void setTile(unsigned int);
      /*! @brief Fetch the address of the display data.
       * @returns pointer to the video data.
       */
      unsigned short * oamData() const;
      //! returns the priority (0 is closer to the viewer, 3 is underneath)
      unsigned int priority() const;
      //! set the priority (0 is closer to the viewer, 3 is underneath)
      void setPriority(unsigned int);
      //! returns the palette (0-15). 16 color Sprite only. @see color
      unsigned int palette() const;
      //! set the palette (0-15), 16 color Sprite only. @see color
      void setPalette(unsigned int);
      //! returns true if the translucent flag is set. @see mode
      bool translucent() const;
      //! set or unset the translucent flag. @see mode
      void setTranslucent(bool);
      //! returns true if the object window flag is set. @see mode
      bool window() const;
      //! set or unset the object window flag. @see mode
      void setWindow(bool);

      /*! @brief Load tile data to the currently configured tile.
       *
       * Data should be in raw format as given by gfx2gba.
       *
       * @param tile pointer to the tile data to load.
       * @param length amount of data to load (in bytes).
       * @see loadTileData(const char*)
       */
      void loadTileData(void const * tile, unsigned int length);

      /*! @brief Load tile data to the currently configured tile using \a map file to "expand" the data.
       *
       * Data should be in raw format as given by gfx2gba. \a tile is assumed to have enough data
       * to satisfy the map data.
       *
       * @param tile pointer to the tile data to load.
       * @param map map data used to expand the tiles given by \a tile
       * @param length amount of map data to load (in bytes).
       * @see loadTileMapData(const char*, const char*)
       */
      void loadTileMapData(const void * tile, const void * map, unsigned int length);

#if !defined(ARM9) && !defined(ARM7)
      void render();
      bool operator<(const Sprite & other) const;
      bool operator>(const Sprite & other) const;
#endif

    private:
      //! screen that the sprite is on
      unsigned short int m_screen;
      //! sprite index on hardware.
      signed short int m_index;
      //! pointer to the hardware sprites used for the screen that the sprite belongs to.
      bool * m_used;
      //! which hardware sprites are in use for each screen.
      static bool s_used[2][128];

      //! OAM memory for the screen that this sprite belongs to.
      unsigned short int * m_OAM;

      /*! @brief helper function to set bit mask.
       * @param attr attribute to set/clear
       * @param bits bit mask to set
       * @param set true to set, false to clear
       * @param clear mask to clear - if 0 then uses ~@a bits otherwise uses ~@a clear
       */
      void setAttrBit(unsigned int attr, unsigned int bits,bool set,unsigned int clear=0);

      void draw8x8Tile(int xPos, int yPos, int xi, int yi, const unsigned char * gfx);

      DISALLOW_COPY_AND_ASSIGN(Sprite);
  };
};
#endif

