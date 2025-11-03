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

  This file incorporates work covered by the following copyright and
  permission notice:

    Copyright (C) 1998-2005 Gilles Vollant

    This software is provided 'as-is', without any express or implied
    warranty.  In no event will the authors be held liable for any damages
    arising from the use of this software.

    Permission is granted to anyone to use this software for any purpose,
    including commercial applications, and to alter it and redistribute it
    freely, subject to the following restrictions:

    1. The origin of this software must not be misrepresented; you must not
       claim that you wrote the original software. If you use this software
       in a product, an acknowledgment in the product documentation would be
       appreciated but is not required.
    2. Altered source versions must be plainly marked as such, and must not be
       misrepresented as being the original software.
    3. This notice may not be removed or altered from any source distribution.

*/
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include "unzip.h"
#include "ZipFile.h"
#include "File.h"
#include "scoped_ptr.h"
#include <errno.h>
#include <time.h>
#include <utime.h>
using std::string;

static int makedir (const char * newdir)
{
  char *buffer ;
  char *p;
  int  len = (int)strlen(newdir);

  if (len <= 0)
    return 0;

  buffer = (char*)malloc(len+1);
  strcpy(buffer,newdir);

  if (buffer[len-1] == '/') {
    buffer[len-1] = '\0';
  }
  if (nds::File::mkdir(buffer) == 0)
    {
      free(buffer);
      return 1;
    }

  p = buffer+1;
  while (1)
    {
      char hold;

      while(*p && *p != '\\' && *p != '/')
        p++;
      hold = *p;
      *p = 0;
      if ((!nds::File::mkdir(buffer)) && (errno == ENOENT))
        {
          // printf("couldn't create directory %s\n",buffer);
          free(buffer);
          return 0;
        }
      if (hold == 0)
        break;
      *p++ = hold;
    }
  free(buffer);
  return 1;
}
void change_file_date( const char *filename,
    uLong dosdate,
    tm_unz tmu_date)
{
  struct utimbuf ut;
  struct tm newdate;
  newdate.tm_sec = tmu_date.tm_sec;
  newdate.tm_min=tmu_date.tm_min;
  newdate.tm_hour=tmu_date.tm_hour;
  newdate.tm_mday=tmu_date.tm_mday;
  newdate.tm_mon=tmu_date.tm_mon;
  if (tmu_date.tm_year > 1900)
      newdate.tm_year=tmu_date.tm_year - 1900;
  else
      newdate.tm_year=tmu_date.tm_year ;
  newdate.tm_isdst=-1;

  ut.actime=ut.modtime=mktime(&newdate);
  nds::File::utime(filename,&ut);
}

#define WRITEBUFFERSIZE (8192)
static int do_extract_currentfile(
    ExtractListener * listener,
    unzFile uf,
    const int* popt_extract_without_path,
    int* popt_overwrite,
    const char* password)
{
  char filename_inzip[256];
  char* filename_withoutpath;
  char* p;
  int err=UNZ_OK;
  FILE *fout=NULL;
  uInt size_buf;

  unz_file_info file_info;
  // uLong ratio=0;
  err = unzGetCurrentFileInfo(uf,&file_info,filename_inzip,sizeof(filename_inzip),NULL,0,NULL,0);

  if (err!=UNZ_OK)
  {
    // printf("error %d with zipfile in unzGetCurrentFileInfo\n",err);
    return err;
  }

  size_buf = WRITEBUFFERSIZE;
  scoped_ptr_malloc<unsigned char> buf(static_cast<unsigned char*>(malloc(size_buf)));
  if (buf.get() == NULL)
  {
    // printf("Error allocating memory\n");
    return UNZ_INTERNALERROR;
  }

  p = filename_withoutpath = filename_inzip;
  if (listener) listener->before(filename_inzip);
  if (listener and not listener->extract(filename_inzip))
  {
    // skip the file
    listener->after(filename_inzip);
    return UNZ_OK;
  }
  while ((*p) != '\0')
  {
    if (((*p)=='/') || ((*p)=='\\'))
      filename_withoutpath = p+1;
    p++;
  }

  if ((*filename_withoutpath)=='\0')
  {
    if ((*popt_extract_without_path)==0)
    {
      // printf("creating directory: %s\n",filename_inzip);
      nds::File::mkdir(filename_inzip);
    }
  }
  else
  {
    const char* write_filename;
    int skip=0;

    if ((*popt_extract_without_path)==0)
      write_filename = filename_inzip;
    else
      write_filename = filename_withoutpath;

    err = unzOpenCurrentFilePassword(uf,password);
    if (err!=UNZ_OK)
    {
      // printf("error %d with zipfile in unzOpenCurrentFilePassword\n",err);
    }

    if ((skip==0) && (err==UNZ_OK))
    {
      fout=fopen(write_filename,"wb");

      /* some zipfile don't contain directory alone before file */
      if ((fout==NULL) && ((*popt_extract_without_path)==0) &&
          (filename_withoutpath!=(char*)filename_inzip))
      {
        char c=*(filename_withoutpath-1);
        *(filename_withoutpath-1)='\0';
        makedir(write_filename);
        *(filename_withoutpath-1)=c;
        fout=fopen(write_filename,"wb");
      }

      if (fout==NULL)
      {
        // printf("error opening %s\n",write_filename);
      }
    }

    if (fout!=NULL)
    {
      // printf(" extracting: %s\n",write_filename);

      do
      {
        err = unzReadCurrentFile(uf,buf.get(),size_buf);
        if (err<0)
        {
          // printf("error %d with zipfile in unzReadCurrentFile\n",err);
          break;
        }
        if (err>0)
          if (fwrite(buf.get(),err,1,fout)!=1)
          {
            // printf("error in writing extracted file\n");
            err=UNZ_ERRNO;
            break;
          }
      }
      while (err>0);
      if (fout)
        fclose(fout);

      if (err==0)
        change_file_date(write_filename,file_info.dosDate,
            file_info.tmu_date);
    }

    if (err==UNZ_OK)
    {
      err = unzCloseCurrentFile (uf);
      if (err!=UNZ_OK)
      {
        // printf("error %d with zipfile in unzCloseCurrentFile\n",err);
      }
    }
    else
      unzCloseCurrentFile(uf); /* don't lose the error */
  }

  if (listener) listener->after(filename_inzip);
  return err;
}

