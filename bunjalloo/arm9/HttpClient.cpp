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
#include "matrixSsl.h"

using namespace std;
using nds::Wifi9;

static const int MAX_CONNECT_ATTEMPTS(10);
extern const char * VERSION;
sslKeys_t * s_sslKeys(0);

class SslClient
{
  public:
    SslClient(HttpClient & httpClient)
      : m_httpClient(httpClient),
      m_conn(0),
      m_sessionId(NULL),
      m_buffer(new char[SSL_BUFFER_SIZE]),
      m_cipherSuite(0)
    {}
    ~SslClient();

    // set up session, handshake etc.
    int sslConnect();

    // write via matrixSslEncode
    int write(const char * buf, int len);

    // read via matrixSslDecode
    int read();

    void handle(void * bufferIn, int amountRead);

    void freeSession();

  private:

    const static int SSL_BUFFER_SIZE;
    const static int SOCKET_ERROR;

    HttpClient & m_httpClient;
    struct sslConn_t
    {
      ssl_t     *ssl;
      sslBuf_t  inbuf;
      sslBuf_t  insock;
      sslBuf_t  outsock;
      int       outBufferCount;
    };
    sslConn_t * m_conn;
    sslSessionId_t *m_sessionId;
    char * m_buffer;
    short m_cipherSuite;
    int m_lastRead;

    static int32 certChecker(sslCertInfo_t * cert, void * arg);

    int sslHandshake();

    int writeBuffer(sslBuf_t * out);

    int sslRead(char * buf, int len);

    void freeConnection();
};

const int SslClient::SSL_BUFFER_SIZE(HttpClient::BUFFER_SIZE);
const int SslClient::SOCKET_ERROR(-1);

SslClient::~SslClient()
{
  freeConnection();
  delete [] m_buffer;
}

void SslClient::freeSession()
{
  if (not m_conn) {
    return;
  }
  matrixSslFreeSessionId(m_sessionId);
  matrixSslDeleteSession(m_conn->ssl);
  m_conn->ssl = 0;
}

void SslClient::freeConnection()
{
  if (not m_conn) {
    return;
  }
  freeSession();

  if (m_conn->insock.buf) {
    free(m_conn->insock.buf);
    m_conn->insock.buf = 0;
  }
  if (m_conn->outsock.buf) {
    free(m_conn->outsock.buf);
    m_conn->outsock.buf = 0;
  }
  if (m_conn->inbuf.buf) {
    free(m_conn->inbuf.buf);
    m_conn->inbuf.buf = 0;
  }
  free(m_conn);
  m_conn = 0;
}

int32 SslClient::certChecker(sslCertInfo_t * cert, void * arg)
{
  sslCertInfo_t   *next;
  sslKeys_t               *keys;
  /* Make sure we are checking the last cert in the chain */
  next = cert;
  keys = (sslKeys_t*)arg;
  while (next->next != 0) {
    next = next->next;
  }
#if ENFORCE_CERT_VALIDATION
  /* This case passes the true RSA authentication status through */
  return next->verified;
#else
  /* This case passes an authenticated server through, but flags a
     non-authenticated server correctly.  The user can call the
     matrixSslGetAnonStatus later to see the status of this connection. */
  if (next->verified != 1) {
    return SSL_ALLOW_ANON_CONNECTION;
  }
  return next->verified;
#endif
}

void SslClient::handle(void * bufferIn, int amountRead)
{
  // called when we read SSL data.
  int toRead = (int)((m_conn->insock.buf + m_conn->insock.size) - m_conn->insock.end);
  if (toRead > amountRead) {
    toRead = amountRead;
  }
  m_lastRead = amountRead;
  ::memcpy(m_conn->insock.end, bufferIn, amountRead);
}

int SslClient::sslConnect()
{
  if (!m_conn)
  {
    m_conn = (sslConn_t*)calloc(sizeof(sslConn_t), 1);
  }
  if (matrixSslNewSession(&m_conn->ssl,
        s_sslKeys, m_sessionId, m_cipherSuite) < 0)
  {
    // printf("Starting session error\n");
    return -1;
  }
  matrixSslSetCertValidator(m_conn->ssl, SslClient::certChecker, s_sslKeys);

  int result = sslHandshake();
  return result;
}

