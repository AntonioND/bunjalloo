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
#ifndef TextArea_h_seen
#define TextArea_h_seen

#include "Font.h"
#include "Component.h"
#include <list>

/** A widget for displaying text.*/
class TextArea : public Component
{
  public:
    /** Sets the initial state of the TextArea.
     * @param font the font to use. This is not deleted.
     */
    TextArea(Font * font);

    /** Destructor. Deletes the palette data. */
    ~TextArea();

    /** Append text to the text area.
     * @param unicodeString the text to append.
     */
    void appendText(const std::string &unicodeString);

    /** Clear the text (empty the document model).
     */
    void clearText();

    /** Get the text that is held internally.
     * @param returnString the return value.
     */
    void document(std::string & returnString) const;

    /** Get the current font.
     * @return the current font.
     */
    inline const Font & font() const;

    /** Set the font to use. TextArea takes control of the memory and deletes
     * on end/font reset.
     * @param font the font to use from now on.
     */
    void setFont(Font * font);

    /** Set the text colour. Since the text is anti aliased, this creates a coloured
     * scale palette based on the original grey-scale palette.
     * @param color the new text colour in 15 bit RGB format.
     */
    void setTextColor(unsigned short color);

    /** Set the background colour. Since the text is anti aliased, this creates a coloured
     * scale palette based on the original grey-scale palette.
     * @param color the new text colour in 15 bit RGB format.
     */
    void setBackgroundColor(unsigned short color);

    /** Return the text colour to the default, initial value. */
    void setDefaultColor();

    unsigned short backgroundColor() const;
    unsigned short foregroundColor() const;

    /** Get the size of the text in pixels using the current font.
     * @param unicodeString the string to check the size of.
     * @return the size of the string in pixels.
     */
    int textSize(const std::string & unicodeString) const;

    size_t characters(size_t line) const;

    /** A hack for debugging.
     * @return the current document converted to a char string.
     */
    std::string asString() const;

    inline int cursorY() const;
    inline int cursorX() const;

    inline bool parseNewline() const;
    inline void setParseNewline(bool parse=true);

    // Component method reimplementation.
    virtual void setSize(unsigned int w, unsigned int h);
    virtual void paint(const nds::Rectangle & clip);

    virtual bool stylusUp(const Stylus * stylus) { return false; }
    virtual bool stylusDownFirst(const Stylus * stylus) { return false; }
    virtual bool stylusDownRepeat(const Stylus * stylus) { return false; }
    virtual bool stylusDown(const Stylus * stylus) { return false; }

  protected:
    /** The document model contains one std::string per line of text.*/
    std::vector<std::string> m_document;

    /** Perform layout of the text. This is a pretty costly procedure in terms
     * of memory and CPU.
     */
    void layoutText();

    /** Get the current (last) line of text.
     * @return the last line of text in the document vector.
     */
    inline std::string & currentLine();

    /** Called for each line of text that is visible. */
    virtual void printu(const std::string & unicodeString);

    bool doSingleChar(int value);

  protected:
    //! Position that the current line is at.
    int m_cursorx;
    int m_cursory;
    int m_initialCursorx;

    const std::string nextWord(const std::string & unicodeString,
        int currPosition) const;

    virtual void incrLine();
    /** Set the cursor position. This is where the text will be "drawn" the
     * next time a print routine is called.
     * @param x the cursor x position in pixels.
     * @param y the cursor y position in pixels.
     */
    void setCursor(int x, int y);

  private:
    Font * m_font;
    int m_appendPosition;
    unsigned int m_preferredWidthFixed;
    bool m_parseNewline;
    unsigned short m_bgCol;
    unsigned short m_fgCol;

    DISALLOW_COPY_AND_ASSIGN(TextArea);
};

// inline implementations
const Font & TextArea::font() const {
  return *m_font;
}

std::string & TextArea::currentLine()
{
  if (m_document.empty())
    m_document.push_back(std::string());
  return m_document.back();
}
int TextArea::cursorY() const
{
  return m_cursory;
}
int TextArea::cursorX() const
{
  return m_cursory;
}
bool TextArea::parseNewline() const
{
    return m_parseNewline;
}
void TextArea::setParseNewline(bool parse)
{
    m_parseNewline = parse;
}
#endif
