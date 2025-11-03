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
#ifndef HtmlElement_h_seen
#define HtmlElement_h_seen

#include <string>
#include <cstring>
#include "ElementList.h"

class Visitor;

#define DEFINE_ACCEPT(klass) \
  void accept(Visitor & visitor);

#define IMPLEMENT_ACCEPT(klass) \
void klass::accept(Visitor & visitor)\
{\
  visitor.begin(*this);\
  if (visitor.visit(*this)) \
    visitChildren(visitor);\
  visitor.end(*this);\
}

class HtmlElement
{

  public:
    virtual DEFINE_ACCEPT();

    /** Constructor.
     * @param tagName the name of the tag for this element.
     */
    HtmlElement(const std::string & tagName)
      : m_tagName(tagName),
        m_parent(0),
        m_block(false)
    {}

    //! Destructor.
    virtual ~HtmlElement();

    /** Set an attribute value. Only the attributes valid for a particular element are stored.
     * @param name the attribute name
     * @param value the value
     */
    virtual void setAttribute(const std::string & name, const std::string & value);

    /** Get an attribute value.
     * @param name the attribute name.
     * @return the attribute value as a string.
     */
    virtual std::string attribute(const std::string & name) const;

    /** Append a child node.
     * @param child the node to append
     */
    void append(HtmlElement * child);

    /** Remove a previously appended child node.
     * @param child the node to remove.
     */
    void remove(HtmlElement * child);

    /** Get the child element that is the sibling immediately before "child".
     * May return null.
     * @param child the child node.
     * @return the child node immediately before child, or 0 if no child present.
     */
    const HtmlElement * previousSibling(const HtmlElement * child);

    /** Get the child element that is the sibling immediately afer "child".
     * May return null.
     * @param child the child node.
     * @return the child node immediately after child, or 0 if no child present.
     */
    const HtmlElement * nextSibling(const HtmlElement * child);

    /** Append a text character. If the last child node is not a text node,
     * then one is created and appended. The text is appended to the last child
     * node.
     * @param value a unicode text value to append.
     */
    virtual void appendText(unsigned int value);

    /** Get the has children status.
     * @return true if there are children, false otherwise.
     */
    inline bool hasChildren() const;

    /** Fetch the first child.
     * @return the first child of this node or null of no children.
     */
    inline HtmlElement * firstChild() const;

    /** Fetch the last child.
     * @return the last child of this node or null of no children.
     */
    inline HtmlElement * lastChild() const;

    /** Get the list of child elements.
     * @return the list of child elements. May be empty.
     */
    inline const ElementList & children() const;

    /** Get the list of child elements with the given tag name.
     * @param name the tag name to search for.
     * @return the list of child elements. May be empty.
     */
    ElementList elementsByTagName(const std::string & name) const;

    /** Get the parent element.
     * @return the parent element. May be null for top level element.
     */
    inline HtmlElement* parent() const;

    /** Set the parent of this element.
     * @param newParent the new parent element.
     */
    inline void setParent(HtmlElement * newParent);

    /** Is this element a particular type?.
     * @param name the name to use for matching.
     * @return true if the tag name matches the given name.
     */
    inline bool isa(const std::string & name) const;
    inline bool isa(const char *name) const;

    /** Is this element a particular type?. This ptr variation is for use as a
     * matching function in find_if. @see isa() for the preferred version.
     * @param name a pointer to the name to use for matching.
     * @return true if the tag name matches the given name.
     */
    inline bool isa_ptr(const std::string * name) const;

    /** Get the tag name.
     * @return the tag name
     */
    inline const std::string & tagName() const;

    /** Get the text value.
     * @return the unicode text.
     */
    inline const std::string & text() const;

    /** Get the text value.
     * @return the unicode text.
     */
    inline std::string & text();

    /** Make a shallow copy. Copies only the attributes, not the children.
     * @return the cloned element.
     */
    virtual HtmlElement * clone() const;

    void removeAllChildren();

    inline bool isBlock() const;
    inline void setBlock(bool isblock=true);

    void clearText() const;

    //void dump() const;
  protected:
    std::string m_tagName;
    std::string m_id;
    std::string m_title;
    std::string m_lang;
    std::string m_dir;
    mutable std::string m_text;
    // not implemented
    //std::string m_className;
    // std::vector<std::string> m_classList;
    HtmlElement * m_parent;
    ElementList m_children;
    bool m_block;

    virtual const std::string * attributePtr(const std::string & name) const;
    virtual void copyAttributes(HtmlElement * copyTo) const;
    void visitChildren(Visitor & visitor);
};

bool HtmlElement::hasChildren() const
{
  return not m_children.empty();
}
bool HtmlElement::isa(const std::string & name) const
{
  return m_tagName == name;
}

bool HtmlElement::isa(const char *name) const
{
  return strcmp(name, m_tagName.c_str()) == 0;
}

bool HtmlElement::isa_ptr(const std::string * name) const
{
  return name and isa(*name);
}
HtmlElement * HtmlElement::firstChild() const
{
  if (hasChildren())
  {
    return m_children.front();
  }
  return 0;
}
HtmlElement * HtmlElement::lastChild() const
{
  if (hasChildren())
  {
    return m_children.back();
  }
  return 0;
}
const ElementList & HtmlElement::children() const
{
  return m_children;
}

const std::string & HtmlElement::tagName() const
{
  return m_tagName;
}
const std::string & HtmlElement::text() const
{
  return m_text;
}
std::string & HtmlElement::text()
{
  return m_text;
}

HtmlElement* HtmlElement::parent() const
{
  return m_parent;
}
void HtmlElement::setParent(HtmlElement * newParent)
{
  m_parent = newParent;
}
bool HtmlElement::isBlock() const
{
  return m_block;
}
void HtmlElement::setBlock(bool isblock)
{
  m_block = isblock;
}
#endif
