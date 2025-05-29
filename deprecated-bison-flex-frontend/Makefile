CC = clang++

FLEX_SRC := lex.yy.cc
FLEX_ARTIFACTS := $(FLEX_SRC)
BISON_SRC := sigil.tab.cc
BISON_ARTIFACTS := $(BISON_SRC) sigil.tab.hh

LDFLAGS := -L/opt/homebrew/opt/flex/lib -lfl
LDLIBS :=

DRIVER := main.cc

SOURCES := $(BISON_SRC) $(FLEX_SRC) $(DRIVER)

sigil_parser: $(FLEX_ARTIFACTS) $(BISON_ARTIFACTS)
	# $(CC) -o sigil_parser $(FLEX_ARTIFACTS) $(BISON_ARTIFACTS) $(DRIVER) $(LDFLAGS)
	$(CC) $(LDFLAGS) $(SOURCES) $(LDLIBS) -o $@

$(FLEX_ARTIFACTS): sigil.l
	flex -o lex.yy.cc -d sigil.l

$(BISON_ARTIFACTS): sigil.y
	bison -o sigil.tab.cc -d sigil.y

