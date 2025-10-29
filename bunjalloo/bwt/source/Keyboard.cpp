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
#include <cstring>
#include <utf8.h>
#include "libnds.h"
#include "Button.h"
#include "Canvas.h"
#include "Palette.h"
#include "EditableTextArea.h"
#include "Keyboard.h"
#include "Language.h"
#include "RichTextArea.h"
#include "ScrollPane.h"
#include "Stylus.h"
#include "TextAreaFactory.h"
#include "TextEntryI.h"

// TODO: use config for this?
// Based on http://en.wikipedia.org/wiki/Image:KB_US-International.svg
static const char * NUMBERS          = "1234567890-=";
static const char * NUMBERS_SHIFT    = "!@#$%^&*()_+";
static const char * LETTERS          = "qwertyuiopasdfghjklzxcvbnm,./[]:\"";
static const char * LETTERS_SHIFT    = "QWERTYUIOPASDFGHJKLZXCVBNM<>?{};'";

// QWERTYUIOPASDFGHJKLZXCVBNM{};'<>?
static const char * EXTRA = "~\\àáãäåèéêëìíîïñðòóôõöùúûü¿£¥ýþç¡";
static const char * EXTRA_SHIFT = "ÀÀÁÂÃÄÅÈÉÊÝÞËÌÍÎÏÑÐÇßÒÓÔÕÖÙÚÛÜ¢|ÿ";

static const std::string BACKSPACE_STR("BkSp.");
static const std::string CAPS_STR("Cap");
static const std::string ENTER_STR(" Enter");
static const std::string SHIFT_STR("Shift");
static const std::string SPACE_STR(" ");
static const std::string EXTRA_STR(" Alt");
static const std::string CLEAR_STR(" Clr");

const static int KEY_HEIGHT = 18;
const static int KEY_WIDTH = 19;
const static int GAP = 10;
const static int SCROLLPANE_POS_Y = 4;
const static int SCROLLPANE_SIZE = SCREEN_HEIGHT*2/5 - GAP;
const static int INITIAL_Y = SCROLLPANE_SIZE + GAP + SCREEN_HEIGHT;
const static int INITIAL_X = 22;

const static int ROW1_LENGTH = 10;
const static int ROW2_LENGTH = 9;
const static int ROW3_LENGTH = 10;
const static int ROW4_LENGTH = 4;

const static int TICK_COUNT = 20;
const static int SCROLLBAR_DECOR = 7;

