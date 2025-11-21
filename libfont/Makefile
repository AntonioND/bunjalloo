# SPDX-License-Identifier: CC0-1.0
#
# SPDX-FileContributor: Antonio Niño Díaz, 2025

export WONDERFUL_TOOLCHAIN	?= /opt/wonderful
ARM_NONE_EABI_PATH	?= $(WONDERFUL_TOOLCHAIN)/toolchain/gcc-arm-none-eabi/bin/

ifeq ($(PLAT),NDS)
    ARCHIVE  := lib/libfont_nds.a
    BUILDDIR := build/nds
else ifeq ($(PLAT),SDL)
    ARCHIVE	:= lib/libfont_sdl.a
    BUILDDIR := build/sdl
endif

ifeq ($(PLAT),SDL)
    PREFIX		:=
else ifeq ($(PLAT),NDS)
    PREFIX		:= $(ARM_NONE_EABI_PATH)arm-none-eabi-
endif

AR		:= $(PREFIX)ar
LD		:= $(PREFIX)ld
RM		:= rm -rf
MKDIR	:= mkdir

ifeq ($(VERBOSE),1)
V		:=
else
V		:= @
endif

.PHONY: all clean

all: $(ARCHIVE)

$(BUILDDIR)/sans.map.o: fonts/sans.map
	@$(MKDIR) -p $(@D)
	@echo "  LD      $<"
	$(V)cd fonts && $(LD) -r -z noexecstack -b binary -o ../$@ sans.map

$(BUILDDIR)/sans.set.o: fonts/sans.set
	@$(MKDIR) -p $(@D)
	@echo "  LD      $<"
	$(V)cd fonts && $(LD) -r -z noexecstack -b binary -o ../$@ sans.set

$(ARCHIVE): $(BUILDDIR)/sans.map.o $(BUILDDIR)/sans.set.o
	@$(MKDIR) -p $(@D)
	@echo "  AR      $@"
	$(V)$(AR) rcs $@ $^

clean:
	@echo "  CLEAN"
	$(V)$(RM) build lib
