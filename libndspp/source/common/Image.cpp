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
#include <memory>
#include <cstdlib>
#include <cstring>
#include "libnds.h"
#include "Image.h"
#include "File.h"
#include "png.h"
#include "gif_lib.h"
#include "png.h"
extern "C" {
#include "jpeglib.h"
};
using nds::Image;
using std::auto_ptr;
static const int PNG_BYTES_TO_CHECK = 8;
static const int JPEG_BYTES_TO_CHECK = 11;
static const u8 JPEG_SIGNATURE[11] = {
  0xFF, 0xD8, 0xFF, 0xE0, 0x00, 0x10, 0x4A, 0x46,
  0x49, 0x46, 0x00
};
static const unsigned int MAX_IMAGE_WIDTH(SCREEN_WIDTH-7);
static const unsigned int MAX_IMAGE_HEIGHT(SCREEN_HEIGHT*2);

static bool isPng(const char *filename)
{
   png_byte buf[PNG_BYTES_TO_CHECK];

   nds::File f;
   f.open(filename, "rb");
   /* Open the prospective PNG file. */
   if (not f.is_open())
      return 0;

   /* Read in some of the signature bytes */
   if (f.read((char*)buf, PNG_BYTES_TO_CHECK) != PNG_BYTES_TO_CHECK)
      return 0;

   /* Compare the first PNG_BYTES_TO_CHECK bytes of the signature.
      Return nonzero (true) if they match */
   return(!png_sig_cmp(buf, (png_size_t)0, PNG_BYTES_TO_CHECK));
}

static bool isGif(const char *filename)
{
   char buf[GIF_STAMP_LEN+1];

   nds::File f;
   f.open(filename, "rb");
   if (not f.is_open())
      return 0;

   /* Read in some of the signature bytes */
   if (f.read(buf, GIF_STAMP_LEN) != GIF_STAMP_LEN)
      return 0;

   buf[GIF_STAMP_LEN] = 0;
   /* Compare the first 3 bytes of the signature.  Return true if they match */
   return (strncmp(GIF_STAMP, buf, GIF_VERSION_POS) == 0);
}

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

static bool isJpeg(const char * filename)
{
  nds::File f;
  f.open(filename, "rb");
  if (f.is_open())
  {
    Array array(JPEG_BYTES_TO_CHECK);
    f.read(array, array.length());
    f.close();
    // skip 2 bytes.
    array[4] = 0x00;
    array[5] = 0x10;
    if (memcmp((const char*)array, JPEG_SIGNATURE, 2/* JPEG_BYTES_TO_CHECK*/) == 0)
    {
      return true;
    }
  }
  return false;
}

Image::ImageType Image::imageType(const char * filename)
{
  if (isPng(filename)) return Image::ImagePNG;
  if (isGif(filename)) return Image::ImageGIF;
  if (isJpeg(filename)) return Image::ImageJPEG;
  return Image::ImageUNKNOWN;
}

Image::Image(const char * filename, ImageType type, bool keepPalette):
  m_valid(false),
  m_keepPalette(keepPalette),
  m_filename(filename),
  m_width(0),
  m_height(0),
  m_currentLine(0),
  m_paletteSize(0),
  m_channels(3),
  m_data(0),
  m_palette(0)
{
  ImageType actualType(imageType(filename));
  if (actualType != type)
  {
    // make your mind up
    return;
  }
  if (type == ImageUNKNOWN)
  {
    return;
  }
  m_type = type;
  reload();
}

Image::Image(const char * filename, bool keepPalette):
  m_valid(false),
  m_keepPalette(keepPalette),
  m_filename(filename),
  m_width(0),
  m_height(0),
  m_currentLine(0),
  m_paletteSize(0),
  m_channels(3),
  m_data(0),
  m_palette(0)
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

