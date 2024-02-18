#------------------------------------------------------------------------------
# zx-81 decompile
# Copyright (C) Paulo Custodio, 2023-2024
# License: The Artistic License 2.0, http://www.perlfoundation.org/artistic_license_2_0
#------------------------------------------------------------------------------

DECOMPILE	= zx81_decompile
COMPILE		= zx81_compile

ifeq ($(OS),Windows_NT)
  EXESUFFIX 		:= .exe
else
  EXESUFFIX 		?=
endif

COMMON_FLAGS=  -ggdb -MMD -Wall -Wextra -Werror -pedantic-errors
CC 			?= gcc
CFLAGS		+= -std=gnu11 $(COMMON_FLAGS)
CXX			?= g++
CXX_FLAGS	+= -std=gnu++17 $(COMMON_FLAGS)
LDFLAGS		+=

C_SRCS		= $(wildcard *.c)
CXX_SRCS	= $(wildcard *.cpp)
ALL_OBJS	= $(C_SRCS:.c=.o) $(CXX_SRCS:.cpp=.o)
COMMON_OBJS	= $(filter-out $(DECOMPILE).o, $(filter-out $(COMPILE).o, $(ALL_OBJS)))
DEPENDS		= $(C_SRCS:.c=.d) $(CXX_SRCS:.cpp=.d)

#------------------------------------------------------------------------------

define MAKE_EXE
all:: $(1)$(EXESUFFIX)

$(1)$(EXESUFFIX): $(2)
	$(CXX) $(CXXFLAGS) $(2) $(LDFLAGS) -o $(1)$(EXESUFFIX)
	
clean::
	$(RM) $(1) $(1)$(EXESUFFIX) $(2)
endef

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

%.o: %.cpp
	$(CXX) $(CXX_FLAGS) -c -o $@ $<

#------------------------------------------------------------------------------

$(eval $(call MAKE_EXE,$(DECOMPILE),$(DECOMPILE).o $(COMMON_OBJS)))
$(eval $(call MAKE_EXE,$(COMPILE),$(COMPILE).o $(COMMON_OBJS)))

all::
	$(MAKE) -C FortressOfZorlac
	$(MAKE) -C GrimmsFairyTrails
	
clean::
	$(RM) $(ALL_OBJS) $(DEPENDS)
	$(MAKE) -C FortressOfZorlac clean
	$(MAKE) -C GrimmsFairyTrails clean

#------------------------------------------------------------------------------

-include $(DEPENDS)

#------------------------------------------------------------------------------

test: $(DECOMPILE)$(EXESUFFIX) $(COMPILE)$(EXESUFFIX) 
	perl -S prove t/*.t
	./$(COMPILE)$(EXESUFFIX) -o test_t2p.p t/test_t2p.b81
	perl make_p.pl t/test_keyboard.bas

clean::
	$(RM) *.p *.p.txt *.b81 *.bak t/*.p.txt t/*.bak
	$(RM) t/test_keyboard.asm t/test_keyboard.b81 t/test_keyboard.bin t/test_keyboard.map t/test_keyboard.p t/test_keyboard.sym
