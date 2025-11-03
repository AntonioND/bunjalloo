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
#ifndef TextField_h_seen
#define TextField_h_seen
#include "TextContainer.h"
#include "TextEntryI.h"

/** An entry that displays a single line of text.*/
class TextField: public TextContainer, public TextEntryI
{
  public:

    /** Construct a new TextField.
     * @param text the initial text to show.
     */
    TextField(const std::string & text);

    // from TextEntryI
    virtual void text(std::string &returnString) const
    {
      returnString = TextContainer::text();
    }
    virtual void setText(const std::string &text)
    {
      TextContainer::setText(text);
    }
    virtual bool isMultiLine() const
    {
      return false;
    }

    // From Component.
    virtual void paint(const nds::Rectangle & clip);
    virtual bool stylusUp(const Stylus * stylus);
    virtual bool stylusDownFirst(const Stylus * stylus);
    virtual bool stylusDownRepeat(const Stylus * stylus);
    virtual bool stylusDown(const Stylus * stylus);
  private:
    bool m_touched;
    DISALLOW_COPY_AND_ASSIGN(TextField);
};
#endif
