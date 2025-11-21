#!/bin/bash
# SPDX-License-Identifier: CC0-1.0
#
# SPDX-FileContributor: Antonio Niño Díaz, 2025

FONTS=dejavu-fonts-ttf-2.37/ttf/
OUT=../nitrofs/fonts/

LIBFONT=../libfont/fonts/

../tools/convertftf/convertftf \
    --map=$OUT/sans.map --set=$OUT/sans.set \
    --size=11 \
    --offset_ascender=3 \
    --offset_descender=-1 \
    $FONTS/DejaVuSans.ttf

cp $OUT/sans.map $OUT/sans.set $LIBFONT

exit 0
