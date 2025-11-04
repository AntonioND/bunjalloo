# SPDX-License-Identifier: CC0-1.0
#
# SPDX-FileContributor: Antonio Niño Díaz, 2025

.PHONY: all nds sdl

all: nds sdl

.PHONY: 3rdparty-nds bunjalloo-nds libbunjalloo-nds libbwt-nds libndspp-sdl

nds: bunjalloo-nds

3rdparty-nds:
	make -C 3rdparty -j`nproc` nds

bunjalloo-nds: libbunjalloo-nds
	PLAT=NDS make -C bunjalloo -j`nproc`

libbunjalloo-nds: libbwt-nds libndspp-nds
	PLAT=NDS make -C libbunjalloo -j`nproc`

libbwt-nds: 3rdparty-nds libndspp-nds
	PLAT=NDS make -C libbwt -j`nproc`

libndspp-nds: 3rdparty-nds
	PLAT=NDS make -C libndspp -j`nproc`

.PHONY: 3rdparty-sdl bunjalloo-sdl libbunjalloo-sdl libbwt-sdl libndspp-sdl

sdl: bunjalloo-sdl

3rdparty-sdl:
	make -C 3rdparty -j`nproc` sdl

bunjalloo-sdl: libbunjalloo-sdl
	PLAT=SDL make -C bunjalloo -j`nproc`

libbunjalloo-sdl: libbwt-sdl libndspp-sdl
	PLAT=SDL make -C libbunjalloo -j`nproc`

libbwt-sdl: 3rdparty-sdl libndspp-sdl
	PLAT=SDL make -C libbwt -j`nproc`

libndspp-sdl: 3rdparty-sdl
	PLAT=SDL make -C libndspp -j`nproc`

.PHONY: clean tests tools

clean:
	PLAT=NDS make -C 3rdparty -j`nproc` clean
	PLAT=NDS make -C bunjalloo -j`nproc` clean
	PLAT=NDS make -C libbunjalloo -j`nproc` clean
	PLAT=NDS make -C libbwt -j`nproc` clean
	PLAT=NDS make -C libndspp -j`nproc` clean
	make -C tests -j`nproc` clean
	make -C tools -j`nproc` clean

tests: libbunjalloo-sdl
	make -C tests -j`nproc`

tools:
	make -C tools -j`nproc`
