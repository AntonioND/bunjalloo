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
#ifndef Visitor_h_seen
#define Visitor_h_seen

#include "HtmlElementFwd.h"

/** Separate the actions on Html items from their implementations. 
 * See http://en.wikipedia.org/wiki/Visitor_pattern for more details.
 */
class Visitor
{
  public:
    virtual ~Visitor() {}
    virtual bool visit(HtmlAnchorElement & element) = 0;
    virtual void begin(HtmlAnchorElement & element) = 0;
    virtual void end(HtmlAnchorElement & element) = 0;

    virtual bool visit(HtmlBlockElement & element) = 0;
    virtual void begin(HtmlBlockElement & element) = 0;
    virtual void end(HtmlBlockElement & element) = 0;

    virtual bool visit(HtmlBodyElement & element) = 0;
    virtual void begin(HtmlBodyElement & element) = 0;
    virtual void end(HtmlBodyElement & element) = 0;

    virtual bool visit(HtmlElement & element) = 0;
    virtual void begin(HtmlElement & element) = 0;
    virtual void end(HtmlElement & element) = 0;

    virtual bool visit(HtmlFormElement & element) = 0;
    virtual void begin(HtmlFormElement & element) = 0;
    virtual void end(HtmlFormElement & element) = 0;

    virtual bool visit(HtmlImageElement & element) = 0;
    virtual void begin(HtmlImageElement & element) = 0;
    virtual void end(HtmlImageElement & element) = 0;

    virtual bool visit(HtmlInputElement & element) = 0;
    virtual void begin(HtmlInputElement & element) = 0;
    virtual void end(HtmlInputElement & element) = 0;

    virtual bool visit(HtmlMetaElement & element) = 0;
    virtual void begin(HtmlMetaElement & element) = 0;
    virtual void end(HtmlMetaElement & element) = 0;

    virtual bool visit(HtmlOptionElement & element) = 0;
    virtual void begin(HtmlOptionElement & element) = 0;
    virtual void end(HtmlOptionElement & element) = 0;

    virtual bool visit(HtmlPreElement & element) = 0;
    virtual void begin(HtmlPreElement & element) = 0;
    virtual void end(HtmlPreElement & element) = 0;

    virtual bool visit(HtmlTextAreaElement & element) = 0;
    virtual void begin(HtmlTextAreaElement & element) = 0;
    virtual void end(HtmlTextAreaElement & element) = 0;

};

#endif
