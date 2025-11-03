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
#ifndef TextContainer_h_seen
#define TextContainer_h_seen

#include "Component.h"
#include <string>

class TextArea;
/** A container that prevents text from overflowing its boundary. */
class TextContainer: public Component
{
  public:

    /** The width of the decoration used. */
    static const int BORDER_WIDTH;
    /** The height of the decoration used. */
    static const int BORDER_HEIGHT;

    /** Create an empty container.*/
    TextContainer();

    /** Create a container with the given text.
     * @param text the text to display
     */
    TextContainer(const std::string &text);

    /** Set the text contents.
     * @param text the new text contents.
     */
    void setText(const std::string &text);

    /** Get the text contents. Note that not all of the text may have been
     * displayed.
     * @return the current text.
     */
    const std::string & text() const;

    /** Set the background color.
     * @param color the color to use.
     */
    void setBackgroundColor(unsigned short color);

    // Reimplemented from Component.
    virtual void paint(const nds::Rectangle & clip);
    virtual void setSize(unsigned int w, unsigned int h);

  protected:
    // protected, since inheriting classes can get at m_children anyway.
    inline TextArea* textArea()
    {
      return (TextArea*)m_children.front();
    }

  private:
    std::string m_text;
    void layout();

    DISALLOW_COPY_AND_ASSIGN(TextContainer);
};
#endif
