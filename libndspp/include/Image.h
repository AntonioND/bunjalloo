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
#ifndef Image_h_seen
#define Image_h_seen
#include <stdint.h>
#include <string>
#include "util/classhelper.h"

namespace nds
{
  class Image
  {
    public:
      /** Types of image. */
      enum ImageType
      {
        ImageNONE = 0,

        // Make sure that HtmlParser::MimeType entries match the values here
        ImagePNG  = 1,
        ImageGIF  = 2,
        ImageJPEG = 3,
        ImageAPNG = 4,
        ImageBMP  = 5,
      };

      /** Create an image from the given file name.
       * @param filename the name of the file to open.
       * @param keepPalette if the image is recognised as having palette data,
       *   then if this flag is true the palette will be kept. Otherwise the
       *   palette data will be lost (converted to RGB triplets vs indexed)
       */
      Image(const char * filename, bool keepPalette=false);

      std::string filename() const;

      ImageType type() const;

      /** Free up the data.*/
      ~Image();

      /** Is this valid or not.
       * @return true if the image is valid, false otherwise.
       */
      bool isValid() const;

      /** Get the width.
       * @return the image width in pixels.
       */
      unsigned int width() const;

      /** Get the height.
       * @return the image height in pixels.
       */
      unsigned int height() const;

      /** Max height allowed for an image. */
      static unsigned int MAX_HEIGHT;

      /** Max width allowed for an image. */
      static unsigned int MAX_WIDTH;

      /** Get the size of the palette, if this is a palettized image and the
       * keepPalette flag was used.
       * @return the number of colours in the palette.
       */
      unsigned int paletteSize() const;

      /** Get the number of channels (color depth) used by the image.
       * @returns the number of channels.
       */
      unsigned int channels() const;

      /** Get a pointer to the palette data.
       * @return the pointer to the palette data.
       * @see paletteSize()
       */
      const uint16_t *palette() const;

      /** Get the RGB or indexed image data.
       * @return pointer to the pixel data.
       */
      const uint16_t * data() const;

      void reload();

    private:

      bool m_valid { false };
      bool m_keepPalette;
      ImageType m_type { ImageNONE };
      std::string m_filename;

      // scaled w, h
      unsigned int m_width { 0 };
      unsigned int m_height { 0 };

      // actual w, h of image
      unsigned int m_realWidth { 0 };
      unsigned int m_realHeight { 0 };

      unsigned int m_paletteSize { 0 };
      unsigned int m_channels { 3 };
      unsigned short * m_data { NULL };
      unsigned short * m_palette { NULL };

      void readFile();

      void calculateScale();
      void allocData();
      void allocPalette(size_t size);

      DISALLOW_COPY_AND_ASSIGN(Image);
  };
}
#endif
