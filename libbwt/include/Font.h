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
#ifndef Font_h_seen
#define Font_h_seen

#include <string>
#include <stdint.h>
#include <vector>
#include "util/classhelper.h"

class Range;
struct t_prerenderedSet;
struct t_charMap;
struct t_prerenderedGlyph;

/** Encapsulates the Font handling. */
class Font
{
  public:

    /** Construct a font. You should call init() afterwards. @see FontFactory
     */
    Font();

    //! Destructor.
    ~Font();

    /** Get the size of a given string of text.
     * @param text encoded text data.
     * @param textLength the number of bytes in the text.
     * @param width is filled with the width.
     * @param height is filled with the height.
     * @param encoding the type of encoding used - utf-8 or iso_8859-1.
     */
    void textSize(const char * text,
        int textLength,
        int & width,
        int & height,
        const std::string & encoding="utf-8") const;

    /** Find the end of the string. Common side effects/return values
     * size of actual string (only if size is a limitation)
     * index of the last character (only if size is a limitation)
     * @param text the text value to check
     * @param maxSize the maximum width in pixels that we want to go to
     * @param maxLastChar the maximum number of characters that we want to span
     * @param sizeOut the size of the string in pixels
     * @param lastCharOut the last character index (character != byte)
     * @returns true if we reached the end of the string
     */
     bool findEnd(
        const std::string &text,
        unsigned int maxSize,
        unsigned int maxLastChar,
        unsigned int *sizeOut,
        unsigned int *lastCharOut) const;

    /** Get the total width of all the font glyphs. This is the width of an
     * image that contains all available fonts.
     * @return the total width of the font glyphs.
     */
    inline int totalWidth() const;

    /** Get the font height. This is fixed and is the distance from the max
     * descending character to the min ascending one.
     * @return font height.
     */
    int height() const;

    /** Get the font base line, where y rests on the "line".
     * @return font height.
     */
    int base() const;

    /** Get a shorter word that will fit in a given width.
     * @param it start of the word
     * @param end_it the end of the word
     * @param width the maximum size, in pixels, that can be occuppied
     * @param size the size of the string (fixed point?)
     * @returns the shorter string, the iterator that points to the end of that
     *          string, and the size in fixed point pixels of the string
     */
    std::string shorterWordFromLong(
      std::string::const_iterator *it,
      const std::string::const_iterator &end_it,
      int width,
      int *size) const;

    /** Print a single character.
     * @param value the value to paint
     * @param x the x position in 24.8 fixed point
     * @param y the y position in 24.8 fixed point
     * @param right edge of the area in 24.8 fixed point
     * @param color the color of the glyph
     * @return -1 if a new line, else the size of the value advance.
     */
    int doSingleChar(unsigned int value, int x, int y, int right, int color, int bgcolor);

    /** Initialise the font.
     * @param imageData the glyph image data
     * @param mapData the glyph index to image data map
     */
    void init(const unsigned char * imageData, const unsigned char * mapData);

  private:

    //! Total width of the image.
    unsigned int m_width;
    t_prerenderedSet *m_prerenderedSet;
    t_charMap *m_charMap;

    /** For a given character code, get the equivalent glyph data.
     * @param charCode the character code
     * @returns a pointer to the glyph data
     */
    t_prerenderedGlyph *glyph(unsigned int charCode) const;
    int valueToIndex(unsigned int codepoint) const;
    int minGlyph() const;
    void printAt(t_prerenderedGlyph &g, int xPosition, int yPosition, int color, int bg);

    DISALLOW_COPY_AND_ASSIGN(Font);
};

inline int Font::totalWidth() const
{
  return m_width;
}
#endif
