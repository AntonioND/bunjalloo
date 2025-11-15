# SPDX-License-Identifier: CC0-1.0
#
# SPDX-FileContributor: Antonio Niño Díaz, 2023-2025

export BLOCKSDS			?= /opt/blocksds/core
export BLOCKSDSEXT		?= /opt/blocksds/external

export WONDERFUL_TOOLCHAIN	?= /opt/wonderful
ARM_NONE_EABI_PATH	?= $(WONDERFUL_TOOLCHAIN)/toolchain/gcc-arm-none-eabi/bin/

# User config
# ===========

ifeq ($(PLAT),NDS)

    NAME		:= bunjalloo

    GAME_TITLE	:= Bunjalloo 0.10.0
    GAME_SUBTITLE	:= Web Browser
    GAME_AUTHOR	:= By Richard Quirk
    GAME_ICON	:= icon.bmp

else ifeq ($(PLAT),SDL)

    NAME		:= bunjalloo_sdl

endif

# DLDI and internal SD slot of DSi
# --------------------------------

# Root folder of the SD image
SDROOT		:= sdroot
# Name of the generated image it "DSi-1.sd" for no$gba in DSi mode
SDIMAGE		:= image.bin

# Source code paths
# -----------------

SOURCEDIRS	:= source

INCLUDEDIRS	:= ../libbunjalloo/include/ \
			   ../3rdparty/mbedtls-3.6.4/include/

GFXDIRS		:=
BINDIRS		:=
AUDIODIRS	:=
# List of folders to combine into the root of NitroFS:
NITROFSDIR	:= ../nitrofs/

# Defines passed to all files
# ---------------------------

DEFINES		:=

# Libraries
# ---------

ifeq ($(PLAT),SDL)

    LIBS    := -lbunjalloo_sdl -lbwt_sdl -lndspp_sdl \
               -lz_sdl -lplum_sdl -lmbedtls_sdl -lSDL -lGL -lGLU

else ifeq ($(PLAT),NDS)

    ARM7ELF := $(BLOCKSDS)/sys/arm7/main_core/arm7_dswifi.elf

    LIBS    := -lbunjalloo_nds -lbwt_nds -lndspp_nds \
               -lz_nds -lplum_nds -lmbedtls_nds \
               -ldswifi9 -lnds9

    LIBDIRS := $(BLOCKSDS)/libs/libnds \
               $(BLOCKSDS)/libs/dswifi \

endif

LIBDIRS += ../3rdparty \
           ../libbunjalloo \
           ../libbwt \
           ../libndspp \

# Build artifacts
# ---------------

ifeq ($(PLAT),SDL)

    BUILDDIR	:= build/sdl/$(NAME)
    ELF		:= $(NAME).elf

else ifeq ($(PLAT),NDS)

    BUILDDIR	:= build/nds/$(NAME)
    ELF		:= build/nds/$(NAME).elf
    DUMP		:= build/nds/$(NAME).dump
    MAP		:= build/nds/$(NAME).map

endif

ROM		:= $(NAME).nds

# If NITROFSDIR is set, the soundbank created by mmutil will be saved to NitroFS
SOUNDBANKINFODIR	:= $(BUILDDIR)/maxmod
ifeq ($(strip $(NITROFSDIR)),)
    SOUNDBANKDIR	:= $(BUILDDIR)/maxmod
else
    SOUNDBANKDIR	:= $(BUILDDIR)/maxmod_nitrofs
endif

# Tools
# -----

ifeq ($(PLAT),SDL)
    PREFIX		:=
else ifeq ($(PLAT),NDS)
    PREFIX		:= $(ARM_NONE_EABI_PATH)arm-none-eabi-
endif

CC		:= $(PREFIX)gcc
CXX		:= $(PREFIX)g++
LD		:= $(PREFIX)gcc
OBJDUMP		:= $(PREFIX)objdump
MKDIR		:= mkdir
RM		:= rm -rf

# Verbose flag
# ------------

ifeq ($(VERBOSE),1)
V		:=
else
V		:= @
endif

# Source files
# ------------

ifneq ($(BINDIRS),)
    SOURCES_BIN	:= $(shell find -L $(BINDIRS) -name "*.bin")
    INCLUDEDIRS	+= $(addprefix $(BUILDDIR)/,$(BINDIRS))
endif
ifneq ($(GFXDIRS),)
    SOURCES_PNG	:= $(shell find -L $(GFXDIRS) -name "*.png")
    INCLUDEDIRS	+= $(addprefix $(BUILDDIR)/,$(GFXDIRS))
endif
ifneq ($(AUDIODIRS),)
    SOURCES_AUDIO	:= $(shell find -L $(AUDIODIRS) -regex '.*\.\(it\|mod\|s3m\|wav\|xm\)')
    ifneq ($(SOURCES_AUDIO),)
        INCLUDEDIRS	+= $(SOUNDBANKINFODIR)
    endif