class ZipFileImpl
{
  public:
    ZipFileImpl(ExtractListener * listener): m_extractListener(listener)
    {
    }

    void setListener(ExtractListener * listener)
    {
      m_extractListener = listener;
    }

    void open(const char * filename)
    {
      m_filename = filename;
      if (nds::File::exists(filename) == nds::File::F_REG and filename[0] == '/')
      {
        // try relative if it exists.
        m_filename = m_filename.substr(1,m_filename.length());
      }
      m_file = unzOpen(m_filename.c_str());
    }

    bool is_open() const
    {
      return m_file != 0;
    }

    void list(std::vector<std::string> & ls)
    {
      int err = unzGoToFirstFile(m_file);
      if (err != UNZ_OK) {
        // printf("error %d with zipfile in unzGoToFirstFile \n",err);
        return;
      }

      unz_global_info gi;
      err = unzGetGlobalInfo (m_file,&gi);
      if (err!=UNZ_OK)
      {
        // printf("error %d with zipfile in unzGetGlobalInfo \n",err);
        return;
      }

      char filename_inzip[256];
      for (unsigned int i = 0; i < gi.number_entry; i++)
      {
        unz_file_info file_info;
        err = unzGetCurrentFileInfo(m_file,&file_info,filename_inzip,sizeof(filename_inzip),NULL,0,NULL,0);
        if (err!=UNZ_OK)
        {
          // printf("error %d with zipfile in unzGetCurrentFileInfo\n",err);
          break;
        }
        ls.push_back(filename_inzip);
        if ((i+1)<gi.number_entry)
        {
          err = unzGoToNextFile(m_file);
          if (err!=UNZ_OK)
          {
            // printf("error %d with zipfile in unzGoToNextFile\n",err);
            break;
          }
        }
      }
    }

    void extract()
    {
      int err = unzGoToFirstFile(m_file);
      if (err != UNZ_OK) {
        // printf("error %d with zipfile in unzGoToFirstFile \n",err);
        return;
      }

      unz_global_info gi;
      err = unzGetGlobalInfo (m_file,&gi);
      if (err!=UNZ_OK) {
        // printf("error %d with zipfile in unzGetGlobalInfo \n",err);
        return;
      }

      int opt_overwrite = 1;
      int opt_extract_without_path = 0;
      const char * password = 0;
      for (uLong i = 0; i < gi.number_entry; i++)
      {
        if (do_extract_currentfile(
              m_extractListener,
              m_file,&opt_extract_without_path,
              &opt_overwrite,
              password) != UNZ_OK)
        {
          break;
        }

        if ( (i+1) < gi.number_entry)
        {
          err = unzGoToNextFile(m_file);
          if (err!=UNZ_OK)
          {
            // printf("error %d with zipfile in unzGoToNextFile\n",err);
            break;
          }
        }
      }
    }

  private:
    string m_filename;
    unzFile m_file;
    ExtractListener * m_extractListener;
};


ZipFile::ZipFile(ExtractListener * listener) :
  m_impl(new ZipFileImpl(listener))
{
}
ZipFile::~ZipFile()
{
}

void ZipFile::open(const char * filename)
{
  m_impl->open(filename);
}

void ZipFile::list(std::vector<std::string> & ls)
{
  m_impl->list(ls);
}

void ZipFile::close()
{
}

bool ZipFile::is_open() const
{
  return m_impl->is_open();
}

void ZipFile::extract()
{
  m_impl->extract();
}

void ZipFile::setListener(ExtractListener * listener)
{
  m_impl->setListener(listener);
}
