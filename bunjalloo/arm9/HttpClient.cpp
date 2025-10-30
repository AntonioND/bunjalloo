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

  This file incorporates work covered by the following copyright and  
  permission notice:  

     Copyright (c) PeerSec Networks, 2002-2007. All Rights Reserved.
     The latest version of this code is available at http://www.matrixssl.org

     This software is open source; you can redistribute it and/or modify
     it under the terms of the GNU General Public License as published by
     the Free Software Foundation; either version 2 of the License, or
     (at your option) any later version.

     This General Public License does NOT permit incorporating this software 
     into proprietary programs.  If you are unable to comply with the GPL, a 
     commercial license for this software may be purchased from PeerSec Networks
     at http://www.peersec.com
     
     This program is distributed in WITHOUT ANY WARRANTY; without even the 
     implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
     See the GNU General Public License for more details.
     
     You should have received a copy of the GNU General Public License
     along with this program; if not, write to the Free Software
     Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
     http://www.gnu.org/copyleft/gpl.html
*/
#include <string>
#include <algorithm>
#include <cstring>
#include <cstdio>
#include "libnds.h"
#include "System.h"
#include "Config.h"
#include "Controller.h"
#include "CookieJar.h"
#include "Document.h"
#include "HttpClient.h"
#include "Language.h"
#include "URI.h"
#include "File.h"
#include "Wifi9.h"


using namespace std;
using nds::Wifi9;

static const int MAX_CONNECT_ATTEMPTS(3);
extern const char * VERSION;

HttpClient::HttpClient():
  nds::Client("", 0),
  m_total(0),
  m_finished(false),
  m_connectAttempts(0),
  m_state(WIFI_OFF),
  m_controller(0),
  m_maxConnectAttempts(MAX_CONNECT_ATTEMPTS),
  m_log(false)
{
}

HttpClient::~HttpClient()
{
}

std::string HttpClient::proxyString() const
{
  string proxy;
  if (m_controller
      and m_controller->config().resource(Config::PROXY_STR, proxy)) {
    return proxy;
  }
  return "";
}

void HttpClient::setController(Controller * c)
{
  m_controller = c;
  const string &proxy(proxyString());
  if (not proxy.empty())
  {
    URI uri(proxy);
    setConnection(uri.server().c_str(), uri.port());
  }
  m_controller->config().resource("logger", m_log);
  // TODO(Antonio): Enable certificate checks
  // Load CA certificates
  string caFile;
  if (m_controller->config().resource(Config::CERT_FILE, caFile))
  {
    sslLoadCerts(caFile);
  }

  m_maxConnectAttempts = 3;
  m_controller->config().resource(Config::MAX_CONNECT, m_maxConnectAttempts);
  if (m_maxConnectAttempts == 0)
  {
    m_maxConnectAttempts = MAX_CONNECT_ATTEMPTS;
  }
}

// implement the pure virtual functions
void HttpClient::handle(void * bufferIn, int amountRead)
{
  handleRaw(bufferIn, amountRead);
}

void HttpClient::handleRaw(void * bufferIn, int amountRead)
{
  char * buffer = (char*)bufferIn;
  buffer[amountRead] = 0;
  // printf("%s", buffer);
  if (m_controller) m_controller->m_document->appendData(buffer, amountRead);
  m_total += amountRead;
  // FIXME: cache this?
  if (m_uri.method() == "HEAD")
  {
    if (strstr(buffer, "\r\n\r\n"))
      m_finished = true;
  }
  //printf("0x0x End of buffer x0x0", buffer);
}

bool HttpClient::finished() const
{
  return m_finished;
}

void HttpClient::finish() {
  //printf("%d\n",m_total);
  if (m_total == 0)
  {
    if (m_controller)
      m_controller->loadError();
  }
  else
  {
    m_controller->m_document->setStatus(Document::LOADED_HTML);
  }
}

void HttpClient::print(const char * s)
{
  debug(s);
}
void HttpClient::debug(const char * s)
{
  if (m_log) {
    nds::File log;
    log.open("bunjalloo.log", "a+");
    log.write(s);
    log.write("\n");
    //printf("debug:%s\n",s);
  }
}

void HttpClient::proxyConnect()
{
  const string &proxy(proxyString());
  if (!proxy.empty())
  {
    // need to trick the proxy into providing the TCP/IP tunnel.
    string s;
    s += "CONNECT ";
    s += m_uri.server();
    s += ":";
    char buffer[256];
    snprintf(buffer,256, "%d", m_uri.port());
    s += buffer;
    s += " HTTP/1.1\r\n";
    s += "Proxy-Connection: keep-alive\r\n";
    s += "Host:";
    s += m_uri.server();
    s += "\r\n\r\n";
    write(s.c_str(), s.length());
    // read the response - it doesn't interest us much (yet)
    read();
  }
}

