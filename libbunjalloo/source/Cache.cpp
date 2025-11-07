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
#include <time.h>
#include <stdio.h>
#include "Cache.h"
#include "CacheControl.h"
#include "HeaderParser.h"
#include "Document.h"
#include "URI.h"
#include "File.h"
#include "config_defs.h"

const char * Cache::CACHE_DIR(DATADIR "/cache");

Cache::Cache(Document & document, bool useCache)
: m_document(document), m_useCache(useCache)
{
  m_fileIds.clear();
  setUseCache(useCache);
}

void Cache::setUseCache(bool useCache)
{
  m_useCache = useCache;
  if (m_useCache)
  {
    if (nds::File::exists(CACHE_DIR) == nds::File::F_NONE and not nds::File::mkdir(CACHE_DIR) )
    {
      // didn't exist, nor can we make it..
      m_useCache = false;
    }
  }
}

static std::string uri2CacheFile(const URI & uri)
{
  // strip #internal from URI
  std::string crc = uri.navigateTo(uri.fileName()).crc32();
  std::string cacheFile = Cache::CACHE_DIR;
  // Cache name is 8 letter Hex crc32
  // Headers are stored in cacheFile.hdr
  cacheFile += "/"+crc;
  return cacheFile;
}


bool Cache::contains(const URI & uri, bool stripInternal) const
{
  if (m_useCache)
  {
    CachedMap::const_iterator pos;
    if (stripInternal and not uri.internalLink().empty())
    {
      // has an internal link... strip it, since we are interested in 
      // real page hits
      const std::string & fn(uri.fileName());
      const URI & noint = uri.navigateTo(fn);
      pos = m_fileIds.find(noint.crc32int());
    }
    else
    {
      pos = m_fileIds.find(uri.crc32int());
    }
    return (pos != m_fileIds.end());
  }
  return false;
}

void Cache::feed(const std::string & filename)
{
  nds::File file;
  file.open(filename.c_str());
  // read the lot
  if (file.is_open())
  {
    char buffer[256];
    int read;
    while ( (read = file.read(buffer, 256)) != 0)
    {
      m_document.appendData(buffer, read);
    }
    m_document.flush();
  }
}

void Cache::add(const URI & uri)
{
  m_fileIds.insert(uri.crc32int());
  const URI & noint(uri.navigateTo(uri.fileName()));
  m_fileIds.insert(noint.crc32int());
}

void Cache::remove(const URI & uri)
{
  if (m_useCache)
  {
    std::string cacheFile(uri2CacheFile(uri));
    if (nds::File::exists(cacheFile.c_str()) == nds::File::F_REG)
    {
      nds::File::unlink(cacheFile.c_str());
    }
    if (nds::File::exists((cacheFile+".hdr").c_str()) == nds::File::F_REG)
    {
      nds::File::unlink((cacheFile+".hdr").c_str());
    }
    m_fileIds.erase(uri.crc32int());
  }
}

void Cache::clearCache(void)
{
  //fprintf(stderr, "Clearing cache...\n");

  std::vector<std::string> entries;
  nds::File::ls(CACHE_DIR, entries);

  for(const std::string& entry : entries)
  {
    const std::string full_path = std::string(CACHE_DIR) + std::string("/") + entry;
    //fprintf(stderr, "- '%s'\n", full_path.c_str());
    nds::File::unlink(full_path.c_str());
  }
}

bool Cache::load(const URI & uri)
{
  if (m_useCache)
  {
    m_document.setCacheFile("");
    std::string cacheFile(uri2CacheFile(uri));
    if (contains(uri) and nds::File::exists(cacheFile.c_str()) == nds::File::F_REG)
    {
      add(uri);
      if (m_document.historyEnabled())
        m_document.reset();
      feed(cacheFile+".hdr");
      m_document.appendData("\r\n", 2);
      m_document.flush();

      // now check the cache control header
      CacheControl control(m_document.headerParser().cacheControl());
      time_t then(nds::File::mtime(cacheFile.c_str()));
      control.setResponseTime(then);

      time_t now(::time(0));
      if (control.shouldCache(now)) {
        feed(cacheFile);
        return true;
      }
      if (m_document.historyEnabled())
        m_document.reset();
    }
    add(uri);
    m_document.setCacheFile(cacheFile);
  }
  return false;
}

std::string Cache::fileName(const URI & uri) const
{
  if (m_useCache and contains(uri))
  {
    return uri2CacheFile(uri);
  }
  return "";
}
