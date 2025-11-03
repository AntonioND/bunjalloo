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
#ifndef HtmlDocument_h_seen
#define HtmlDocument_h_seen

#include "HtmlParser.h"
#include "ElementList.h"

class Bookmark;

/** Parse the HTML tokens after the tokenisation phase.
 */
class HtmlDocument : public HtmlParser
{

  public:

    //! Constructor.
    HtmlDocument();
    //! Destructor.
    ~HtmlDocument();

    /** Get the data contents.
     * @return reference to the data.
     */
    inline const std::string & data() const;

    /** Get the current amount of data retrieved.
     * @return amount of data retrieved.
     */
    inline unsigned int dataGot() const;
    /** Set the amount of data. Used for chunked content that does not set the content-length header.
     * @param value the new amount of data retrieved (0?)
     */
    inline void setDataGot(unsigned int value);

    /** Reset the internal state of the parser. */
    void reset();

    /** Get the root node of the document model.
     * @return The root node.
     */
    const HtmlElement * rootNode() const;

    void dumpDOM();
    void dumpAF();

    void handleEOF();
  protected:
    virtual void handleStartEndTag(const std::string & tag, const AttributeVector & attrs);
    virtual void handleStartTag(const std::string & tag, const AttributeVector & attrs);
    virtual void handleEndTag(const std::string & tag);
    virtual void handleData(unsigned int ucodeChar);
    virtual void handleBinaryData(const void * data, unsigned int length);

  private:


    enum TreeState
    {
      INITIAL,
      ROOT_ELEMENT,
      MAIN,
      MAIN_WAITING_TOKEN,
      TEXTAREA_WAITING_TOKEN,
      TRAILING_END
    };

    enum InsertionMode
    {
      BEFORE_HEAD,
      IN_HEAD,
      AFTER_HEAD,
      IN_BODY,
      IN_TABLE,
      IN_CAPTION,
      IN_COLUMN_GROUP,
      IN_TABLE_BODY,
      IN_ROW,
      IN_CELL,
      IN_SELECT,
      AFTER_BODY,
      IN_FRAMESET,
      AFTER_FRAMESET
    };
    std::string m_data;
    unsigned int m_dataGot;
    TreeState m_state;
    InsertionMode m_insertionMode;
    bool m_isFirst;
    typedef std::vector<HtmlElement*> ElementVector;
    ElementVector m_openElements;
    // things are added to the front of the list.
    ElementList m_activeFormatters;
    HtmlElement * m_head;
    HtmlElement * m_form;
    HtmlElement * m_currentNode;

    // end tag in main phase.
    void mainPhase(const std::string & tag);
    // start tag in main phase.
    void mainPhase(const std::string & tag, const AttributeVector & attrs);
    // unicode char in the mainPhase
    void mainPhase(unsigned int ucodeChar);

    // BEFORE_HEAD phase, start tag.
    void beforeHead(const std::string & tag, const AttributeVector & attrs);
    // end tag.
    void beforeHead(const std::string & tag);
    // IN_HEAD phase, start tag.
    void inHead(const std::string & tag, const AttributeVector & attrs);
    // end tag
    void inHead(const std::string & tag);
    // AFTER_HEAD phase, start tag.
    void afterHead(const std::string & tag, const AttributeVector & attrs);
    // end tag
    void afterHead(const std::string & tag);
    // IN_BODY phase, start tag.
    void inBody(const std::string & tag, const AttributeVector & attrs);
    // end tag
    void inBody(const std::string & tag);
    void inBody(unsigned int ucode);
    void eofInBody();
    // IN_SELECT phase, start tag.
    void inSelect(const std::string & tag, const AttributeVector & attrs);
    // end tag
    void inSelect(const std::string & tag);
    // AFTER_BODY phase, start tag.
    void afterBody(const std::string & tag, const AttributeVector & attrs);
    // end tag
    void afterBody(const std::string & tag);

    bool inScope(const std::string & element, bool inTableScope=false) const;
    HtmlElement* activeFormatContains(const std::string & tagName);
    void removeFromActiveFormat(HtmlElement* element);
    void removeFromOpenElements(HtmlElement* element);

    void insertElement(HtmlElement * element);
    void addActiveFormatter(HtmlElement * element);

    inline HtmlElement * currentNode() const;
    void generateImpliedEndTags(const std::string & except="");
    bool isFormatting(HtmlElement * node);
    bool isPhrasing(HtmlElement * node);
    void reconstructActiveFormatters();

    void createBookmark(Bookmark & marker, ElementList::iterator & bookmarkIt) const;
    void adoptionAgency(const std::string & tag);
    void startScopeClosedElement(const std::string & tag, const std::string & alternate="");

    bool headerInScope() const;
    void appendTextToCurrentNode(unsigned int ucodeChar);
    // disable copies
    HtmlDocument (const HtmlDocument&);
    const HtmlDocument& operator=(const HtmlDocument&);
    // debug
    int m_depth;
    void walkNode(const HtmlElement * node);
};


// inline implementation
const std::string & HtmlDocument::data() const
{
  return m_data;
}

unsigned int HtmlDocument::dataGot() const
{
  return m_dataGot;
}
void HtmlDocument::setDataGot(unsigned int value)
{
  m_dataGot = value;
}

inline HtmlElement * HtmlDocument::currentNode() const
{
  return m_openElements.back();
}
#endif
