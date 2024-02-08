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

CC 			?= gcc
CFLAGS		+= -std=gnu11 -ggdb -MMD -Wall -Wextra -Werror -pedantic-errors
CXX			?= g++
CXX_FLAGS	+= -std=gnu++17 -ggdb -MMD -Wall -Wextra -Werror -pedantic-errors 
LDFLAGS		+=

C_SRCS		= $(wildcard *.c)
CXX_SRCS	= $(wildcard *.cpp)
ALL_OBJS	= $(C_SRCS:.c=.o) $(CXX_SRCS:.cpp=.o)
COMMON_OBJS	= $(filter-out $(DECOMPILE).o, $(filter-out $(COMPILE).o, $(ALL_OBJS)))
DEPENDS		= $(C_SRCS:.c=.d) $(CXX_SRCS:.cpp=.d)

#------------------------------------------------------------------------------

define MAKE_EXE
all: $(1)$(EXESUFFIX)

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

clean::
	$(RM) $(ALL_OBJS) $(DEPENDS)

#------------------------------------------------------------------------------

-include $(DEPENDS)

#------------------------------------------------------------------------------

test: $(DECOMPILE)$(EXESUFFIX) $(COMPILE)$(EXESUFFIX)
	$(MAKE) PROG=test_vars        test_decompile_compile
	$(MAKE) PROG=show_float       test_decompile_compile
	$(MAKE) PROG=slow             test_decompile_compile
	$(MAKE) PROG=fast             test_decompile_compile
	$(MAKE) PROG=FortressOfZorlac test_decompile_compile
	$(MAKE) PROG=test_t2p         test_compile

test_decompile_compile:
	./$(DECOMPILE)$(EXESUFFIX) -o $(PROG).b81 t/$(PROG).p
	./$(COMPILE)$(EXESUFFIX)   -o $(PROG).p   $(PROG).b81
	hexdump -C $(PROG).p   > $(PROG).p.txt
	hexdump -C t/$(PROG).p > t/$(PROG).p.txt
	diff $(PROG).p.txt t/$(PROG).p.txt

test_compile:
	./$(COMPILE)$(EXESUFFIX)   -o $(PROG).p   t/$(PROG).b81
	hexdump -C $(PROG).p   > $(PROG).p.txt
	hexdump -C t/$(PROG).p > t/$(PROG).p.txt
	diff $(PROG).p.txt t/$(PROG).p.txt

clean::
	$(RM) *.p *.p.txt *.b81 t/*.p.txt
