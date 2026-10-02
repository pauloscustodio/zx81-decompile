#------------------------------------------------------------------------------
# zx-81 decompile
# Copyright (C) Paulo Custodio, 2023-2026
# License: The Artistic License 2.0, http://www.perlfoundation.org/artistic_license_2_0
#------------------------------------------------------------------------------

DECOMPILE	= zx81_decompile
COMPILE		= zx81_compile

ifeq ($(OS),Windows_NT)
  EXESUFFIX 	:= .exe
else
  EXESUFFIX 	?=
endif

# compile options
CXX = g++
CXXFLAGS_BASE = -std=gnu++17 -MMD -Wall -Wextra -Wpedantic -Werror -pedantic-errors -Icommon
CXXFLAGS_DEBUG = $(CXXFLAGS_BASE) -D_DEBUG -O0 -g
CXXFLAGS_RELEASE = $(CXXFLAGS_BASE) -DNDEBUG -O3

INSTALL ?= install

# re2c: generate C/C++ sources from .re files
RE2C ?= re2c
RE2CFLAGS ?= -W --no-debug-info --no-generation-date --tags

# astyle
STYLE_WIDTH = 80
ASTYLE	= astyle --style=attach --pad-oper --align-pointer=type \
		  --break-closing-braces --add-braces --attach-return-type \
		  --max-code-length=$(STYLE_WIDTH) --lineend=linux --formatted 
PERLTIDY = perltidy --indent-columns=4 --continuation-indentation=4 \
		  --maximum-line-length=$(STYLE_WIDTH) -b
TIDY_STAMP = .tidy-stamp

# target files
DEBUG_DIR   = build/Debug
RELEASE_DIR = build/Release

COMMON_SRCS		:= $(wildcard common/*.cpp)
COMPILE_SRCS 	:= $(wildcard compile/*.cpp)
DECOMPILE_SRCS 	:= $(wildcard decompile/*.cpp)
DEFS 			:= $(wildcard *.def common/*.def compile/*.def decompile/*.def)

COMPILE_DEBUG_OBJS		:=  $(patsubst %.cpp,$(DEBUG_DIR)/%.o,$(COMPILE_SRCS) $(COMMON_SRCS))
COMPILE_RELEASE_OBJS	:=  $(patsubst %.cpp,$(RELEASE_DIR)/%.o,$(COMPILE_SRCS) $(COMMON_SRCS))

DECOMPILE_DEBUG_OBJS	:=  $(patsubst %.cpp,$(DEBUG_DIR)/%.o,$(DECOMPILE_SRCS) $(COMMON_SRCS))
DECOMPILE_RELEASE_OBJS	:=  $(patsubst %.cpp,$(RELEASE_DIR)/%.o,$(DECOMPILE_SRCS) $(COMMON_SRCS))

DEPENDS	:= $(patsubst %.o,%.d,$(COMPILE_DEBUG_OBJS) $(COMPILE_RELEASE_OBJS) $(DECOMPILE_DEBUG_OBJS) $(DECOMPILE_RELEASE_OBJS))

COMPILE_DEBUG_EXE   := $(DEBUG_DIR)/$(COMPILE)$(EXESUFFIX)
COMPILE_RELEASE_EXE := $(RELEASE_DIR)/$(COMPILE)$(EXESUFFIX)

DECOMPILE_DEBUG_EXE   := $(DEBUG_DIR)/$(DECOMPILE)$(EXESUFFIX)
DECOMPILE_RELEASE_EXE := $(RELEASE_DIR)/$(DECOMPILE)$(EXESUFFIX)

# rules
all: $(COMPILE_DEBUG_EXE)
$(COMPILE_DEBUG_EXE): $(COMPILE_DEBUG_OBJS) $(DEFS)
	$(CXX) $(CXXFLAGS_DEBUG) $(COMPILE_DEBUG_OBJS) -o $@

all: $(COMPILE_RELEASE_EXE)
$(COMPILE_RELEASE_EXE): $(COMPILE_RELEASE_OBJS) $(DEFS)
	$(CXX) $(CXXFLAGS_RELEASE) $(COMPILE_RELEASE_OBJS) -o $@
	strip $@

all: $(DECOMPILE_DEBUG_EXE)
$(DECOMPILE_DEBUG_EXE): $(DECOMPILE_DEBUG_OBJS) $(DEFS)
	$(CXX) $(CXXFLAGS_DEBUG) $(DECOMPILE_DEBUG_OBJS) -o $@

all: $(DECOMPILE_RELEASE_EXE)
$(DECOMPILE_RELEASE_EXE): $(DECOMPILE_RELEASE_OBJS) $(DEFS)
	$(CXX) $(CXXFLAGS_RELEASE) $(DECOMPILE_RELEASE_OBJS) -o $@
	strip $@

$(DEBUG_DIR)/%.o: %.cpp | $(DEBUG_DIR)/compile $(DEBUG_DIR)/decompile $(DEBUG_DIR)/common
	$(CXX) $(CXXFLAGS_DEBUG) -c $< -o $@

$(RELEASE_DIR)/%.o: %.cpp | $(RELEASE_DIR)/compile $(RELEASE_DIR)/decompile $(RELEASE_DIR)/common
	$(CXX) $(CXXFLAGS_RELEASE) -c $< -o $@

$(DEBUG_DIR)/compile $(DEBUG_DIR)/decompile $(DEBUG_DIR)/common $(RELEASE_DIR)/compile $(RELEASE_DIR)/decompile $(RELEASE_DIR)/common:
	mkdir -p $@

## Rule to produce .cpp from .re using re2c
#scan.cpp: scan.re Makefile
#	$(RE2C) $(RE2CFLAGS) $< -o $@
#	dos2unix $@

# test
test: $(COMPILE_DEBUG_EXE) $(COMPILE_RELEASE_EXE) \
	  $(DECOMPILE_DEBUG_EXE) $(DECOMPILE_RELEASE_EXE)
	perl -S prove -j9 --state=slow,save t/*.t

tidy:
	@[ -f $(TIDY_STAMP) ] || touch -t 197001010000 $(TIDY_STAMP)
	find . \( -name '*.c' -o -name '*.cpp' -o -name '*.h' -o -name '*.re' \) \
		-newer $(TIDY_STAMP) -exec $(ASTYLE) {} +
	find . \( -name '*.pl' -o -name '*.pm' -o -name '*.t' \) \
		-newer $(TIDY_STAMP) -exec $(PERLTIDY) {} +
	touch $(TIDY_STAMP)

clean:
	$(RM) $(TIDY_STAMP)
	$(RM) $(COMPILE_DEBUG_EXE) $(COMPILE_DEBUG_OBJS)
	$(RM) $(COMPILE_RELEASE_EXE) $(COMPILE_RELEASE_OBJS)
	$(RM) $(DECOMPILE_DEBUG_EXE) $(DECOMPILE_DEBUG_OBJS)
	$(RM) $(DECOMPILE_RELEASE_EXE) $(DECOMPILE_RELEASE_OBJS)
	$(RM) $(DEPENDS)
	find . -name '*.bak' -o -name '*.orig' -exec $(RM) {} +

install: $(RELEASE_EXE)
	$(INSTALL) $(RELEASE_EXE) $(PREFIX)/bin/$(PROJ)$(EXESUFFIX)

-include $(DEPENDS)