endif

SOURCES_S	:= $(shell find -L $(SOURCEDIRS) -name "*.s")
SOURCES_C	:= $(shell find -L $(SOURCEDIRS) -name "*.c")
SOURCES_CPP	:= $(shell find -L $(SOURCEDIRS) -name "*.cpp")

# Compiler and linker flags
# -------------------------

ifeq ($(PLAT),SDL)
    ARCH		:=
    SPECS		:=
else ifeq ($(PLAT),NDS)
    ARCH		:= -mthumb -mcpu=arm946e-s+nofp
    SPECS		:= -specs=$(BLOCKSDS)/sys/crts/ds_arm9.specs
endif

WARNFLAGS_C		:=
WARNFLAGS_CPP	:= -Wall -Wno-deprecated-declarations

ifeq ($(SOURCES_CPP),)
	LIBS	+= -lc
else
	LIBS	+= -lstdc++ -lc
endif

INCLUDEFLAGS	:= $(foreach path,$(INCLUDEDIRS),-I$(path)) \
		   $(foreach path,$(LIBDIRS),-I$(path)/include)

LIBDIRSFLAGS	:= $(foreach path,$(LIBDIRS),-L$(path)/lib)

ASFLAGS		+= -x assembler-with-cpp $(INCLUDEFLAGS) $(DEFINES) \
		   $(ARCH) -ffunction-sections -fdata-sections $(SPECS)

CFLAGS		+= -std=gnu17 $(WARNFLAGS_C) $(INCLUDEFLAGS) $(DEFINES) \
		   $(ARCH) -Os -ffunction-sections -fdata-sections $(SPECS)

CXXFLAGS	+= -std=gnu++17 $(WARNFLAGS_CPP) $(INCLUDEFLAGS) $(DEFINES) \
		   $(ARCH) -Os -ffunction-sections -fdata-sections $(SPECS)

ifeq ($(PLAT),NDS)
CXXFLAGS	+= -fno-rtti
endif

LDFLAGS		+= $(ARCH) $(LIBDIRSFLAGS)  $(DEFINES) \
		   -Wl,--start-group $(LIBS) -Wl,--end-group $(SPECS)

ifeq ($(PLAT),NDS)
LDFLAGS		+= -Wl,-Map,$(MAP)
endif

# Intermediate build files
# ------------------------

OBJS_ASSETS	:= $(addsuffix .o,$(addprefix $(BUILDDIR)/,$(SOURCES_BIN))) \
		   $(addsuffix .o,$(addprefix $(BUILDDIR)/,$(SOURCES_PNG)))

HEADERS_ASSETS	:= $(patsubst %.bin,%_bin.h,$(addprefix $(BUILDDIR)/,$(SOURCES_BIN))) \
		   $(patsubst %.png,%.h,$(addprefix $(BUILDDIR)/,$(SOURCES_PNG)))

ifneq ($(SOURCES_AUDIO),)
    ifeq ($(strip $(NITROFSDIR)),)
        OBJS_ASSETS		+= $(SOUNDBANKDIR)/soundbank.c.o
    endif
    HEADERS_ASSETS	+= $(SOUNDBANKINFODIR)/soundbank.h
endif

OBJS_SOURCES	:= $(addsuffix .o,$(addprefix $(BUILDDIR)/,$(SOURCES_S))) \
		   $(addsuffix .o,$(addprefix $(BUILDDIR)/,$(SOURCES_C))) \
		   $(addsuffix .o,$(addprefix $(BUILDDIR)/,$(SOURCES_CPP)))

OBJS		:= $(OBJS_ASSETS) $(OBJS_SOURCES)

DEPS		:= $(OBJS:.o=.d)

# Targets
# -------

.PHONY: all clean dump dldipatch sdimage

ifeq ($(PLAT),SDL)

all: $(ELF)

else ifeq ($(PLAT),NDS)

all: $(ROM)

ifneq ($(strip $(NITROFSDIR)),)
# Additional arguments for ndstool
NDSTOOL_ARGS	:= -d $(NITROFSDIR)

ifneq ($(SOURCES_AUDIO),)
    NDSTOOL_ARGS	+= -d $(SOUNDBANKDIR)
endif

# Make the NDS ROM depend on the filesystem only if it is needed
$(ROM): $(NITROFSDIR)
endif

# Combine the title strings
ifeq ($(strip $(GAME_SUBTITLE)),)
    GAME_FULL_TITLE := $(GAME_TITLE);$(GAME_AUTHOR)
else
    GAME_FULL_TITLE := $(GAME_TITLE);$(GAME_SUBTITLE);$(GAME_AUTHOR)
endif

$(ROM): $(ELF)
	@echo "  NDSTOOL $@"
	$(V)$(BLOCKSDS)/tools/ndstool/ndstool -c $@ \
		-7 $(ARM7ELF) -9 $(ELF) \
		-b $(GAME_ICON) "$(GAME_FULL_TITLE)" \
		$(NDSTOOL_ARGS)

