#include <stdio.h>
#include "ISO_8859_1.h"
const unsigned short s_1252[] = {
    127, //127
 0x20AC, //128
    129, //129
 0x201A, //130
 0x0192, //131
 0x201E, //132
 0x2026, //133
 0x2020, //134
 0x2021, //135
 0x02C6, //136
 0x2030, //137
 0x0160, //138
 0x2039, //139
 0x0152, //140
    141, //141
 0x017D, //142
    143, //143
    144, //144
 0x2018, //145
 0x2019, //146
 0x201C, //147
 0x201D, //148
 0x2022, //149
 0x2013, //150
 0x2014, //151
 0x02DC, //152
 0x2122, //153
 0x0161, //154
 0x203A, //155
 0x0153, //156
    157, //157
 0x017E, //158
 0x0178, //159
};

unsigned short ISO_8859_1::decode(unsigned short value)
{
  if ( (value > 0x00 and value <= 0x1F))
  {
    return value;
  }
  else if (value >= 0x7F  and value <= 0x9F)
  {
    // windows 1252 code pages
    return s_1252[value-0x7f];
  }
  else
  {
    return value;
  }
}

