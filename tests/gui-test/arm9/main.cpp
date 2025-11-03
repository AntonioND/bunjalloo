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
#include "ButtonGroup.h"
#include "Button.h"
#include "Canvas.h"
#include "CheckBox.h"
#include "ComboBox.h"
#include "EditableTextArea.h"
#include "Font.h"
#include "FontFactory.h"
#include "Keyboard.h"
#include "RadioButton.h"
#include "RichTextArea.h"
#include "ScrollPane.h"
#include "Stylus.h"
#include "TextAreaFactory.h"
#include "TextArea.h"
#include "TextField.h"
#include "sans.h"

extern const char _binary_test_map_bin_start[];

using namespace nds;
int main(int argc, char * argv[])
{
  Font *font(FontFactory::create(
        (unsigned char*)_binary_sans_set_bin_start,
        (unsigned char*)_binary_sans_map_bin_start));
  TextAreaFactory::setFont(font);
  //TextAreaFactory::usePaletteData((const char*)_binary_vera_pal_bin_start, 32);
  Keyboard * keyBoard = new Keyboard();
  ScrollPane scrollPane;
  keyBoard->setTopLevel(&scrollPane);
#if 1
  TextField * tf = new TextField("Enter the value here. This line is too big to fit in");
  TextField * passwd = new TextField(std::string());
  RichTextArea * rich = (RichTextArea*)TextAreaFactory::create(TextAreaFactory::TXT_RICH);
  Button * goBtn = new Button("Go");
  rich->appendText("This is some long text ");
  passwd->setSize(60,18);
  
  scrollPane.setTopLevel();
  scrollPane.add(rich);
  scrollPane.add(passwd);
  scrollPane.add(goBtn);
  scrollPane.setSize(Canvas::instance().width(),Canvas::instance().height());
  scrollPane.add(tf);
  scrollPane.setScrollIncrement(13);

#else
  RichTextArea * rich = (RichTextArea*)TextAreaFactory::create(TextAreaFactory::TXT_RICH);
  TextArea * t = TextAreaFactory::create();
  EditableTextArea * t2 = (EditableTextArea*)TextAreaFactory::create(TextAreaFactory::TXT_EDIT);
  // t2->setBackgroundColor(Color(31,31,0));

  
  ScrollPane * subPane = new ScrollPane;
  subPane->add(t2);
  subPane->setTopLevel(false);
  subPane->setSize(Canvas::instance().width(),Canvas::instance().height());
  subPane->setScrollIncrement(t2->font().height());
  subPane->setStretchChildren(true);
  
  
  ComboBox * emptyCombo = new ComboBox();
  ComboBox * oneValCombo = new ComboBox();
  ComboBox * combo = new ComboBox();
  combo->setSize(60, 18);
  ComboBox * combo2 = new ComboBox();
  combo2->setSize(90, 18);
  
  std::string str("Hello World");
  Button * b1 = new Button(str);

  str = "Combo box!";
  oneValCombo->addItem(str);
  combo->addItem(str);
  combo2->addItem(str);
  combo2->addItem(str);
  str = "Another One - with very wide text";
  Button * b2 = new Button(str);
  combo->addItem(str);
  combo2->addItem(str);
  combo2->addItem(str);
  str = "Three- wide, but fixed width";
  Button * b3 = new Button(str);
  b3->setSize(60, 18);
  combo2->addItem(str);
  combo2->addItem(str);
  str = "Go";
  Button * b4 = new Button(str);
  //b4->setSize(6*13, 10);
  str = "Last one!";
  Button * b5 = new Button(str);
  combo->addItem(str);
  combo2->addItem(str);
  combo2->addItem(str);

  t2->setListener(keyBoard);
  
  str = "A text field that has a very, very long and pointless string";
  TextField * tf1 = new TextField(str);
  tf1->setSize(160, 18);
  tf1->setListener(keyBoard);
  combo2->addItem(str);

  TextArea * rbLabel = TextAreaFactory::create();
  str = "Radio button label.";
  rbLabel->appendText(str);

  TextArea * cbLabel = TextAreaFactory::create();
  str = "CheckBox label.";
  cbLabel->appendText(str);

  RadioButton * rb = new RadioButton();
  RadioButton * rb2 = new RadioButton();
  RadioButton * rb3 = new RadioButton();
  CheckBox * cb = new CheckBox();
  ButtonGroup bg;
  bg.add(rb);
  bg.add(rb2);
  bg.add(rb3);
  keyBoard->setTopLevel(&scrollPane);
  scrollPane.setTopLevel();
  scrollPane.add(rich);
  scrollPane.add(t);
  scrollPane.add(b1);
  scrollPane.add(b2);
 
  scrollPane.add(b3);
  scrollPane.add(tf1);
  scrollPane.add(b4);
  scrollPane.add(emptyCombo);
  scrollPane.add(oneValCombo);
  scrollPane.add(combo2);
  scrollPane.add(rb);
  scrollPane.add(rb2);
  scrollPane.add(rb3);
  scrollPane.add(rbLabel);
  scrollPane.add(cb);
  scrollPane.add(cbLabel);
  scrollPane.add(subPane);
  scrollPane.add(combo);
  scrollPane.add(b5);
  
  scrollPane.setSize(Canvas::instance().width(),Canvas::instance().height());
  scrollPane.setScrollIncrement(t->font().height());

  std::string richText1("This is normal text. ");
  std::string richText2("This is a link.");
  std::string richText3("Longer text, but still normal. ");
  std::string richText4("Normal text. ");
  std::string richText5("Link text. ");
  rich->appendText(richText1);
  rich->insertNewline();
  rich->appendText(richText1);
  rich->appendText(richText1);
  rich->appendText(richText3);
  rich->appendText(richText4);
  rich->addLink("www.link1.com");
  rich->appendText(richText2);
  rich->endLink();
  rich->appendText(richText3);
  rich->appendText(richText1);
  rich->addLink("www.link2.com");
  rich->appendText(richText5);
  rich->endLink();


  std::string s(_binary_test_map_bin_start, 1000);
  t->appendText(s);
  std::string s2(&_binary_test_map_bin_start[1200], 1300);
  t2->appendText(s2);
  subPane->setSize(t2->width(), 100);
  t2->setParentScroller(subPane);
#endif
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
      //printf("%s\n",t2->asString().c_str());
      break;
    }

    if (keys & KEY_Y) {
      scrollPane.setVisible(not scrollPane.visible());
      keyBoard->setVisible(not keyBoard->visible());
      needsPainting = true;
    }

    if (keys & KEY_TOUCH)
    {
      touchPosition tp;
      touchRead(&tp);
      Stylus stylus;
      stylus.update(Stylus::DOWN, true, tp.px, tp.py+SCREEN_HEIGHT);
      needsPainting = keyBoard->dirty();
      if (not needsPainting)
        needsPainting = scrollPane.dirty();
    }

    needsPainting |= keyBoard->tick();
    if (needsPainting)
    {
      scrollPane.paint(scrollPane.preferredSize());
      keyBoard->paint(scrollPane.preferredSize());
      Canvas::instance().endPaint();
    }
    needsPainting = false;
  }
  delete keyBoard;
}
