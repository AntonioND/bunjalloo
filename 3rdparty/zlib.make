# SPDX-License-Identifier: CC0-1.0
#
# SPDX-FileContributor: Antonio Niño Díaz, 2025

NAME        := z

SOURCEDIRS  := zlib-1.3.1/

INCLUDEDIRS := zlib-1.3.1/ \
               3rdparty/zlib-1.3.1/contrib/minizip/

DEFINES     := -DHAVE_UNISTD_H -Dunix

ifeq ($(PLAT),NDS)
    DEFINES     += -DMINIZIP_FOPEN_NO_64
endif

include ../make/lib_template.make
