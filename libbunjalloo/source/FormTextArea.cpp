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
#include "Canvas.h"
#include "EditableTextArea.h"
#include "ElementFactory.h"
#include "FormTextArea.h"
#include "HtmlConstants.h"
#include "HtmlElement.h"
#include "HtmlTextAreaElement.h"
#include "TextAreaFactory.h"

FormTextArea::FormTextArea(HtmlElement * element) :
  m_element(element)
{
  EditableTextArea * text( (EditableTextArea*)TextAreaFactory::create(TextAreaFactory::TXT_EDIT));
  setStretchChildren();
  add(text);
  int fontSize(text->font().height());
  text->setText(textNode()->text());
  HtmlTextAreaElement * textElement((HtmlTextAreaElement*)m_element);
  m_preferredHeight = fontSize * std::max(3, textElement->rows());
  m_preferredWidth = fontSize * std::max(6, textElement->cols());
  setSize(m_preferredWidth, m_preferredHeight);
  text->setSize(m_preferredWidth, m_preferredHeight);
  text->setListener(this);
  text->setParentScroller(this);
  layoutViewer();
}

void FormTextArea::layoutViewer()
{
  int scrollTo(textArea()->caretLine() * textArea()->font().height() * 256 / textArea()->preferredSize().h);
  scrollToPercent(scrollTo);
}

void FormTextArea::editText(TextEntryI * entry)
{
  if (listener())
    listener()->editText(this);
}

inline EditableTextArea * FormTextArea::textArea()
{
  return (EditableTextArea*)m_children.back();
}

inline EditableTextArea * FormTextArea::textArea() const
{
  return (EditableTextArea*)m_children.back();
}

bool FormTextArea::isMultiLine() const
{
  return textArea()->isMultiLine();
}

void FormTextArea::text(std::string &returnString) const
{
  textArea()->text(returnString);
}

void FormTextArea::setText(const std::string &text)
{
  textArea()->setText(text);
  textNode()->text() = text;
  layoutViewer();
}

HtmlElement * FormTextArea::textNode()
{
  // see if the child nodes have textnodes
  if (m_element->hasChildren() and m_element->firstChild()->isa(HtmlConstants::TEXT))
  {
    return m_element->firstChild();
  }
  else
  {
    m_element->append( ElementFactory::create(HtmlConstants::TEXT));
    return m_element->firstChild();
  }
}
