# SPDX-License-Identifier: CC0-1.0
#
# SPDX-FileContributor: Antonio Niño Díaz, 2025

NAME        := mbedtls

SOURCEDIRS  := mbedtls-3.6.4

INCLUDEDIRS := mbedtls-3.6.4/include

DEFINES     := -Dunix

include ../make/lib_template.make
