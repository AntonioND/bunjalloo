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
#ifndef URI_h_seen
#define URI_h_seen
#include <string>

/** Encapsulate a URI.*/
class URI
{
  public:

    /** Types of protocol supported. */
    enum Protocol_t
    {
      UNKNOWN_PROTOCOL,
      HTTPS_PROTOCOL,
      HTTP_PROTOCOL,
      FILE_PROTOCOL,
      CONFIG_PROTOCOL
    };

    /** Escape a string for use in URIs, removing spaces and other nasty
     * things.
     * @param input The "raw" user string.
     * @return The "safe" string that can be used as a URI.
     */
    static std::string escape(const std::string& input);
    static std::string unescape(const std::string& input);

    /** Create an empty URI.*/
    URI();

    /** Create an URI initialised to the given string.
     * @param uriString the URI location
     * @param fix if true, this will add the protocol if needed.
     */
    URI(const std::string & uriString, bool fix=true);

    /** Set the URI location.
     * @param uriString the new location.
     */
    void setUri(const std::string & uriString);

    /** Get the protocol of this URI.
     * @return the URI protocol type, e.g. if it represents a file:// URI returns FILE_PROTOCOL.
     */
    Protocol_t protocol() const;

    /** Is the URI valid (syntactically). It is entirely possible too have a
     * valid URI that describes a non existent file or location.
     * @return true if the URI looks good.
     */
    bool isValid() const;

    /** Get the server part of the URI.
     * @return the server part of http uris.
     */
    std::string server() const;

    /** Get the port part of the URI.
     * @return the port part of http uris.
     */
    int port() const;

    /** Get the file part of the URI.
     * @return the file part of the location.
     */
    const std::string fileName() const;

    /** Get the lot.
     * @return the whole URI.
     */
    const std::string asString() const;

    /** Get the method (GET or POST).
     * @return the HTTP method.
     */
    const std::string method() const;

    /** Set the method (GET or POST).
     * @param method the HTTP method.
     */
    void setMethod(const std::string & method);

    /** Navigate to a new URI and return the new address.
     * @param newFile the relative or absolute URI.
     * @return the new URI address.
     */
    URI navigateTo(const std::string & newFile) const;

    /** Set an extra request header to be sent as part of the HTTP request.
     * @param headerData the header data to set
     */
    void setRequestHeader(const std::string & headerData);

    /** Retrieve any extra headers for this request.
     * @return the headers to send.
     */
    std::string requestHeader() const;

    /** Retrieve the internal link part of the URI, if it exists.
     * @return the internal link (\#blah) part of the filename
     */
    std::string internalLink() const;

    /** Compare this URI to the @a other one. URIs are the same if they have
     * the same protocol and address.
     * @param other the other URI to compare.
     */
    bool operator==(const URI & other) const;
    /** Inverse compare. @see operator==().
     * @param other the other URI to compare.
     */
    bool operator!=(const URI & other) const;

    /** Get the CRC for the address.
     * @return the 32 bit CRC value for the address.
     */
    unsigned int crc32int() const;

    /** Get the CRC for the address.
     * @return the 32 bit CRC value for the address as a string
     */
    std::string crc32() const;

  private:
    std::string m_method;
    std::string m_protocol;
    std::string m_address;
    std::string m_requestHeader;
    bool m_fix;
};
#endif
