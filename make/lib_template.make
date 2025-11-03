# SPDX-License-Identifier: CC0-1.0
#
# SPDX-FileContributor: Antonio Niño Díaz, 2023-2025

export BLOCKSDS			?= /opt/blocksds/core
export BLOCKSDSEXT		?= /opt/blocksds/external

export WONDERFUL_TOOLCHAIN	?= /opt/wonderful
ARM_NONE_EABI_PATH	?= $(WONDERFUL_TOOLCHAIN)/toolchain/gcc-arm-none-eabi/bin/

# Source code paths
# -----------------

SOURCEDIRS	?= source
INCLUDEDIRS	?= include
GFXDIRS		?=
BINDIRS		?=

# Defines passed to all files
# ---------------------------

DEFINES		?=

# Libraries
# ---------

ifeq ($(PLAT),SDL)
    LIBDIRS		+=
else ifeq ($(PLAT),NDS)
    LIBDIRS		+= $(BLOCKSDS)/libs/libnds \
                   $(BLOCKSDS)/libs/dswifi
else
    $(error "Unsupported platform")
endif

# Build artifacts
# ---------------

NAME		?= test

ifeq ($(PLAT),SDL)
    BUILDDIR	:= build/sdl/$(NAME)
    ARCHIVE		:= lib/lib$(NAME)_sdl.a
else ifeq ($(PLAT),NDS)
    BUILDDIR	:= build/nds/$(NAME)
    ARCHIVE		:= lib/lib$(NAME)_nds.a
else
    $(error "Unsupported platform")
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
AR		:= $(PREFIX)gcc-ar
MKDIR		:= mkdir
RM		:= rm -rf
CP		:= cp

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

WARNFLAGS_C		?= -Wall
WARNFLAGS_CPP	?= -Wall

INCLUDEFLAGS	:= $(foreach path,$(INCLUDEDIRS),-I$(path)) \
		   $(foreach path,$(LIBDIRS),-I$(path)/include)

ASFLAGS		+= -g -x assembler-with-cpp $(DEFINES) $(INCLUDEFLAGS) \
		   $(ARCH) -ffunction-sections -fdata-sections $(SPECS)

CFLAGS		+= -g -std=gnu17 $(WARNFLAGS_C) $(DEFINES) $(INCLUDEFLAGS) \
		   $(ARCH) -Os -ffunction-sections -fdata-sections $(SPECS)

CXXFLAGS	+= -g -std=gnu++17 $(WARNFLAGS_CPP) $(DEFINES) $(INCLUDEFLAGS) \
		   $(ARCH) -Os -ffunction-sections -fdata-sections $(SPECS)

ifeq ($(PLAT),NDS)
    CXXFLAGS	+= -fno-exceptions -fno-rtti
endif

# Intermediate build files
# ------------------------

OBJS_ASSETS	:= $(addsuffix .o,$(addprefix $(BUILDDIR)/,$(SOURCES_BIN))) \
		   $(addsuffix .o,$(addprefix $(BUILDDIR)/,$(SOURCES_PNG)))

HEADERS_ASSETS	:= $(patsubst %.bin,%_bin.h,$(addprefix $(BUILDDIR)/,$(SOURCES_BIN))) \
		   $(patsubst %.png,%.h,$(addprefix $(BUILDDIR)/,$(SOURCES_PNG)))

OBJS_SOURCES	:= $(addsuffix .o,$(addprefix $(BUILDDIR)/,$(SOURCES_S))) \
		   $(addsuffix .o,$(addprefix $(BUILDDIR)/,$(SOURCES_C))) \
		   $(addsuffix .o,$(addprefix $(BUILDDIR)/,$(SOURCES_CPP)))

OBJS		:= $(OBJS_ASSETS) $(OBJS_SOURCES)

DEPS		:= $(OBJS:.o=.d)

# Targets
# -------

.PHONY: all clean

all: $(ARCHIVE)

$(ARCHIVE): $(OBJS)
	@echo "  AR      $@"
	@$(MKDIR) -p $(@D)
	$(V)$(AR) rcs $@ $(OBJS)

clean:
	@echo "  CLEAN"
	$(V)$(RM) lib/ build/

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

# All assets must be built before the source code
# -----------------------------------------------

$(SOURCES_S) $(SOURCES_C) $(SOURCES_CPP): $(HEADERS_ASSETS)

# Include dependency files if they exist
# --------------------------------------

-include $(DEPS)
