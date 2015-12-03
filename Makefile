## Makefile for libparsec.

MAKE 								:= make

CXX									:= g++
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
GTEST_LDFLAGS						:=  -L $(LIBGTEST_DIR) -lgtest_main -lgtest

INCLUDE								:= -I .
INCLUDE 							+= -I $(LIBGTEST_DIR)/include
INCLUDE 							+= -I $(LIBPARSEC_DIR)/

DEBUG 								:= 0
ifeq ($(DEBUG), 1)
	CXXFLAGS 						+= -O0 -ggdb -DDEBUG -DTRACE
else
	CXXFLAGS						+= -O1 -DNDEBUG
endif

UTILS								:= pl0_parser.o pl0_ast.o pl0_tac_gen.o pl0_x86.o pl0_regalloc.o

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

pl0c.out: $(UTILS) pl0c.o
	@$(CXX) $(CXXFLAGS) $(LDFLAGS) $^ -o $@

%.out: $(UTILS) %.o
	@$(CXX) $(CXXFLAGS) $(LDFLAGS) $(GTEST_LDFLAGS) $^ -o $@

%.o: %.cpp
	@$(CXX) $(CXXFLAGS) $(INCLUDE) -c $<

clean:
	@rm -f *.o *.out
	@rm -f *.obj *.exec
.PHONY: clean

%.asm: %.pas
	@pl0c.out $< > $@

%.obj: %.asm
	@nasm $< -f win32 -o $@

%.exec: pl0_cases/%.obj
	@gcc $< -o $@
	@./$@
	@rm -f $@

allpas		:= test_for.exec \
			test_recursive.exec \
			test_fib1.exec \
			test_fib2.exec \
			test_mul_div.exec \
			test_ref1.exec \
			test_ref2.exec \
			test_array1.exec \
			test_case1.exec \
			test_swap1.exec

test-pas: $(allpas)

.SECONDARY: a.asm

compile:
	pl0c.out pl0_cases/test_ref2.pas > a.asm
	@make asm

asm: a.asm
	nasm -f win32 a.asm -o a.obj
	gcc a.obj -o a.exec
	./a.exec
