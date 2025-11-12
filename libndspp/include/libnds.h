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
#ifndef nds_h_seen
#define nds_h_seen

#include <stdint.h>

#if !defined(ARM9) && !defined(ARM7)
// wrapper functions and defines to emulate NDS calls.
typedef unsigned char           u8;
typedef unsigned short int      u16;
typedef unsigned int            u32;
typedef unsigned long long int  u64;

typedef signed char             s8;
typedef signed short int        s16;
typedef signed int              s32;
typedef signed long long int    s64;

typedef volatile u8          vu8;
typedef volatile u16         vu16;
typedef volatile u32         vu32;
typedef volatile u64         vu64;

typedef volatile s8           vs8;
typedef volatile s16          vs16;
typedef volatile s32          vs32;
typedef volatile s64          vs64;
typedef void (* VoidFunctionPointer)(void);

#define BIT(n) (1 << (n))
#define BG_WRAP_ON     (1 << 13)
#define BG_MOSAIC_ON   (BIT(6))
#define BG_256_COLOR   (BIT(7))
#define BG_16_COLOR   (0)
#define BG_SIZE(m)		((m<<14))

#define RGB5(r,g,b)  ((r)|((g)<<5)|((b)<<10))
#define RGB8(r,g,b)  (((r)/8)|(((g)/8)<<5)|(((b)/8)<<10))

// Sprite control defines

// Attribute 0 consists of 8 bits of Y plus the following flags:
#define ATTR0_NORMAL			(0<<8)
#define ATTR0_ROTSCALE			(1<<8)
#define ATTR0_DISABLED			(2<<8)
#define ATTR0_ROTSCALE_DOUBLE	(3<<8)

#define ATTR0_TYPE_NORMAL		(0<<10)
#define ATTR0_TYPE_BLENDED		(1<<10)
#define ATTR0_TYPE_WINDOWED		(2<<10)
#define ATTR0_BMP				(3<<10)

#define ATTR0_MOSAIC			(1<<12)

#define ATTR0_COLOR_16		(0<<13) //16 color in tile mode...16 bit in bitmap mode
#define ATTR0_COLOR_256		(1<<13)

#define ATTR0_SQUARE		(0<<14)
#define ATTR0_WIDE			(1<<14)
#define ATTR0_TALL			(2<<14)

#define OBJ_Y(m)			((m)&0x00ff)

// Atribute 1 consists of 9 bits of X plus the following flags:
#define ATTR1_ROTDATA(n)      ((n)<<9)  // note: overlaps with flip flags
#define ATTR1_FLIP_X          (1<<12)
#define ATTR1_FLIP_Y          (1<<13)
#define ATTR1_SIZE_8          (0<<14)
#define ATTR1_SIZE_16         (1<<14)
#define ATTR1_SIZE_32         (2<<14)
#define ATTR1_SIZE_64         (3<<14)

#define OBJ_X(m)			((m)&0x01ff)

// Atribute 2 consists of the following:
#define ATTR2_PRIORITY(n)     ((n)<<10)
#define ATTR2_PALETTE(n)      ((n)<<12)
#define ATTR2_ALPHA(n)		  ((n)<<12)


typedef struct sSpriteEntry {
  unsigned short attribute[3];
  unsigned short filler;
} SpriteEntry, * pSpriteEntry;
typedef struct sSpriteRotation {
  unsigned short filler1[3];
  short hdx;

  unsigned short filler2[3];
  short hdy;

  unsigned short filler3[3];
  short vdx;

  unsigned short filler4[3];
  short vdy;
} SpriteRotation, * pSpriteRotation;

void dmaCopy(const void * source, void * destination, unsigned int length);

#define POWER_ALL
#define POWER_ALL_2D
void powerOn(void);

#define MODE_0_2D      0x10000
#define MODE_1_2D      0x10001
#define MODE_2_2D      0x10002
#define MODE_3_2D      0x10003
#define MODE_4_2D      0x10004
#define MODE_5_2D      0x10005

