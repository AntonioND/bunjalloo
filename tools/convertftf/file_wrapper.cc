#include <cstdio>
#include "file_wrapper.h"

FileWrapper::FileWrapper()
: m_fp(0)
{}

FileWrapper::~FileWrapper()
{
  if (m_fp)
    fclose(m_fp);
}

void FileWrapper::open(const char *filename, const char *mode)
{
  m_fp = fopen(filename, mode);
}

bool FileWrapper::is_open() const
{
  return m_fp != 0;
}

void FileWrapper::write8(unsigned int value)
{
  unsigned char one(value&0xff);
  fwrite(&one, 1, 1, m_fp);
}

void FileWrapper::write16(unsigned int value)
{
  write8(value>>8);
  write8(value);
}

void FileWrapper::write32(unsigned int value)
{
  write8(value>>24);
  write8(value>>16);
  write8(value>>8);
  write8(value);
}

void FileWrapper::writeData(const void *data, int count)
{
  const char *begin(static_cast<const char*>(data));
  const char *end(begin + count);
  for (const char *it(begin); it != end; ++it) {
    write8(*it);
  }
}
