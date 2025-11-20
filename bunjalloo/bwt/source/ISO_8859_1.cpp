#include <stdio.h>
#include "ISO_8859_1.h"

// Use the unicode replacement character for unused encodings
#define UNUSED 0xFFFD

const unsigned short s_1252[] = {
 UNUSED, // 127
 0x20AC, // 128 - €
 UNUSED, // 129
 0x201A, // 130 - ‚
 0x0192, // 131 - ƒ
 0x201E, // 132 - „
 0x2026, // 133 - …
 0x2020, // 134 - †
 0x2021, // 135 - ‡
 0x02C6, // 136 - ˆ
 0x2030, // 137 - ‰
 0x0160, // 138 - Š
 0x2039, // 139 - ‹
 0x0152, // 140 - Œ
 UNUSED, // 141
 0x017D, // 142 - Ž
 UNUSED, // 143
 UNUSED, // 144
 0x2018, // 145 - ‘
 0x2019, // 146 - ’
 0x201C, // 147 - “
 0x201D, // 148 - ”
 0x2022, // 149 - •
 0x2013, // 150 - –
 0x2014, // 151 - —
 0x02DC, // 152 - ˜
 0x2122, // 153 - ™
 0x0161, // 154 - š
 0x203A, // 155 - ›
 0x0153, // 156 - œ
 UNUSED, // 157
 0x017E, // 158 - ž
 0x0178, // 159 - Ÿ
};

unsigned short ISO_8859_1::decode(unsigned short value)
{
  if (value > 0x00 and value < 0x7F)
  {
    // Standard ASCII values are converted directly to Unicode code points
    return value;
  }
  else if (value >= 0x7F and value <= 0x9F)
  {
    // The values in this range aren't part of ISO 8859-1, so we use the
    // values of the Windows 1252 code page.
    return s_1252[value - 0x7f];
  }
  else if (value >= 0xA0 and value <= 0xFF)
  {
    // The code points of the characters used in Latin-1 have the same values
    // as their Unicode encodings:
    // https://en.wikipedia.org/wiki/Latin-1_Supplement
    return value;
  }
  else
  {
    // This shouldn't happen, the values are always 8-bit wide
    return UNUSED;
  }
}

