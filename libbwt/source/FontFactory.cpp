#include "FontFactory.h"
#include "Font.h"
#include "File.h"
#include "libnds.h"

Font *FontFactory::create(const char *fileName)
{
  nds::File fontFile;
  Font *font = new (std::nothrow) Font();
  if (font == NULL)
    libndsCrash("FontFactory: OOM");

  std::string setName(fileName);
  setName += ".set";
  fontFile.open(setName.c_str());
  if (!fontFile.is_open()) {
    return font;
  }
  // read the lot
  unsigned char * glyphData(0);
  int size = fontFile.size();
  glyphData = new (std::nothrow) unsigned char[size+1];
  if (glyphData == NULL)
    return font;

  fontFile.read((char*)glyphData);
  glyphData[size] = 0;
  fontFile.close();

  // read the map data too
  nds::File mapFile;
  std::string mapName(fileName);
  mapName += ".map";
  mapFile.open(mapName.c_str());
  if (!mapFile.is_open()) {
    delete[] glyphData;
    return font;
  }

  unsigned char * data(0);
  size = mapFile.size();
  data = new (std::nothrow) unsigned char[size+2];
  if (data == NULL) {
    delete[] glyphData;
    return font;
  }

  mapFile.read((char*)data);
  data[size] = 0;
  mapFile.close();
  if (glyphData and data) {
    font->init(glyphData, data);
  }
  delete[] data;
  delete[] glyphData;
  return font;
}

Font *FontFactory::create(
    const unsigned char *imageData,
    const unsigned char *mapData)
{
  Font *font = new (std::nothrow) Font();
  if (font == NULL)
    libndsCrash("FontFactory: OOM");
  font->init(imageData, mapData);
  return font;
}
