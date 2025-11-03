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
#include <algorithm>
#include <assert.h>
#include "Document.h"
#include "HtmlDocument.h"
#include "HtmlConstants.h"
#include "HeaderParser.h"
#include "HtmlElement.h"
#include "CookieJar.h"
#include "CacheControl.h"
#include "URI.h"

using namespace std;

class DocumentHeaderListener: public HeaderListener
{
  public:
    DocumentHeaderListener(Document & doc):
      m_document(doc) { }

    void hasHeaders()
    {
      m_document.setStatus(Document::HAS_HEADERS);
    }
  private:
    Document & m_document;
};

Document::Document():
  m_amount(0),
  m_cookieJar(new CookieJar()),
  m_htmlDocument(new HtmlDocument()),
  m_headerParser(new HeaderParser(m_htmlDocument,m_cookieJar, new CacheControl())),
  m_headerListener(new DocumentHeaderListener(*this)),
  m_historyEnabled(true)
{
  m_history.clear();
  m_historyPosition = m_history.begin();
  reset();
  m_headerParser->setListener(m_headerListener);
}

Document::~Document()
{
  delete m_htmlDocument;
  delete m_headerParser;
  delete m_cookieJar;
  delete m_headerListener;
}

void Document::setUri(const std::string & uriString)
{
  // m_uri = uriString;
  if (m_historyEnabled and (m_history.empty() or uriString != currentHistoryUri()))
  {
    // not refreshing the same page
    HistoryVector::iterator currPosition(m_historyPosition);
    if (hasNextHistory()) {
      ++currPosition;
      m_history.erase(currPosition, m_history.end());
    }
    m_history.push_back(HistoryEntry(uriString,-1));
    m_historyPosition = m_history.end();
    --m_historyPosition;
  }
  m_headerParser->setUri(uriString);
#if 0
  printf("History: %s %d\n", m_historyPosition->first.c_str(), m_history.size());
  HistoryVector::iterator it(m_history.begin());
  for (; it != m_history.end(); ++it)
  {
    printf("History: %s\n",it->first.c_str());
  }
#endif
}

bool Document::historyEnabled() const
{
  return m_historyEnabled;
}

void Document::setHistoryEnabled(bool enable)
{
  m_historyEnabled = enable;
}

const std::string & Document::uri() const
{
  return currentHistoryUri();
}

// const char * Document::asText() const
const std::string & Document::asText() const
{
  return m_htmlDocument->data();
}


void Document::dumpDOM()
{
  m_htmlDocument->dumpDOM();
}

const HtmlElement * Document::rootNode() const
{
  // m_htmlDocument->dumpDOM();
  return m_htmlDocument->rootNode();
}

const HtmlElement * Document::titleNode() const
{
  const HtmlElement * title(0);
  if (m_htmlDocument->mimeType() == HtmlDocument::TEXT_HTML)
  {
    const HtmlElement * root(rootNode());
    if (root->isa(HtmlConstants::HTML_TAG) and root->hasChildren())
    {
      // firstChild is <HEAD> node
      const ElementList titles = root->firstChild()->elementsByTagName(HtmlConstants::TITLE_TAG);
      if (not titles.empty())
      {
        title = titles.front();
        if (not title->hasChildren())
          title = 0;
      }
    }
  }
  return title;
}

void Document::reset()
{
  m_status = NOTHING;
  m_htmlDocument->reset();
  m_headerParser->reset();
}

void Document::registerView(ViewI * view)
{
   vector<ViewI*>::iterator it = find(m_views.begin(), m_views.end(), view);
   if (it == m_views.end())
     m_views.push_back(view);
}

void Document::unregisterView(ViewI * view)
{
   vector<ViewI*>::iterator it = find(m_views.begin(), m_views.end(), view);
   if (it != m_views.end())
     m_views.erase(it);
}

unsigned int Document::percentLoaded() const
{
    unsigned int expected = dataExpected();
    if (expected) {
      return (m_htmlDocument->dataGot()*100) / expected;
    }
    return 0;
}

void Document::appendLocalData(const char * data, int size)
{
  m_headerParser->setDataState();
  if (m_status == NOTHING)
  {
    // need to get mime type from the data...
    magicMimeType(data, size);
  }
  appendData(data, size);
}

void Document::appendData(const char * data, int size)
{
  m_status = INPROGRESS;
  if (size) {
    m_headerParser->feed(data,size);
    unsigned int expected(dataExpected());
    unsigned int dataGot(m_htmlDocument->dataGot());
    if (expected < dataGot)
    {
      m_htmlDocument->setDataGot(0);
    }
    if (not m_headerParser->redirect().empty())
    {
      if (m_historyEnabled)
      {
        URI tmp(currentHistoryUri());
        currentHistoryUri() = tmp.navigateTo(m_headerParser->redirect()).asString();
      }
      setStatus(REDIRECTED);
    }
  }
  notifyAll();
}

void Document::notifyAll() const
{
  for (std::vector<ViewI*>::const_reverse_iterator it(m_views.rbegin()); it != m_views.rend(); ++it)
  {
    size_t size = m_views.size();
    ViewI * v(*it);
    v->notify();
    // someone has been naughty and registered in an update
    if (m_views.size() != size)
    {
      break;
    }
  }
}

