## Makefile for libparsec.

MAKE 								:= make
CXX									:= g++
CXXFLAGS							:= -Wall -Wextra -std=c++11
LD									:= g++
LDFLAGS 							:= -O

DEBUG 								:= 1
ifeq ($(DEBUG), 1)
	CXXFLAGS 						+= -O0 -ggdb -DDEBUG
else
	CXXFLAGS						+= -static -O3 -DNDEBUG
endif

## default target.
all: test

test:
	$(CXX) test.cpp -o test $(CXXFLAGS)
	./test.exe
.PHONY: test

clean:
	rm *.exe
.PHONY: clean

