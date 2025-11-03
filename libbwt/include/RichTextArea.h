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
#ifndef RichTextArea_h_seen
#define RichTextArea_h_seen

#include <map>
#include "TextArea.h"
#include "Link.h"

class LinkListener;
/** Class that adds clickable Links, etc, to a standard TextArea. */
class RichTextArea: public TextArea
{
  public:
    RichTextArea(Font * font);
    ~RichTextArea();

    /** Overridden from TextArea. */
    virtual void appendText(const std::string &unicodeString);

    /** Add a Link to the text.
     * @param href the document address to link to.
     * @param visited true if the link has been visited already.
     */
    void addLink(const std::string & href, bool visited=false);

    /** End the Link. */
    void endLink();

    void addImage(const std::string & src);

    /** End the image section.*/
    void endImage();

    /** Add a listener for when a Link is clicked.
     */
    void addLinkListener(LinkListener * linkListener);

    /** Insert a new line - usefull for when we don't parse new lines
     */
    void insertNewline();

    bool centred() const;
    void setCentred(bool centre=true);
    bool outlined() const;
    void setOutlined(bool outline=true);
    void setUnderline(bool underline=true);
    bool underline() const;

    /** Given a link index, find where abouts in the text area it is.
     */
    int linkPosition(int linkIndex) const;

    /** @return the number of links in total. */
    unsigned int linkCount() const;

    virtual void paint(const nds::Rectangle & clip);

    virtual bool stylusUp(const Stylus * stylus);
    virtual bool stylusDownFirst(const Stylus * stylus);
    virtual bool stylusDownRepeat(const Stylus * stylus);
    virtual bool stylusDown(const Stylus * stylus);

  protected:
    /** Overloaded from TextArea. This checks the current char vs the links to
     * see if the current character is a link or not.*/
    virtual void printu(const std::string &unicodeString);

    virtual void incrLine();

  private:
    /** Keep track of the current document size. */
    unsigned int m_documentSize;
    unsigned int m_nextEvent;
    Link::EventType m_nextEventType;
    unsigned int m_paintPosition;
    Link::EventType m_state;
    typedef std::list<Link*> LinkList;
    LinkList m_links;
    // for painting:
    LinkList::const_iterator m_currentLink;
    LinkListener * m_linkListener;
    // current line number being painted
    int m_lineNumber;

    bool m_centred;
    bool m_outlined;
    int m_startUnderlineX;
    int m_startUnderlineY;
    bool m_underLine;

    Link * m_linkTouched;
    int m_downCount;

    /** Delete the links */
    void removeClickables();
    /** get the size in characters.
     * @param endLine the last line to use - pass NO_INDEX to use whole doc.
     */
    unsigned int documentSize(int endLine) const;

    /** Get the total character count. Uses m_documentSize internally,
     * recalculating only when needed.*/
    unsigned int totalCharacters();

    void handleNextEvent();
    void checkSkippedLines(int skipLines);
    int pointToCharIndex(int x, int y) const;

    // get the document line at the clicked y position
    int lineAt(int y) const;

    Link * linkAt(int index);

    unsigned int charIndexToLine(unsigned int charIndex) const;
    void startUnderline();
    void endUnderline();
    DISALLOW_COPY_AND_ASSIGN(RichTextArea);
};
#endif
