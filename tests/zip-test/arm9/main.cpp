/*
  Copyright (C) 2008 Richard Quirk

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
#include <vector>
#include <string>

#include "libnds.h"
#include "ndspp.h"
#include "Canvas.h"
#include "FontFactory.h"
#include "RichTextArea.h"
#include "ScrollPane.h"
#include "TextAreaFactory.h"
#include "sans.h"

#include "File.h"
#include "ZipFile.h"

using namespace std;
using namespace nds;
int main(int argc, char * argv[])
{
  nds::File::exists("test.zip");
  TextAreaFactory::setFont(FontFactory::create(
        (unsigned char*)_binary_sans_set_bin_start,
        (unsigned char*)_binary_sans_map_bin_start));
  ScrollPane scrollPane;
  Canvas &canvas(Canvas::instance());
  RichTextArea * rich = (RichTextArea*)TextAreaFactory::create(TextAreaFactory::TXT_RICH);
  rich->appendText("Zip contains\n");
  ZipFile *zipfile = new ZipFile(0);

  vector<string> files;
  zipfile->open("test.zip");
  zipfile->list(files);

  for (vector<string>::const_iterator it(files.begin()); it != files.end(); ++it)
  {
    const string& file(*it);
    rich->appendText(file);
    rich->appendText("\n");
  }
  rich->appendText("End of zip file\n");

  scrollPane.setTopLevel();
  scrollPane.add(rich);
  scrollPane.setSize(canvas.width(),canvas.height());
  scrollPane.setScrollIncrement(13);

  scrollPane.setLocation(0,0);
  scrollPane.setSize(canvas.width(),canvas.height());
  scrollPane.scrollToPercent(128);

  bool needsPainting = true;
  while(true)
  {
    swiWaitForVBlank();
    if (needsPainting)
    {
      scrollPane.paint(scrollPane.preferredSize());
      canvas.endPaint();
    }
    needsPainting = false;
  }
}
