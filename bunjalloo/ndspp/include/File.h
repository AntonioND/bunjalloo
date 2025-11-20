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
#ifndef File_h_seen
#define File_h_seen

#include <time.h>
#include <string>
#include <vector>
#include "util/classhelper.h"

struct utimbuf;
namespace nds
{
  class FileImplementation;

  class File
  {
    public:

      /** Get the base name of the file.
       * @param path the file path
       * @return the base name of the file - e.g for /path/to/file returns "file".
       */
      static const char * base(const char * path);

      /** Get the directory of the file.
       * @param path the file path
       * @return the directory of the file - e.g for /path/to/file returns "/path/to", for "file" returns "."
       */
      static const char * dirname(const char * path);

      /** Returned from exists call to get the file type.*/
      enum FileType
      {
        F_NONE,//!< non existant
        F_REG, //!< regular file
        F_DIR  //!< directory
      };
      /** See if a file or directory exists.
       * @param path the file path
       * @return the type of node if it exists, or F_NONE if it does not.
       */
      static FileType exists(const char * path);

      /** Create a directory. Make parent directories as needed.
       * @param path the directory path
       * @return true if the new directory exists, false if it does not.
       */
      static bool mkdir(const char * path);

      static void utime(const char * path, const utimbuf * buf);
      /** Delete a file or directory. 
       * @param path the file or dir to delete 
       */
      static bool unlink(const char * path);
      /** Recursively delete files in a given directory
       * @param path the file or dir to delete 
       */
      static bool rmrf(const char * path);

      /** Copy a file from one place to another.
       * @param src source file name
       * @param dst the target file name
       */
      static bool cp(const char * src, const char * dst);

      /** Get a list of files in a directory.
       * @param path the directory to list
       * @param entries an array of contents.
       */
      static void ls(const char * path, std::vector<std::string> & entries);


      /** Get the time since last modified */
      static time_t mtime(const char * path);

      //! Create a file object.
      File();

      /** Open a file using the given mode.
       * @param name the file name.
       * @param mode the open mode, "w", "r", etc.
       */
      void open(const char * name, const char * mode="r");

      /** @return true if the file is open.*/
      bool is_open() const;

      /** closes the file.
       */
      ~File();

      /** Read at most size bytes, returned as an array of bytes.
       * @param buffer An area to read the data into.
       * @param size the amount to read. If ommitted or negative, reads until EOF.
       */
      int read(char * buffer, int size=-1);

      /** Write some bytes to the file.
       * @param buffer the data to write
       * @param size the amount to write. If ommitted or negative, writes until 0 byte.
       */
      int write(const char * buffer, int size=-1);

      /** is at the end of file?.
       * @return true if at the end of the file, false otherwise.
       */
      bool eof() const;

      /** Get the size of the file in bytes.
       */
      int size();

      /** Read the file as lines.
       * @param lines the lines of the file.
       */
      void readlines(std::vector<std::string> & lines);

      /** Closes the file.*/
      void close();

      /** Hack to get underlying FILE* object
       * */
      void * file() const;

    private:
      FileImplementation * m_details;

      static FileType existsCommon(const char * path);
      static bool mkdirCommon(const char * path);
      static int mkdir(const char * path, unsigned int mode);
      static bool cpCommon(const char * src, const char * dst);
      static void lsCommon(const char * path, std::vector<std::string> & entries);

      DISALLOW_COPY_AND_ASSIGN(File);
  };
}

#endif
