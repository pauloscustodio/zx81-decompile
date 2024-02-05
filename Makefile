#------------------------------------------------------------------------------
# zx-81 decompile
# Copyright (C) Paulo Custodio, 2023-2024
# License: The Artistic License 2.0, http://www.perlfoundation.org/artistic_license_2_0
#------------------------------------------------------------------------------

PROJ1		= zx81_decompile
PROJ2		= zx81_compile

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
COMMON_OBJS	= $(filter-out $(PROJ1).o, $(filter-out $(PROJ2).o, $(ALL_OBJS)))
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

$(eval $(call MAKE_EXE,$(PROJ1),$(PROJ1).o $(COMMON_OBJS)))
$(eval $(call MAKE_EXE,$(PROJ2),$(PROJ2).o $(COMMON_OBJS)))

clean::
	$(RM) $(ALL_OBJS) $(DEPENDS)

#------------------------------------------------------------------------------

-include $(DEPENDS)

#------------------------------------------------------------------------------

test: $(PROJ1)$(EXESUFFIX) $(PROJ2)$(EXESUFFIX)
	$(MAKE) PROG=test_vars        runtest
	$(MAKE) PROG=show_float       runtest
	$(MAKE) PROG=slow             runtest
	$(MAKE) PROG=fast             runtest
	$(MAKE) PROG=FortressOfZorlac runtest

runtest:
	./zx81_decompile -o $(PROG).b81 t/$(PROG).p
	./zx81_compile -o $(PROG).p $(PROG).b81
	hexdump -C $(PROG).p > $(PROG).p.txt
	hexdump -C t/$(PROG).p > t/$(PROG).p.txt
	diff $(PROG).p.txt t/$(PROG).p.txt

clean::
	$(RM) *.p *.p.txt *.b81 t/*.p.txt
