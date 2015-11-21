## Makefile for libparsec.

MAKE 								:= make

CXX									:= clang++
CXXFLAGS							:= -Wall -Wextra -std=c++11 $(CXX_FLAGS_$(ARCH))
LDFLAGS 							:= -O

## add -Wa, -mbig-obj option to mingw-w64.
SYS									:= $(shell $(CXX) -dumpmachine)
ifeq ($(CXX), g++)
ifneq (, $(findstring mingw, $(SYS)))
ifneq (, $(findstring x86_64, $(SYS)))
CXXFLAGS 							+= -Wa,-mbig-obj
endif
endif
endif

LIBGTEST_DIR 						:= ./gtest
LIBPARSEC_DIR						:= ./libparsec
LDFLAGS								+=  -L $(LIBGTEST_DIR) -lgtest_main -lgtest

INCLUDE								:= -I .
INCLUDE 							+= -I $(LIBGTEST_DIR)/include
INCLUDE 							+= -I $(LIBPARSEC_DIR)/

DEBUG 								:= 1
ifeq ($(DEBUG), 1)
	CXXFLAGS 						+= -O0 -ggdb -DDEBUG
else
	CXXFLAGS						+= -static -O3 -DNDEBUG
endif

UTILS								:= pl0_global.o pl0_parser.o

## prevent make deleting the intermedia object file.
.SECONDARY: $(UTILS)

## default target.
all: googletest test

googletest: $(LIBGTEST_DIR)
	make --dir $(LIBGTEST_DIR) CXX=$(CXX)

test: test_pl0_parser.out
	$(foreach case, $^, ./$(case))
.PHONY: test

dist: pl0c.out

%.out: $(UTILS) %.o
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $^ -o $@

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDE) -c $<

clean:
	rm -f *.o
	rm -f *.out
.PHONY: clean