std::string HttpClient::cookieString(const URI &uri) const
{
  string cookies;
  if (m_controller) {
    m_controller->m_document->cookieJar()->cookiesForRequest(uri, cookies);
  }
  return cookies;
}

std::string HttpClient::filenamePart(const URI &uri) const
{
  const string &proxy(proxyString());
  if (!proxy.empty())
  {
    // for proxy connection, need to send the whole request:
    return uri.asString();
  }
  return uri.fileName();
}

std::string HttpClient::userAgent() const
{
  std::string useragent;
  if (m_controller and m_controller->config().resource(Config::USER_AGENT_STR, useragent))
  {
    return useragent;
  }
  else {
    useragent += "Bunjalloo/";
    useragent += VERSION;
    useragent += "(";
    useragent += nds::System::uname();
    useragent += ";U;";
    useragent += Language::instance().currentLanguage();
    useragent += ")";
    return useragent;
  }
}

// GET stuff
void HttpClient::get(const URI & uri)
{
  if (isConnected())
  {
    string s;
    s += uri.method();
    s += " ";
    s += filenamePart(uri);
    s += " HTTP/1.1\r\n";
    s += "Host:" + uri.server()+"\r\n";
    if (uri.method() != "HEAD")
    {
      // HEAD requests can leave the connection open for reuse, since they know
      // when the data ends. Everyone else should request that the server
      // closes the connection, since it is "impossible" to know when a page
      // ends.
      s += "Connection: close\r\n";
    }
    s += "Accept-Language: ";
    s += Language::instance().currentLanguage();
    s += "\r\n";
    // Only send referrer to the same server.
    if (not m_referer.server().empty()
        and m_referer.server() == uri.server())
    {
      s += "Referer: "; s += m_referer.asString();
      s += "\r\n";
    }
    s += "Accept-charset: ISO-8859-1,UTF-8\r\n";
    //If the Accept-Encoding field-value is empty, then only the "identity" encoding is acceptable.
    // -- RFC2616-sec14
    s += "Accept-encoding: gzip,deflate\r\n";
    s += "Accept: text/html,image/png,image/jpeg,image/gif,text/plain\r\n";

    s += "User-Agent: ";
    s += userAgent();
    s += "\r\n";

    s += cookieString(uri);
    if (uri.requestHeader().empty())
    {
      s += "\r\n";
    }
    else
    {
      s += uri.requestHeader();
    }
    write(s.c_str(), s.length());
    m_finished = false;
  }
}

void HttpClient::wifiConnection()
{
  switch (Wifi9::instance().status())
  {
    case Wifi9::CANNOTCONNECT:
      debug("FAILED Wifi9::CANNOTCONNECT");
      m_state = FAILED;
      break;

    case Wifi9::DISCONNECTED:
    case Wifi9::SEARCHING:
    case Wifi9::AUTHENTICATING:
    case Wifi9::ASSOCIATING:
    case Wifi9::ACQUIRINGDHCP:
      break;

    case Wifi9::ASSOCIATED:
      m_state = CONNECT_SOCKET;
      break;

  }
}

void HttpClient::handleNextState()
{
  switch (m_state)
  {
    case WIFI_OFF:
      Wifi9::instance().connect();
      m_state = CONNECT_WIFI;
      break;

    case CONNECT_WIFI:
      wifiConnection();
      m_connectAttempts = 0;
      m_reconnects = 0;
      break;

    case CONNECT_SOCKET:
      // connect to the socket.
      setTimeout(1);
      this->connect();
      if (isConnected())
      {
        debug("Connected, setup proxy and SSL");
        m_state = PROXY_SSL_HANDSHAKE;
        m_connectAttempts = 0;
      }
      else
      {
        m_connectAttempts++;
        if (m_connectAttempts == m_maxConnectAttempts)
        {
          debug("FAILED m_maxConnectAttempts");
          m_state = FAILED;
        }
      }
      break;

    case PROXY_SSL_HANDSHAKE:
      {
        proxyConnect();

        if (m_uri.protocol() == URI::HTTPS_PROTOCOL)
        {
          // Do the SSL handshake now
          int result = this->sslEnable();
          if (result == -1)
          {
            debug("SSL handshake failed");
            m_state = FAILED;
          }
          else
          {
            debug("SSL handshake ok!");
            m_state = GET_URL;
          }
        }
        else
        {
          debug("No SSL handshake");
          m_state = GET_URL;
        }
      }
      break;

    case GET_URL:
      setTimeout(5);
      get(m_uri);
      m_controller->waitVBlank();
      m_controller->waitVBlank();
      m_state = READING_ALL;
      break;

    case READING_FIRST:
#if 0
      // read something, anything, to make sure all is well
      setTimeout(5);
      readFirst();
#endif
      m_state = READING_ALL;
      break;

    case READING_ALL:
      // now we know the server is connected, read the remaining bytes.
      setTimeout(1);
      readAll();
      break;

    case FINISHED:
      m_finished = true;
#if 0
      // TODO(Antonio): Free SSL information
      m_sslClient->freeSession();
#endif
      break;

    case FAILED:
      m_total = 0;
      m_finished = true;
      finish();
      break;
  }
}

