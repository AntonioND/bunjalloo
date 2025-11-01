/*
  Copyright (C) 2007,2008 Richard Quirk
  Copyright (C) 2025 Antonio Niño Díaz

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
#include <memory>
#include <cstdlib>
#include <cstring>
#include "libnds.h"
#include "Image.h"
#include "File.h"

#include <libplum.h>

using nds::Image;
using std::auto_ptr;

static const unsigned int MAX_IMAGE_WIDTH(SCREEN_WIDTH-7);
static const unsigned int MAX_IMAGE_HEIGHT(SCREEN_HEIGHT*2);

class Array
{
  public:
    Array(int size)
      : m_array(new unsigned char[size]),
      m_size(size)
    {}

    ~Array()
    {
      delete [] m_array;
    }

    inline operator unsigned char* () const
    {
      return m_array;
    }

    inline operator char* () const
    {
      return (char*)m_array;
    }

    inline unsigned char & operator[] (int i)
    {
      return m_array[i];
    }

    unsigned int length() const
    {
      return m_size;
    }

  private:
    unsigned char * m_array;
    unsigned int m_size;
};

Image::ImageType Image::imageType(const char * filename)
{
  // TODO: Add more types to this enum
  return Image::ImagePNG;
}

Image::Image(const char * filename, bool keepPalette):
  m_valid(false),
  m_keepPalette(keepPalette),
  m_filename(filename),
  m_width(0),
  m_height(0),
  m_paletteSize(0),
  m_channels(3),
  m_data(NULL),
  m_palette(NULL)
{
  ImageType type(imageType(filename));
  m_type = type;
  reload();
}

Image::~Image()
{
  free(m_data);
  free(m_palette);
}

bool Image::isValid() const
{
  return m_valid;
}

unsigned int Image::width() const
{
  return m_width;
}

unsigned int Image::height() const
{
  return m_height;
}

// RGB data.
const unsigned short * Image::data() const
{
  return m_data;
}

// allocate data, assuming it isn't already alloc'd
void Image::allocData()
{
  size_t size(m_width * m_height * sizeof(u16));
  m_data = (unsigned short*)realloc( m_data, size);
  memset(m_data, 0, size);
}

void Image::allocPalette(size_t size)
{
  if (!m_palette) {
    m_palette = (unsigned short*)malloc(size);
  }
  else {
    m_palette = (unsigned short*)realloc(m_palette, size);
  }
}

void Image::readFile()
{
    m_valid = false;

    const char *path = m_filename.c_str();

    //fprintf(stderr, "Loading %s (%d)\n", path, m_keepPalette);
    //fflush(stderr);

    unsigned int error;
    struct plum_image *image =
        plum_load_image_limited(path, PLUM_MODE_FILENAME,
                                PLUM_COLOR_16 | PLUM_PALETTE_LOAD,
                                2048 * 2048, &error);
    if (image == NULL) {
        //fprintf(stderr, "Error loading image '%s': %s (%u)\n", path,
        //        plum_get_error_text(error), error);
        //fflush(stderr);
        return;
    }

    m_realWidth = image->width;
    m_realHeight = image->height;
    calculateScale();

    m_channels = 1;

    //fprintf(stderr, "%d %ux%u -> %ux%u\n", image->color_format,
    //        m_realWidth, m_realHeight, m_width, m_height);
    //fflush(stderr);

    if (image->palette != NULL) {
        //fprintf(stderr, "Has palette\n");
        //fflush(stderr);

        m_paletteSize = (int)image->max_palette_index + 1;
        m_palette = (unsigned short *)malloc(m_paletteSize * 2);
        if (m_palette == NULL)
            goto error;
        memcpy(m_palette, image->palette, m_paletteSize * 2);
    }

    if (m_palette)
    {
        if (m_keepPalette)
        {
            m_data = (unsigned short *)malloc(m_height * m_width);
            if (m_data == NULL)
            {
                free(m_palette);
                goto error;
            }

            if (m_height == m_realHeight && m_width == m_realWidth)
            {
                memcpy(m_data, image->data, m_height * m_width);
            }
            else
            {
                for (unsigned int y = 0; y < m_height; y++)
                {
                    unsigned int realY = (y * m_realHeight) / m_height;
                    for (unsigned int x = 0; x < m_width; x++)
                    {
                        unsigned int realX = (x * m_realWidth) / m_width;
                        unsigned int src_index = realY * m_realWidth + realX;
                        unsigned int dst_index = y * m_width + x;
                        ((uint8_t*)m_data)[dst_index] = ((uint8_t*)image->data)[src_index];
                    }
                }
            }

            m_bpp = 8;
        }
        else
        {
            m_data = (unsigned short *)malloc(m_height * m_width * 2);
            if (m_data == NULL)
            {
                free(m_palette);
                goto error;
            }

            for (unsigned int y = 0; y < m_height; y++)
            {
                unsigned int realY = (y * m_realHeight) / m_height;
                for (unsigned int x = 0; x < m_width; x++)
                {
                    unsigned int realX = (x * m_realWidth) / m_width;
                    unsigned int src_index = realY * m_realWidth + realX;
                    unsigned int dst_index = y * m_width + x;

                    uint8_t color = ((uint8_t*)image->data)[src_index];
                    ((uint16_t*)m_data)[dst_index] = ((uint16_t *)m_palette)[color];
                }
            }

            free(m_palette);
            m_palette = NULL;

            m_paletteSize = 0;
            m_bpp = 16;
        }
    }
    else
    {
        m_data = (unsigned short *)malloc(m_height * m_width * 2);
        if (m_data == NULL)
        {
            free(m_palette);
            goto error;
        }

        if (m_height == m_realHeight && m_width == m_realWidth)
        {
            memcpy(m_data, image->data, m_height * m_width * 2);
        }
        else
        {
            for (unsigned int y = 0; y < m_height; y++)
            {
                unsigned int realY = (y * m_realHeight) / m_height;
                for (unsigned int x = 0; x < m_width; x++)
                {
                    unsigned int realX = (x * m_realWidth) / m_width;
                    unsigned int src_index = realY * m_realWidth + realX;
                    unsigned int dst_index = y * m_width + x;
                    ((uint16_t*)m_data)[dst_index] = ((uint16_t*)image->data)[src_index];
                }
            }
        }

        m_bpp = 16;
    }

    plum_destroy_image(image);
    m_valid = true;
    return;
error:
    m_realWidth = 0;
    m_realHeight = 0;
    m_width = 0;
    m_height = 0;
    plum_destroy_image(image);
    return;
}

void Image::calculateScale()
{
  // calc the ratios
  int xRatio = (m_realWidth * 256) / m_realHeight;
  // e.g. 400x200 => xRatio = 2
  int yRatio = (m_realHeight * 256) / m_realWidth;
  // e.g. 400x200 => yRatio = 0.5

  if (m_realWidth > MAX_IMAGE_WIDTH or m_realHeight > MAX_IMAGE_HEIGHT)
  {
    // scale so that fits on screen.
    if (xRatio > yRatio)
    {
      if (m_realWidth > MAX_IMAGE_WIDTH)
      {
        m_width = MAX_IMAGE_WIDTH;
        m_height = (m_width * yRatio)/256;
      }
      else
      {
        m_height = MAX_IMAGE_HEIGHT;
        m_width = (m_height * xRatio)/256;
      }
    }
    else
    {
      if (m_realHeight > MAX_IMAGE_HEIGHT)
      {
        m_height = MAX_IMAGE_HEIGHT;
        m_width = (m_height * xRatio)/256;
      }
      else
      {
        m_width = MAX_IMAGE_WIDTH;
        m_height = (m_width * yRatio)/256;
      }
    }
  }
  else
  {
    m_width = m_realWidth;
    m_height = m_realHeight;
  }
}

unsigned int Image::paletteSize() const
{
  return m_paletteSize;
}
const unsigned short * Image::palette() const
{
  return m_palette;
}

void Image::reload()
{
  readFile();
}

unsigned int Image::channels() const
{
  return m_channels;
}

std::string Image::filename() const
{
  return m_filename;
}

void Image::setType(ImageType type)
{
  m_type = type;
}
Image::ImageType Image::type() const
{
  return m_type;
}
