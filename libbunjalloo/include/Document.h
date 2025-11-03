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
#ifndef Document_h_seen
#define Document_h_seen
#include <vector>
#include <string>
#include "ViewI.h"

class HeaderParser;
class HeaderListener;
class HtmlDocument;
class HtmlElement;
class CookieJar;

/** This class represents the Document Model and the model in the MVC pattern. */
class Document
{
  public:

    //! Status of the document.
    enum Status {
      NOTHING,   //!< No status set.
      LOADED_HTML,  //!< Has loaded the HTML page
      LOADED_ITEM,  //!< Has loaded an item on the page
      LOADED_PAGE,  //!< Has fully loaded the page
      INPROGRESS, //!< Is in the progress of loading.
      HAS_HEADERS,//!< Has got the HTTP headers
      REDIRECTED  //!< Page has been redirected
    };
    //! Constructor.
    Document();

    ~Document();

    /** Set the URI.
     * @param uriString a string representing the URI.
     */
    void setUri(const std::string & uriString);

    /** Append data to the model.
     * @param data pointer to the data to add.
     * @param size the amount of data in bytes.
     */
    void appendData(const char * data, int size);

    /** Append data from a local source to the model. This bypasses the HeaderParser.
     * @param data pointer to the data to add.
     * @param size the amount of data in bytes.
     */
    void appendLocalData(const char * data, int size);

    /** Fetch the current URI.
     * @return the URI.
     */
    const std::string & uri() const;

    /** Get the model as text.
     * @return a string of universal characters.
     */
    const std::string &asText() const;

    /** Get the root node of the document model.
     * @return The root node.
     */
    const HtmlElement * rootNode() const;

    /** Get the title node of the document model.
     * @return The title node, or 0 if not available.
     */
    const HtmlElement * titleNode() const;

    CookieJar * cookieJar() const;

    /** Get the status of the document.
     * @return the current status.
     */
    Status status() const;

    /** Set the status.
     * @param status the new status.
     */
    void setStatus(Status status);

    /** Reset the internal state of the model.
     */
    void reset();

    /** Get the percentage of the page that has loaded. Note that for chunked pages this returns the
     * percentage of the chunk loaded.
     * @return the percentage of the page (or chunk) loaded
     */
    unsigned int percentLoaded() const;

    /** Register a view to be notified when the model changes.
     * @param v the view to register.
     */
    void registerView(ViewI * v);

    /** Unregister a previously registered view.
     * @param v the view to unregister.
     */
    void unregisterView(ViewI * v);

    //! Debug function - dump to stdout the DOM.
    void dumpDOM();

    /** Check if there are history pages before the actual page.
     * @return true if there is history, false otherwise.
     */
    bool hasPreviousHistory() const;

    /** Check if there are history pages after the actual page.
     * @return true if there is forward history, false otherwise.
     */
    bool hasNextHistory() const;

    /** Go to the previous (older) page in the history, returning its URL.
     * @return the URL of the previous page.
     */
    std::string gotoPreviousHistory();

    /** Go to the next (newer) page in the history, returning its URL.
     * @return the URL of the next page.
     */
    std::string gotoNextHistory();

    /** Get the value of any refresh: headers or meta-headers.
     * @param refresh the url that refresh was set to.
     * @param time the time until the page should refresh.
     * If either refresh is empty or time is negative, then not valid.
     */
    void refresh(std::string & refresh, int & time) const;

    void setCacheFile(const std::string & cacheFile);

    HtmlDocument * htmlDocument() const
    {
      return m_htmlDocument;
    }

    const HeaderParser & headerParser() const;

    /** Set the user defined position of the document.
     * @param position the position of the document
     */
    void setPosition(int position);

    /** Get the user defined position of the document.
     * @return the position of the document
     */
    int position() const;

    //! Set whether a URI update changes the history or not
    void setHistoryEnabled(bool enable);

    bool historyEnabled() const;

    /** Clear config: urls from the history.
     */
    void clearConfigHistory();

    /** Get the amount of data that is expected to be downloaded, based on the
     * HTTP header or chunk size values. But don't believe its lies.
     * @return the number of bytes to download.
     */
    unsigned int dataExpected() const;

  private:
    //std::string m_uri;
    Status m_status;
    int m_amount;
    std::vector<ViewI*> m_views;
    CookieJar*    m_cookieJar;
    HtmlDocument* m_htmlDocument;
    HeaderParser* m_headerParser;
    HeaderListener * m_headerListener;
    //! Store history entry as the url and user defined scroll position.
    typedef std::pair<std::string, int> HistoryEntry;
    typedef std::vector<HistoryEntry> HistoryVector;

    bool m_historyEnabled;
    HistoryVector m_history;
    HistoryVector::iterator m_historyPosition;

    //! Notify all registered views that there are model changes.
    void notifyAll() const;

    void magicMimeType(const char * data, int length);
    inline std::string & currentHistoryUri();
    inline const std::string & currentHistoryUri() const;
    inline int & currentHistoryPosition();
    inline const int & currentHistoryPosition() const;

    //! No copies.
    Document (const Document&);
    const Document& operator=(const Document&);
};
#endif
