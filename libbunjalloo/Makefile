# SPDX-License-Identifier: CC0-1.0
#
# SPDX-FileContributor: Antonio Niño Díaz, 2025

BLOCKSDS    ?= /opt/blocksds/core

NAME        := bunjalloo

SOURCEDIRS  := source
INCLUDEDIRS := include \
               ../3rdparty/mbedtls-3.6.4/include/ \
               ../3rdparty/zlib-1.3.1/ \
               ../libbwt/include/ \
               ../libndspp/include/ \

# Declare timegm() in <time.h>
DEFINES     += -D_GNU_SOURCE=1
# Declare strptime() in <time.h>
DEFINES     += -D_XOPEN_SOURCE=1

WARNFLAGS_CPP := -Wall -Wno-deprecated-declarations

include ../make/lib_template.make
