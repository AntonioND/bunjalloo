# SPDX-License-Identifier: CC0-1.0
#
# SPDX-FileContributor: Antonio Niño Díaz, 2025

.PHONY: all clean nds sdl
.PHONY: libplum-nds libplum-sdl mbedtls-nds
.PHONY: mbedtls-sdl zlib-nds zlib-sdl googletest-sdl

all: nds sdl

nds: libplum-nds mbedtls-nds zlib-nds

sdl: libplum-sdl mbedtls-sdl zlib-sdl googletest-sdl

libplum-nds:
	PLAT=NDS make -f libplum.make -j`nproc`

libplum-sdl:
	PLAT=SDL make -f libplum.make -j`nproc`

mbedtls-nds:
	PLAT=NDS make -f mbedtls.make -j`nproc`

mbedtls-sdl:
	PLAT=SDL make -f mbedtls.make -j`nproc`

zlib-nds:
	PLAT=NDS make -f zlib.make -j`nproc`

zlib-sdl:
	PLAT=SDL make -f zlib.make -j`nproc`

googletest-sdl:
	make -f googletest.make -j`nproc`

clean:
	rm -rf lib build
