# SPDX-License-Identifier: CC0-1.0
#
# SPDX-FileContributor: Antonio Niño Díaz, 2023-2025

# Source code paths
# -----------------

SOURCEDIRS	:= .
INCLUDEDIRS	:= source

# Version string handling
# -----------------------

# Libraries
# ---------

LIBS		:= `pkg-config --libs freetype2`
LIBDIRS		:=

# Build artifacts
# ---------------

NAME		:= convertftf
BUILDDIR	:= build
ELF		:= $(NAME)

# Tools
# -----

HOSTCC		?= gcc
HOSTCXX		?= g++
HOSTLD	:= $(HOSTCXX)
CP		:= cp
MKDIR		:= mkdir
RM		:= rm -rf
MAKE		:= make

# Verbose flag
# ------------

ifeq ($(VERBOSE),1)
V		:=
else
V		:= @
endif

# Source files
# ------------

SOURCES_CPP	:= $(shell find -L $(SOURCEDIRS) -name "*.cc")

# Compiler and linker flags
# -------------------------

WARNFLAGS_CXX	:= -Wall -Wextra -Wpedantic -Wshadow

INCLUDEFLAGS	:= $(foreach path,$(INCLUDEDIRS),-I$(path)) \
		   $(foreach path,$(LIBDIRS),-I$(path)/include) \
		   `pkg-config --cflags freetype2`

LIBDIRSFLAGS	:= $(foreach path,$(LIBDIRS),-L$(path)/lib)

CXXFLAGS	+= -std=gnu++14 $(WARNFLAGS_CXX) $(DEFINES) $(INCLUDEFLAGS) -O3

LDFLAGS		+= $(LIBDIRSFLAGS) $(LIBS)

# Intermediate build files
# ------------------------

OBJS		:= $(addsuffix .o,$(addprefix $(BUILDDIR)/,$(SOURCES_CPP)))

DEPS		:= $(OBJS:.o=.d)

# Targets
# -------

.PHONY: all clean install

all: $(ELF)

$(ELF): $(OBJS)
	@echo "  HOSTLD  $@"
	$(V)$(HOSTLD) -o $@ $(OBJS) $(LDFLAGS)

clean:
	@echo "  CLEAN  "
	$(V)$(RM) $(ELF) $(BUILDDIR)

# Rules
# -----

$(BUILDDIR)/%.cc.o : %.cc
	@echo "  HOSTCXX $<"
	@$(MKDIR) -p $(@D)
	$(V)$(HOSTCXX) $(CXXFLAGS) -MMD -MP -c -o $@ $<

# Include dependency files if they exist
# --------------------------------------

-include $(DEPS)
