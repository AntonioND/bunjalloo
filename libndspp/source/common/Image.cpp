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
#include "Slot2Allocator.h"

#include <libplum.h>

using nds::Image;
using std::auto_ptr;

unsigned int Image::MAX_HEIGHT(SCREEN_HEIGHT * 2);
unsigned int Image::MAX_WIDTH(SCREEN_WIDTH - 7);

Image::Image(const char * filename, bool keepPalette):
  m_keepPalette(keepPalette),
  m_filename(filename)
{
  reload();
}

Image::~Image()
{
  freeData();
  freePalette();
}

void Image::allocData(size_t size, bool allowExternal)
{
  freeData();

  if (allowExternal)
  {
    unsigned short * ptr = (unsigned short *)Slot2Allocator::instance().alloc(size);
    if (ptr != NULL)
    {
      m_data = ptr;
      m_dataExternal = true;
      return;
    }
  }

  m_data = (unsigned short*)malloc(size);
  m_dataExternal = false;
}

void Image::allocPalette(size_t size)
{
  freePalette();
  m_palette = (unsigned short*)malloc(size);
}

void Image::freeData()
{
  if (m_data == NULL)
    return;

  if (m_dataExternal)
    Slot2Allocator::instance().free(m_data);
  else
    free(m_data);

  m_data = NULL;
}

void Image::freePalette()
{
  if (m_palette == NULL)
    return;

  free(m_palette);
  m_palette = NULL;
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

void Image::readFile()
{
    m_valid = false;

    const char *path = m_filename.c_str();

    //fprintf(stderr, "Loading %s (%d)\n", path, m_keepPalette);
    //fflush(stderr);

    unsigned int error;
    struct plum_image *image =
        plum_load_image_limited(path, PLUM_MODE_FILENAME,
                                PLUM_COLOR_16 | PLUM_ALPHA_INVERT | PLUM_PALETTE_LOAD,
                                2048 * 2048, &error);
    if (image == NULL) {
        //fprintf(stderr, "Error loading image '%s': %s (%u)\n", path,
        //        plum_get_error_text(error), error);
        //fflush(stderr);
        return;
    }

    switch (image->type)
    {
        case PLUM_IMAGE_BMP:
            m_type = ImageBMP;
            break;
        case PLUM_IMAGE_GIF:
            m_type = ImageGIF;
            break;
        case PLUM_IMAGE_PNG:
            m_type = ImagePNG;
            break;
        case PLUM_IMAGE_APNG:
            m_type = ImageAPNG;
            break;
        case PLUM_IMAGE_JPEG:
            m_type = ImageJPEG;
            break;

        case PLUM_IMAGE_PNM: // Not a valid MIME type
        default:
            goto error;
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
        allocPalette(m_paletteSize * 2);
        if (m_palette == NULL)
            goto error;
        memcpy(m_palette, image->palette, m_paletteSize * 2);
    }

    if (m_palette)
    {
        if (m_keepPalette)
        {
            allocData(m_height * m_width); // Don't allow external RAM
            if (m_data == NULL)
            {
                freePalette();
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
        }
        else
        {
            allocData(m_height * m_width * 2, true); // Allow external RAM
            if (m_data == NULL)
            {
                freePalette();
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

                    // TODO: Should we check transparency?
                    uint8_t color = ((uint8_t*)image->data)[src_index];
                    ((uint16_t*)m_data)[dst_index] = ((uint16_t *)m_palette)[color];
                }
            }

            freePalette();
            m_palette = NULL;

            m_paletteSize = 0;
        }
    }
    else
    {
        allocData(m_height * m_width * 2, true); // Allow external RAM
        if (m_data == NULL)
        {
            freePalette();
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

                    uint16_t color = ((uint16_t*)image->data)[src_index];
                    ((uint16_t*)m_data)[dst_index] = color;
                }
            }
        }
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
  if ((m_realWidth > MAX_WIDTH) && (m_realHeight > MAX_HEIGHT))
  {
    int factor_for_height = (MAX_HEIGHT * 256) / m_realHeight;
    int factor_for_width = (MAX_WIDTH * 256) / m_realWidth;

    int factor;

    // We need to pick the factor that reduces the image the most so that both
    // dimensions fit
    if (factor_for_height < factor_for_width)
      factor = factor_for_height;
    else
      factor = factor_for_width;

    m_width = (m_realWidth * factor) / 256;
    m_height = (m_realHeight * factor) / 256;
  }
  else if (m_realWidth > MAX_WIDTH)
  {
    m_width = MAX_WIDTH;
    m_height = (m_realHeight * m_width) / m_realWidth;
  }
  else if (m_realHeight > MAX_HEIGHT)
  {
    m_height = MAX_HEIGHT;
    m_width = (m_realWidth * m_height) / m_realHeight;
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
  if (m_valid)
    return;

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

Image::ImageType Image::type() const
{
  return m_type;
}
