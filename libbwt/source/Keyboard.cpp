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

Keyboard::Keyboard():
  m_textArea((EditableTextArea*)TextAreaFactory::create(TextAreaFactory::TXT_EDIT)),
  m_richTextArea((RichTextArea*)TextAreaFactory::create(TextAreaFactory::TXT_RICH))
{
  if (m_textArea == NULL or m_richTextArea == NULL)
    libndsCrash("Keyboard: OOM");

  initUI();

  int x = INITIAL_X;
  int y = INITIAL_Y;

  createRow(x-KEY_WIDTH/2, y, NUMBERS, &m_numberKeys[0], ROW_NUM_LENGTH);

  {
    const char * text = LETTERS;
    Button * buttons = &m_rowKeys[0];

    // QWERTY
    y += KEY_HEIGHT;
    createRow(x, y, text, buttons, ROW_1_LENGTH);

    // ASDFG
    x += KEY_WIDTH/2;
    y += KEY_HEIGHT;
    text += ROW_1_LENGTH;
    buttons += ROW_1_LENGTH;
    createRow(x, y, text, buttons, ROW_2_LENGTH);

    // ZXCVB
    x += KEY_WIDTH/2;
    y += KEY_HEIGHT;
    text += ROW_2_LENGTH;
    buttons += ROW_2_LENGTH;
    createRow(x, y, text, buttons, ROW_3_LENGTH);

    // special, end of chars. [] and /"
    x += KEY_WIDTH/2;
    y += KEY_HEIGHT;
    text += ROW_3_LENGTH;
    buttons += ROW_3_LENGTH;
    createRow(x, y, text, buttons, 2); // ROW_4_LENGTH
    x += (KEY_WIDTH)*(ROW_3_LENGTH-3);
    text += 2;
    buttons += 2;
    createRow(x, y, text, buttons, 2);
  }

  // shift, capsLock, tab, enter, backspace, delete, altkeys, space
  // backspace - at the end of the Q-P row, 2 keys wide
  createSpecialKey(INITIAL_X+(ROW_1_LENGTH)*(KEY_WIDTH), INITIAL_Y+KEY_HEIGHT,
      KEY_WIDTH*2, KEY_HEIGHT,
      BACKSPACE_STR,
      &m_backspaceKey);

  // enter - at the end of the A-L row, 2.5 keys wide.
  createSpecialKey(INITIAL_X+(ROW_2_LENGTH)*(KEY_WIDTH)+KEY_WIDTH/2, INITIAL_Y+(KEY_HEIGHT*2),
      KEY_WIDTH*5/2, KEY_HEIGHT,
      ENTER_STR,
      &m_enterKey);

  // m_clearKey - at the end of the space and final row
  createSpecialKey(INITIAL_X+KEY_WIDTH/2+(KEY_WIDTH)*ROW_3_LENGTH-1, INITIAL_Y+(KEY_HEIGHT*4),
      (KEY_WIDTH*3/2), KEY_HEIGHT,
      CLEAR_STR,
      &m_clearKey);

  // spaceKey - in between the final 4 chars on the final row
  createSpecialKey(INITIAL_X+(KEY_WIDTH*3/2)+(KEY_WIDTH)*2-1, INITIAL_Y+(KEY_HEIGHT*4),
      (KEY_WIDTH)*5, KEY_HEIGHT,
      SPACE_STR,
      &m_spaceKey);

  // ok key - floating after the keyboard
  createSpecialKey(INITIAL_X+KEY_WIDTH, INITIAL_Y+(KEY_HEIGHT*5)+KEY_HEIGHT/3,
      (KEY_WIDTH)*3, KEY_HEIGHT,
      T("ok"), // Get translation
      &m_ok);

  // cancel key - floating after the keyboard
  createSpecialKey(INITIAL_X+KEY_WIDTH*8, INITIAL_Y+(KEY_HEIGHT*5)+KEY_HEIGHT/3,
      (KEY_WIDTH)*3, KEY_HEIGHT,
      T("cancel"), // Get translation
      &m_cancel);

  // caps - at the start of the a-l row. 1.5 keys wide
  createSpecialKey(INITIAL_X-KEY_WIDTH, INITIAL_Y+(KEY_HEIGHT*2),
      (KEY_WIDTH*3/2), KEY_HEIGHT,
      CAPS_STR,
      &m_capsLockKey);

  // shift - at the start of the z-m row, 2 keys wide.
  createSpecialKey(INITIAL_X-KEY_WIDTH, INITIAL_Y+(KEY_HEIGHT*3),
      KEY_WIDTH*2-1, KEY_HEIGHT,
      SHIFT_STR,
      &m_shiftKey);

  // m_extraKey
  createSpecialKey(INITIAL_X-(KEY_WIDTH/2)-2, INITIAL_Y+(KEY_HEIGHT*4),
      (KEY_WIDTH*2), KEY_HEIGHT,
      EXTRA_STR,
      &m_extraKey);

  add(&m_scrollPane, false);
  add(m_richTextArea, false);
  setVisible(false);
  Stylus::instance()->registerListener(this);
}

