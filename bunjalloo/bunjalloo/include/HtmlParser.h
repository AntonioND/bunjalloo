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
#ifndef HtmlParser_h_seen
#define HtmlParser_h_seen

#include <string>
#include "Attribute.h"
class HtmlParserImpl;
class HtmlElement;
class ParameterSet;

/**
 * A parser for HTML. Consumes bytes and calls the handler functions for tags.
 */
class HtmlParser
{
  public:
    //! Types of encoding of a page.
    enum Encoding {
      UTF8_ENCODING, //!< char stream is parsed using UTF-8.
      ISO_ENCODING   //!< char stream is parsed as ISO-8859-1.
    };

    //! Constructor.
    HtmlParser();
    //! Destructor.
    virtual ~HtmlParser();

    /** Feed data to the parser. Call this with any amount of data, the state is kept between feeds.
     * To reset the state, call setToStart().
     * @param data data to be parsed.
     * @param length the amount of data in bytes.
     */
    void feed(const char * data, unsigned int length);

    //! Reset the parser to the initial state.
    void setToStart();

    /** Set the encoding to use. The default is UTF-8.
     * @param enc the new encoding.
     */
    void setEncoding(Encoding enc);

    /** Get the current encoding.
     * @return the current Encoding.
     */
    Encoding encoding() const;

    /** Parse the content-type value and act accordingly.
     * May set teh model to plain text or the decoding to iso-8851.
     * @param value the content-type value.
     */
    void parseContentType(const std::string & value);

    /** Parses the provided charset name
     * @param value the charset name.
     */
    void parseCharset(const std::string & value);

    /** Parse the content-disposition value and act accordingly.
     * @param value the content-disposition value.
     */
    void parseContentDisposition(const std::string & value);

    /** Get the content disposition filename value for this document.
     * See this page http://www.ietf.org/rfc/rfc2183.txt for details.
     * @return the Content-Disposition filename.
     */
    std::string getContentDisposition() const;

    /** Parse the refresh value and act accordingly.
     * Refresh the page and possibly go to a new url.
     * @param value the refresh value.
     */
    void parseRefresh(const std::string & value);

    void refresh(std::string & refresh, int & time) const;

    /** Set the name of the file to cache to.
     */
    void setCacheFile(const std::string & filename);

    void parseCacheControl(const std::string &value);

    enum MimeType
    {
      // Make sure that nds::Image::ImageType entries match the values here
      IMAGE_PNG     = 1,
      IMAGE_GIF     = 2,
      IMAGE_JPEG    = 3,
      IMAGE_APNG    = 4,
      IMAGE_BMP     = 5,

      IMAGE_START   = IMAGE_PNG,
      IMAGE_END     = IMAGE_BMP,

      TEXT_HTML     = 10,
      TEXT_PLAIN    = 11,

      ZIP           = 20,

      UNINITIALISED = 30,

      OTHER         = 40
    };

    static const char * IMAGE_JPEG_STR;
    static const char * IMAGE_JPG_STR;

    MimeType mimeType() const;
    std::string mimeTypeValue() const;
  protected:
    enum ContentModel {
      PCDATA,
      RCDATA,
      CDATA,
      PLAINTEXT
    };

    MimeType m_mimeType;
    std::string m_mimeTypeValue;

    friend class HtmlParserImpl;
    /** Called when a start-end tag is found.
     * @param tag the tag name (lowercase).
     * @param attrs a vector of attributes for the tag.
     **/
    virtual void handleStartEndTag(const std::string & tag, const AttributeVector & attrs) = 0;
    /** Called when a start tag is found.
     * @param tag the tag name (lowercase).
     * @param attrs a vector of attributes for the tag.
     **/
    virtual void handleStartTag(const std::string & tag, const AttributeVector & attrs) = 0;
    /** Called when an end tag is found.
     * @param tag the tag name (lowercase).
     */
    virtual void handleEndTag(const std::string & tag) = 0;
    /** Called when mime type is binary.
     * @param data binary data.
     * @param length size of data in bytes.
     */
    virtual void handleBinaryData(const void * data, unsigned int length) = 0;

    /** Called for each "glyph" of data between tags.
     * @param ucodeChar unicode character index of the data.
     */
    virtual void handleData(unsigned int ucodeChar) = 0;

    void setContentModel(ContentModel newModel);

    void checkMetaTagHttpEquiv(const HtmlElement * meta);

  private:
    //! Nothing to see here.
    HtmlParserImpl* m_details;

    void setMimeType(const std::string &paramSet);
};
#endif