void Document::setStatus(Document::Status status)
{
  m_status = status;
  if (m_status == LOADED_HTML or m_status == LOADED_ITEM)
  {
    m_htmlDocument->handleEOF();
    //m_htmlDocument->dumpDOM();
  }
  notifyAll();
}

void Document::refresh(std::string & refreshUrl, int & time) const
{
  m_headerParser->refresh(refreshUrl, time);
}

Document::Status Document::status() const
{
  return m_status;
}


bool Document::hasPreviousHistory() const
{
  HistoryVector::iterator currPosition(m_historyPosition);
  return currPosition != m_history.begin();
}
bool Document::hasNextHistory() const
{
  HistoryVector::iterator currPosition(m_historyPosition);
  if (currPosition == m_history.end())
  {
    return false;
  }
  ++currPosition;
  return currPosition != m_history.end();
}

std::string Document::gotoPreviousHistory()
{
  if (hasPreviousHistory())
  {
    if (m_historyEnabled) {
      --m_historyPosition;
    }
    return currentHistoryUri();
  }
  return "";
}

std::string Document::gotoNextHistory()
{
  if (hasNextHistory())
  {
    if (m_historyEnabled) {
      ++m_historyPosition;
    }
    return currentHistoryUri();
  }
  return "";
}

CookieJar * Document::cookieJar() const
{
  return m_cookieJar;
}

void Document::setCacheFile(const std::string & cacheFile)
{
  m_htmlDocument->setCacheFile(cacheFile);
  if (not cacheFile.empty())
  {
    m_headerParser->setCacheFile(cacheFile+".hdr");
  }
  else
  {
    m_headerParser->setCacheFile(cacheFile);
  }
}

void Document::magicMimeType(const char * data, int length)
{
  // This is used to determine the type of local cached data. Data from http
  // should already have content-type in the headers or in meta or whatever.
  if (length > 128)
    length = 128;
  if (length < 3)
  {
    // assume plain text we only have 2 bytes...
    m_htmlDocument->parseContentType("text/plain");
    return;
  }
  else
  {
    if ((unsigned char)data[0] == 0x89 && (unsigned char)data[1] == 0x50 &&
        (unsigned char)data[2] == 0x4E)
    {
      m_htmlDocument->parseContentType("image/png");
      // This could also be an APNG file, but we need to do a more clever
      // parsing to detect it...
    }
    else if ((unsigned char)data[0] == 0x47 && (unsigned char)data[1] == 0x49 &&
             (unsigned char)data[2] == 0x46)
    {
      m_htmlDocument->parseContentType("image/gif");
    }
    else if ((unsigned char)data[0] == 'B' && (unsigned char)data[1] =='M')
    {
      m_htmlDocument->parseContentType("image/bmp");
    }
    else if ((unsigned char)data[0] == 0xFF && (unsigned char)data[1] == 0xD8)
    {
      // naive check, but if it isn't a jpeg, then the proper
      // isJpeg call will catch it...
      m_htmlDocument->parseContentType(HtmlParser::IMAGE_JPEG_STR);
    }
    else if (strncmp("PK", data, 2) == 0)
    {
      m_htmlDocument->parseContentType("application/zip");
    }
    // lame hack to get html type
    else
    {
      string htmlTest(data);
      transform(htmlTest.begin(), htmlTest.end(), htmlTest.begin(), ::tolower);
      size_t gt = htmlTest.find('<');
      size_t doctype = htmlTest.find("<!doctype");
      size_t html = htmlTest.find("<html");
      size_t body = htmlTest.find("<body");
      if ((html != string::npos and html >= gt and (html < 5 or (doctype < html and doctype < 5)))
          or
          (body != string::npos and body < 5)
          or (gt == 0)
         )
      {
        m_htmlDocument->parseContentType("text/html");
      }
      else
      {
        for (const char * src=data; src != data+length; ++src)
        {
          if (not isascii(*src))
          {
            m_htmlDocument->parseContentType("application/octet-stream");
            return;
          }
        }
        m_htmlDocument->parseContentType("text/plain");
      }
    }
  }
}

void Document::setPosition(int position)
{
  currentHistoryPosition() = position;
}

int Document::position() const
{
  return currentHistoryPosition();
}

std::string & Document::currentHistoryUri()
{
  return m_historyPosition->first;
}

const std::string & Document::currentHistoryUri() const
{
  return m_historyPosition->first;
}

int & Document::currentHistoryPosition()
{
  return m_historyPosition->second;
}

const int & Document::currentHistoryPosition() const
{
  return m_historyPosition->second;
}

unsigned int Document::dataExpected() const
{
  return m_headerParser->expected();
}

void Document::clearConfigHistory()
{
  for (HistoryVector::iterator it(m_history.begin()); it != m_history.end(); ++it)
  {
    HistoryVector::iterator tmp(it);
    if ( URI(it->first).protocol() == URI::CONFIG_PROTOCOL)
    {
      m_history.erase(tmp);
      --m_historyPosition;
      --it;
    }
  }
}

const HeaderParser & Document::headerParser() const
{
  return *m_headerParser;
}