Keyboard::~Keyboard()
{
  Stylus::instance()->unregisterListener(this);

  delete m_textArea;
  delete m_richTextArea;
}

void Keyboard::initUI()
{
  m_richTextArea->setCentred();
  m_richTextArea->setOutlined();
  m_textArea->setParentScroller(&m_scrollPane);
  m_scrollPane.add(m_textArea, false);
  m_scrollPane.setTopLevel(false);
  m_scrollPane.setSize(nds::Canvas::instance().width(),SCROLLPANE_SIZE);
  m_scrollPane.setLocation(0, SCREEN_HEIGHT+SCROLLPANE_POS_Y);
  m_scrollPane.setScrollIncrement(m_textArea->font().height());
  m_scrollPane.setStretchChildren(true);
  m_scrollPane.setVisible(true);
}

void Keyboard::createSpecialKey(int x, int y, int w, int h, const std::string & text, Button * button)
{
  button->setSize(w, h);
  button->setLocation(x, y);
  button->setText(text);
  add(button, false);
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

void Keyboard::createRow(int x, int y, const char * text, Button * buttons, int keys)
{
  for (int i = 0; i < keys; ++i)
  {
    Button * key = &buttons[i];
    // TODO: fix this for utf-8
    char uchar[] = {text[i], 0};
    key->setSize(KEY_WIDTH, KEY_HEIGHT);
    key->setText(std::string(uchar));
    //key->setLocation(x+i*(KEY_WIDTH+1), y);
    key->setLocation(x+i*(KEY_WIDTH), y);
    add(key, false);
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
    m_scrollPane.forceRedraw();
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
  if (button == &m_shiftKey) return SPKY_SHIFT;
  if (button == &m_capsLockKey) return SPKY_CAPS;
  if (button == &m_enterKey) {
    if (multiLine()) {
      return SPKY_ENTER;
    }
    return SPKY_OK;
  }
  if (button == &m_backspaceKey) return SPKY_BACKSPACE;
  if (button == &m_spaceKey) return SPKY_SPACE;
  if (button == &m_extraKey) return SPKY_EXTRA;
  if (button == &m_ok) return SPKY_OK;
  if (button == &m_cancel) return SPKY_CANCEL;
  if (button == &m_clearKey) return SPKY_CLEAR;
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
    m_scrollPane.scrollToPercent(scrollTo);
  }
}

void Keyboard::updateModifierKeys()
{
  m_extraKey.setSelected(m_extra);
  m_shiftKey.setSelected(m_shift);
  m_capsLockKey.setSelected(m_capsLock);

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
  int start = ROW_NUM_LENGTH;
  updateRow(numbers, start, 0);
  int size = ROW_1_LENGTH + ROW_2_LENGTH + ROW_3_LENGTH;
  updateRow(text, size, start);
  // now the odd keys
  // special, end of chars. [] and /"
  utf8::unchecked::advance(text, size);
  start += size;
  updateRow(text, ROW_4_LENGTH, start);

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
  // This code is used by the SDL port so that you can use the keyboard to use
  // the keyboard of your PC instead of using the on-screen keyboard. Note that
  // this only works for regular symbols. Modifier keys, backspace, etc, don't
  // work.

  static int press_delay = 0;
  static int last_pressed = -1;

  int cur_pressed = keysRealKeyboard();

  if (cur_pressed)
  {
    if (cur_pressed != last_pressed)
    {
      last_pressed = cur_pressed;
      press_delay = 0;
    }

    if (press_delay == 0)
    {
      press_delay = 30;

      if (cur_pressed == '\b') // SDLK_BACKSPACE
      {
        m_textArea->deleteChar();
      }
      else if (cur_pressed == '\r') // SDLK_RETURN
      {
        m_selectedStatus = OK;
        applyResult();
      }
      else
      {
        char sdlKeyPress[2] = { char(cur_pressed), 0};
        appendText(std::string(sdlKeyPress));
      }

      m_dirty = true;
    }
    else
    {
      press_delay--;
    }
  }
  else
  {
    press_delay = 0;
    last_pressed = -1;
  }
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
  m_scrollPane.forceRedraw();
}
