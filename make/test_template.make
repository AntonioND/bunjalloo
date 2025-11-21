# SPDX-License-Identifier: CC0-1.0
#
# SPDX-FileContributor: Antonio Niño Díaz, 2023-2025

export BLOCKSDS			?= /opt/blocksds/core
export BLOCKSDSEXT		?= /opt/blocksds/external

# User config
# ===========

NAME		?= test

# Source code paths
# -----------------

SOURCEDIRS	?= .

INCLUDEDIRS += ../../bunjalloo/bunjalloo/include/ \
               ../../bunjalloo/bwt/include/ \
               ../../bunjalloo/ndspp/include/ \
               ../../libfont/include/ \
               ../../3rdparty/googletest-1.17.0/googletest/include/ \

GFXDIRS		:=
BINDIRS		:=

# Defines passed to all files
# ---------------------------

DEFINES		:=

# Libraries
# ---------

LIBS    := -lbunjalloo_sdl -lvera_sdl \
           -lgoogletest_sdl -lz_sdl -lplum_sdl -lmbedtls_sdl \
           -lSDL -lGL -lGLU -lm

LIBDIRS += ../../3rdparty \
           ../../bunjalloo \
           ../../libfont \

# Build artifacts
# ---------------

BUILDDIR	:= build
ELF		:= $(NAME).elf

# Tools
# -----

ifeq ($(PLAT),SDL)
    PREFIX		:=
else ifeq ($(PLAT),NDS)
    PREFIX		:= $(ARM_NONE_EABI_PATH)arm-none-eabi-
else
    $(error "Define a platform with PLAT=SDL or PLAT=NDS")
endif

CC		:= $(PREFIX)gcc
CXX		:= $(PREFIX)g++
LD		:= $(PREFIX)gcc
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
CXXFLAGS	+= -fno-exceptions -fno-rtti
endif

LDFLAGS		+= $(ARCH) $(LIBDIRSFLAGS) $(DEFINES) \
		   -Wl,--start-group $(LIBS) -Wl,--end-group $(SPECS)

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

all: $(ELF)

$(ELF): $(OBJS)
	@echo "  LD      $@"
	$(V)$(LD) -o $@ $(OBJS) $(LDFLAGS)

clean:
	@echo "  CLEAN"
	$(V)$(RM) $(ELF) build

# Only define the "run" target if PLAT=SDL so that "make run" exits with an
# error in all other cases. For PLAT=SDL, only make the target do something if
# AUTO=1.
ifeq ($(PLAT),SDL)

.PHONY: run
ifeq ($(AUTO),1)
run: $(ELF)
	./$(ELF)
else
run:
endif

endif

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

# All assets must be built before the source code
# -----------------------------------------------

$(SOURCES_S) $(SOURCES_C) $(SOURCES_CPP): $(HEADERS_ASSETS)

# Include dependency files if they exist
# --------------------------------------

-include $(DEPS)
