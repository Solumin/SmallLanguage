SRCDIR=src
BUILDDIR=build

# FLEX source files
LEXDIR=$(SRCDIR)/flex
LEXIN=$(LEXDIR)/small_lang.lex
LEXOUT=$(BUILDDIR)/small_lex.yy.c
FLEXOPTS=--outfile=$(LEXOUT)

# BISON source files
BISONDIR=$(SRCDIR)/bison
# Force bison to output to the build dir by prefixing the files with
# "build/small_lang", will out "build/small_lang.tab.c" etc.
BISONPREFIX=small_lang
BISONOUTPRE=$(BUILDDIR)/$(BISONPREFIX)
BISONIN=$(BISONDIR)/$(BISONPREFIX).y
BTABH=$(BISONOUTPRE).tab.h
BTABC=$(BISONOUTPRE).tab.c
BALLOUT=$(BTABH) $(BTABC)
BISONOPTS=--file-prefix=$(BISONOUTPRE) -d

# AST source files
ASTC=$(SRCDIR)/ast/*.cpp
ASTH=$(SRCDIR)/ast/*.hpp

# Visitor header
VISITH=$(SRCDIR)/visitor/*.hpp

# Misc headers
MISC=$(SRCDIR)/*.h $(SRCDIR)/*.hpp

# Evaluator source
EVALC=$(SRCDIR)/evaluator/*.cpp
EVALH=$(SRCDIR)/evaluator/*.hpp
EVALEXE=small_eval.exe

# Parser source
PARSEC=$(SRCDIR)/parser/*.cpp
PARSEH=$(SRCDIR)/parser/*.hpp
PARSEEXE=small_parser.exe

# Other options
GPPOPTS=-g -I src

.PHONY: parser lexer bison clean clean-all

# High-level targets for making the parser, the lexer and the bison files

all: parser evaluator

evaluator: $(EVALEXE)

parser: $(PARSEEXE)

flex: $(LEXOUT)

bison: $(BALLOUT)

# "Low-level" targets for making the executable and other files

$(EVALEXE): flex bison $(EVALC) $(ASTC)
	g++ $(GPPOPTS) -o $(EVALEXE) $(BTABC) $(LEXOUT) $(ASTC) $(EVALC)

$(PARSEEXE): $(LEXOUT) $(BALLOUT) $(PARSEC) $(ASTC)
	g++ $(GPPOPTS) -o $(PARSEEXE) $(BTABC) $(LEXOUT) $(ASTC) $(PARSEC)

$(BALLOUT): $(BISONIN) $(BUILDDIR)
	bison $(BISONOPTS) $(BISONIN)

$(LEXOUT): $(LEXIN) $(BTABH) $(BUILDDIR)
	flex $(FLEXOPTS) $(LEXIN)

$(BUILDDIR):
	mkdir -p $(BUILDDIR)

clean:
	rm -f $(BUILDDIR)/*

clean-all: clean
	rm -f $(PARSEEXE) $(EVALEXE)
	rmdir $(BUILDDIR)
