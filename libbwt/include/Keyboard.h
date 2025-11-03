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
#ifndef Keyboard_h_seen
#define Keyboard_h_seen

#include "Component.h"
#include "ButtonListener.h"
#include "TextListener.h"

class Button;
class EditableTextArea;
class RichTextArea;
class ScrollPane;
class Stylus;
/** Show a virtual keyboard. Accepts touch input and keypad input. */
class Keyboard : public Component, public TextListener, public ButtonListener
{
  public:

    /** Construct a keyboard.  */
    Keyboard();

    ~Keyboard();

    /** Call this each frame to handle keyboard input.*/
    //void handleInput();

    /** Get the result of the last string entered (after Enter is "pressed").
     * @return the last string entered.
     */
    std::string result() const;

    /** Set the Component that paints the top level.
     * @param topLevel the top level component that will be hidden when the
     * keyboards is shown.
     */
    void setTopLevel(ScrollPane * topLevel);

    /** Update timer to deactivate current key selection.
     * @return true if the activation means a repaint is required.
     */
    bool tick();


    // Interface implementation.
    virtual bool stylusUp(const Stylus * stylus);
    virtual bool stylusDownFirst(const Stylus * stylus);
    virtual bool stylusDownRepeat(const Stylus * stylus);
    virtual bool stylusDown(const Stylus * stylus);

    virtual void paint(const nds::Rectangle & clip);
    virtual void editText(TextEntryI * entry);
    virtual void pressed(ButtonI * button);

    enum KeyboardSelection
    {
      CANCEL,
      OK
    };

    inline KeyboardSelection selected() const
    {
      return m_selectedStatus;
    }

    void setTitle(const std::string & title);

    void forceRedraw();

  private:
    bool m_extra;
    bool m_shift;
    bool m_capsLock;
    KeyboardSelection m_selectedStatus;

    ScrollPane * m_scrollPane;
    EditableTextArea * m_textArea;
    RichTextArea * m_richTextArea;
    Button * m_shiftKey;
    Button * m_capsLockKey;
    Button * m_enterKey;
    Button * m_backspaceKey;
    Button * m_spaceKey;
    Button * m_extraKey;
    Button * m_ok;
    Button * m_cancel;
    Button * m_clearKey;

    enum SpecialKey
    {
      SPKY_SHIFT,
      SPKY_CAPS,
      SPKY_ENTER,
      SPKY_BACKSPACE,
      SPKY_SPACE,
      SPKY_EXTRA,
      SPKY_OK,
      SPKY_CANCEL,
      SPKY_CLEAR,
      SPKY_UNKNOWN
    };
    SpecialKey buttonToSpecialKey(const ButtonI * button);

    std::string m_initialText;
    ScrollPane * m_topLevel;
    TextEntryI * m_entry;

    void initUI();

    void createRow(int x, int y, const char * text, int keys);
    void createSpecialKey(int x, int y, int w, int h,
                          const std::string & text, Button * button);
    void updateRow(const char * newText, int keys, int offset);
    void updateModifierKeys();
    void updateLayout(const char * text, const char * numbers);
    void appendText(const std::string & text);
    void layoutViewer();
    void applyResult();

    /** Get the current multi line state.
     * @return true if multi line entries are allowed, false otherwise.
     */
    inline bool multiLine() const;

    DISALLOW_COPY_AND_ASSIGN(Keyboard);
};
#endif
