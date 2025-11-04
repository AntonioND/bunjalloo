# SPDX-License-Identifier: CC0-1.0
#
# SPDX-FileContributor: Antonio Niño Díaz, 2025

ARCHIVE	:= lib/libvera_sdl.a

AR		:= ar
LD		:= ld
RM		:= rm -rf
MKDIR	:= mkdir

ifeq ($(VERBOSE),1)
V		:=
else
V		:= @
endif

.PHONY: all clean

all: $(ARCHIVE)

build/sans.map.o: fonts/sans.map
	@$(MKDIR) -p $(@D)
	@echo "  LD      $<"
	$(V)cd fonts && $(LD) -r -z noexecstack -b binary -o ../$@ sans.map

build/sans.set.o: fonts/sans.set
	@$(MKDIR) -p $(@D)
	@echo "  LD      $<"
	$(V)cd fonts && $(LD) -r -z noexecstack -b binary -o ../$@ sans.set

$(ARCHIVE): build/sans.map.o build/sans.set.o
	@$(MKDIR) -p $(@D)
	@echo "  AR      $@"
	$(V)$(AR) rcs $@ $^

clean:
	@echo "  CLEAN"
	$(V)$(RM) build lib
