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
#ifndef Controller_h_seen
#define Controller_h_seen

#include "URI.h"
#include <queue>

class Document;
class View;
class Config;
class Cache;

class HttpClient;
/** Controller maps user actions to model updates. For example, responds to setting the URI.
 */
class Controller
{
  friend class HttpClient;
  public:

    static const char LICENCE_URL[];
    //!Constructor.
    Controller();
    void initialise();
    //! Destructor
    ~Controller();

    /** Handle the user request for setting a URI.
     * @param uri the URI to use.
     */
    void doUri(const URI & uri);

    void queueUri(const URI & uri);

    /** Set the HTTP Referer URI.
     * @param referer the referring site.
     */
    void setReferer(const URI & referer);

    /** Clear the Referer field.
     */
    void clearReferer();

    /** Refresh the page - reload from http not from cache.  */
    void reload();

    enum SaveAs_t
    {
      NO_SAVE,
      SAVE_DOWNLOADING,
      SAVE_CURRENT_FILE
    };
    /** Save the current page as...  */
    void saveAs(const char * fileName, SaveAs_t saveType=SAVE_CURRENT_FILE);

    /** Go to the previous page in the history. */
    void previous();

    /** Go to the next page in the history. */
    void next();

    /** Stop loading the page. */
    void stop();

    bool stopped() const;

    /** Get the Config object.
     * @return a reference to the Config object.
     */
    const Config & config() const;

    /** Loops forever.*/
    void mainLoop();

    /** Is the wifi initialised?
     * @return true if wifi has been initialised, or false if it has not.
     */
    bool wifiInitialised() const;

    /** Get the cache object.
     * @return the Cache object.
     */
    Cache * cache() const;

    /** Save the list of accepted domains to file.  */
    void saveCookieSettings();

    URI downloadingFile() const;

    void waitVBlank() const;

  private:
    Document * m_document;
    View * m_view;
    Config * m_config;
    Cache * m_cache;
    HttpClient * m_httpClient;
    bool m_wifiInit;
    bool m_stop;
    int m_redirected;
    int m_maxRedirects;
    SaveAs_t m_saveAs;
    std::string m_saveFileName;
    std::queue<URI> m_downloadQ;
    bool m_checkingQueue;


    void localFile(const std::string &);
    void localConfigFile(const std::string & fileName);
    void fetchHttp(const URI &);
    void finishFetchHttp(const URI & uri);

    void configureUrl(const std::string & fileName);
    void loadError();
    // helper to avoid code dupe
    void handleUri(const URI & uri);
    void saveCurrentFileAs(const char * fileName);
    // Check saving when page loads or user tries to save
    void checkSave();
    void checkUpdates();

    void checkDownloadQueue();

    /** Show the software licence.*/
    void showLicence();

};
#endif
