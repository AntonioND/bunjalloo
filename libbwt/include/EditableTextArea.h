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
#ifndef EditableTextArea_h_seen
#define EditableTextArea_h_seen

#include "TextArea.h"
#include "TextEntryI.h"

class ScrollPane;

/** An EditableTextArea includes a cursor, handles user input and allows random
 * text insertion at any position.
 */
class EditableTextArea: public TextArea, public TextEntryI
{
  public:
    /** Create a new instance.
     * @param font the font to be used.
     */
    EditableTextArea(Font * font);

    /** Reimplemented (shadowed) from TextArea::appendText()
     * @param unicodeString the string to append to the text
     */
    void appendText(const std::string &unicodeString);
    /** Delete a character at the current caret position.*/
    void deleteChar();
    /** Clear all text, reset caret. */
    void clearText();

    /** Get the current caret line number.
     * @return the line number where the caret is located.
     */
    int caretLine() const;
    /** Set the parent ScrollPane that holds this EditableTextArea.
     * @param scrollPane the parent
     */
    void setParentScroller(ScrollPane * scrollPane);

    /** Check if the caret is currently visible.
     * @return true if visible, false otherwise.
     */
    bool caretVisible() const;

    inline void setEchoText(bool echo=true);
    inline bool echoText() const;

    // interface implementation
    virtual void text(std::string &returnString) const;
    virtual void setText(const std::string &text);
    virtual void paint(const nds::Rectangle & clip);
    virtual bool isMultiLine() const;

    virtual bool stylusUp(const Stylus * stylus);
    virtual bool stylusDownFirst(const Stylus * stylus);
    virtual bool stylusDownRepeat(const Stylus * stylus);
    virtual bool stylusDown(const Stylus * stylus);
  protected:
    /** Overloaded from TextArea. If echo is off, shows * instead of text.*/
    virtual void printu(const std::string &unicodeString);

  private:
    int m_caretLine;
    // the character in the current line (not the byte)
    int m_caretChar;
    int m_caretPixelX;
    bool m_appendedNewLine;
    bool m_echoText;
//    bool m_layingOut;
    ScrollPane * m_scrollPane;

    void setCaret(int x, int y);
    void recalculateCaret();
    void resizeParent();

    bool inBounds(int x, int y);
    DISALLOW_COPY_AND_ASSIGN(EditableTextArea);
};
void EditableTextArea::setEchoText(bool echo)
{
  m_echoText = echo;
}
bool EditableTextArea::echoText() const
{
  return m_echoText;
}
#endif
