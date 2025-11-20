#include "CookieWriter.h"
#include "Cookie.h"
#include "File.h"
#include "config_defs.h"

static const char COOKIE_DIR[] = DATADIR "/cookies/";

CookieWriter::CookieWriter(time_t now)
  : m_now(now) {}

static void linesToSave(const std::string &filename,
    Cookie *cookie, std::vector<std::string> *wanted)
{
  nds::File f;
  f.open(filename.c_str(), "r");
  if (f.is_open()) {
    // grep out the line that starts with the cookie name
    // write back all lines
    std::vector<std::string> lines;
    f.readlines(lines);
    std::string match(cookie->name() + "=");
    size_t sz(match.size());
    for (std::vector<std::string>::const_iterator it(lines.begin());
        it != lines.end();
        ++it) {
      const std::string &line(*it);
      const std::string &first(line.substr(0, sz));
      if (first == match)
        continue;
      wanted->push_back(line);
    }
  }
}

void CookieWriter::operator()(Cookie *cookie)
{
  if (cookie->session())
    return;
  if (cookie->saved())
    return;
  std::string filename(COOKIE_DIR);
  const std::string &domain(cookie->domain());
  if (domain[0] == '.')
    filename += domain.substr(1, domain.size()-1);
  else
    filename += domain;

  std::vector<std::string> wanted;
  linesToSave(filename, cookie, &wanted);
  nds::File f;
  f.open(filename.c_str(), "w");
  if (f.is_open()) {
    for (std::vector<std::string>::const_iterator it(wanted.begin());
      it != wanted.end();
      ++it) {
      f.write(it->c_str());
      f.write("\n");
    }
    if (not cookie->expired(m_now)) {
      f.write(cookie->asString().c_str());
      f.write("\n");
    }
  }
  cookie->setSaved(true);
}

void CookieWriter::remove(const char *domain)
{
  std::string filename(COOKIE_DIR);
  filename += domain;
  nds::File::rmrf(filename.c_str());
}
