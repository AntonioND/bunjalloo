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
#define DEBUG_WITH_SSTREAM 0
#include "Client.h"
#include "Client_platform.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#if DEBUG_WITH_SSTREAM
#include <sstream>
#endif
#include <unistd.h>
#include <stdio.h>
#include <algorithm>
#include <functional>
#include <string>
#include <errno.h>

using namespace std;
using namespace nds;
const int Client::BUFFER_SIZE(1024*16);

Client::Client(const char * ip, int port):
  m_ip(0),
  m_port(port),
  m_tcp_socket(0),
  m_connected(false),
  m_sslEnabled(false),
  m_timeout(TIMEOUT),
  m_connectState(CLIENT_CONNECT_INITIAL),
  m_socketAddress(0)
{
  setConnection(ip, port);
}

Client::~Client()
{
  if (isConnected())
  {
    ::close(m_tcp_socket);
  }
  free(m_ip);
}

void Client::setConnection(const char * ip, int port)
{
  free(m_ip);
  m_ip = (char*)malloc(strlen(ip)+1);
  memcpy(m_ip, ip, strlen(ip));
  m_ip[strlen(ip)] = 0;
  m_port = port;
}

void Client::disconnect()
{
  if (isConnected())
    ::close(m_tcp_socket);
  m_connected = false;
  m_connectState = CLIENT_CONNECT_INITIAL;
}

void Client::makeNonBlocking() {
  int i(1);
  int iotclResult = ::ioctl(m_tcp_socket, FIONBIO, (char *)&i);
  if (iotclResult == -1) {
    debug("iotcl non blocking failed");
    // never happens on DS...
  }
}

bool Client::tryConnect(sockaddr_in &socketAddress)
{
  makeNonBlocking();
  socklen_t addrlen = sizeof(struct sockaddr_in);
  errno = 0;
  int result = ::connect(m_tcp_socket, (struct sockaddr*)&socketAddress, addrlen);
  int tmperr = errno;
  if (result == 0) {
    // connected immediately
    m_connected = true;
  }
  else {
    switch (tmperr) {
      case EINPROGRESS:
        {
          m_connectState = CLIENT_CONNECT_EINPROGRESS;
        }
        break;
      default:
        debug("connect() error)");
        return false;
    }
  }
  return m_connected;
}

void Client::connectInitial()
{
  // DS - must create the socket moments before using it!
  m_tcp_socket = socket(AF_INET, SOCK_STREAM, 0);

  struct sockaddr_in socketAddress = { 0 };
  socketAddress.sin_family = AF_INET;              /* address family */
  socketAddress.sin_port = htons(m_port);          /* port in network byte order */
  socketAddress.sin_addr.s_addr = inet_addr(m_ip); /* internet address - network byte order */

  if (!m_socketAddress) {
    m_socketAddress = new sockaddr_in;
  }
  *m_socketAddress = socketAddress;
  m_hostByNameEntry = 0;
  // is it an ip address?
  std::string serverName(m_ip);
  std::string::const_iterator end = std::find_if(serverName.begin(), serverName.end(), ::isalpha);
  if (end != serverName.end())
  {
    // it is not an IP address, it contains letters
    errno = 0;
    //struct hostent * host = gethostbyname(m_ip);
    m_hostByNameEntry = gethostbyname(m_ip);
    m_hostAddrIndex = -1;
    m_connectState = CLIENT_CONNECT_TRY_NEXT_HADDR;
  } else {
    m_connectState = CLIENT_CONNECT_READY_TO_CONNECT;
  }
}

void Client::connectReadyToConnect()
{
  if (m_hostByNameEntry)
    memcpy(&m_socketAddress->sin_addr, m_hostByNameEntry->h_addr_list[m_hostAddrIndex], sizeof(struct in_addr));
  if (tryConnect(*m_socketAddress)) {
    m_connectState = CLIENT_CONNECT_DONE;
  }
}

void Client::connectInprogress()
{
  debug("Client::connectInprogress");
  // select connect for write
  fd_set wfds;
  timeval timeout;
  int retval;
  FD_ZERO(&wfds);
  FD_SET(m_tcp_socket, &wfds);
  timeout.tv_sec = m_timeout;
  timeout.tv_usec = 0;
  retval = select(m_tcp_socket+1, NULL, &wfds, NULL, &timeout);
  debug("Client::connectInprogress, post select");
  if (retval == -1) {
    debug("Select error");
    m_connected = false;
    m_connectState = CLIENT_CONNECT_TRY_NEXT_HADDR;
  }
  else if (retval) {
    debug("connect - Data is available now.");
    m_connected = true;
    m_connectState = CLIENT_CONNECT_DONE;
  }
}

