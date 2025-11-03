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
#include "ndspp.h"
#include "libnds.h"
#include "Button.h"
#include "Canvas.h"
#include "FontFactory.h"
#include "RichTextArea.h"
#include "ScrollPane.h"
#include "Stylus.h"
#include "TextAreaFactory.h"
#include "TextArea.h"
#include "TextField.h"
#include "Language.h"
#include "sans.h"

using namespace nds;
int main(int argc, char * argv[])
{
  // wait for arm7 to read settings.
  for (int i = 0; i < 120; ++i)
    swiWaitForVBlank();

  TextAreaFactory::setFont(FontFactory::create(
        (unsigned char*)_binary_sans_set_bin_start,
        (unsigned char*)_binary_sans_map_bin_start));
  ScrollPane scrollPane;
  Language::instance().setDirectory("data/docs");
  TextField * tf = new TextField(T("text"));
  TextField * passwd = new TextField(std::string());
  RichTextArea * rich = (RichTextArea*)TextAreaFactory::create(TextAreaFactory::TXT_RICH);
  Button * goBtn = new Button(T("Go"));
  rich->appendText(T("long"));
  rich->appendText(T(Language::instance().currentLanguage()));
  passwd->setSize(60,18);

  scrollPane.setTopLevel();
  scrollPane.add(rich);
  scrollPane.add(passwd);
  scrollPane.add(goBtn);
  scrollPane.setSize(Canvas::instance().width(),Canvas::instance().height());
  scrollPane.add(tf);
  scrollPane.setScrollIncrement(13);

  scrollPane.setLocation(0,0);
  scrollPane.setSize(Canvas::instance().width(),Canvas::instance().height());

  bool needsPainting = true;
  while(true)
  {
    swiWaitForVBlank();
    scanKeys();
    u16 keys = keysDownRepeat();
    //u16 keys = keysHeld();
    if (keys & KEY_UP)
    {
      scrollPane.up();
      scrollPane.up();
      needsPainting = true;
    }
    if (keys & KEY_DOWN)
    {
      scrollPane.down();
      scrollPane.down();
      needsPainting = true;
    }
    if (keys & KEY_X)
    {
      break;
    }

    if (keys & KEY_Y) {
      scrollPane.setVisible(not scrollPane.visible());
      needsPainting = true;
    }

    if (keys & KEY_TOUCH)
    {
      touchPosition tp;
      touchRead(&tp);
      Stylus stylus;
      stylus.update(Stylus::DOWN, true, tp.px, tp.py+SCREEN_HEIGHT);
      if (not needsPainting)
        needsPainting = scrollPane.dirty();
    }

    if (needsPainting)
    {
      scrollPane.paint(scrollPane.preferredSize());
      Canvas::instance().endPaint();
    }
    needsPainting = false;
  }
}