$(DUMP): $(ELF)
	@echo "  OBJDUMP   $@"
	$(V)$(OBJDUMP) -h -C -S $< > $@

dump: $(DUMP)

dldipatch: $(ROM)
	@echo "  DLDIPATCH $(ROM)"
	$(V)$(BLOCKSDS)/tools/dldipatch/dldipatch patch \
		$(BLOCKSDS)/sys/dldi_r4/r4tf.dldi $(ROM)

endif # PLAT

$(ELF): $(OBJS)
	@echo "  LD      $@"
	$(V)$(LD) -o $@ $(OBJS) $(LDFLAGS)

clean:
	@echo "  CLEAN"
	$(V)$(RM) $(ROM) build $(SDIMAGE)

sdimage:
	@echo "  MKFATIMG $(SDIMAGE) $(SDROOT)"
	$(V)$(BLOCKSDS)/tools/mkfatimg/mkfatimg -t $(SDROOT) $(SDIMAGE)

# Rules
# -----

$(BUILDDIR)/%.s.o : %.s
	@echo "  AS      $<"
	@$(MKDIR) -p $(@D)
	$(V)$(CC) $(ASFLAGS) -MMD -MP -c -o $@ $<

$(BUILDDIR)/%.c.o : %.c
	@echo "  CC      $<"
	@$(MKDIR) -p $(@D)
	$(V)$(CC) $(CFLAGS) -MMD -MP -c -o $@ $<

$(BUILDDIR)/%.cpp.o : %.cpp
	@echo "  CXX     $<"
	@$(MKDIR) -p $(@D)
	$(V)$(CXX) $(CXXFLAGS) -MMD -MP -c -o $@ $<

ifeq ($(PLAT),NDS)

$(BUILDDIR)/%.arm.c.o : %.arm.c
	@echo "  CC      $<"
	@$(MKDIR) -p $(@D)
	$(V)$(CC) $(CFLAGS) -MMD -MP -marm -mlong-calls -c -o $@ $<

$(BUILDDIR)/%.arm.cpp.o : %.arm.cpp
	@echo "  CXX     $<"
	@$(MKDIR) -p $(@D)
	$(V)$(CXX) $(CXXFLAGS) -MMD -MP -marm -mlong-calls -c -o $@ $<

endif

$(BUILDDIR)/%.bin.o $(BUILDDIR)/%_bin.h : %.bin
	@echo "  BIN2C   $<"
	@$(MKDIR) -p $(@D)
	$(V)$(BLOCKSDS)/tools/bin2c/bin2c $< $(@D)
	$(V)$(CC) $(CFLAGS) -MMD -MP -c -o $(BUILDDIR)/$*.bin.o $(BUILDDIR)/$*_bin.c

$(BUILDDIR)/%.png.o $(BUILDDIR)/%.h : %.png %.grit
	@echo "  GRIT    $<"
	@$(MKDIR) -p $(@D)
	$(V)$(BLOCKSDS)/tools/grit/grit $< -ftc -W1 -o$(BUILDDIR)/$*
	$(V)$(CC) $(CFLAGS) -MMD -MP -c -o $(BUILDDIR)/$*.png.o $(BUILDDIR)/$*.c
	$(V)touch $(BUILDDIR)/$*.png.o $(BUILDDIR)/$*.h

ifneq ($(SOURCES_AUDIO),)

$(SOUNDBANKINFODIR)/soundbank.h: $(SOURCES_AUDIO)
	@echo "  MMUTIL  $^"
	@$(MKDIR) -p $(SOUNDBANKDIR)
	@$(MKDIR) -p $(SOUNDBANKINFODIR)
	$(V)$(BLOCKSDS)/tools/mmutil/mmutil $^ -d \
		-o$(SOUNDBANKDIR)/soundbank.bin -h$(SOUNDBANKINFODIR)/soundbank.h

ifeq ($(strip $(NITROFSDIR)),)
$(SOUNDBANKDIR)/soundbank.c.o: $(SOUNDBANKINFODIR)/soundbank.h
	@echo "  BIN2C   soundbank.bin"
	$(V)$(BLOCKSDS)/tools/bin2c/bin2c $(SOUNDBANKDIR)/soundbank.bin \
		$(SOUNDBANKDIR)
	@echo "  CC.9    soundbank_bin.c"
	$(V)$(CC) $(CFLAGS) -MMD -MP -c -o $(SOUNDBANKDIR)/soundbank.c.o \
		$(SOUNDBANKDIR)/soundbank_bin.c
endif

endif

# All assets must be built before the source code
# -----------------------------------------------

$(SOURCES_S) $(SOURCES_C) $(SOURCES_CPP): $(HEADERS_ASSETS)

# Include dependency files if they exist
# --------------------------------------

-include $(DEPS)
