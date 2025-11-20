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
#include <vector>
#include "ElementFactory.h"
#include "HtmlAnchorElement.h"
#include "HtmlImageElement.h"
#include "HtmlMetaElement.h"
#include "HtmlBodyElement.h"
#include "HtmlBlockElement.h"
#include "HtmlPreElement.h"
#include "HtmlOptionElement.h"
#include "HtmlFormElement.h"
#include "HtmlInputElement.h"
#include "HtmlTextAreaElement.h"
#include "HtmlElement.h"
#include "HtmlConstants.h"


HtmlElement * ElementFactory::create(const std::string & elementType)
{
  if (elementType == HtmlConstants::A_TAG)
  {
    return new HtmlAnchorElement(elementType);
  }
  else if (elementType == HtmlConstants::META_TAG)
  {
    return new HtmlMetaElement(elementType);
  }
  else if (elementType == HtmlConstants::IMG_TAG)
  {
    return new HtmlImageElement(elementType);
  }
  else if (elementType == HtmlConstants::BODY_TAG)
  {
    return new HtmlBodyElement(elementType);
  }
  else if (elementType == HtmlConstants::P_TAG or elementType == HtmlConstants::DIV_TAG)
  {
    return new HtmlBlockElement(elementType);
  }
  else if (elementType == HtmlConstants::PRE_TAG)
  {
    return new HtmlPreElement(elementType);
  }
  else if (elementType == HtmlConstants::OPTION_TAG)
  {
    return new HtmlOptionElement(elementType);
  }
  else if (elementType == HtmlConstants::FORM_TAG)
  {
    return new HtmlFormElement(elementType);
  }
  else if (elementType == HtmlConstants::INPUT_TAG)
  {
    return new HtmlInputElement(elementType);
  }
  else if (elementType == HtmlConstants::SELECT_TAG)
  {
    return new HtmlInputElement(elementType);
  }
  else if (elementType == HtmlConstants::TEXTAREA_TAG)
  {
    return new HtmlTextAreaElement(elementType);
  }
  /*
  else if (elementType == HtmlConstants::UL_TAG or elementType == HtmlConstants::OL_TAG or elementType == HtmlConstants::LI_TAG)
  {
    // lists have a pricipal box, then inline item boxes.
    return new HtmlListElement(elementType);
  }
  */

  return new HtmlElement(elementType);
}

HtmlElement * ElementFactory::create(const std::string & elementType,
    const AttributeVector & attrs)
{
  HtmlElement * element(create(elementType));
  AttributeVector::const_iterator it(attrs.begin());
  for (; it != attrs.end(); ++it) {
    Attribute * attr(*it);
    element->setAttribute(attr->name, attr->value);
  }
  return element;
}

void ElementFactory::remove(HtmlElement * element)
{
  delete element;
}