Keyboard::Keyboard():
  m_extra(false),
  m_shift(false),
  m_capsLock(false),
  m_selectedStatus(OK),
  m_scrollPane(new ScrollPane),
  m_textArea((EditableTextArea*)TextAreaFactory::create(TextAreaFactory::TXT_EDIT)),
  m_richTextArea((RichTextArea*)TextAreaFactory::create(TextAreaFactory::TXT_RICH)),
  m_shiftKey(new Button),
  m_capsLockKey(new Button),
  // m_tabKey(new Button),
  m_enterKey(new Button),
  m_backspaceKey(new Button),
  //m_deleteKey(new Button),
  m_spaceKey(new Button),
  m_extraKey(new Button),
  m_ok(new Button),
  m_cancel(new Button),
  m_clearKey(new Button)
{
  initUI();
  int x = INITIAL_X;
  int y = INITIAL_Y;
  const char * text(NUMBERS);
  createRow(x-KEY_WIDTH/2, y, text, strlen(NUMBERS));

  // QWERTY
  text = LETTERS;
  y += KEY_HEIGHT;
  createRow(x, y, text, ROW1_LENGTH);

  // ASDFG
  x += KEY_WIDTH/2;
  y += KEY_HEIGHT;
  text += ROW1_LENGTH;
  createRow(x, y, text, ROW2_LENGTH);

  // ZXCVB
  x += KEY_WIDTH/2;
  y += KEY_HEIGHT;
  text += ROW2_LENGTH;
  createRow(x, y, text, ROW3_LENGTH);


  // special, end of chars. [] and /"
  x += KEY_WIDTH/2;
  y += KEY_HEIGHT;
  text += ROW3_LENGTH;
  createRow(x, y, text, 2);
  x += (KEY_WIDTH)*(ROW3_LENGTH-3);
  text += 2;
  createRow(x, y, text, 2);

  // shift, capsLock, tab, enter, backspace, delete, altkeys, space
  // backspace - at the end of the Q-P row, 2 keys wide
  createSpecialKey(INITIAL_X+(ROW1_LENGTH)*(KEY_WIDTH), INITIAL_Y+KEY_HEIGHT,
      KEY_WIDTH*2, KEY_HEIGHT,
      BACKSPACE_STR,
      m_backspaceKey);

  // enter - at the end of the A-L row, 2.5 keys wide.
  createSpecialKey(INITIAL_X+(ROW2_LENGTH)*(KEY_WIDTH)+KEY_WIDTH/2, INITIAL_Y+(KEY_HEIGHT*2),
      KEY_WIDTH*5/2, KEY_HEIGHT,
      ENTER_STR,
      m_enterKey);

  // m_clearKey - at the end of the space and final row
  createSpecialKey(INITIAL_X+KEY_WIDTH/2+(KEY_WIDTH)*ROW3_LENGTH-1, INITIAL_Y+(KEY_HEIGHT*4),
      (KEY_WIDTH*3/2), KEY_HEIGHT,
      CLEAR_STR,
      m_clearKey);

  // spaceKey - in between the final 4 chars on the final row
  createSpecialKey(INITIAL_X+(KEY_WIDTH*3/2)+(KEY_WIDTH)*2-1, INITIAL_Y+(KEY_HEIGHT*4),
      (KEY_WIDTH)*5, KEY_HEIGHT,
      SPACE_STR,
      m_spaceKey);

  // ok key - floating after the keyboard
  createSpecialKey(INITIAL_X+KEY_WIDTH, INITIAL_Y+(KEY_HEIGHT*5)+KEY_HEIGHT/3,
      (KEY_WIDTH)*3, KEY_HEIGHT,
      T("ok"),
      m_ok);

  // cancel key - floating after the keyboard
  createSpecialKey(INITIAL_X+KEY_WIDTH*8, INITIAL_Y+(KEY_HEIGHT*5)+KEY_HEIGHT/3,
      (KEY_WIDTH)*3, KEY_HEIGHT,
      T("cancel"),
      m_cancel);

  // caps - at the start of the a-l row. 1.5 keys wide
  createSpecialKey(INITIAL_X-KEY_WIDTH, INITIAL_Y+(KEY_HEIGHT*2),
      (KEY_WIDTH*3/2), KEY_HEIGHT,
      CAPS_STR,
      m_capsLockKey);

  // shift - at the start of the z-m row, 2 keys wide.
  createSpecialKey(INITIAL_X-KEY_WIDTH, INITIAL_Y+(KEY_HEIGHT*3),
      KEY_WIDTH*2-1, KEY_HEIGHT,
      SHIFT_STR,
      m_shiftKey);

  // m_extraKey
  createSpecialKey(INITIAL_X-(KEY_WIDTH/2)-2, INITIAL_Y+(KEY_HEIGHT*4),
      (KEY_WIDTH*2), KEY_HEIGHT,
      EXTRA_STR,
      m_extraKey);

  // By adding the m_scrollPane to the Component::m_children it gets deleted
  // in the destructor.
  add(m_scrollPane);
  add(m_richTextArea);
  setVisible(false);
  Stylus::instance()->registerListener(this);
}

Keyboard::~Keyboard()
{
  Stylus::instance()->unregisterListener(this);
}

void Keyboard::initUI()
{
  m_richTextArea->setCentred();
  m_richTextArea->setOutlined();
  m_textArea->setParentScroller(m_scrollPane);
  m_scrollPane->add(m_textArea);
  m_scrollPane->setTopLevel(false);
  m_scrollPane->setSize(nds::Canvas::instance().width(),SCROLLPANE_SIZE);
  m_scrollPane->setLocation(0, SCREEN_HEIGHT+SCROLLPANE_POS_Y);
  m_scrollPane->setScrollIncrement(m_textArea->font().height());
  m_scrollPane->setStretchChildren(true);
  m_scrollPane->setVisible(true);
}

void Keyboard::createSpecialKey(int x, int y, int w, int h, const std::string & text, Button * button)
{
  button->setSize(w, h);
  button->setLocation(x, y);
  button->setText(text);
  add(button);
  button->setListener(this);
}

