/*
  Copyright (C) 2008 Richard Quirk

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
#ifndef ZipFile_h_seen
#define ZipFile_h_seen

#include <vector>
#include <string>
#include "util/classhelper.h"

class ZipFileImpl;
class ExtractListener
{
  public:
    virtual ~ExtractListener() {}
    /** Should the file be extracted.
     * @param filename the name of the file.
     * @return true if it should, false otherwise.
     */
    virtual bool extract(const char * filename) = 0;
    virtual void before(const char * filename) = 0;
    virtual void after(const char * filename) = 0;
};

class ZipFile
{
  public:
    /** Create an object for handling zipped files.
     */
    explicit ZipFile(ExtractListener * listener);

    /** Close the zip file on destruction. */
    ~ZipFile();

    /** Open a zip archive.
     * @param filename The name of the archive to open.
     */
    void open(const char * filename);

    /** Fetch the list of files inside the zip file.
     * @param ls the returned list of files is in this vector.
     */
    void list(std::vector<std::string> & ls);

    /** Extract the compressed files from the zip file.  */
    void extract();

    /** Close the file.
     */
    void close();

    bool is_open() const;

    void setListener(ExtractListener * listener);
  private:
    ZipFileImpl * m_impl;
    DISALLOW_COPY_AND_ASSIGN(ZipFile);
};
#endif
