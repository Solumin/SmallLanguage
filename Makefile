SRCDIR=src
BUILDDIR=build

# FLEX source files
LEXPREFIX=flex/small_lang
LEXIN=$(SRCDIR)/$(LEXPREFIX).lex
LEXSRC=$(SRCDIR)/$(LEXPREFIX).flex.cpp
LEXOBJ=$(patsubst $(SRCDIR)/%.cpp,$(BUILDDIR)/%.o,$(LEXSRC))
FLEXOPTS=--outfile=$(LEXSRC)

# BISON source files
BISONPREFIX=bison/small_lang
BISONIN=$(SRCDIR)/$(BISONPREFIX).y
BISONSRC=$(SRCDIR)/$(BISONPREFIX).tab.cpp
BISONH=$(BISONSRC:.cpp=.hpp)
BISONOBJ=$(patsubst $(SRCDIR)/%.cpp,$(BUILDDIR)/%.o,$(BISONSRC))
BISONOPTS=--output=$(BISONSRC) --defines=$(BISONH)

# All the known source files -- used for general rules like making the .d files
ALLSRCS=$(shell find $(SRCDIR) -type f -name "*.cpp")
ALLOBJS=$(patsubst $(SRCDIR)/%.cpp,$(BUILDDIR)/%.o,$(ALLSRCS))
ALLPRES=$(ALLOBJS:.o=.d)

# Component-specific source files: AST, Evaluator and Parser
ASTSRCS=$(wildcard $(SRCDIR)/ast/*.cpp)
ASTH=$(wildcard $(SRCDIR)/ast/*.h)
ASTOBJS=$(patsubst $(SRCDIR)/%.cpp,$(BUILDDIR)/%.o,$(ASTSRCS))

EVALSRCS=$(wildcard $(SRCDIR)/evaluator/*.cpp)
EVALOBJS=$(patsubst $(SRCDIR)/%.cpp,$(BUILDDIR)/%.o,$(EVALSRCS))

PARSESRCS=$(wildcard $(SRCDIR)/parser/*.cpp)
PARSEOBJS=$(patsubst $(SRCDIR)/%.cpp,$(BUILDDIR)/%.o,$(PARSESRCS))

EVALEXE=small_eval.exe
PARSEEXE=small_parser.exe
# The -std flag is "gnu++14" by default. Making it an explicit dependency.
GPPOPTS=-g -I src -std=gnu++14

.PHONY: all evaluator parser bison flex clean clean-all

all: evaluator parser

evaluator: $(EVALEXE)

parser: $(PARSEEXE)

bison: $(BISONSRC)

flex: $(LEXSRC)

$(EVALEXE): $(EVALOBJS) $(ASTOBJS) $(BISONSRC) $(LEXSRC)
	g++ $(GPPOPTS) -o $(EVALEXE) $^

$(PARSEEXE): $(PARSEOBJS) $(ASTOBJS) $(BISONSRC) $(LEXSRC)
	g++ $(GPPOPTS) -o $(PARSEEXE) $^

$(ALLOBJS): $(BUILDDIR)/%.o : $(SRCDIR)/%.cpp | $(BUILDDIR)
	g++ $(GPPOPTS) -c -o $@ $<

# The output rule in the .d file will list both the .d and the .o files as
# targets. This ensures BOTH will be updated if either prereq changes.
$(ALLPRES): $(BUILDDIR)/%.d : $(SRCDIR)/%.cpp | $(BUILDDIR)
	g++ -MM -MF $@ -MT "$@ $(patsubst %.d,%.o,$@)" $(GPPOPTS) $<

$(BISONSRC) $(BISONH): $(BISONIN) $(ASTSRCS) $(ASTH) $(BUILDDIR)
	bison $(BISONOPTS) $(BISONIN)
	g++ -MM -MF $(BISONOBJ) -MT $(BISONOBJ:.o=.d) $(GPPOPTS) $(BISONSRC)

$(LEXSRC): $(LEXIN) $(BISONH) $(BUILDDIR)
	flex $(FLEXOPTS) $(LEXIN)
	g++ -MM -MF $(LEXOBJ) -MT $(LEXOBJ:.o=.d) $(GPPOPTS) $(LEXSRC)

# This is inlucded as an order-only prereq on the OBJS and PRES rules in order
# to make sure the build directory exists first!
$(BUILDDIR):
	mkdir -p $(patsubst $(SRCDIR)/%,$(BUILDDIR)/%,$(shell find $(SRCDIR) -type d))

# Include all the .d files (which indicate the prerequisites of each file)
-include $(ALLPRES)

clean:
	rm -rf $(BUILDDIR)/*

clean-all: clean
	rm -f $(PARSEEXE) $(EVALEXE)
	rm -f $(BISONSRC) $(BISONH)
	rm -f $(LEXSRC)
	rmdir $(BUILDDIR)
