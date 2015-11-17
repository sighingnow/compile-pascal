## Makefile for libparsec.

MAKE 								:= make
CXX									:= clang++
CXXFLAGS							:= -Wall -Wextra -std=c++11
LD									:= clang++
LDFLAGS 							:= -O
INCLUDE								:= -I .

DEBUG 								:= 1
ifeq ($(DEBUG), 1)
	CXXFLAGS 						+= -O0 -ggdb -DDEBUG
else
	CXXFLAGS						+= -static -O3 -DNDEBUG
endif

## default target.
all: clean test

test: test_parsec.out
	./$<
.PHONY: test

%.out: %.o
	$(CXX) $^ -o $@ $(CXXFLAGS)

%.o: %.cpp
	$(CXX) -c $< $(CXXFLAGS) $(INCLUDE)

clean:
	rm -f *.o
	rm -f *.out
.PHONY: clean