void Image::renderLine(const unsigned char * line, int n)
{
  // copy the data in m_channels format to the m_data at line n
  int yScale = (m_height * 256) / m_realHeight;

  // draw 0.. yScale, yScale*2, yScale*3 pixels
  int lastN = n-1;
  if ( ((lastN * yScale)/256) == ((n*yScale)/256))
  {
    return;
  }

  int xScale = (m_width * 256) / m_realWidth;

  //printf("m_currentLine %d\n", m_currentLine);
  u16 *db = &m_data[m_width*m_currentLine];
  int bytes = (m_channels * m_bpp)/8;

  for (int x = m_realWidth; x > 0; x--, line += bytes)
  {
    int lastX = x-1;
    if (((lastX * xScale)/256) == ((x*xScale)/256))
    {
      continue;
    }
    if (m_channels >= 3)
    {
      // standard RGB or RGBA images.
      switch (m_bpp) {
        case 16:
          db[0] = RGB8(line[0], line[2], line[4]);
          break;
        default:
          db[0] = RGB8(line[0], line[1], line[2]);
          break;
      }
    }
    else if (m_paletteSize == 0)
    {
      // Gray scale images.
      db[0] = RGB8(line[0], line[0], line[0]);
    }
    else if (m_keepPalette)
    {
      // Palettized images, keep the palette indices.
      db[0] = line[0];
    }
    else
    {
      // Palettized images, convert from palette to colour.
      db[0] = m_palette[line[0]];
    }
    db++;
  }
  m_currentLine++;
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

static void user_read_fn(png_structp png_ptr, unsigned char *data, png_size_t size)
{
  nds::File * f = (nds::File*)png_get_io_ptr(png_ptr);
  size_t n = f->read((char*)data, size);
  if(size && (n == 0)) {
    // ups - go to the error handler
    longjmp(png_jmpbuf(png_ptr), 1);
  }
}

void Image::readPng()
{
   m_valid = false;
   nds::File f;
   f.open(m_filename.c_str(), "rb");
   if (not f.is_open())
   {
     return;
   }

   png_structp png_ptr= png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
   if (png_ptr == NULL)
   {
      return;
   }

   /* Allocate/initialize the memory for image information.  REQUIRED. */
   png_infop info_ptr = png_create_info_struct(png_ptr);
   if (info_ptr == NULL)
   {
      png_destroy_read_struct(&png_ptr, png_infopp_NULL, png_infopp_NULL);
      return;
   }

   /* Set error handling if you are using the setjmp/longjmp method (this is
    * the normal method of doing things with libpng).  REQUIRED unless you
    * set up your own error handlers in the png_create_read_struct() earlier.
    */
   if (setjmp(png_jmpbuf(png_ptr)))
   {
      /* Free all of the memory associated with the png_ptr and info_ptr */
      png_destroy_read_struct(&png_ptr, &info_ptr, png_infopp_NULL);
      /* If we get here, we had a problem reading the file */
      return;
   }

   // compiled png with NO_STIO - avoids sprintf madness.
   png_set_read_fn(png_ptr, (void *)&f, user_read_fn);
   // read the easy way
   png_read_info(png_ptr, info_ptr);

   // alpha images require this:
   if (info_ptr->color_type & PNG_COLOR_MASK_ALPHA)
     png_set_strip_alpha(png_ptr);

   // paletted images require this:
   if(info_ptr->color_type == PNG_COLOR_TYPE_PALETTE)
   {
     if (not m_keepPalette) {
       m_paletteSize = 0;
       png_set_palette_to_rgb(png_ptr);
     }
     else
     {
       png_colorp png_palette;
       int palette_entries;
       png_get_PLTE(png_ptr,info_ptr, &png_palette, &palette_entries);
       m_paletteSize = palette_entries;
       allocPalette(sizeof(unsigned short) * palette_entries);
       for (int i = 0; i < palette_entries; ++i)
       {
         m_palette[i] = RGB8(png_palette[i].red, png_palette[i].green, png_palette[i].blue);
       }
     }
   }

   int number_passes = png_set_interlace_handling(png_ptr);
   png_read_update_info(png_ptr, info_ptr);


   m_realWidth =info_ptr->width;
   m_realHeight =info_ptr->height;
   m_channels = png_get_channels(png_ptr, info_ptr);
   m_bpp = png_get_bit_depth(png_ptr, info_ptr);
   unsigned int h = m_height;
   unsigned int w = m_width;
   calculateScale();
   if (!m_data or (w != m_width or h != m_height))
   {
     allocData();
   }

   png_bytep rowBuffer = (png_bytep)malloc(sizeof(png_bytep) * info_ptr->rowbytes);
   for (int pass = 0; pass < number_passes; pass++)
   {
     m_currentLine = 0;
     for (unsigned int line = 0; line < m_realHeight; line++)
     {
       png_read_row(png_ptr, rowBuffer, png_bytep_NULL);
       renderLine((const unsigned char*)rowBuffer, line);
     }
   }
   m_height = m_currentLine;
   free(rowBuffer);

   /* clean up after the read, and free any memory allocated - REQUIRED */
   png_destroy_read_struct(&png_ptr, &info_ptr, png_infopp_NULL);

   /* that's it */
   m_valid = true;
}

static int user_read_gif(GifFileType * gif, GifByteType * data, int size)
{
  nds::File * f = (nds::File*)(gif->UserData);
  size_t n = f->read((char*)data, size);
  return n;
}

/** RAII wrapper around the GifFileType. */
class GifClass
{
  public:
    inline GifClass(const char * filename):m_buffer(0)
    {
      m_file.open(filename, "rb");
      m_gifFile = DGifOpen((void*)&m_file, user_read_gif);
    }

    inline ~GifClass()
    {
      if (m_gifFile)
        DGifCloseFile(m_gifFile);
      delete m_buffer;
    }

    // hackily throw in the screen buffer pointer too...
    inline void setScreenBuffer(unsigned char * buf)
    {
      m_buffer = buf;
    }

    inline operator GifFileType *()
    {
      return m_gifFile;
    }

    inline GifFileType * operator ->()
    {
      return m_gifFile;
    }

  private:
    GifFileType * m_gifFile;
    unsigned char * m_buffer;
    nds::File m_file;
};

static const int InterlacedOffset[] = { 0, 4, 2, 1 };
static const int InterlacedJumps[] = { 8, 8, 4, 2 };

void Image::readGif()
{
  m_valid = false;
  GifClass gifFile(m_filename.c_str());
  if (gifFile == 0)
  {
    return;
  }
  m_realWidth = gifFile->SWidth;
  m_realHeight = gifFile->SHeight;
  m_channels = 1;
  m_bpp = m_channels * 8;
  unsigned int h = m_height;
  unsigned int w = m_width;
  calculateScale();
  if (!m_data or w != m_width or h != m_height)
  {
    allocData();
  }
  if (!m_data)
  {
    return;
  }

  // map ScreenBuffer to m_data
  ColorMapObject * colorMap = (gifFile->Image.ColorMap ? gifFile->Image.ColorMap : gifFile->SColorMap);
  // TODO: handle other gif types
  if (!colorMap)
    return;
  m_paletteSize = colorMap->ColorCount;
  allocPalette(sizeof(unsigned short) * m_paletteSize);
  if (!m_palette)
  {
    return;
  }

  for (unsigned int i = 0; i < m_paletteSize; ++i)
  {
    GifColorType * colorMapEntry = &colorMap->Colors[i];
    m_palette[i] = RGB8(colorMapEntry->Red, colorMapEntry->Green, colorMapEntry->Blue);
  }

  // initialise m_data to bg color
  for (unsigned int i = 0; i < m_width * m_height; i++)
  {
    if (m_keepPalette)
    {
      m_data[i] = gifFile->SBackGroundColor;
    }
    else
    {
      m_data[i] = m_palette[gifFile->SBackGroundColor];
    }
  }

  u8 * rowBuffer = new u8[m_realWidth];
  GifRecordType recordType;
  do {
    GifByteType *extension;
    if (DGifGetRecordType(gifFile, &recordType) == GIF_ERROR) {
      return;
    }
    switch (recordType) {
      case IMAGE_DESC_RECORD_TYPE:
        {
          if (DGifGetImageDesc(gifFile) == GIF_ERROR) {
            return;
          }
          int row = gifFile->Image.Top; /* Image Position relative to Screen. */
          int col = gifFile->Image.Left;
          int width = gifFile->Image.Width;
          int height = gifFile->Image.Height;

          if ((col + width) > gifFile->SWidth
              or (row + height) > gifFile->SHeight)
          {
            return;
          }
          if (gifFile->Image.Interlace) {
            /* Need to perform 4 passes on the images: */
            for (int i = 0; i < 4; i++)
            {
              m_currentLine = 0;
              for (int j = row + InterlacedOffset[i]; j < row + height;
                  j += InterlacedJumps[i])
              {
                if (DGifGetLine(gifFile, rowBuffer, width) == GIF_ERROR)
                {
                  return;
                }
                //printf("renderLine j %d\n", j);
                renderLine(rowBuffer, j-row);
              }
            }
          }
          else {
            m_currentLine = 0;
            for (int i = 0; i < height; i++, row++) {
              if (DGifGetLine(gifFile, rowBuffer, width) == GIF_ERROR)
              {
                return;
              }
              //printf("renderLine i %d\n", i);
              renderLine(rowBuffer, i);
            }
          }
          m_height = m_currentLine;
        }
        break;
      case EXTENSION_RECORD_TYPE:
        /* Skip any extension blocks in file: */
        {
          int extCode;
          if (DGifGetExtension(gifFile, &extCode, &extension) == GIF_ERROR) {
            return;
          }
        }
        while (extension != NULL) {
          if (DGifGetExtensionNext(gifFile, &extension) == GIF_ERROR) {
            return;
          }
        }
        break;
      case TERMINATE_RECORD_TYPE:
        break;
      default:
        /* Should be traps by DGifGetRecordType. */
        break;
    }
  }
  while (recordType != TERMINATE_RECORD_TYPE);
  delete [] rowBuffer;

  m_valid = true;
}

struct safer_err_mgr
{
  struct jpeg_error_mgr pub;
  jmp_buf jumpbuf;
};

void user_error_exit (j_common_ptr cinfo)
{
  safer_err_mgr* err = (safer_err_mgr*) cinfo->err;
  (*cinfo->err->output_message) (cinfo);
  longjmp(err->jumpbuf, 1);
}

// create a new jpeg_decoder_file_stream
class JpegFileStream
{
  public:
    JpegFileStream(const char * filename):m_scanLine(0)
    {
      cinfo.err = jpeg_std_error(&jerr.pub);
      jerr.pub.error_exit = user_error_exit;
      jpeg_create_decompress(&cinfo);
      m_file.open(filename);
    }

    jmp_buf * get_jmpbuf()
    {
      return &jerr.jumpbuf;
    }

    ~JpegFileStream()
    {
      free(m_scanLine);
      jpeg_destroy_decompress(&cinfo);

    }

    bool is_open() const
    {
      return m_file.is_open();
    }

    bool begin()
    {
      jpeg_stdio_src(&cinfo, (FILE*)m_file.file());
      int retcode = jpeg_read_header(&cinfo, FALSE);
      if (retcode != JPEG_HEADER_OK)
      {
        return false;
      }

      jpeg_calc_output_dimensions(&cinfo);
      m_scanLine = (unsigned char*)calloc(components()*width(), 1);
      return jpeg_start_decompress(&cinfo);
    }

    int width() const
    {
      return cinfo.output_width;
    }

    int height() const
    {
      return cinfo.output_height;
    }

    int components() const
    {
      return cinfo.output_components;
    }

    int bytes_per_pixel() const
    {
      return 1;
    }

    bool decode(void * &scanline)
    {
      if (jpeg_read_scanlines(&cinfo, &m_scanLine, 1))
      {
        scanline = m_scanLine;
        return true;
      }
      return false;
    }

    void done()
    {
      jpeg_finish_decompress(&cinfo);
    }

  private:
    struct jpeg_decompress_struct cinfo;
    struct safer_err_mgr jerr;
    nds::File m_file;
    unsigned char * m_scanLine;
};

void Image::calculateScale()
{
  // calc the ratios
  int xRatio = (m_realWidth * 256) / m_realHeight;
  // e.g. 400x200 => xRatio = 2
  int yRatio = (m_realHeight * 256) / m_realWidth;
  // e.g. 400x200 => yRatio = 0.5
  m_currentLine = 0;

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
void Image::readJpeg()
{
  m_valid = false;
  auto_ptr<JpegFileStream> inputStream(new JpegFileStream(m_filename.c_str()));
  if (setjmp(*inputStream->get_jmpbuf()))
  {
    /* If we get here, we had a problem reading the file */
    return;
  }

  if (not inputStream->is_open())
  {
    return;
  }

  if (not inputStream->begin())
  {
    return;
  }

  m_realWidth = inputStream->width();
  m_realHeight = inputStream->height();
  m_channels = inputStream->components();
  unsigned int h = m_height;
  unsigned int w = m_width;
  calculateScale();
  if (!m_data or (w != m_width or h != m_height))
  {
    allocData();
  }
  if (!m_data)
  {
    return;
  }

  for (unsigned int line = 0; line < m_realHeight; ++line)
  {
    void * scanline;
    if (not inputStream->decode(scanline))
      break;
    m_bpp = inputStream->bytes_per_pixel() * 8;
    renderLine((const unsigned char*)scanline, line);
  }
  inputStream->done();
  m_height = m_currentLine;

  m_valid = true;
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
  switch (m_type)
  {
    case ImagePNG:
      readPng();
      break;
    case ImageGIF:
      readGif();
      break;
    case ImageJPEG:
      readJpeg();
      break;
    case ImageUNKNOWN:
      m_type = imageType(m_filename.c_str());
      break;
  }
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
