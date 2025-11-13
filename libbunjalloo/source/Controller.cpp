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
#include <cstring>
#include "libnds.h"
#include "config_defs.h"
#include "Cache.h"
#include "Config.h"
#include "ConfigParser.h"
#include "CookieJar.h"
#include "FontFactory.h"
#include "HeaderParser.h"
#include "Language.h"
#include "Controller.h"
#include "Document.h"
#include "File.h"
#include "Font.h"
#include "HttpClient.h"
#include "TextAreaFactory.h"
#include "System.h"
#include "Wifi9.h"
#include "Updater.h"
#include "URI.h"
#include "View.h"

using namespace std;

static const char s_homeText[] = {
#include "home_text.h"
};
static const char s_errorText[] = {
"<html> <meta http-equiv='Content-Type' content='text/html; charset=utf-8' />"
};

const char Controller::HOME_URL[] = "about://home";
const char Controller::SYSINFO_URL[] = "about://sysinfo";

const static char * UNABLE_TO_LOAD = "cannot_load";
const static int MAX_REDIRECTS(7);

Controller::Controller()
: m_document(new Document()),
  m_view(0),
  m_config(0),
  m_cache(new Cache(*m_document, false)),
  m_httpClient(new HttpClient()),
  m_wifiInit(false),
  m_stop(false),
  m_redirected(0),
  m_maxRedirects(MAX_REDIRECTS),
  m_saveAs(NO_SAVE),
  m_checkingQueue(false)
{
}

void Controller::initialise()
{
  m_config = new Config();
  m_config->checkPre();
  m_config->reload();
  m_config->checkPost();
  m_document->cookieJar()->loadAcceptList(*m_config);
  string font;
  m_config->resource(Config::FONT_STR, font);
  TextAreaFactory::setFont(FontFactory::create(font.c_str()));

  bool useCache(false);
  if (m_config->resource(Config::USECACHE, useCache))
    m_cache->setUseCache(useCache);

  bool clearCache(false);
  if (m_config->resource(Config::CLEARCACHE, clearCache))
  {
    if (clearCache)
    {
      m_cache->clearCache();
    }
  }

  m_view = new View(*m_document, *this);
  m_config->resource("redirects", m_maxRedirects);
  m_httpClient->setController(this);
}

Controller::~Controller()
{
  delete m_document;
  delete m_view;
  delete m_config;
}

void Controller::showHome()
{
  m_document->reset();
  m_document->setUri(HOME_URL);
  m_document->appendLocalData(s_homeText, strlen(s_homeText));
  m_document->flush();
  m_document->setStatus(Document::LOADED_HTML);
}

void Controller::showSysInfo()
{
  m_document->reset();
  m_document->setUri(SYSINFO_URL);

  std::string info = "<html><head><title>System information</title></head><body>";
  info += std::string("Platform: ") + nds::System::uname() + std::string("<br>");
  info += nds::System::meminfo() + std::string("<br>");
  info += nds::System::slot2meminfo() + std::string("<br>");
  info += "</body></html>";

  m_document->appendLocalData(info.c_str(), info.length());
  m_document->flush();
  m_document->setStatus(Document::LOADED_HTML);
}

const Config & Controller::config() const
{
  return *m_config;
}

void Controller::handleUri(const URI & uri)
{
  m_document->reset();
  m_document->setCacheFile("");
  switch (uri.protocol())
  {
    case URI::ABOUT_PROTOCOL:
      if (uri.asString() == HOME_URL)
      {
        showHome();
      }
      else if (uri.asString() == SYSINFO_URL)
      {
        showSysInfo();
      }
      else
      {
        loadError();
      }
      break;

    case URI::FILE_PROTOCOL:
      if (uri.server() == "bunjalloo")
      {
        localFile(std::string(DATADIR) + uri.fileName());
      }
      else if (uri.server() == "localhost")
      {
        localFile(uri.fileName());
      }
      else
      {
        loadError();
      }
      break;

    case URI::HTTPS_PROTOCOL:
    case URI::HTTP_PROTOCOL:
      fetchHttp(uri);
      break;

    case URI::CONFIG_PROTOCOL:
      configureUrl(DATADIR "/config/" + uri.fileName());
      break;

    default:
      break;
  }
}

void Controller::doUri(const URI & uri)
{
  // cout << uri.asString() << endl;
  if (uri.isValid()) {
    m_document->setUri(uri.asString());
    do {
      if (m_document->status() == Document::REDIRECTED)
      {
        handleUri(uri.navigateTo(m_document->uri()));
      }
      else
      {
        handleUri(uri);
        m_document->setPosition(-1);
      }
    }
    while (m_document->status() == Document::REDIRECTED);
    checkDownloadQueue();

  }
}

