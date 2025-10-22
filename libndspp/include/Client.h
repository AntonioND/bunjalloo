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
#ifndef Client_h_seen
#define Client_h_seen

#include <string>

#include <mbedtls/ctr_drbg.h>
#include <mbedtls/debug.h>
#include <mbedtls/entropy.h>
#include <mbedtls/error.h>
#include <mbedtls/net_sockets.h>
#include <mbedtls/platform.h>
#include <mbedtls/ssl.h>

#include "util/classhelper.h"

struct sockaddr_in;
struct hostent;
namespace nds {
  class Client
  {
    public:
      /** Size of buffers used internally.*/
      static const int BUFFER_SIZE;

      /*!@brief Create a TCP client that connects to an IP address and port
       * @param ip IP address to connect to.
       * @param port port to connect to.
       */
      Client(const char * ip, int port);

      /*! @brief virtual destructor. Closes the socket (if open) */
      virtual ~Client();

      /*! @brief Connect to the given IP addres and port.*/
      void connect();

      /** Error values. */
      enum ReadError {
        CONNECTION_CLOSED=0,
        READ_ERROR=-1,
        RETRY_LATER=-2
      };
      /*! @brief Read bytes from the server address and port.
       * @return the number of bytes read. -1 for an error. It may be one of the ReadError codes above.
       */
      int read(int max=BUFFER_SIZE);

      /*! @brief Disconnect from the server.*/
      void disconnect();

      /*! @brief Write data to the target address and port. 
       * @param data pointer to the data to send
       * @param size size in bytes of each element of data
       * @returns number of bytes written
       * */
      unsigned int write(const void * data, unsigned int size);

      /*! @brief Fetch the current status of the connection.
       * @returns true if connected false otherwise.*/
      inline bool isConnected() const { return m_connected; }

      /** Set the connection details.
       * @param ip IP address to connect to.
       * @param port port to connect to.
       */
      void setConnection(const char * ip, int port);

      /** Load CA certificates.
       * @param cacerts Path to a file containing certificates.
       */
      int sslLoadCerts(std::string cacerts);

      /*! @brief Sends the SSL handshake and starts SSL encryption.
       * @returns 0 on success, other values on error.
       */
      int sslEnable(void);

    protected:
      /*! @brief Pure virtual function to handle the bytes read from the server
       * @param buffer Data recieved from the other end.
       * @param amountRead Number of bytes read.
       */
      virtual void handle(void * buffer, int amountRead) = 0;

      /*! @brief debug string 
       * @param c Characters to print (or whatever). */
      virtual void debug(const char * c)=0;

      /** Set the current timeout. */
      void setTimeout(int timeout);

    private:
      //! The IP address
      char * m_ip;
      //! The port on the server to connect to
      int m_port;
      //! The status of the connection
      bool m_connected;
      ///! The connection has done the SSL handshake and is encrypted
      bool m_sslEnabled;
      ///! Set to true if Mbed TLS has been initialized
      bool m_mbedtlsInitialized;
      //! The time to wait between selects (in seconds)
      int m_timeout;
      //! Set to true when the certificates are loaded
      bool m_certificatesLoaded;

      mbedtls_net_context server_fd;
      mbedtls_entropy_context entropy;
      mbedtls_ctr_drbg_context ctr_drbg;
      mbedtls_ssl_context ssl;
      mbedtls_ssl_config conf;
      mbedtls_x509_crt cacert;

      enum ClientConnectState {
        CLIENT_CONNECT_INITIAL, // not started
        CLIENT_CONNECT_READY_TO_CONNECT, // initialised, now can call ::connect()
        CLIENT_CONNECT_DONE  // tried all addresses *or* connected
                             // check the m_connected flag for result
      };
      ClientConnectState m_connectState;

      bool tryConnect();
      void connectInitial();
      void connectReadyToConnect();

      void mbedtls_print_error(int ret);

      DISALLOW_COPY_AND_ASSIGN(Client);
  };
}
#endif
