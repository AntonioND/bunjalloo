#!/bin/bash

FONTS=dejavu-fonts-ttf-2.37/ttf/
OUT=../nitrofs/fonts/

../tools/convertftf/convertftf \
    --map=$OUT/sans.map --set=$OUT/sans.set \
    --size=11 \
    --offset_ascender=3 \
    --offset_descender=-1 \
    $FONTS/DejaVuSans.ttf

exit 0