bool HttpClient::hasPage() const
{
  return m_state == FINISHED;
}

void HttpClient::readFirst()
{
  // printf("Read First\n");
  int read = this->read();

  switch (read)
  {
    case CONNECTION_CLOSED:
      debug("readFirst returned 0 - Failure");
      m_state = FAILED;
      break;

    case READ_ERROR:
      {
        // not be ready yet. This is the select() returning early.
        m_connectAttempts++;
        if (m_connectAttempts >= m_maxConnectAttempts)
        {
          // once we reach the maximum number, retry the socket.
          this->disconnect();
          m_connectAttempts = 0;
          m_state = CONNECT_SOCKET;
          m_reconnects++;
          if (m_reconnects == 3)
          {
            debug("FAILED m_reconnects == 3");
            m_state = FAILED;
          }
        }
        m_controller->waitVBlank();
        m_controller->waitVBlank();
      }
      break;

    case RETRY_LATER:
      /* Keep going! */
      debug("RETRY_LATER readFirst");
      m_controller->waitVBlank();
      m_controller->waitVBlank();
      m_connectAttempts++;
      if (m_connectAttempts == m_maxConnectAttempts) {
        m_state = FAILED;
      }
      break;

    default:
      m_state = READING_ALL;
      m_connectAttempts = 0;
      break;
  }

}

void HttpClient::readAll()
{
  // printf("Read All\n");
  int read = this->read();

  switch (read)
  {
    case READ_ERROR:
      m_state = FINISHED;
      break;
    case CONNECTION_CLOSED:
      debug("readAll returned 0 - set to FINISHED");
      m_state = FINISHED;
      break;

    case RETRY_LATER:
      /* Keep going */
      debug("RETRY_LATER readAll");
      m_connectAttempts++;
      if (m_controller->m_document->uri() != m_uri.asString())
      {
        // redirected.
        debug("Redirect:");
        debug(m_controller->m_document->uri().c_str());
        m_state = FINISHED;
      }
      if (m_connectAttempts == m_maxConnectAttempts) {
        debug("m_maxConnectAttempts reached - surely it is done...");
        m_state = FINISHED;
      } else {
        m_controller->waitVBlank();
        m_controller->waitVBlank();
        m_controller->waitVBlank();
        m_controller->waitVBlank();
        m_controller->waitVBlank();
      }
      break;

    default:
      // ok - there may be more bytes.
      m_state = READING_ALL;
      m_connectAttempts = 0;
      break;
  }
}

HttpClient::ConnectionState HttpClient::state() const
{
  return m_state;
}

void HttpClient::setUri(const URI & uri)
{
  m_uri = uri;
  m_state = GET_URL;
  m_finished = false;
  const string &proxy(proxyString());
  if (proxy.empty()) {
    // when no proxy, connect directly to the remote http server
    setConnection(uri.server().c_str(), uri.port());
  }
}

const URI & HttpClient::uri() const
{
  return m_uri;
}

void HttpClient::reset()
{
  disconnect();
  m_total = 0;
  m_finished = false;
  m_connectAttempts = 0;
  m_state = WIFI_OFF;
  m_maxConnectAttempts = MAX_CONNECT_ATTEMPTS;
}

void HttpClient::setReferer(const URI & referer)
{
  bool fullReferer(false);
  m_controller->config().resource(Config::FULL_REF, fullReferer);
  if (referer.server().empty() or fullReferer)
  {
    m_referer = referer;
  }
  else
  {
    // only send the top level path
    m_referer = referer.navigateTo("/");
  }
}


void HttpClient::clearReferer()
{
  m_referer = URI("");
}
