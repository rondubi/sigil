CC = clang++

INCLUDE_FLAGS = -I .
CPPFLAGS = -c -Wall -Wpedantic -std=c++2a $(INCLUDE_FLAGS) 
LDFLAGS =

TEST_PROGS =
TEST_PROGS += tests/test-0-repeater.cc
TEST_PROGS += tests/test-1-torch.cc
TEST_PROGS += tests/test-2-comparison-comparator.cc
TEST_PROGS += tests/test-3-subtract-comparator.cc
TEST_PROGS += tests/test-4-clock.cc
TEST_PROGS += tests/test-5-and.cc
TEST_PROGS += tests/test-6-xor.cc
TEST_PROGS += tests/test-7-full-adder.cc
TEST_PROGS += tests/test-8-add-4-bits.cc

TEST_STEMS = $(patsubst tests/%.cc, %, $(TEST_PROGS))

BUILD_PROGS = $(patsubst %, build/%, $(TEST_STEMS))

make: $(BUILD_PROGS)

build/%: build/%.o build/sigil.o
	$(CC) $(LDFLAGS) $^ $(LDLIBS) -o $@

build/%.o: tests/%.cc 
	$(CC) $(CPPFLAGS) -o $@ $< 

build/sigil.o: sigil.cc
	$(CC) $(CPPFLAGS) -o $@ $< 

debug:
	@echo $(TEST_PROGS)
	@echo $(TEST_STEMS)
	@echo $(BUILD_PROGS)