void Controller::reload()
{
  m_cache->remove(m_document->uri());
  doUri(m_document->uri());
}

void Controller::saveAs(const char * fileName, SaveAs_t saveType)
{
  // 2 types of "save as" - save the current displayed file, or save a file not yet downloaded.
  m_saveAs = saveType;
  switch (m_saveAs)
  {
    case NO_SAVE:
      break;

    case SAVE_CURRENT_FILE:
      saveCurrentFileAs(fileName);
      break;

    default:
      // download the file first!
      m_saveFileName = fileName;
      if (m_document->status() == Document::LOADED_ITEM or m_document->status() == Document::LOADED_HTML)
      {
        checkSave();
      }
      break;
  }
}

void Controller::saveCurrentFileAs(const char * fileName)
{
  // save the current document as fileName
  // simply copy from the cache.
  string cachedFile = m_cache->fileName(m_document->uri());
  if (nds::File::exists(cachedFile.c_str()) == nds::File::F_REG)
  {
    string path;
    m_config->resource(Config::DOWNLOAD, path);
    if (nds::File::exists(path.c_str()) == nds::File::F_NONE)
    {
      nds::File::mkdir(path.c_str());
    }
    path += "/";
    // remove any path stuff at the start
    path += nds::File::base(fileName);
    /*bool ok = */nds::File::cp(cachedFile.c_str(), path.c_str());
  }
}

void Controller::previous()
{
  if (m_document->status() != Document::LOADED_PAGE)
    stop();
  string ph = m_document->gotoPreviousHistory();
  if (not ph.empty())
  {
    URI uri(ph);
    handleUri(uri);
    checkDownloadQueue();
  }
}

void Controller::next()
{
  if (m_document->status() != Document::LOADED_PAGE)
    stop();
  string ph = m_document->gotoNextHistory();
  if (not ph.empty())
  {
    URI uri(ph);
    handleUri(uri);
    checkDownloadQueue();
  }
}

void Controller::mainLoop()
{
  for (;;) {
    m_view->tick();
    cothread_yield_irq(IRQ_VBLANK);
  }
}

void Controller::loadError()
{
  m_document->reset();
  m_document->appendLocalData(s_errorText, strlen(s_errorText));
  string errorStr(T(UNABLE_TO_LOAD));
  m_document->appendLocalData(errorStr.c_str(), errorStr.length());
  string href("<a href='");
  href += m_document->uri();
  href += "'>";
  href += m_document->uri();
  href += "</a>";
  m_document->appendLocalData(href.c_str(), href.length());
  m_document->flush();
  m_document->setStatus(Document::LOADED_HTML);
}

void Controller::configureUrl(const std::string & fileName)
{
  // if file exists, localfile it
  // search for ? - if found, send the encoding to configure
  // otherwise localFile the fileName

  size_t position = fileName.find_first_of("?");
  if (position != string::npos)
  {
    string postedUrl = fileName.substr(position+1, fileName.length() - position - 1);
    string postedAction = nds::File::base(fileName.substr(0, position).c_str());
    if (postedAction == "config")
    {
      m_config->postConfiguration(postedUrl);
      m_view->endBookmark();
    }
    else if (postedAction == "update")
    {
      // check for updates!
      checkUpdates();
    }
  }
  else
  {
    // parse the local file and replace %Key% with Value
    localConfigFile(fileName);
  }
}

void Controller::checkUpdates()
{
  if (not m_cache->useCache())
  {
    return;
  }
  Updater * updater = new Updater(*this, *m_document, *m_view);
  m_view->setUpdater(updater);
  updater->show();
}

void Controller::localConfigFile(const std::string & fileName)
{
  nds::File uriFile;
  uriFile.open(fileName.c_str());
  // read the lot
  if (uriFile.is_open())
  {
    vector<string> lines;
    uriFile.readlines(lines);
    m_document->reset();
    ConfigParser configParser(*m_config);
    for (vector<string>::iterator it(lines.begin()); it != lines.end(); ++it)
    {
      string & line(*it);
      configParser.replaceMarkers(line);
      m_document->appendLocalData(line.c_str(), line.length());
      m_document->flush();
    }
    m_document->setStatus(Document::LOADED_HTML);
  }
  else
  {
    // could not load file.
    loadError();
  }

}

void Controller::localFile(const std::string & fileName)
{
  nds::File uriFile;
  uriFile.open(fileName.c_str());
  // read the lot
  if (uriFile.is_open())
  {
    int size = uriFile.size();
    char * data = new char[size+2];
    uriFile.read(data);
    data[size] = 0;
    m_document->reset();
    m_document->appendLocalData(data, size);
    m_document->flush();
    m_document->setStatus(Document::LOADED_HTML);
    delete [] data;
    uriFile.close();
  }
  else
  {
    // could not load file.
    loadError();
  }

}

