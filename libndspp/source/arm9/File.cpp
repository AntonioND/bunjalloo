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
#include <string>
#include <sys/stat.h>
#include <cstring>
#include <fat.h>
#include <unistd.h>
#include <stdio.h>
#include <dirent.h>

using namespace nds;

class FatLibrary
{
  public:
    static FatLibrary & instance();
  private:
    FatLibrary();
    ~FatLibrary();
};

FatLibrary & FatLibrary::instance()
{
  static FatLibrary s_instance;
  return s_instance;
}
FatLibrary::FatLibrary()
{
  bool result = fatInitDefault();
  MiniMessage msg("Initialising FAT card");
  if (not result)
  {
    msg.failed();
  }
  msg.ok();
}
FatLibrary::~FatLibrary()
{
  // Shut down fat? "should never happen"...
}

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

    FileImplementation(const FileImplementation&);
    const FileImplementation operator=(const FileImplementation&);
};

// delegate class:
FileImplementation::FileImplementation(): m_stream(0)
{
  // make sure we have initialised the fat library.
  FatLibrary::instance();
}

void * FileImplementation::file() const
{
  return m_stream;
}

void FileImplementation::open(const char * name, const char * mode)
{
  if (m_stream) {
    close();
  }
  m_stream = fopen(name, mode);
}


FileImplementation::~FileImplementation()
{
  if (m_stream) {
    fflush(m_stream);
    fclose(m_stream);
  }
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
  fseek(m_stream, 0, SEEK_SET);
  fflush(m_stream);
  fclose(m_stream);
  m_stream = 0;
}

bool FileImplementation::eof() const
{
  return feof(m_stream) != 0;
}

// File proxy as follows:
File::File()
{
  m_details = new FileImplementation();
  if (m_details == NULL)
    libndsCrash("File: OOM");
}

File::~File()
{
  delete m_details;
}

void File::open(const char * name, const char * mode)
{
  m_details->open(name, mode);
}

int File::read(char * buffer, int amount)
{
  return m_details->read(buffer, amount);
}

bool File::eof() const
{
  return m_details->eof();
}
int File::write(const char * buffer, int amount)
{
  return m_details->write(buffer, amount);
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

nds::File::FileType nds::File::exists(const char * path)
{
  FatLibrary::instance();
  return existsCommon(path);
}
bool nds::File::mkdir(const char * path)
{
  FatLibrary::instance();
  return mkdirCommon(path);
}

int nds::File::mkdir(const char * path, unsigned int mode)
{
  FatLibrary::instance();
  return ::mkdir(path, mode);
}

bool nds::File::unlink(const char * path)
{
  FatLibrary::instance();
  return ::remove(path) == 0;
}

void nds::File::ls(const char * path, std::vector<std::string> & entries)
{
  lsCommon(path, entries);
}

bool nds::File::cp(const char *src, const char *dst)
{
  FatLibrary::instance();
  return cpCommon(src, dst);
}

void * File::file() const
{
  return m_details->file();
}


void File::utime(const char * path, const utimbuf * buf)
{
  // Something like this in libfat, if I could be bothered.
  /*
  PARTITION * partition = _FAT_partition_getPartitionFromPath (path);
  if (partition == 0 or partition->readOnly)
    return;
  bool fileExists = _FAT_directory_entryFromPath (partition, &dirEntry, path, NULL);
  if (fileExists and not  _FAT_directory_isDirectory(&dirEntry))
  {
    // Set the creation time and date
    dirEntry.entryData[DIR_ENTRY_cTime_ms] = 0;
    // _FAT_filetime_get*From_time_t do not really exist.
    u16_to_u8array (dirEntry.entryData, DIR_ENTRY_cTime, _FAT_filetime_getTimeFrom_time_t( utimbuf->actime ));
    u16_to_u8array (dirEntry.entryData, DIR_ENTRY_cDate, _FAT_filetime_getDateFrom_time_t( utimbuf->actime ));
  }
  */
}


time_t nds::File::mtime(const char * path) {
  struct stat buf;
  buf.st_mtime = 0;
  ::stat(path, &buf);
  return buf.st_mtime;
}