#define SCREEN_HEIGHT  192
#define SCREEN_WIDTH   256

#define IRQ_VBLANK 0
void irqInit();

void cothread_yield_irq(uint32_t flag);

#define COPY_MODE_HWORD  (0)
#define COPY_MODE_WORD  (1<<26)
#define COPY_MODE_COPY  (0)
#define COPY_MODE_FILL  (1<<24)
void swiCopy(const void * source, void * dest, int flags);

//!	Bit values for the keypad buttons.
typedef enum KEYPAD_BITS {
  KEY_A      = BIT(0),  //!< Keypad A button.
  KEY_B      = BIT(1),  //!< Keypad B button.
  KEY_SELECT = BIT(2),  //!< Keypad SELECT button.
  KEY_START  = BIT(3),  //!< Keypad START button.
  KEY_RIGHT  = BIT(4),  //!< Keypad RIGHT button.
  KEY_LEFT   = BIT(5),  //!< Keypad LEFT button.
  KEY_UP     = BIT(6),  //!< Keypad UP button.
  KEY_DOWN   = BIT(7),  //!< Keypad DOWN button.
  KEY_R      = BIT(8),  //!< Right shoulder button.
  KEY_L      = BIT(9),  //!< Left shoulder button.
  KEY_X      = BIT(10), //!< Keypad X button.
  KEY_Y      = BIT(11), //!< Keypad Y button.
  KEY_TOUCH  = BIT(12), //!< Touchscreen pendown.
  KEY_LID    = BIT(13)  //!< Lid state.
} KEYPAD_BITS;
void scanKeys();
u32 keysDown();
u32 keysUp();
u32 keysHeld();
u32 keysDownRepeat();
void keysSetRepeat( u8 setDelay, u8 setRepeat );
//!     Obtains the current touchscreen co-ordinates.
typedef struct {
  int x;
  int y;
  int px;
  int py;
} touchPosition;

void touchRead(touchPosition *data);

u32 keysRealKeyboard();

void irqSet(int irq, VoidFunctionPointer fp);

void setGenericSound( u32 rate, u8 vol, u8 pan, u8 format);
void playGenericSound(const void* data, u32 length);

short cosLerp(short angle);
short sinLerp(short angle);

bool peripheralSlot2InitDefault(void);
uint16_t *peripheralSlot2RamStart(void);
uint32_t peripheralSlot2RamSize(void);

__attribute__((noreturn)) void libndsCrash(const char *message);

// ARM9, ARM7
#else
#ifdef __cplusplus
extern "C" {
#endif
#include <nds/ndstypes.h>
#include <nds/bios.h>
#include <nds/card.h>
#include <nds/dma.h>
#include <nds/interrupts.h>
#include <nds/ipc.h>
#include <nds/memory.h>
#include <nds/system.h>
#include <nds/timers.h>
#include <nds/arm9/background.h>
#include <nds/arm9/cache.h>
#include <nds/arm9/console.h>
#include <nds/arm9/exceptions.h>
#include <nds/arm9/image.h>
#include <nds/arm9/input.h>
#include <nds/arm9/math.h>
#include <nds/arm9/pcx.h>
#include <nds/arm9/rumble.h>
#include <nds/arm9/sound.h>
#include <nds/arm9/trig_lut.h>
#include <nds/arm9/video.h>
// No! #include <nds/arm9/videoGL.h>
// No! #include <nds/arm9/boxtest.h>
#include <nds/arm9/sprite.h>
#include <nds/arm9/peripherals/slot2.h>
#include <nds/cothread.h>
#ifdef __cplusplus
}
#endif
#ifdef ARM9
void setGenericSound( u32 rate, u8 vol, u8 pan, u8 format);
void playGenericSound(const void* data, u32 length);
#endif
#endif
#endif
