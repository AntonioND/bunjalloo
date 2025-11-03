# SPDX-License-Identifier: CC0-1.0
#
# SPDX-FileContributor: Antonio Niño Díaz, 2025

BLOCKSDS    ?= /opt/blocksds/core

NAME        := ndspp

GFXDIRS     := img

ifeq ($(PLAT),SDL)
    SOURCEDIRS  := source/pc source/common
    INCLUDEDIRS := include/ source/pc/

    WARNFLAGS_CPP := -Wall -Wno-deprecated-declarations
else ifeq ($(PLAT),NDS)
    SOURCEDIRS  := source/arm9 source/common
    INCLUDEDIRS := include/ source/arm9/
    LIBDIRS     := $(BLOCKSDS)/libs/libnds \
                   $(BLOCKSDS)/libs/dswifi
endif

LIBDIRS     += ../3rdparty/mbedtls-3.6.4 \
               ../3rdparty/libplum

include ../make/lib_template.make