void Keyboard::updateRow(const char * newText, int keys, int offset)
{
  // TODO: fix this to use utf-8
  const char *it(newText);
  const char *end(newText + strlen(newText));
  for (int i = 0; i < keys; ++i)
  {
    Button & key(*(Button*)m_children[i+offset]);
    std::string uchar;
    utf8::unchecked::append(utf8::next(it, end), back_inserter(uchar));
    key.setText(uchar);
  }
}

void Keyboard::createRow(int x, int y, const char * text, int keys)
{
  for (int i = 0; i < keys; ++i)
  {
    Button * key = new Button();
    // TODO: fix this for utf-8
    char uchar[] = {text[i], 0};
    key->setSize(KEY_WIDTH, KEY_HEIGHT);
    key->setText(std::string(uchar));
    //key->setLocation(x+i*(KEY_WIDTH+1), y);
    key->setLocation(x+i*(KEY_WIDTH), y);
    add(key);
    key->setListener(this);
  }
}

std::string Keyboard::result() const
{
  std::string tmp;
  m_textArea->text(tmp);
  return tmp;
}

void Keyboard::applyResult()
{
  tick();
  m_topLevel->setVisible();
  m_topLevel->screenUp();
  m_topLevel->forceRedraw();
  this->setVisible(false);
  if (m_selectedStatus == OK)
  {
    std::string tmp;
    m_textArea->text(tmp);
    if (m_entry) {
      m_entry->setText(tmp);
    }
  }
}

void Keyboard::paint(const nds::Rectangle & clip)
{
  if (not dirty())
   return;
  m_dirty = false;
  if (visible())
  {
    nds::Canvas::instance().setClip(clip);
    nds::Canvas::instance().fillRectangle(m_richTextArea->bounds().left(), m_richTextArea->bounds().top(), clip.right(), clip.bottom(),
        nds::Color(31,31,31));
    // FIXME: this is a hack to get the edit area to repaint
    // really we should only redraw what is necessary each time
    // eg. clear once at the start only.
    m_scrollPane->forceRedraw();
    std::vector<Component*>::iterator it(m_children.begin());
    for (; it != m_children.end(); ++it)
    {
      Component * c(*it);
      c->paint(c->bounds());
    }
  }
}

void Keyboard::editText(TextEntryI * entry)
{
  m_topLevel->screenDown();
  m_topLevel->forceRedraw();
  this->setVisible();
  m_initialText.clear();
  entry->text(m_initialText);
  m_textArea->setEchoText(entry->echoText());
  m_textArea->setText("");
  m_textArea->appendText(m_initialText);
  m_entry = entry;
  layoutViewer();
}

Keyboard::SpecialKey Keyboard::buttonToSpecialKey(const ButtonI * button)
{
  if (button == m_shiftKey) return SPKY_SHIFT;
  if (button == m_capsLockKey) return SPKY_CAPS;
  if (button == m_enterKey) {
    if (multiLine()) {
      return SPKY_ENTER;
    }
    return SPKY_OK;
  }
  if (button == m_backspaceKey) return SPKY_BACKSPACE;
  if (button == m_spaceKey) return SPKY_SPACE;
  if (button == m_extraKey) return SPKY_EXTRA;
  if (button == m_ok) return SPKY_OK;
  if (button == m_cancel) return SPKY_CANCEL;
  if (button == m_clearKey) return SPKY_CLEAR;
  return SPKY_UNKNOWN;
}

void Keyboard::pressed(ButtonI * button)
{
  SpecialKey key = buttonToSpecialKey(button);
  switch (key)
  {
    case SPKY_SHIFT:
      m_shift = not m_shift;
      break;
    case SPKY_CAPS:
      m_capsLock = not m_capsLock;
      break;
    case SPKY_ENTER:
      appendText("\n");
      break;
    case SPKY_BACKSPACE:
      m_textArea->deleteChar();
      layoutViewer();
      break;
    case SPKY_SPACE:
      appendText(((Button*)button)->text());
      break;
    case SPKY_EXTRA:
      m_extra = not m_extra;
      break;
    case SPKY_CANCEL:
      m_selectedStatus = CANCEL;
      m_textArea->clearText();
      m_textArea->appendText(m_initialText);
      applyResult();
      break;
    case SPKY_OK:
      m_selectedStatus = OK;
      applyResult();
      break;
    case SPKY_CLEAR:
      m_textArea->clearText();
      layoutViewer();
      break;
    case SPKY_UNKNOWN:
      // anything else
      appendText(((Button*)button)->text());
      m_shift = false;
      m_extra = false;
      break;
  }
  updateModifierKeys();
}

