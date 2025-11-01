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
#include "libnds.h"
#include "BoxLayout.h"
#include "Canvas.h"
#include "Document.h"
#include "HeaderParser.h"
#include "HtmlDocument.h"
#include "ImageComponent.h"
#include "Image.h"
#include "Link.h"
#include "LinkListener.h"
#include "Palette.h"
#include "Stylus.h"
#include "URI.h"
#include "WidgetColors.h"

using nds::Canvas;
using nds::Image;


ImageComponent::ImageComponent(
    nds::Image *image,
    BoxLayout *boxLayout,
    Document *doc):
  m_image(image), m_boxLayout(boxLayout), m_document(doc), m_link(0), m_linkListener(0)
{
  if (m_image)
  {
    setSize(m_image->width(), m_image->height());
  }
  if (m_document)
  {
    m_document->registerView(this);
  }
}

ImageComponent::~ImageComponent()
{
  m_document->unregisterView(this);
  delete m_link;
  delete m_image;
}

static void drawImage(Canvas & canvas, const nds::Image & image, int startx, int starty)
{
  const unsigned short * data = image.data();
  for (unsigned int y = 0; y < image.height(); ++y)
  {
    const unsigned short * row = &data[y*image.width()];
    for (unsigned int x = 0; x < image.width(); ++x, row++)
    {
      canvas.drawPixel(startx+x, starty+y, *row);
    }
  }
}

void ImageComponent::paint(const nds::Rectangle & clip)
{
  if (m_image)
  {
    //nds::Canvas::instance().drawRectangle(x(),y(),width(),height(), nds::Color(31,0,0));
    drawImage(Canvas::instance(), *m_image, x(), y());
    if (m_link)
    {
      // TODO: need to css or whatever this part of the code...
      nds::Color linkColor(WidgetColors::LINK_REGULAR);
      if (m_link->clicked()) {
        linkColor = WidgetColors::LINK_CLICKED;
        nds::Canvas::instance().drawRectangle(x(), y(), width(), height(), linkColor);
      }
    }
  }
  else
  {
    Canvas::instance().fillRectangle(x(),y(),width(),height(), nds::Color(31,0,0));
  }
  m_dirty = false;
}

void ImageComponent::reload()
{
  if (not m_image)
    return;

  //if (m_image->type() == nds::Image::ImageUNKNOWN)
  //{
  //  m_image->setType((nds::Image::ImageType)m_document->htmlDocument()->mimeType());
  //}
  unsigned int w = m_image->width();
  unsigned int h = m_image->height();
  m_image->reload();
  if (w != m_image->width() or h != m_image->height())
  {
    setSize(m_image->width(), m_image->height());
    if (m_boxLayout) {
      m_boxLayout->doLayout();
    }
  }
  m_dirty = true;
}

void ImageComponent::notify()
{
  if ( m_image and  m_document->headerParser().cacheFile() == (m_image->filename()+".hdr"))
  {
    if (m_document->status() == Document::HAS_HEADERS)
    {
      // checks for probs
      HtmlDocument::MimeType mimeType = m_document->htmlDocument()->mimeType();
      bool isImg = (mimeType >= HtmlDocument::IMAGE_START) &&
                   (mimeType <= HtmlDocument::IMAGE_END);
      if (not isImg or m_document->headerParser().httpStatusCode() != 200)
      {
        setSize(12,12);
        if (m_boxLayout) m_boxLayout->doLayout();
        m_image = 0;
      }
    }
    switch (m_document->status())
    {
      case Document::INPROGRESS:
      case Document::LOADED_ITEM:
      case Document::LOADED_HTML:
        reload();
        break;
      default:
        break;
    }
  }
}

void ImageComponent::addLink(const std::string & href, LinkListener * listener)
{
  m_linkListener = listener;
  delete m_link;
  m_link = 0;
  if (m_linkListener)
  {
    m_link = new Link(href);
  }
}

bool ImageComponent::stylusUp(const Stylus * stylus)
{
  if (m_link and m_link->clicked())
  {
    if (m_bounds.hit(stylus->lastX(), stylus->lastY()))
    {
      m_linkListener->linkClicked(m_link);
    }
    m_link->setClicked(false);
    return true;
  }
  return false;
}
bool ImageComponent::stylusDownFirst(const Stylus * stylus)
{
  if (m_bounds.hit(stylus->startX(), stylus->startY()))
  {
    if (m_link)
      m_link->setClicked();
    return true;
  }
  return false;
}

bool ImageComponent::stylusDownRepeat(const Stylus * stylus)
{ return false; }

bool ImageComponent::stylusDown(const Stylus * stylus)
{
  if (m_link and m_link->clicked() and not m_bounds.hit(stylus->lastX(), stylus->lastY()))
  {
    m_link->setClicked(false);
  }
  return false;
}

