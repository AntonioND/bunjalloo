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
#include "DefaultVisitor.h"
bool DefaultVisitor::visit(HtmlAnchorElement & element) { return true; }
void DefaultVisitor::begin(HtmlAnchorElement & element) {}
void DefaultVisitor::end(HtmlAnchorElement & element) {}

bool DefaultVisitor::visit(HtmlBlockElement & element) { return true; }
void DefaultVisitor::begin(HtmlBlockElement & element) {}
void DefaultVisitor::end(HtmlBlockElement & element) {}

bool DefaultVisitor::visit(HtmlBodyElement & element) { return true; }
void DefaultVisitor::begin(HtmlBodyElement & element) {}
void DefaultVisitor::end(HtmlBodyElement & element) {}

bool DefaultVisitor::visit(HtmlElement & element) { return true; }
void DefaultVisitor::begin(HtmlElement & element) {}
void DefaultVisitor::end(HtmlElement & element) {}

bool DefaultVisitor::visit(HtmlFormElement & element) { return true; }
void DefaultVisitor::begin(HtmlFormElement & element) {}
void DefaultVisitor::end(HtmlFormElement & element) {}

bool DefaultVisitor::visit(HtmlImageElement & element) { return true; }
void DefaultVisitor::begin(HtmlImageElement & element) {}
void DefaultVisitor::end(HtmlImageElement & element) {}

bool DefaultVisitor::visit(HtmlInputElement & element) { return true; }
void DefaultVisitor::begin(HtmlInputElement & element) {}
void DefaultVisitor::end(HtmlInputElement & element) {}

bool DefaultVisitor::visit(HtmlMetaElement & element) { return true; }
void DefaultVisitor::begin(HtmlMetaElement & element) {}
void DefaultVisitor::end(HtmlMetaElement & element) {}

bool DefaultVisitor::visit(HtmlOptionElement & element) { return true; }
void DefaultVisitor::begin(HtmlOptionElement & element) {}
void DefaultVisitor::end(HtmlOptionElement & element) {}

bool DefaultVisitor::visit(HtmlPreElement & element) { return true; }
void DefaultVisitor::begin(HtmlPreElement & element) {}
void DefaultVisitor::end(HtmlPreElement & element) {}

bool DefaultVisitor::visit(HtmlTextAreaElement & element) { return true; }
void DefaultVisitor::begin(HtmlTextAreaElement & element) {}
void DefaultVisitor::end(HtmlTextAreaElement & element) {}

DefaultVisitor::~DefaultVisitor() {}
