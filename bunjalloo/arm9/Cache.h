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
#ifndef Cache_h_seen
#define Cache_h_seen


#include <string>
#include <set>

class Document;
class URI;
class CacheControl;
/** Store web pages on disk to save requesting them again. */
class Cache
{
  public:
    static const char * CACHE_DIR;

    /** Create the cache.
     * @param document needed for setCacheFile, so doc model persists itself.
     * @param useCache use a cache or not.
     */
    Cache(Document & document, bool useCache);

    /** Does the given URI live in the cache already?
     * @param uri the uri to check.
     * @param stripInternal remove any internal link, to check cache hits
     * @return true if the file exists, false otherwise.
     */
    bool contains(const URI & uri, bool stripInternal=true) const;

    /** Load a file from the cache. Loads the original headers and the data.
     * @param uri the uri to load.
     * @return true if the file was loaded from the cache, false otherwise.
     */
    bool load(const URI & uri);

    /** Remove a web site address from the cache. This removes the actual file
     * from the disk too.
     * @param uri The address to remove.
     */
    void remove(const URI & uri);

    /** Removes all files from the cache in the disk. */
    void clearCache(void);

    /** Given a URI, return the name of the file that it is stored in the cache.
     * @param uri A website address.
     * @return the cache file name that this website is stored as.
     */
    std::string fileName(const URI & uri) const;

    /** Remove the given uri form the cache.
     * @param uri the uri to clean.
     */
    void clean(const URI & uri);

    /** Get the status of the cache usage.
     * @return true if we are currently using a cache at all, false otherwise.
     */
    inline bool useCache() const;

    /**
     * @param useCache use the cache
     */
    void setUseCache(bool useCache);

  private:
    Document & m_document;
    bool m_useCache;
    typedef std::set<int> CachedMap;
    CachedMap m_fileIds;

    void feed(const std::string & filename);
    void add(const URI & uri);
};

bool Cache::useCache() const
{
  return m_useCache;
}
#endif
