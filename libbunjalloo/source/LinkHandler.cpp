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
#include "Link.h"
#include "Language.h"
#include "LinkHandler.h"
#include "LinkListener.h"
#include "WidgetColors.h"

LinkHandler::LinkHandler(LinkListener * parent):
  m_parent(parent)
{
  setVisible(false);
  addMenuItem(T("url"), urlCallback, this);
  addMenuItem(T("image"), imageCallback, this);
  updateLayout();
}

void LinkHandler::setLink(const Link * link)
{
  m_link = link;
}

void LinkHandler::urlCallback(void * self)
{
  ((LinkHandler*)self)->url();
}

void LinkHandler::imageCallback(void * self)
{
  ((LinkHandler*)self)->image();
}

void LinkHandler::url()
{
  // goto link
  Link link(m_link->href());
  m_parent->linkClicked(&link);
}

void LinkHandler::image()
{
  // goto src
  Link link(Link::STATE_COLOR);
  link.setColor(WidgetColors::LINK_IMAGE);
  link.setSrc(m_link->src());
  m_parent->linkClicked(&link);
}
