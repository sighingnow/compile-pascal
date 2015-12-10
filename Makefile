## Makefile for libparsec.

MAKE 								:= make
CXX									:= clang++
CXXFLAGS							:= -Wall -Wextra -std=c++11
LDFLAGS 							:= -O
INCLUDE								:= -I .

LIBGTEST_DIR 						:= ./gtest

LDFLAGS								+=  -L $(LIBGTEST_DIR) -lgtest_main -lgtest
INCLUDE 							+= -I $(LIBGTEST_DIR)/include

DEBUG 								:= 1
ifeq ($(DEBUG), 1)
	CXXFLAGS 						+= -O0 -ggdb -DDEBUG
else
	CXXFLAGS						+= -static -O3 -DNDEBUG
endif

PATCH 								:= 0
ifeq ($(PATCH), 1)
	CXXFLAGS						+= -DENABLE_PATCH
endif

## default target.
all: clean googletest test

googletest: $(LIBGTEST_DIR)
	make --dir $(LIBGTEST_DIR) CXX=$(CXX)

test: test_parsec.out
	$(foreach case, $^, ./$(case))
.PHONY: test

%.out: %.o
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $^ -o $@

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDE) -c $<

clean:
	rm -f *.o
	rm -f *.out
.PHONY: clean
