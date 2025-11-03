# SPDX-License-Identifier: CC0-1.0
#
# SPDX-FileContributor: Antonio Niño Díaz, 2023-2025

# Source code paths
# -----------------

INCLUDEDIRS := googletest-1.17.0/googletest \
               googletest-1.17.0/googletest/include \

SOURCES_CPP	:= googletest-1.17.0/googletest/src/gtest-all.cc \
               googletest-1.17.0/googletest/src/gtest_main.cc \

# Build artifacts
# ---------------

NAME		?= googletest

BUILDDIR	:= build/sdl/$(NAME)
ARCHIVE		:= lib/lib$(NAME)_sdl.a

# Tools
# -----

PREFIX		:=
CC		:= $(PREFIX)gcc
CXX		:= $(PREFIX)g++
AR		:= $(PREFIX)gcc-ar
MKDIR		:= mkdir
RM		:= rm -rf
CP		:= cp

# Verbose flag
# ------------

ifeq ($(VERBOSE),1)
V		:=
else
V		:= @
endif

# Compiler and linker flags
# -------------------------

WARNFLAGS_CPP	?= -Wall

INCLUDEFLAGS	:= $(foreach path,$(INCLUDEDIRS),-I$(path))

CXXFLAGS	+= -g -std=gnu++17 $(WARNFLAGS_CPP) $(DEFINES) $(INCLUDEFLAGS)

# Intermediate build files
# ------------------------

OBJS		:= $(addsuffix .o,$(addprefix $(BUILDDIR)/,$(SOURCES_CPP)))
DEPS		:= $(OBJS:.o=.d)

# Targets
# -------

.PHONY: all clean

all: $(ARCHIVE)

$(ARCHIVE): $(OBJS)
	@echo "  AR      $@"
	@$(MKDIR) -p $(@D)
	$(V)$(AR) rcs $@ $(OBJS)

clean:
	@echo "  CLEAN"
	$(V)$(RM) lib/ build/

# Rules
# -----

$(BUILDDIR)/%.cc.o : %.cc
	@echo "  CXX     $<"
	@$(MKDIR) -p $(@D)
	$(V)$(CXX) $(CXXFLAGS) -MMD -MP -c -o $@ $<

# Include dependency files if they exist
# --------------------------------------

-include $(DEPS)