int SslClient::sslHandshake()
{
  // define buffers
  m_conn->insock.size = SSL_BUFFER_SIZE;
  m_conn->insock.start =
    m_conn->insock.end =
    m_conn->insock.buf = (unsigned char *)realloc(m_conn->insock.buf, m_conn->insock.size);
  m_conn->outsock.size = SSL_BUFFER_SIZE;
  m_conn->outsock.start =
    m_conn->outsock.end =
    m_conn->outsock.buf = (unsigned char *)realloc(m_conn->outsock.buf, m_conn->outsock.size);
  m_conn->inbuf.size = 0;
  m_conn->inbuf.start =
    m_conn->inbuf.end =
    m_conn->inbuf.buf = 0;

  int bytes = matrixSslEncodeClientHello(m_conn->ssl, &m_conn->outsock, m_cipherSuite);
  if (bytes < 0) {
    return -1;
  }
  // write the hello bytes.
  int written = writeBuffer(&m_conn->outsock);
  //printf("Written %d bytes in Hello\n", written);
  m_httpClient.print("sslHandshake - after write");
  if (written < 0)
  {
    return -1;
  }
  m_conn->outsock.start = m_conn->outsock.end = m_conn->outsock.buf;

  m_httpClient.print("Read back hello respose\n");
  // read back the data.

  int result(-1);
  m_lastRead = 0;
  do
  {
    m_httpClient.print("Read hello respose");
    result = sslRead(m_buffer, SSL_BUFFER_SIZE);
    //printf("hello response read - result %d\n", result);
  } while (result == 0 and not matrixSslHandshakeIsComplete(m_conn->ssl));

  //printf("hello response ended - result %d\n", result);
  return result;
}

int SslClient::writeBuffer(sslBuf_t * out)
{
  unsigned char *s;
  int bytes;
  s = out->start;
  while (out->start < out->end) {
    if (m_httpClient.isConnected())
    {
      bytes = m_httpClient.write(out->start, (int)(out->end - out->start));
      if (bytes == SOCKET_ERROR) {
        //printf("writeBuffer error\n");
        return -1;
      }
      if (bytes == 0)
        break;
      out->start += bytes;
    }
    else
    {
      return -1;
    }
  }
  return (int)(out->start - s);

}

int SslClient::read()
{
  m_lastRead = 0;
  int result = sslRead(m_buffer, SSL_BUFFER_SIZE);
  if (result < 0)
  {
    return HttpClient::CONNECTION_CLOSED;
  }
  //printf("SslClient::read - read %d, now handleRaw...\n", result);
  if (result == 0)
  {
    if (m_httpClient.state() == HttpClient::READING_FIRST)
      return HttpClient::READ_ERROR;
    else if (m_lastRead)
      return HttpClient::RETRY_LATER;
    else
      return HttpClient::READ_ERROR;
  }
  m_httpClient.handleRaw(m_buffer, result);
  return result;
}

int SslClient::write(const char * buf, int len)
{
  // write with SSL
  int rc;

  /* Pack the buffered socket data (if any) so that start is at zero. */
  if (m_conn->outsock.buf < m_conn->outsock.start) {
    if (m_conn->outsock.start == m_conn->outsock.end) {
      m_conn->outsock.start = m_conn->outsock.end = m_conn->outsock.buf;
    } else {
      memmove(m_conn->outsock.buf, m_conn->outsock.start, m_conn->outsock.end - m_conn->outsock.start);
      m_conn->outsock.end -= (m_conn->outsock.start - m_conn->outsock.buf);
      m_conn->outsock.start = m_conn->outsock.buf;
    }
  }
  /*
     If there is buffered output data, the caller must be trying to
     send the same amount of data as last time.  We don't support 
     sending additional data until the original buffered request has
     been completely sent.
     */
  if (m_conn->outBufferCount > 0 && len != m_conn->outBufferCount) {
    // socketAssert(len != m_conn->outBufferCount);
    return -1;
  }
  /*
     If we don't have buffered data, encode the caller's data
     */
  if (m_conn->outBufferCount == 0) {
retryEncode:
    rc = matrixSslEncode(m_conn->ssl, (unsigned char *)buf, len, &m_conn->outsock);
    switch (rc) {
      case SSL_ERROR:
        return -1;
      case SSL_FULL:
        if (m_conn->outsock.size > SSL_MAX_BUF_SIZE) {
          return -1;
        }
        m_conn->outsock.size *= 2;
        m_conn->outsock.buf =
          (unsigned char *)realloc(m_conn->outsock.buf, m_conn->outsock.size);
        m_conn->outsock.end = m_conn->outsock.buf + (m_conn->outsock.end - m_conn->outsock.start);
        m_conn->outsock.start = m_conn->outsock.buf;
        goto retryEncode;
    }
  }
  /*
     We've got data to send.
     */
  rc = m_httpClient.write((char *)m_conn->outsock.start, (int)(m_conn->outsock.end - m_conn->outsock.start));
  if (rc == SOCKET_ERROR) {
    // *status = getSocketError();
    return -1;
  }
  m_conn->outsock.start += rc;
  /*
     If we wrote it all return the length, otherwise remember the number of
     bytes passed in, and return 0 to be called again later.
     */
  if (m_conn->outsock.start == m_conn->outsock.end) {
    m_conn->outBufferCount = 0;
    return len;
  }
  m_conn->outBufferCount = len;
  return 0;
}