void Client::connectTryNextHaddr()
{
  ++m_hostAddrIndex;
  if (m_hostByNameEntry and m_hostByNameEntry->h_addr_list[m_hostAddrIndex] != NULL)
    m_connectState = CLIENT_CONNECT_READY_TO_CONNECT;
  else
    m_connectState = CLIENT_CONNECT_DONE;
}

void Client::connect()
{
  if (!m_ip)
    return;
  switch (m_connectState) {
    case CLIENT_CONNECT_INITIAL:
      connectInitial();
      break;
    case CLIENT_CONNECT_READY_TO_CONNECT:
      connectReadyToConnect();
      break;
    case CLIENT_CONNECT_EINPROGRESS:
      connectInprogress();
      break;
    case CLIENT_CONNECT_TRY_NEXT_HADDR:
      connectTryNextHaddr();
      break;
    case CLIENT_CONNECT_DONE:
      break;
  }
}

int Client::sslEnable(void)
{
  m_sslEnabled = true;
  return 0;
}

unsigned int Client::write(const void * data, unsigned int length)
{
  // can only send "buffer" amount of data, and when the buffer is full we need to wait
  unsigned int total(0);
  char * cdata = (char*)data;
#define SEND_SIZE 2048
  do
  {
#if DEBUG_WITH_SSTREAM
    {
      stringstream dbg;
      dbg << "About to send " << length << " bytes of data";
      debug(dbg.str().c_str());
    }
#endif
    //writeCallback();
    fd_set wfds;
    timeval timeout;
    int retval;
    FD_ZERO(&wfds);
    FD_SET(m_tcp_socket, &wfds);
    timeout.tv_sec = m_timeout;
    timeout.tv_usec = 0;
    retval = select(m_tcp_socket+1, NULL, &wfds, NULL, &timeout);
    if (retval < 0) {
      continue;
    }
    int sent = ::send(m_tcp_socket, cdata, length, 0);
    if (sent <= 0)
      break;
    cdata += sent;
    length -= sent;
    total += sent;
    // wait for the cores to sync
#if DEBUG_WITH_SSTREAM
    {
      stringstream dbg;
      dbg << "Remaining: " << length << " bytes of data";
      debug(dbg.str().c_str());
    }
#endif
  } while (length);
  debug("Done write");
  return total;
}

static char s_buffer[Client::BUFFER_SIZE];
int Client::read(int max)
{
  if (max > BUFFER_SIZE)
    max = BUFFER_SIZE;
  fd_set rfds;
  timeval timeout;
  int retval;
  FD_ZERO(&rfds);
  FD_SET(m_tcp_socket, &rfds);

  timeout.tv_sec = m_timeout;
  timeout.tv_usec = 0;

  errno = 0;
  retval = select(m_tcp_socket+1, &rfds, NULL, NULL, &timeout);
  int tmperr = errno;
  if (retval == -1) {
#if DEBUG_WITH_SSTREAM
    stringstream dbg;
    dbg << "select error: " << tmperr;
    debug(dbg.str().c_str());
#endif
    return READ_ERROR;
  } 
  else if (retval) {
    debug("Data is readable");
  }
  else
  {
    debug("not ready");
    // on the DS this plays tricks with us.
    // May return not ready when the socket has actually been closed.
    // In that case, recv will return -1 with errno == ESHUTDOWN.
//#ifndef ARM9
    return RETRY_LATER;
//#endif
  }
  errno = 0;
  int amountRead = ::recv(m_tcp_socket, s_buffer, max, 0 /*MSG_DONTWAIT*/);
  tmperr = errno;
  if (amountRead < 0) {
    debug("Error on recv");
#if DEBUG_WITH_SSTREAM
    stringstream dbg;
    dbg << "Error on recv errno: " << tmperr;
    debug(dbg.str().c_str());
#endif
    // if we read and there is an error
    if (tmperr == EAGAIN)
    {
      // not ready yet - from man recv:
      // EAGAIN The socket is marked non-blocking and the receive operation
      // would block.
      return RETRY_LATER;
    }
    return READ_ERROR;
  }
  if (amountRead == 0)
  {
    // man recv(2):
    // The return value will be 0 when the peer has performed an orderly shutdown.
    debug("Read 0 bytes");
#if DEBUG_WITH_SSTREAM
    stringstream dbg;
    dbg << "Read 0 errno: " << tmperr;
    debug(dbg.str().c_str());
#endif
    if (tmperr == EINVAL or tmperr == ESHUTDOWN) {
      debug("EINVAL or ESHUTDOWN");
      return READ_ERROR;
    }
    return amountRead;
  }
  handle(s_buffer, amountRead);
#if DEBUG_WITH_SSTREAM
  stringstream dbg;
  dbg << "Read " << amountRead << " bytes";
  debug(dbg.str().c_str());
#endif
  return amountRead;
}

void Client::setTimeout(int timeout)
{
  m_timeout = timeout;
}
