# SPDX-License-Identifier: CC0-1.0
#
# SPDX-FileContributor: Antonio Niño Díaz, 2025

.PHONY: all clean convertftf libfont-nds libfont-sdl

all: convertftf libfont-nds libfont-sdl

clean:
	make clean -C convertftf
	make clean -C libfont

convertftf:
	make -C convertftf -j`nproc`

libfont-nds:
	PLAT=NDS make -C libfont -j`nproc`

libfont-sdl:
	PLAT=SDL make -C libfont -j`nproc`