int SslClient::sslRead(char * buf, int len)
{
  int bytes, rc;
  unsigned char error, alertLevel, alertDescription, performRead;

  if (m_conn->ssl == 0 || len <= 0) {
    m_httpClient.print("sslRead failed - ssl nul or len <= 0\n");
    return -1;
  }
  /* If inbuf is valid, then we have previously decoded data that must be
     returned, return as much as possible.  Once all buffered data is
     returned, free the inbuf.  */
  if (m_conn->inbuf.buf) {
    if (m_conn->inbuf.start < m_conn->inbuf.end) {
      int remaining = (int)(m_conn->inbuf.end - m_conn->inbuf.start);
      bytes = (int)min(len, remaining);
      memcpy(buf, m_conn->inbuf.start, bytes);
      m_conn->inbuf.start += bytes;
      return bytes;
    }
    free(m_conn->inbuf.buf);
    m_conn->inbuf.buf = 0;
  }
  /*
     Pack the buffered socket data (if any) so that start is at zero.
     */
  if (m_conn->insock.buf < m_conn->insock.start) {
    if (m_conn->insock.start == m_conn->insock.end) {
      m_conn->insock.start = m_conn->insock.end = m_conn->insock.buf;
    } else {
      memmove(m_conn->insock.buf, m_conn->insock.start, m_conn->insock.end - m_conn->insock.start);
      m_conn->insock.end -= (m_conn->insock.start - m_conn->insock.buf);
      m_conn->insock.start = m_conn->insock.buf;
    }
  }
  /* Read up to as many bytes as there are remaining in the buffer.  We could
     Have encrypted data already cached in m_conn->insock, but might as well read more
     if we can.  */
  performRead = 0;
readMore:
  if (m_conn->insock.end == m_conn->insock.start || performRead) {
    performRead = 1;
    int result = m_httpClient.read((int)((m_conn->insock.buf + m_conn->insock.size) - m_conn->insock.end));
    // read() causes this->handle() to be called.
    if (m_lastRead == SOCKET_ERROR and result == HttpClient::READ_ERROR) {
      // *status = getSocketError();
      //printf("sslRead failed - m_lastRead %d result %d\n", m_lastRead, result);
      m_httpClient.print("sslRead failed - SOCKET_ERROR and READ_ERROR");
      return -1;
    }
    if (m_lastRead == 0 and result == HttpClient::CONNECTION_CLOSED) {
      // *status = SSLSOCKET_EOF;
      m_httpClient.print("sslRead failed - lastRead 0 and CONNECTION_CLOSED");
      return 0;
    }
    m_conn->insock.end += m_lastRead;
    char debugBuffer[512];
    snprintf(debugBuffer, 512, "readMore result - m_lastRead %d result %d\n", m_lastRead, result);
    m_httpClient.print(debugBuffer);
  }
  /* Define a temporary sslBuf */
  m_conn->inbuf.start = m_conn->inbuf.end = m_conn->inbuf.buf = (unsigned char*)malloc(len);
  m_conn->inbuf.size = len;
  /* Decode the data we just read from the socket */
decodeMore:
  error = 0;
  alertLevel = 0;
  alertDescription = 0;

  rc = matrixSslDecode(m_conn->ssl, &m_conn->insock, &m_conn->inbuf,
                       &error, &alertLevel, &alertDescription);
  switch (rc) {
    /*
       Successfully decoded a record that did not return data or require a response.
       */
    case SSL_SUCCESS:
      //printf("SSL_SUCCESS no data...\n");
      return 0;
      /*
         Successfully decoded an application data record, and placed in tmp buf
         */
    case SSL_PROCESS_DATA:
      /*
         Copy as much as we can from the temp buffer into the caller's buffer
         and leave the remainder in m_conn->inbuf until the next call to read
         It is possible that len > data in buffer if the encoded record
         was longer than len, but the decoded record isn't!
         */
      //printf("Process Data...\n");
      rc = (int)(m_conn->inbuf.end - m_conn->inbuf.start);
      rc = min(rc, len);
      memcpy(buf, m_conn->inbuf.start, rc);
      m_conn->inbuf.start += rc;
      return rc;
      /*
         We've decoded a record that requires a response into tmp
         If there is no data to be flushed in the out buffer, we can write out
         the contents of the tmp buffer.  Otherwise, we need to append the data 
         to the outgoing data buffer and flush it out.
         */
    case SSL_SEND_RESPONSE:
      m_httpClient.print("SSL_SEND_RESPONSE...");
      bytes = m_httpClient.write((char *)m_conn->inbuf.start,
                                 (int)(m_conn->inbuf.end - m_conn->inbuf.start));
      if (bytes == SOCKET_ERROR) {
        // *status = getSocketError();
        //if (*status != WOULD_BLOCK) {
        //  fprintf(stdout, "Socket send error:  %d\n", *status);
        //  goto readError;
        //}
        //*status = 0;
      }
      m_conn->inbuf.start += bytes;
      if (m_conn->inbuf.start < m_conn->inbuf.end) {
        /* This must be a non-blocking socket since it didn't all get sent
           out and there was no error.  We want to finish the send here
           simply because we are likely in the SSL handshake.  */
#if 0
        setSocketBlock(m_conn->fd);
        bytes = send(m_conn->fd, (char *)m_conn->inbuf.start,
            (int)(m_conn->inbuf.end - m_conn->inbuf.start), MSG_NOSIGNAL);
        if (bytes == SOCKET_ERROR) {
          *status = getSocketError();
          goto readError;
        }
        m_conn->inbuf.start += bytes;
        socketAssert(m_conn->inbuf.start == m_conn->inbuf.end);
        /* Can safely set back to non-blocking because we wouldn't
           have got here if this socket wasn't non-blocking to begin with.  */
        setSocketNonblock(m_conn->fd);
#endif
      }
      m_conn->inbuf.start = m_conn->inbuf.end = m_conn->inbuf.buf;
      return 0;
      /* There was an error decoding the data, or encoding the out buffer.
         There may be a response data in the out buffer, so try to send.
         We try a single hail-mary send of the data, and then close the socket.
         Since we're closing on error, we don't worry too much about a clean flush.  */
    case SSL_ERROR:
      //fprintf(stderr, "SSL: Closing on protocol error %d\n", error);
      if (m_conn->inbuf.start < m_conn->inbuf.end) {
        // setSocketNonblock(m_conn->fd);
        /*bytes = send(m_conn->fd, (char *)m_conn->inbuf.start, 
            (int)(m_conn->inbuf.end - m_conn->inbuf.start), MSG_NOSIGNAL);*/
        bytes = m_httpClient.write((char *)m_conn->inbuf.start,
                      (int)(m_conn->inbuf.end - m_conn->inbuf.start));
      }
      goto readError;
      /*
         We've decoded an alert.  The level and description passed into
         matrixSslDecode are filled in with the specifics.
         */
    case SSL_ALERT:
      if (alertDescription == SSL_ALERT_CLOSE_NOTIFY) {
        //*status = SSLSOCKET_CLOSE_NOTIFY;
        goto readZero;
      }
      //fprintf(stderr, "SSL: Closing on client alert %d: %d\n", alertLevel, alertDescription);
      goto readError;
      /*
         We have a partial record, we need to read more data off the socket.
         If we have a completely full m_conn->insock buffer, we'll need to grow it
         here so that we CAN read more data when called the next time.
         */
    case SSL_PARTIAL:
      m_httpClient.print("SSL_PARTIAL...");
      if (m_conn->insock.start == m_conn->insock.buf && m_conn->insock.end ==
          (m_conn->insock.buf + m_conn->insock.size)) {
        if (m_conn->insock.size > SSL_MAX_BUF_SIZE) {
          m_httpClient.print("size > SSL_MAX_BUF_SIZE\n");
          goto readError;
        }
        m_conn->insock.size *= 2;
        m_conn->insock.start = m_conn->insock.buf =
          (unsigned char *)realloc(m_conn->insock.buf, m_conn->insock.size);
        m_conn->insock.end = m_conn->insock.buf + (m_conn->insock.size / 2);
      }
      if (!performRead) {
        performRead = 1;
        free(m_conn->inbuf.buf);
        m_conn->inbuf.buf = 0;
        m_httpClient.print("SSL_PARTIAL...readMore");
        goto readMore;
      } else {
        m_httpClient.print("SSL_PARTIAL...readZero (end)");
        goto readZero;
      }
      /*
         The out buffer is too small to fit the decoded or response
         data.  Increase the size of the buffer and call decode again
         */
    case SSL_FULL:
      //printf("SSL_FULL...\n");
      m_conn->inbuf.size *= 2;
      if (m_conn->inbuf.buf != (unsigned char*)buf) {
        free(m_conn->inbuf.buf);
        m_conn->inbuf.buf = 0;
      }
      m_conn->inbuf.start = m_conn->inbuf.end = m_conn->inbuf.buf = 
        (unsigned char *)malloc(m_conn->inbuf.size);
      goto decodeMore;
  }
  /*
     We consolidated some of the returns here because we must ensure
     that m_conn->inbuf is cleared if pointing at caller's buffer, otherwise
     it will be freed later on.
     */
readZero:
  if (m_conn->inbuf.buf == (unsigned char*)buf) {
    m_conn->inbuf.buf = 0;
  }
  return 0;
readError:
  if (m_conn->inbuf.buf == (unsigned char*)buf) {
    m_conn->inbuf.buf = 0;
  }
  //printf("sslRead failed - readError\n");
  return -1;
}


