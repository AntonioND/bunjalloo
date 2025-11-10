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
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <cstring>
#include <cstdio>
#include "File.h"
using namespace std;

void nds::File::readlines(std::vector<std::string> & lines)
{
  lines.clear();
  if (is_open())
  {
    int size = this->size();

    char * data = new char[size+2];
    if (data == NULL)
      return;

    read(data);
    int startOfLine = 0;
    for (int i = 0; i < size; ++i)
    {
      if (data[i] == '\n')
      {
        lines.push_back(string(&data[startOfLine], i-startOfLine));
        startOfLine = i+1;
      }
    }
    if (startOfLine < size) {
      lines.push_back(string(&data[startOfLine], size - startOfLine));
    }

    delete [] data;
  }
}

const char * nds::File::base(const char * path)
{
  static string str;
  str = path;
  size_t pos = str.rfind("/");
  if (pos == string::npos)
    return path;
  else
    return &path[pos+1];
}

const char * nds::File::dirname(const char * path)
{
  static string str;
  str = path;
  size_t pos = str.rfind("/");
  if (pos == string::npos)
  {
    return ".";
  }
  else {
    str = str.substr(0, pos);
    return str.c_str();
  }
}

nds::File::FileType nds::File::existsCommon(const char * path)
{
  struct stat s;
  int result = stat(path, &s);
  if (result == 0)
  {
    if (S_ISREG(s.st_mode))
    {
      return F_REG;
    }
    else if (S_ISDIR(s.st_mode))
    {
      return F_DIR;
    }
  }
  return F_NONE;
}


bool nds::File::mkdirCommon(const char * path)
{
  FileType ex = exists(path);
  if (ex == F_DIR)
  {
    return true;
  }
  if (ex == F_REG) {
    return false;
  }

  bool ok(false);
  // Start at the top and make the path
  string str(path);
  string component(path);
  size_t pos = str.find("/");
  while (  pos != string::npos)
  {
    component = str.substr(0, pos+1);
    if (exists(component.c_str()) == F_NONE)
    {
      nds::File::mkdir(component.c_str(), 0777);
    }
    pos = str.find("/", pos+1);
    if ((int)pos == -1)
        break;
  }
  component = str;
  if (not component.empty() and component != "." and exists(component.c_str()) == F_NONE)
  {
      int result = nds::File::mkdir(component.c_str(), 0777);
      ok = result == 0;
  }
  return ok;
}

bool nds::File::rmrf(const char * path)
{
  FileType ex(exists(path));
  if (ex == F_REG)
  {
    return unlink(path);
  }
  if (ex == F_NONE)
  {
    return true;
  }

  // directory - recursively remove.
  for (;;)
  {
    vector<string> contents;
    contents.clear();
    ls(path, contents);
    if (contents.empty())
    {
      unlink(path);
      return true;
    }
    for ( vector<string>::const_iterator it(contents.begin());
          it != contents.end(); ++it)
    {
      string name(path);
      name += "/";
      name += *it;
      FileType ftype = exists(name.c_str());
      switch (ftype)
      {
        case F_REG:
          unlink(name.c_str());
          break;
        case F_DIR:
          rmrf(name.c_str());
          break;
        case F_NONE:
          break;
      }
    }
  }
}

bool nds::File::cpCommon(const char * src, const char * dst)
{
  bool ok = true;
  FILE *in = fopen(src, "rb");
  if (in != 0)
  {
    FILE *out = fopen(dst, "wb");
    if (out != 0)
    {
      int ch = 0;
      while((ch = getc(in)) != EOF)
      {
        putc(ch, out);
      }
      if (ferror(in) or ferror(out))
      {
        ok = false;
      }
      if (fclose(out) != 0)
      {
        ok = false;
      }
    }
    else
    {
      ok = false;
    }
    fclose(in);
  }
  else
  {
    ok = false;
  }
  return ok;
}

void nds::File::lsCommon(const char * path, std::vector<std::string> & entries)
{
  DIR * dir = ::opendir(path);
  if (dir == NULL)
  {
    return;
  }
  struct dirent * ent;

  while ( (ent = ::readdir(dir)) != 0)
  {
    if (strcmp( ent->d_name, ".") != 0 and strcmp(ent->d_name, "..") != 0 )
    {
      entries.push_back(ent->d_name);
    }
  }
  ::closedir(dir);
}
