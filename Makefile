## Makefile for libparsec.

MAKE 								:= make
CXX									:= clang++
CXXFLAGS							:= -Wall -Wextra -std=c++11
LDFLAGS 							:= -O
INCLUDE								:= -I .

LIBPARSEC_DIR						:= ./libparsec

INCLUDE 							+= -I $(LIBPARSEC_DIR)/

DEBUG 								:= 1
ifeq ($(DEBUG), 1)
	CXXFLAGS 						+= -O0 -ggdb -DDEBUG
else
	CXXFLAGS						+= -static -O3 -DNDEBUG
endif

UTILS								:= 

## default target.
all: clean test

test: test_pl0_parser.out
	$(foreach case, $^, ./$(case))
.PHONY: test

%.out: %.o $(UTILS)
	$(CXX) $^ -o $@ $(CXXFLAGS)

%.o: %.cpp
	$(CXX) -c $< $(CXXFLAGS) $(INCLUDE)

clean:
	rm -f *.o
	rm -f *.out
.PHONY: clean
