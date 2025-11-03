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
#include "ndspp.h"
#include "libnds.h"
#include "Image.h"
#include "Canvas.h"
#include "File.h"

using namespace nds;

void drawImage(Canvas & canvas, Image & image, int startx, int starty)
{
  const unsigned short * data = image.data();
  //const unsigned int channels = image.channels();
  canvas.fillRectangle(startx, starty, image.width(), image.height(), nds::Color(21,21,21));
  for (unsigned int y = 0; y < image.height(); ++y)
  {
    //const unsigned short * row = &data[y*image.width()*channels];
    const unsigned short * row = &data[y*image.width()];
    for (unsigned int x = 0; x < image.width(); ++x, ++row)
    {
      canvas.drawPixel(startx+x, starty+y, *row);//RGB8(row[0], row[1], row[2]));
    }
  }
}

const char * filenames[] = {
  "test.gif",
  "test.png",
  "test.jpg",
  0
};
int main(int argc, char * argv[])
{
  Canvas & canvas = Canvas::instance();
  canvas.fillRectangle(0, 0, canvas.width(), canvas.height(), nds::Color(31,21,21));
  int y = 0;
  for (const char **file = filenames; *file != 0; ++file)
  {
    Image img(*file);
    if (img.isValid())
    {
      drawImage(canvas, img, 0, y);
      y+= img.height()+2;
    }
  }
  canvas.endPaint();
  while(true)
  {
    swiWaitForVBlank();
  }
}
