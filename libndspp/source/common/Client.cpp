/*
  Copyright (C) 2007,2008 Richard Quirk
  Copyright (C) 2025 Antonio Niño Díaz

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

#include <mbedtls/ctr_drbg.h>
#include <mbedtls/debug.h>
#include <mbedtls/entropy.h>
#include <mbedtls/error.h>
#include <mbedtls/net_sockets.h>
#include <mbedtls/platform.h>
#include <mbedtls/ssl.h>

// Debug level for Mbed TLS
#define DEBUG_LEVEL 0

using namespace std;
using namespace nds;
const int Client::BUFFER_SIZE(1024*16);

#if DEBUG_WITH_SSTREAM
// This function will be called when Mbed TLS prints debug messages
void mbedtls_debug_callback(void *ctx, int level, const char *file, int line,
                            const char *str)
{
  (void)ctx;
  (void)level;

  fprintf(stderr, "%s:%04d: %s\n", file, line, str);
}
#endif

void Client::mbedtls_print_error(int ret)
{
  char error_buf[256];
  mbedtls_strerror(ret, error_buf, 100);
  debug(error_buf);
}

Client::Client(const char * ip, int port):
  m_ip(NULL),
  m_port(port),
  m_connected(false),
  m_sslEnabled(false),
  m_mbedtlsInitialized(false),
  m_timeout(TIMEOUT),
  m_connectState(CLIENT_CONNECT_INITIAL)
{
  setConnection(ip, port);
}

Client::~Client()
{
  disconnect();
  free(m_ip);
}

void Client::setConnection(const char * ip, int port)
{
  free(m_ip);
  m_ip = strdup(ip);
  m_port = port;
}

void Client::disconnect()
{
  debug("Client::disconnect()");

  if (isConnected())
  {
    m_connected = false;
    m_sslEnabled = false;
    m_connectState = CLIENT_CONNECT_INITIAL;
  }

  if (m_mbedtlsInitialized)
  {
    mbedtls_net_close(&server_fd);
    mbedtls_net_free(&server_fd);
    mbedtls_x509_crt_free(&cacert);
    mbedtls_ssl_free(&ssl);
    mbedtls_ssl_config_free(&conf);
    mbedtls_ctr_drbg_free(&ctr_drbg);
    mbedtls_entropy_free(&entropy);

    m_mbedtlsInitialized = false;
  }
}

void Client::connectInitial()
{
  debug("Client::connectInitial");

  disconnect();

  mbedtls_debug_set_threshold(DEBUG_LEVEL);

  mbedtls_net_init(&server_fd);
  mbedtls_ssl_init(&ssl);
  mbedtls_ssl_config_init(&conf);
  mbedtls_x509_crt_init(&cacert);
  mbedtls_ctr_drbg_init(&ctr_drbg);
  mbedtls_entropy_init(&entropy);

  m_mbedtlsInitialized = true;

  const char *pers = "ssl_client1";
  int ret = mbedtls_ctr_drbg_seed(&ctr_drbg, mbedtls_entropy_func, &entropy,
                                  (const unsigned char *)pers, strlen(pers));
  if (ret != 0)
    mbedtls_print_error(ret);

  m_connectState = CLIENT_CONNECT_READY_TO_CONNECT;
}

bool Client::tryConnect()
{
  debug("Client::tryConnect");

  std::string port_str = std::to_string(m_port);
  debug(m_ip);
  debug(port_str.c_str());
  int ret = mbedtls_net_connect(&server_fd, m_ip, port_str.c_str(),
                                MBEDTLS_NET_PROTO_TCP);
  if (ret != 0)
  {
    mbedtls_print_error(ret);
    return false;
  }

  // TODO: Is this actually working?
  ret = mbedtls_net_set_nonblock(&server_fd);
  if (ret != 0)
  {
    mbedtls_print_error(ret);
    return false;
  }

  mbedtls_ssl_conf_rng(&conf, mbedtls_ctr_drbg_random, &ctr_drbg);
#if DEBUG_WITH_SSTREAM
  mbedtls_ssl_conf_dbg(&conf, mbedtls_debug_callback, stdout);
#endif

  return true;
}

void Client::connectReadyToConnect()
{
  if (tryConnect())
  {
    m_connected = true;
    m_connectState = CLIENT_CONNECT_DONE;
  }
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
    case CLIENT_CONNECT_DONE:
      break;
  }
}

#if 0
int Client::sslLoadCerts()
{
  printf("Client::sslLoadCerts");

  // In Linux, for example, they are stored in /etc/ssl/certs/
  int ret = mbedtls_x509_crt_parse_file(&cacert, "example-com-chain.pem");
  if (ret < 0)
  {
    mbedtls_print_error(ret);
    //printf("\n  Error! ret = -0x%x\n", (unsigned int)-ret);
    return -1;
  }

  //printf(" (%d skipped)", ret);
  return 0;
}
#endif

int Client::sslEnable(void)
{
  debug("Setting up SSL/TLS");

  int ret = mbedtls_ssl_config_defaults(&conf, MBEDTLS_SSL_IS_CLIENT,
                                        MBEDTLS_SSL_TRANSPORT_STREAM,
                                        MBEDTLS_SSL_PRESET_DEFAULT);
  if (ret != 0)
  {
    mbedtls_print_error(ret);
    return -1;
  }

  // MBEDTLS_SSL_VERIFY_OPTIONAL means that the certificates are checked but
  // any failure is ignored, which is useful for debugging. You need to set
  // DEBUG_LEVEL to a non-zero value if you want to see the error messages.
  //
  // If you use MBEDTLS_SSL_VERIFY_REQUIRED Mbed TLS will refuse to connect if
  // the certificates aren't valid, which is the right setting for a finished
  // program.
  //
  // MBEDTLS_SSL_VERIFY_NONE skips the decrificate check.
  mbedtls_ssl_conf_authmode(&conf, MBEDTLS_SSL_VERIFY_NONE);
  //mbedtls_ssl_conf_authmode(&conf, MBEDTLS_SSL_VERIFY_REQUIRED);
  mbedtls_ssl_conf_ca_chain(&conf, &cacert, NULL);

  ret = mbedtls_ssl_setup(&ssl, &conf);
  if (ret != 0)
  {
    mbedtls_print_error(ret);
    return -1;
  }

  ret = mbedtls_ssl_set_hostname(&ssl, m_ip);
  if (ret != 0)
  {
    mbedtls_print_error(ret);
    return -1;
  }

  mbedtls_ssl_set_bio(&ssl, &server_fd, mbedtls_net_send, mbedtls_net_recv, NULL);

  debug("Doing SSL/TLS handshake");

  while (1)
  {
    ret = mbedtls_ssl_handshake(&ssl);
    if (ret == 0)
      break;

    if (ret != MBEDTLS_ERR_SSL_WANT_READ && ret != MBEDTLS_ERR_SSL_WANT_WRITE)
    {
      mbedtls_print_error(ret);
      return -1;
    }

    //cothread_yield_irq(IRQ_VBLANK);
  }

  debug("Verifying peer X.509 cert");

  // In real life, we probably want to exit when ret != 0
  uint32_t flags = mbedtls_ssl_get_verify_result(&ssl);
  if (flags != 0)
  {
    debug("Certificate check failed!");
    char vrfy_buf[512];
    mbedtls_x509_crt_verify_info(vrfy_buf, sizeof(vrfy_buf), "  ! ", flags);
    debug(vrfy_buf);
    return -1;
  }

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

    int ret;
    if (m_sslEnabled)
      ret = ::mbedtls_ssl_write(&ssl, (const unsigned char *)cdata, length);
    else
      ret = ::mbedtls_net_send(&server_fd, (const unsigned char *)cdata, length);

    if (ret != MBEDTLS_ERR_SSL_WANT_READ && ret != MBEDTLS_ERR_SSL_WANT_WRITE)
    {
      // Retry later
      break;
    }
    else if (ret <= 0)
    {
      Client::mbedtls_print_error(ret);
      break;
    }

    cdata += ret;
    length -= ret;
    total += ret;

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

  int ret;
  if (m_sslEnabled)
    ret = ::mbedtls_ssl_read(&ssl, (unsigned char *)s_buffer, max);
  else
    ret = ::mbedtls_net_recv(&server_fd, (unsigned char *)s_buffer, max);

  if (ret == MBEDTLS_ERR_SSL_WANT_READ || ret == MBEDTLS_ERR_SSL_WANT_WRITE)
  {
    return RETRY_LATER;
  }
  else if (ret == MBEDTLS_ERR_SSL_PEER_CLOSE_NOTIFY)
  {
    debug("Connection closed from server");
    return CONNECTION_CLOSED;
  }
  else if (ret < 0)
  {
    debug("Error on recv");
    Client::mbedtls_print_error(ret);
#if DEBUG_WITH_SSTREAM
    stringstream dbg;
    dbg << "Error on recv errno: " << errno;
    debug(dbg.str().c_str());
#endif
    return READ_ERROR;
  }
  else if (ret == 0)
  {
    // man recv(2):
    // The return value will be 0 when the peer has performed an orderly shutdown.
    debug("Read 0 bytes");
#if DEBUG_WITH_SSTREAM
    stringstream dbg;
    dbg << "Read 0 errno: " << errno;
    debug(dbg.str().c_str());
#endif
    if (errno == EINVAL or errno == ESHUTDOWN)
    {
      debug("EINVAL or ESHUTDOWN");
      return CONNECTION_CLOSED;
    }
    return ret;
  }

  handle(s_buffer, ret);
#if DEBUG_WITH_SSTREAM
  stringstream dbg;
  dbg << "Read " << ret << " bytes";
  debug(dbg.str().c_str());
#endif
  return ret;
}

void Client::setTimeout(int timeout)
{
  // TODO: Use the timeout for something
  m_timeout = timeout;
}