void Keyboard::appendText(const std::string & text)
{
  m_textArea->appendText(text);
  layoutViewer();
}

void Keyboard::layoutViewer()
{
  // should scroll to caret line - only if it is not already visible
  // otherwise the scrolling is not very stable.
  if (not m_textArea->caretVisible())
  {
    int scrollTo(m_textArea->caretLine() * m_textArea->font().height() * 256 / m_textArea->preferredSize().h);
    m_scrollPane->scrollToPercent(scrollTo);
  }
}

void Keyboard::updateModifierKeys()
{
  m_extraKey->setSelected(m_extra);
  m_shiftKey->setSelected(m_shift);
  m_capsLockKey->setSelected(m_capsLock);

  bool caps = (m_shift and not m_capsLock) or (m_capsLock and not m_shift);
  const char * text;
  const char * numbers;
  if (m_extra)
  {
    numbers = NUMBERS;
    if (caps)
    {
      text = EXTRA_SHIFT;
    }
    else
    {
      text = EXTRA;
    }
  }
  else
  {
    numbers = NUMBERS;
    if (caps)
    {
      text = LETTERS_SHIFT;
    }
    else
    {
      text = LETTERS;
    }
    if (m_shift)
    {
      numbers = NUMBERS_SHIFT;
    }
  }

  updateLayout(text, numbers);
}

void Keyboard::updateLayout(const char * text, const char * numbers)
{
  int start = strlen(NUMBERS);
  updateRow(numbers, start, 0);
  int size = ROW1_LENGTH + ROW2_LENGTH + ROW3_LENGTH;
  updateRow(text, size, start);
  // now the odd keys
  // special, end of chars. [] and /"
  utf8::unchecked::advance(text, size);
  start += size;
  updateRow(text, ROW4_LENGTH, start);

}

bool Keyboard::stylusUp(const Stylus * stylus)
{
  if (not visible())
    return false;
  m_dirty = true;
  return FOR_EACH_CHILD(stylusUp);
}

bool Keyboard::stylusDownFirst(const Stylus * stylus)
{
  if (not visible())
    return false;
  m_dirty = true;
  return FOR_EACH_CHILD(stylusDownFirst);
}
bool Keyboard::stylusDownRepeat(const Stylus * stylus)
{
  if (not visible())
    return false;
  m_dirty = true;
  return FOR_EACH_CHILD(stylusDownRepeat);
}
bool Keyboard::stylusDown(const Stylus * stylus)
{
  if (not visible())
    return false;
  return FOR_EACH_CHILD(stylusDown);
}

void Keyboard::setTopLevel(ScrollPane * topLevel)
{
  m_topLevel = topLevel;
}

bool Keyboard::tick()
{
  if (not visible())
  {
    return false;
  }

#ifndef ARM9
  static int pressed = 0;
  char sdlKeyPress[2] = { keysRealKeyboard(), 0};
  if ((not pressed) and sdlKeyPress[0])
  {
    pressed = 30;
    appendText(std::string(sdlKeyPress));
    m_dirty = true;
    return true;
  }
  if (pressed)
    pressed--;
#endif
  return m_dirty;
}

bool Keyboard::multiLine() const
{
  return m_entry->isMultiLine();
}

void Keyboard::setTitle(const std::string & title)
{
  m_richTextArea->clearText();
  m_richTextArea->appendText(title);
  m_richTextArea->setLocation(0, SCREEN_HEIGHT -  m_richTextArea->preferredSize().h-GAP);
  m_richTextArea->setSize(nds::Canvas::instance().width()-1,m_richTextArea->preferredSize().h);
  m_dirty = true;
}

void Keyboard::forceRedraw()
{
  m_dirty = true;
  m_scrollPane->forceRedraw();
}
