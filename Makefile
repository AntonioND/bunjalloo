# SPDX-License-Identifier: CC0-1.0
#
# SPDX-FileContributor: Antonio Niño Díaz, 2025

.PHONY: all nds sdl

all: nds sdl

.PHONY: 3rdparty-nds 3rdparty-sdl bunjalloo-nds bunjalloo-sdl

nds: bunjalloo-nds

sdl: bunjalloo-sdl

3rdparty-nds:
	make -C 3rdparty -j`nproc` nds

3rdparty-sdl:
	make -C 3rdparty -j`nproc` sdl

bunjalloo-nds: 3rdparty-nds
	PLAT=NDS make -C bunjalloo -j`nproc`

bunjalloo-sdl: 3rdparty-sdl
	PLAT=SDL make -C bunjalloo -j`nproc`

.PHONY: libvera-nds libvera-sdl

libvera-nds:
	PLAT=NDS make -C libvera -j`nproc`

libvera-sdl:
	PLAT=SDL make -C libvera -j`nproc`

.PHONY: clean tests tools

clean:
	PLAT=NDS make -C 3rdparty -j`nproc` clean
	PLAT=NDS make -C bunjalloo -j`nproc` clean
	PLAT=NDS make -C tests -j`nproc` clean
	make -C tools -j`nproc` clean

tests: bunjalloo-sdl bunjalloo-nds libvera-sdl libvera-nds
	make -C tests -j`nproc`

tools:
	make -C tools -j`nproc`

run-tests: tests
	PLAT=SDL make -C tests run
