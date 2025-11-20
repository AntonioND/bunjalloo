#ifndef MiniMessage_h_seen
#define MiniMessage_h_seen

#include "Background.h"
#include "util/classhelper.h"
namespace nds
{
  class MiniMessage
  {
    public:
      MiniMessage(const char * msg);
      void failed();
      void ok();

    private:
      static int s_lastRow;
      int m_max;
      int m_pal;
      Background m_background;

      void init();
      void print(const char * msg, int x, int y);

      DISALLOW_COPY_AND_ASSIGN(MiniMessage);
  };
}
#endif
