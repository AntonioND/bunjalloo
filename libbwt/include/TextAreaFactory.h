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
#ifndef TextAreaFactory_h_seen
#define TextAreaFactory_h_seen

#include <string>

class TextArea;
class Font;
/** A factory that generates TextArea instances. */
class TextAreaFactory
{

  public:
    enum TextType
    {
      TXT_NORMAL,
      TXT_EDIT,
      TXT_RICH
    };
    /** The user is responsible for calling delete on each TextArea created.
     * @return a new TextArea.
     */
    static TextArea * create(TextType type=TXT_NORMAL);

    /** Set the font to be used, must be called before calling create(),
     * @param font the font to use.
     */
    static void setFont(Font * font);

  private:
    static Font * s_font;

    TextAreaFactory();
    ~TextAreaFactory();
    TextAreaFactory(const TextAreaFactory &);
    const TextAreaFactory operator=(const TextAreaFactory &);

};
#endif
