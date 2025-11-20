#include <stdio.h>
#include "MiniMessage.h"
#include "Background.h"
#include "Palette.h"
#include "Video.h"
#include "libnds.h"
#include "minimessage_data.h"

using nds::MiniMessage;

int MiniMessage::s_lastRow(2);

MiniMessage::MiniMessage(const char * msg):
  m_background(0, 0, 0, 31)
{
  // dump to screen
  // first set the screen to the right mode
  init();
  // print out msg to  0, s_lastRow
  print(msg, 0, s_lastRow);
}

void MiniMessage::print(const char * msg, int x, int y)
{
  int row = y;
  int col = x;
  while (*msg)
  {
    unsigned short * mapData = m_background.mapData();
    mapData[col + row*32] = (*msg - ' ') | (m_pal<<12);
    col++;
    if (col == m_max) {
      col = 0;
      s_lastRow++;
      row++;
    }
    msg++;
  }
}

void MiniMessage::init()
{
  using nds::Video;
  Video & vid(Video::instance(0));
  vid.setMode(0);
  m_background.setEnabled();
  m_background.setColor(16);
  unsigned short *tileData = m_background.tileData();
  dmaCopy(minimessage_dataTiles, tileData, minimessage_dataTilesLen);
  nds::Palette p(0);
  p[0] = 0;
  p[1] = nds::Color(31, 31, 31);
  p[16 + 1] = nds::Color(31, 0, 0);
  p[16*2 + 1] = nds::Color(0, 31, 0);
  m_max = 25;
  m_pal = 0;
}

void MiniMessage::failed()
{
  m_max = 32;
  m_pal = 1;
  print("FAILED", 26, s_lastRow);
  for (;;)
  {
    cothread_yield_irq(IRQ_VBLANK);
  }
}

void MiniMessage::ok()
{
  m_max = 32;
  m_pal = 2;
  print("OK", 26, s_lastRow);
  s_lastRow++;
}
