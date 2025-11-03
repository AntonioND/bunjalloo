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
#include <algorithm>
#include "config_defs.h"
#include "DateUtils.h"
#include "Config.h"
#include "Cookie.h"
#include "CookieWriter.h"
#include "CookieJar.h"
#include "File.h"
#include "ParameterSet.h"
#include "URI.h"
#include "string_utils.h"
#include "Delete.h"

using namespace std;
static const char * PATH_STR("path");
static const char * DOMAIN_STR("domain");
static const char * SECURE_STR("secure");
static const char * EXPIRES_STR("expires");
static const char * HTTP_ONLY("httponly");
static const char COOKIE_TEMPLATE[] = "ckallow-example.txt";

CookieJar::CookieJar()
{
}

CookieJar::~CookieJar()
{
  for_each(m_cookies.begin(), m_cookies.end(), delete_ptr());
  m_cookies.clear();
}


Cookie * CookieJar::hasCookieForDomain(const std::string &server, const std::string & name) const
{
  vector<Cookie *>::const_iterator it(m_cookies.begin());
  vector<Cookie *>::const_iterator end(m_cookies.end());
  for (; it != end; ++it)
  {
    Cookie * c(*it);
    if (c->matchesDomain(server) and c->name() == name)
    {
      return c;
    }
  }
  return 0;
}

void CookieJar::loadDomainCookies(const std::string &domain)
{
  // load cookies for the domain
  if (m_domain != domain)
  {
    m_domain = domain;
    string cookietemp(DATADIR);
    cookietemp += "/cookies/";
    if (!nds::File::exists(cookietemp.c_str()))
    {
      nds::File::mkdir(cookietemp.c_str());
    }
    cookietemp += domain;
    if (nds::File::exists(cookietemp.c_str()) != nds::File::F_NONE)
    {
      nds::File f;
      f.open(cookietemp.c_str());
      if (f.is_open())
      {
        std::vector<std::string> lines;
        f.readlines(lines);
        for (std::vector<std::string>::const_iterator line(lines.begin());
            line != lines.end(); ++line)
        {
          addCookieHeader(domain, "/", *line);
        }
      }
    }
  }
}

void CookieJar::addCookieHeader(const URI &uri, const std::string &request)
{
  string domain = uri.server();
  if (not acceptCookies(domain))
  {
    // this server is blocked.
    return;
  }
  string path = uri.fileName();
  addCookieHeader(domain, path, request);
  m_domain = domain;
  saveCookiesToDisk();
}

void CookieJar::addCookieHeader(
    const std::string &domain_in,
    const std::string &path_in,
    const std::string &request)
{
  // printf("addCookieHeader: %s \nFrom: %s\n",request.c_str(), domain_in.c_str());
  std::string path(path_in);
  std::string domain(domain_in);
  bool secure(false);

  string lowCaseRequest(request);
  transform(lowCaseRequest.begin(), lowCaseRequest.end(), lowCaseRequest.begin(), ::tolower);

  ParameterSet paramSet(request);
  ParameterSet lowCaseParamSet(lowCaseRequest);

  if ( lowCaseParamSet.hasParameter(PATH_STR) )
  {
    lowCaseParamSet.parameter(PATH_STR, path);
  }
  else
  {
    // sort out basename
    if (path.length() and path[path.length()-1] != '/')
    {
      path = nds::File::dirname(path.c_str());
    }
  }
  if ( lowCaseParamSet.hasParameter(DOMAIN_STR) )
  {
    lowCaseParamSet.parameter(DOMAIN_STR, domain);
    // reject domains that do not start with a dot
    /*if (domain[0] != '.') {
      printf("Reject cookie for %s\n", domain.c_str());
      return;
    }*/
  }
  if ( lowCaseParamSet.hasParameter(SECURE_STR) )
  {
    secure = true;
  }
  int expires = -1;
  if (lowCaseParamSet.hasParameter(EXPIRES_STR))
  {
    std::string expval;
    const KeyValueMap & keyValueMap(paramSet.keyValueMap());
    for (KeyValueMap::const_iterator it(keyValueMap.begin()); it != keyValueMap.end(); ++it)
    {
      string name = it->first;
      string lowCaseName(name);
      transform(lowCaseName.begin(), lowCaseName.end(), lowCaseName.begin(), ::tolower);
      if (lowCaseName == EXPIRES_STR) {
        expires = DateUtils::parseDate(it->second.c_str());
        break;
      }
    }
  }
  const KeyValueMap & keyValueMap(paramSet.keyValueMap());
  for (KeyValueMap::const_iterator it(keyValueMap.begin()); it != keyValueMap.end(); ++it)
  {
    string name = it->first;
    string lowCaseName(name);
    transform(lowCaseName.begin(), lowCaseName.end(), lowCaseName.begin(), ::tolower);
    if (lowCaseName == PATH_STR or lowCaseName == SECURE_STR
        or lowCaseName == HTTP_ONLY or lowCaseName == EXPIRES_STR
        or lowCaseName == DOMAIN_STR)
    {
      continue;
    }
    Cookie * existingCookie = hasCookieForDomain(domain, name);
    const string &value(it->second);
    if (existingCookie != 0)
    {
      // replace with new value
      existingCookie->setValue(value);
      existingCookie->setExpires(expires);
      existingCookie->setSaved(false);
    }
    else
    {
      Cookie * cookie(new Cookie(name, value, domain, path, expires, secure));
      m_cookies.push_back(cookie);
    }
  }
}