void Controller::fetchHttp(const URI & uri)
{
  /* this works as follows:
   *
   * if not in the cache
   *   do head request to find mime type
   *   if not a friendly mime type
   *      get save name and wait for saveAs callback
   *      saveAs really gets the file, saving it as needed
   *   else 
   *      really get the file.
   *   endif
   * else
   *   load from the cache
   * endif
   * if have a page
   *   finishFetchHttp 
   * endif
   *
   */
  bool hasPage = false;
  m_httpClient->setUri(uri);
  m_httpClient->reset();
  m_stop = false;
  if (not m_cache->load(uri))
  {
    // loop one, if get, then head
    // if that is ok, then get again
    m_saveAs = NO_SAVE;
    while (not m_httpClient->finished())
    {
      m_httpClient->handleNextState();
      if (m_httpClient->state() > HttpClient::WIFI_OFF)
      {
        m_wifiInit = true;
      }
      if (m_view)
        m_view->tick();
      if (m_stop)
      {
        return;
      }
      waitVBlank();
    }
    hasPage = m_httpClient->hasPage();
    m_httpClient->disconnect();
  }
  else
  {
    // page was cached.
    hasPage = true;
  }

  if (hasPage)
  {
    finishFetchHttp(uri);
  }
}

URI Controller::downloadingFile() const
{
  URI uri(m_document->uri());
  switch (uri.protocol())
  {
    case URI::HTTPS_PROTOCOL:
    case URI::HTTP_PROTOCOL:
      return m_httpClient->uri();

    default:
      return uri;
  }
}

void Controller::finishFetchHttp(const URI & uri)
{
  if (m_document->status() == Document::REDIRECTED and m_document->historyEnabled() and m_redirected < m_maxRedirects)
  {
    // redirected.
    m_redirected++;
    waitVBlank();
    waitVBlank();
    m_document->reset();
    m_document->setStatus(Document::REDIRECTED);
  }
  else
  {
    m_redirected = 0;
    if (m_checkingQueue)
    {
      m_document->setStatus(Document::LOADED_ITEM);
    }
    else
    {
      m_document->setStatus(Document::LOADED_HTML);
    }
    checkSave();
  }
}

void Controller::checkSave()
{
  if (m_saveAs == SAVE_DOWNLOADING)
  {
    saveCurrentFileAs(m_saveFileName.c_str());
    m_saveAs = NO_SAVE;
  }
}

bool Controller::wifiInitialised() const
{
  return m_wifiInit;
}

void Controller::stop()
{
  m_stop = true;
  m_saveAs = NO_SAVE;
  m_saveFileName.clear();
  if (m_checkingQueue)
  {
    const URI &uri(m_httpClient->uri());
    if (uri.protocol() == URI::HTTPS_PROTOCOL or uri.protocol() == URI::HTTP_PROTOCOL)
    {
      m_cache->remove(uri);
      while (not m_downloadQ.empty()) {
        m_downloadQ.pop();
      }
    }
  }
}

bool Controller::stopped() const
{
  return m_stop;
}

Cache * Controller::cache() const
{
  return m_cache;
}

void Controller::setReferer(const URI & referer)
{
  m_httpClient->setReferer(referer);
}

void Controller::clearReferer()
{
  m_httpClient->clearReferer();
}

void Controller::saveCookieSettings()
{
  std::string cookieFile;
  if ( m_config->resource(Config::COOKIE_STR, cookieFile) and not cookieFile.empty())
  {
    nds::File allowed;
    allowed.open(cookieFile.c_str(), "w");
    if (allowed.is_open())
    {
      CookieJar::AcceptedDomainSet domains;
      m_document->cookieJar()->acceptedDomains(domains);
      for (CookieJar::AcceptedDomainSet::const_iterator it(domains.begin());
          it != domains.end(); ++it)
      {
        allowed.write(it->c_str());
        allowed.write("\n");
      }
    }
  }
}

void Controller::queueUri(const URI & uri)
{
  if (uri.protocol() == URI::HTTPS_PROTOCOL or
      uri.protocol() == URI::HTTP_PROTOCOL)
  {
    m_downloadQ.push(uri);
  }
}

void Controller::checkDownloadQueue()
{
  m_document->setHistoryEnabled(false);
  m_checkingQueue = true;
  while (m_downloadQ.size())
  {
    URI uri(m_downloadQ.front());
    m_downloadQ.pop();
    fetchHttp(uri);
  }
  m_document->setHistoryEnabled(true);
  m_document->setStatus(Document::LOADED_PAGE);
  m_checkingQueue = false;
}

void Controller::waitVBlank() const
{
  if (m_view)
    cothread_yield_irq(IRQ_VBLANK);
}
