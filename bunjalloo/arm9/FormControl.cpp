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
#include <cstdio>
#include "config_defs.h"
#include "FormControl.h"
#include "HtmlElement.h"
#include "HtmlConstants.h"
#include "HtmlInputElement.h"
#include "Controller.h"
#include "URI.h"

const int FormControl::MAX_SIZE(120);
const int FormControl::MIN_SIZE(8);

FormControl::FormControl(const HtmlElement * element, const std::string &text):
  Button(text),
  m_element(element)
{
}

#if 0
void FormControl::walkForm(const HtmlElement * formElement)
{
  std::string name = formElement->attribute("name");
  if (not name.empty() )
  {
    std::string value = formElement->attribute("value");
    m_processedData +=
  }
  if (formElement->hasChildren())
  {
    ElementList::const_iterator it(formElement->children());
    for (; it != formElement->children().end(); ++it)
    {
      const HtmlElement * element(*it);
      if (element)
        walkForm(element);
    }
  }
}

static void addBoundary(std::string & processedData, unsigned char boundary[16])
{
  for (int i = 0; i < 16; ++i) {
    char buffer[3];
    sprintf_platform(buffer,"%02x",boundary[i]);
    processedData += buffer;
  }
  processedData += "\r\n";
}
#endif

static void appendFormElements(ElementList & list, const HtmlElement * formElement)
{
  if (formElement->isa(HtmlConstants::INPUT_TAG)
      or formElement->isa(HtmlConstants::SELECT_TAG)
      or formElement->isa(HtmlConstants::TEXTAREA_TAG))
  {
    list.push_back(const_cast<HtmlElement*>(formElement));
  }
  if (formElement->hasChildren())
  {
    ElementList::const_iterator it(formElement->children().begin());
    for (; it != formElement->children().end(); ++it)
    {
      const HtmlElement * element(*it);
      if (element) {
        appendFormElements(list, element);
      }
    }
  }
}

void FormControl::input(Controller & controller, URI & uri)
{
  // need to walk up m_element until we find the form father or the html element.
  HtmlElement * currentNode = m_element->parent();
  while (not currentNode->isa("html"))
  {
    if (currentNode->isa("form"))
      break;
    currentNode = currentNode->parent();
  }
  if (currentNode->isa("html"))
  {
    return;
  }
  uri.setMethod(currentNode->attribute("method"));

  ElementList inputs;
  appendFormElements(inputs, currentNode);
  ElementList::iterator inputEnd(inputs.end());
  ElementList::const_iterator inputIt(inputs.begin());
  bool needAmp(false);
  bool isGet = uri.method() == "GET";
  /*
  // use md5 when forms support larger fields...
  std::vector<std::string> postData;
  md5_context ctx;
  if (not isGet)
  {
    md5_starts(&ctx);
  }
  */
  std::string processedData;
  std::string myName = m_element->attribute("name");
  const std::string one("1");
  for (; inputIt != inputs.end(); ++inputIt)
  {
    const HtmlElement * element(*inputIt);
    std::string name = element->attribute("name");
    std::string value = element->attribute("value");
    bool includeValue(false);
    if (element->isa(HtmlConstants::INPUT_TAG))
    {
      HtmlInputElement::InputType inputType = ((const HtmlInputElement*)element)->inputType();
      bool checked = ((const HtmlInputElement*)element)->checked();
      switch (inputType)
      {
        case HtmlInputElement::TEXT:
        case HtmlInputElement::PASSWORD:
          includeValue = not name.empty();
          break;

        case HtmlInputElement::IMAGE:
          includeValue = m_element == element;
          if (includeValue)
          {
            if (needAmp) {
              processedData += '&';
            }
            needAmp = true;
            // FIXME - add correct x,y value..
            processedData += "x=0&y=0";
          }
          break;
        case HtmlInputElement::SUBMIT:
          includeValue = m_element == element and not name.empty();
          break;

        case HtmlInputElement::CHECKBOX:
          includeValue = checked;
          if (value.empty())
          {
            value = "on";
          }
          break;

        case HtmlInputElement::RADIO:
          // only include the selected radio button
          includeValue = checked;
          break;

        case HtmlInputElement::HIDDEN:
          includeValue = true;
          break;

      }
    }
    else if (element->isa(HtmlConstants::SELECT_TAG))
    {
      includeValue = not value.empty();
    }
    else if (element->isa(HtmlConstants::TEXTAREA_TAG))
    {
      includeValue = false; // because it is already hacked in here
      if (needAmp) {
        processedData += '&';
      }
      processedData += URI::escape(name);
      processedData += "=";
      processedData += element->firstChild()->text();
    }
    if (m_element == element and not myName.empty())
    {
      includeValue = true;
    }
    if (includeValue)
    {
        //if (isGet) {
          if (needAmp) {
            processedData += '&';
          }
          processedData += URI::escape(name);
          processedData += "=";
          processedData += URI::escape(value);
          needAmp = true;
        /*}
        else
        {
          // post adds the data to the content type..
          postData.push_back(unicode2string(URI::escape(name)));
          md5_update(&ctx, (unsigned char*)postData.back().c_str(), postData.back().length());
          postData.push_back(unicode2string(URI::escape(value)));
          md5_update(&ctx, (unsigned char*)postData.back().c_str(), postData.back().length());
        }
        */
    }
  }
  if (isGet) {
    m_processedData = currentNode->attribute("action");
    m_processedData += '?';
    m_processedData += processedData;
    uri = uri.navigateTo(m_processedData);
  }
  else {
    std::string contentType;
    char buffer[256];
    snprintf(buffer, sizeof(buffer), "%d", processedData.length());
    contentType += "Content-Length: ";
    contentType += buffer;
    contentType += "\r\n";
    contentType += "Content-Type: application/x-www-form-urlencoded\r\n";
    contentType += "\r\n";
    // add url
    std::string action = currentNode->attribute("action");
    if (not action.empty())
    {
      uri = uri.navigateTo(action);
    }

    contentType += processedData;
    uri.setRequestHeader(contentType);

    /*
    unsigned char output[16];
    md5_finish(&ctx, output);
    std::vector<std::string>::const_iterator it(postData.begin());
    std::string processedData;
    processedData += "Content-Type: multipart/form-data; boundary=";
    addBoundary(processedData, output);
    processedData += "\r\n";
    processedData += "--";
    addBoundary(processedData, output);
    for ( ; it != postData.end(); ++it)
    {
      processedData += "Content-Disposition: form-data; name=\"";
      processedData += *it;
      processedData += "\"\r\n";
      //processedData += "Content-Type: text/plain\r\n";
      processedData += "\r\n";
      // add the value
      ++it;
      processedData += *it;
      processedData += "\r\n";
      processedData += "--";
      addBoundary(processedData, output);
    }
    uri.setRequestHeader(processedData);
    */
  }
  //printf("Go to %s\n", uri.asString().c_str());
  controller.doUri(uri);
}