void CookieJar::saveCookiesToDisk()
{
  CookieWriter cw(0);
  for_each(m_cookies.begin(), m_cookies.end(), cw);
}

void CookieJar::cookiesForRequest(
    const URI &request,
    std::string &headers)
{
  cookiesForRequest(request, headers, ::time(0));
}

void CookieJar::cookiesForRequest(
    const URI &request,
    std::string &headers,
    time_t now)
{
  loadDomainCookies(request.server());
  loadDomainCookies(topLevel(request.server()));
  // printf("cookiesForRequest: %s \n",request.asString().c_str());
  string domain(request.server());
  // find a cookie in the jar that corresponds to the requested domain...
  string tmp("Cookie: ");
  bool needSep(false);
  const static string sep("; ");
  bool haveExpired(false);
  std::vector<Cookie *>::const_iterator it(m_cookies.begin());
  for (; it != m_cookies.end(); ++it)
  {
    Cookie *c(*it);
    if (c->name() == DOMAIN_STR)
      continue;
    if (c->matchesDomain(domain)) {
      if (c->expired(now)) {
        haveExpired = true;
        continue;
      }

      if (not c->path().empty())
      {
        // check to see if the requested path is a sub dir of path
        string value = c->path();
        string file = request.fileName();
        size_t pos = file.find(value);
        if (pos != 0)
        {
          continue;
        }
      }
      // do not send secure cookies to unsecure domains
      if (c->secure() and request.protocol() != URI::HTTPS_PROTOCOL)
      {
        continue;
      }

      if (needSep)
      {
        tmp += sep;
      }
      string value(c->value());
      bool needQuotes = value.find_first_of(" ")!=string::npos;
      tmp += c->name();
      tmp += "=";
      if (needQuotes) {
        tmp += "\"";
      }
      tmp += c->value();
      if (needQuotes) {
        tmp += "\"";
      }
      needSep = true;
    }
  }
  if (needSep) {
    tmp += "\r\n";
    headers = tmp;
  }
  if (haveExpired) {
    gcExpiredCookies(now);
  }
}

#if 0
void CookieJar::dumpCookieList()
{
  printf("Cookies: %d\n", m_cookies.size());
  std::vector<Cookie *>::const_iterator it(m_cookies.begin());
  for (; it != m_cookies.end(); ++it)
  {
    Cookie *c(*it);
    printf("%s\n", c->asString().c_str());
  }
}
#endif

void CookieJar::gcExpiredCookies(time_t now)
{
  // there are expired cookies
  // remove them from memory and disk
  std::vector<Cookie *> tmp;
  std::vector<Cookie *> expired;
  std::vector<Cookie *>::const_iterator it(m_cookies.begin());
  for (; it != m_cookies.end(); ++it)
  {
    Cookie *c(*it);
    if (c->expired(now)) {
      expired.push_back(c);
      continue;
    }
    tmp.push_back(c);
  }
  m_cookies.swap(tmp);
  for_each(expired.begin(), expired.end(), CookieWriter(now));
  for_each(expired.begin(), expired.end(), delete_ptr());
}

std::string CookieJar::topLevel(const std::string & sub)
{
  // given foo.blahblah.com, return blahblah.com
  size_t pos = sub.rfind(".");
  if (pos == string::npos or pos == 0)
  {
    return sub;
  }
  pos = sub.rfind(".", pos-1);
  if (pos == string::npos)
  {
    return sub;
  }
  return sub.substr(pos+1, sub.length()-pos-1);
}

bool CookieJar::acceptCookies(const std::string & domain) const
{

  // first get the top level domain of "domain".
  string top(topLevel(domain));
  AcceptedDomainSet::const_iterator topIt(m_acceptedDomains.find(top));
  if (topIt != m_acceptedDomains.end())
  {
    return true;
  }
  AcceptedDomainSet::const_iterator it(m_acceptedDomains.find(domain));
  return (it != m_acceptedDomains.end());
}

void CookieJar::setAcceptCookies(const std::string & domain, bool accept)
{
  if (not accept and acceptCookies(domain))
  {
    // no longer accept them
    m_acceptedDomains.erase(domain);
    // TODO: remove the actual cookie vals too?
  }

  if (accept)
  {
    m_acceptedDomains.insert(domain);
  }
}

void CookieJar::acceptedDomains(AcceptedDomainSet & set) const
{
  set = m_acceptedDomains;
}

void CookieJar::loadAcceptList(Config & config)
{
  // configure the cookie list, read each line in the m_cookieList file and
  // add it as an allowed one to CookieJar
  nds::File cookieList;
  string cookieFile;
  config.resource(Config::COOKIE_STR, cookieFile);
  if (nds::File::exists(cookieFile.c_str()) == nds::File::F_NONE)
  {
    string cookietemp(DATADIR);
    cookietemp += "/docs/";
    cookietemp += COOKIE_TEMPLATE;
    Config::copyTemplate(cookietemp.c_str(), cookieFile.c_str());
  }
  cookieList.open(cookieFile.c_str());
  if (cookieList.is_open())
  {
    vector<string> lines;
    cookieList.readlines(lines);
    for (vector<string>::iterator it(lines.begin());
        it != lines.end();
        ++it)
    {
      string & line(*it);
      stripWhitespace(line);
      URI uri(line);
      setAcceptCookies(uri.server());
    }
    cookieList.close();
  }
}
