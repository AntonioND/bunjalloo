/*
  Copyright (C) 2008 Richard Quirk

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
#ifndef DefaultVisitor_h_seen
#define DefaultVisitor_h_seen

#include "Visitor.h"
#include "HtmlElementFwd.h"

/** Do nothing implementation of the Visitor interface.  This class is to make
 * it easier to create Visitor sub classes.
 */
class DefaultVisitor: public Visitor
{
  public:
    virtual bool visit(HtmlAnchorElement & element);
    virtual void begin(HtmlAnchorElement & element);
    virtual void end(HtmlAnchorElement & element);

    virtual bool visit(HtmlBlockElement & element);
    virtual void begin(HtmlBlockElement & element);
    virtual void end(HtmlBlockElement & element);

    virtual bool visit(HtmlBodyElement & element);
    virtual void begin(HtmlBodyElement & element);
    virtual void end(HtmlBodyElement & element);

    virtual bool visit(HtmlElement & element);
    virtual void begin(HtmlElement & element);
    virtual void end(HtmlElement & element);

    virtual bool visit(HtmlFormElement & element);
    virtual void begin(HtmlFormElement & element);
    virtual void end(HtmlFormElement & element);

    virtual bool visit(HtmlImageElement & element);
    virtual void begin(HtmlImageElement & element);
    virtual void end(HtmlImageElement & element);

    virtual bool visit(HtmlInputElement & element);
    virtual void begin(HtmlInputElement & element);
    virtual void end(HtmlInputElement & element);

    virtual bool visit(HtmlMetaElement & element);
    virtual void begin(HtmlMetaElement & element);
    virtual void end(HtmlMetaElement & element);

    virtual bool visit(HtmlOptionElement & element);
    virtual void begin(HtmlOptionElement & element);
    virtual void end(HtmlOptionElement & element);

    virtual bool visit(HtmlPreElement & element);
    virtual void begin(HtmlPreElement & element);
    virtual void end(HtmlPreElement & element);

    virtual bool visit(HtmlTextAreaElement & element);
    virtual void begin(HtmlTextAreaElement & element);
    virtual void end(HtmlTextAreaElement & element);

    virtual ~DefaultVisitor();
};

#endif
