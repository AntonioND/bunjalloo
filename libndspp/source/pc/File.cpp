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
#include "libnds.h"
#include "File.h"
#include "MiniMessage.h"
#include <errno.h>
#include <cstring>
#include <cstdlib>
#include <string>
#include <stdio.h>
#include <unistd.h>
#include <utime.h>

using namespace nds;

class StaticFileInit
{
  private:
    StaticFileInit()
    {
      char * dldiCheck = getenv("NDS_DLDI");
      if (dldiCheck)
      {
        MiniMessage msg("Initialising FAT card");
        int ok = strtol(dldiCheck, 0, 0);
        if (not ok)
        {
          // show an error message and "hang"
          msg.failed();
        }
        else
        {
          msg.ok();
        }
      }
    }

  public:
    static StaticFileInit & instance()
    {
      static StaticFileInit s_instance;
      return s_instance;
    }
};

class nds::FileImplementation
{
  public:
    FileImplementation();
    void open(const char * name, const char * mode);
    ~FileImplementation();

    int read(char * buffer, int amount=-1);
    int write(const char * buffer, int amount=-1);
    int size();
    bool is_open() const;
    void close();
    bool eof() const;
    void * file() const;

  private:
    FILE * m_stream;

};

// delegate class:
FileImplementation::FileImplementation(): m_stream(0)
{
  StaticFileInit::instance();
}

void FileImplementation::open(const char * name, const char * mode)
{
  if (m_stream) {
    close();
  }
  m_stream = fopen(name, mode);
}

void * FileImplementation::file() const
{
  return m_stream;
}

FileImplementation::~FileImplementation()
{
  if (m_stream)
    fclose(m_stream);
}

int FileImplementation::read(char * buffer, int amount)
{
  if (not m_stream)
  {
    return 0;
  }

  int bytesExpected = amount;
  if (amount < 0) {
    // read all bytes
    rewind(m_stream);
    bytesExpected = this->size();
  }
  return fread(buffer, 1, bytesExpected, m_stream);
}

int FileImplementation::write(const char * buffer, int amount)
{
  if (not m_stream)
  {
    return 0;
  }

  int bytesExpected = amount;
  if (amount < 0) {
    bytesExpected = strlen(buffer);
  }
  return fwrite(buffer, 1, bytesExpected, m_stream);
}

int FileImplementation::size()
{
  if (not m_stream)
    return 0;

  int current = ftell(m_stream);
  fseek(m_stream, 0, SEEK_END);
  int fileSize = ftell(m_stream);
  fseek(m_stream, current, SEEK_SET);
  return fileSize;
}

bool FileImplementation::is_open() const
{
  return m_stream != 0;
}
void FileImplementation::close()
{
  if (m_stream) 
    fclose(m_stream);
  m_stream = 0;
}

bool FileImplementation::eof() const
{
  return feof(m_stream) != 0;
}
// File proxy as follows:
File::File():m_details(new FileImplementation())
{}

static std::string toFat(const char * path)
{
  char * dirname = get_current_dir_name();
  std::string fullpath(dirname);
  free(dirname);
  if (path[0] != '/') {
    fullpath += "/";
  }
  fullpath += path;
  return fullpath;
}

void File::open(const char * name, const char * mode)
{
  // append name to cwd to emulate FAT behaviour.
  std::string fullpath(toFat(name));
  m_details->open(fullpath.c_str(), mode);
}

File::~File()
{
  delete m_details;
}

int File::read(char * buffer, int amount)
{
  return m_details->read(buffer, amount);
}

int File::write(const char * buffer, int amount)
{
  return m_details->write(buffer, amount);
}

bool File::eof() const
{
  return m_details->eof();
}

int File::size()
{
  return m_details->size();
}

bool File::is_open() const
{
  return m_details->is_open();
}

void File::close()
{
  m_details->close();
}

#include <fcntl.h>
#include <sys/types.h>
/*
bool nds::File::direxists(const char * path)
{
  // see if the file exists
  std::string fullpath(toFat(path));
  int fp = ::open(fullpath.c_str(), O_RDONLY|O_DIRECTORY);
  printf("fp %d  for %s from %s\n", fp, fullpath.c_str(), path);
  bool ex = false;
  if (fp != -1)
  {
    ex = true;
    ::close(fp);
  }
  return ex;
}
*/

nds::File::FileType nds::File::exists(const char * path)
{
  StaticFileInit::instance();
  return existsCommon(toFat(path).c_str());
}

bool nds::File::mkdir(const char * path)
{
  StaticFileInit::instance();
  return mkdirCommon(path);
}

int File::mkdir(const char * path, unsigned int mode)
{
  std::string fullpath(toFat(path));
  return ::mkdir(fullpath.c_str(), mode);
}

bool File::unlink(const char * path)
{
  StaticFileInit::instance();
  //return ::unlink(toFat(path).c_str()) == 0;
  int result(0);
  const std::string & actualPath = toFat(path);
  FileType type = exists(path);
  switch (type)
  {
    case F_DIR:
      result = ::rmdir(actualPath.c_str());
      break;
    case F_REG:
      result = ::unlink(actualPath.c_str());
      break;
    case F_NONE:
      return true;
  }
  int err = errno;
  if (result != 0)
  {
    char * errmsg = strerror(err);
    printf("Error removing %s: %d, %s\n", actualPath.c_str(), err, errmsg);
  }
  return result == 0;
}

void nds::File::ls(const char * path, std::vector<std::string> & entries)
{
  lsCommon(toFat(path).c_str(), entries);
}

bool nds::File::cp(const char *src, const char *dst)
{
  return cpCommon(toFat(src).c_str(), toFat(dst).c_str());
}

void * File::file() const
{
  return m_details->file();
}

void File::utime(const char * path, const utimbuf * buf)
{
  ::utime(path, buf);
}

time_t File::mtime(const char * path)
{
  struct stat buf;
  buf.st_mtime = 0;
  ::stat(toFat(path).c_str(), &buf);
  return buf.st_mtime;
}

