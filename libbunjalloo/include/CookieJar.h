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
#ifndef CookieJar_h_seen
#define CookieJar_h_seen

#include <string>
#include <vector>
#include <set>
#include <time.h>

class Config;
class Cookie;
class URI;

/** A class to hold all the Cookies. @see Cookie.
 */
class CookieJar
{
  public:

    typedef std::set<std::string> AcceptedDomainSet;

    /** Given a sub domain, find the top level domain.
     * @param sub the sub domain
     * @return the top level domain, or an @a sub if unable to work it out
     */
    static std::string topLevel(const std::string & sub);

    /** Create a cookie holder. */
    CookieJar();

    /** End of the jar. */
    ~CookieJar();

    /** Add cookies based on the request header.
     * @param uri The uri of the page that is sending the cookie set request.
     * @param request Set-Cookie request header value.
     */
    void addCookieHeader(const URI &uri, const std::string &request);

    /** Given a request, fill the headers with the defined cookies.
     * @param request the request string.
     * @param headers the output headers.
     */
    void cookiesForRequest(const URI & request,
        std::string & headers);

    /** Given a request, fill the headers with the defined cookies.
     * @param request the request string.
     * @param headers the output headers.
     * @param now the current time (for expires checks)
     */
     void cookiesForRequest(const URI & request,
        std::string & headers,
        time_t now);

    /** Check if there is a cookie for this domain already in the jar.
     * @param uri the URL of the cookie.
     * @param name the cookie name.
     * @return pointer to a cookie from the domain with the given name, 0 otherwise.
     */
    Cookie * hasCookieForDomain(
            const std::string &server,
            const std::string &name) const;

    /** Check if we should accept cookies from the given domain.
     * @param domain the domain that has sent the cookie.
     * @return true if we accept cookies from this domain. false otherwise.
     */
    bool acceptCookies(const std::string & domain) const;

    /** Set if we should accept cookies, or not, from the given domain.
     * Bunjalloo uses a white-list cookie management policy - everything is
     * rejected unless it is on the list. Therefore this function is used to
     * add domains from the internal list of allowed sites, or to remove
     * already added sites.
     * @param domain the domain to accept or reject cookies from.
     * @param accept if true, then cookies are accepted for the domain.
     * Otherwise they are rejected.
     */
    void setAcceptCookies(const std::string & domain, bool accept=true);

    void acceptedDomains(AcceptedDomainSet & set) const;

    void loadAcceptList(Config & config);
  private:
    //! List of cookies.
    std::vector<Cookie *> m_cookies;

    //! Name of the domain that we are caching cookies for
    //! When this changes we dump/reload non-session cookies
    std::string m_domain;

    AcceptedDomainSet m_acceptedDomains;

    void gcExpiredCookies(time_t now);
    void saveCookiesToDisk();
    void loadDomainCookies(const std::string &domain);

    void addCookieHeader(
            const std::string &domain,
            const std::string &path_in,
            const std::string &request);
};

#endif