HttpClient::HttpClient():
  nds::Client("",0),
  m_total(0),
  m_finished(false),
  m_connectAttempts(0),
  m_state(WIFI_OFF),
  m_controller(0),
  m_maxConnectAttempts(MAX_CONNECT_ATTEMPTS),
  m_hasSsl(true),
  m_sslClient(new SslClient(*this)),
  m_log(false)
{
}

HttpClient::~HttpClient()
{
  delete m_sslClient;
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
  string caFile;
  if (m_hasSsl and s_sslKeys == 0 and m_controller->config().resource(Config::CERT_FILE, caFile))
  {
    // initialise keys - this is a one off deal
    if (matrixSslReadKeys(&s_sslKeys, 0, 0, 0, caFile.c_str()) < 0)
    {
      m_hasSsl = false;
    }
  }
  m_maxConnectAttempts = 60;
  m_controller->config().resource(Config::MAX_CONNECT, m_maxConnectAttempts);
  if (m_maxConnectAttempts == 0)
  {
    m_maxConnectAttempts = MAX_CONNECT_ATTEMPTS;
  }
}

// implement the pure virtual functions
void HttpClient::handle(void * bufferIn, int amountRead)
{
  // if in SSL mode, need to see if we have read enough...
  if (isSsl())
  {
    m_sslClient->handle(bufferIn, amountRead);
  }
  else
  {
    if (m_state != SSL_HANDSHAKE)
    {
      handleRaw(bufferIn, amountRead);
    }
  }
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
  if (isSsl() and not proxy.empty())
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
    m_hasSsl = false;
    read();
    m_hasSsl = true;
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
  if (not isSsl() and not proxy.empty())
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
    if (isSsl())
    {
      debug("Write SSL");
      int result = m_sslClient->write(s.c_str(), s.length());
      if (result < 0)
      {
        debug("Write SSL failed -1");
      }
      else
      {
        debug("Write SSL >= 0");
      }
    }
    else
    {
      write(s.c_str(), s.length());
    }
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
        debug("Connected, get URL");
        if (isSsl())
        {
          m_state = SSL_HANDSHAKE;
        }
        else
        {
          m_state = GET_URL;
          m_controller->waitVBlank();
          m_controller->waitVBlank();
        }
        m_connectAttempts = 0;
      }
      else
      {
        m_connectAttempts++;
        if (m_connectAttempts == m_maxConnectAttempts)
        {
          debug("FAILED m_connectAttempts == m_maxConnectAttempts");
          m_state = FAILED;
        }
      }
      break;

    case SSL_HANDSHAKE:
      {
        proxyConnect();
        int result = m_sslClient->sslConnect();
        if (result == -1)
        {
          debug("SSL_HANDSHAKE failed");
          m_state = FAILED;
        }
        else {
          debug("SSL_HANDSHAKE OK!");
          m_state = GET_URL;
        }
      }
      break;

    case GET_URL:
      setTimeout(5);
      get(m_uri);
      m_controller->waitVBlank();
      m_controller->waitVBlank();
      m_state = READING_FIRST;
      break;

    case READING_FIRST:
      // read something, anything, to make sure all is well
      setTimeout(5);
      readFirst();
      break;

    case READING_ALL:
      // now we know the server is connected, read the remaining bytes.
      setTimeout(1);
      readAll();
      break;

    case FINISHED:
      m_finished = true;
      m_sslClient->freeSession();
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
  int read;
  if (isSsl())
  {
    read = m_sslClient->read();
  }
  else
  {
    read = this->read();
  }
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
  int read;
  if (isSsl())
  {
    read = m_sslClient->read();
  }
  else
  {
    read = this->read();
  }
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

bool HttpClient::isSsl() const
{
  return m_hasSsl and m_uri.protocol() == URI::HTTPS_PROTOCOL;
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
