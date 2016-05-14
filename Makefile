LEXIN=small_lang.lex
LEXOUT=lex.yy.c

BISONIN=small_lang.y
BTABH=small_lang.tab.h
BTABC=small_lang.tab.c
BALLOUT=$(BTABH) $(BTABC)

ASTH=small_ast.h

EXEF=small_parser.exe

.PHONY: parser lexer bison clean clean-all

# High-level targets for making the parser, the lexer and the bison files

parser: $(EXEF)

lexer: $(LEXOUT)

bison: $(BALLOUT)

# "Low-level" targets for making the executable and other files

$(EXEF): $(LEXOUT) $(BALLOUT)
	g++ -g -o $(EXEF) $(BTABC) $(LEXOUT)

$(BALLOUT): $(BISONIN) $(ASTH)
	bison -d $(BISONIN)

$(LEXOUT): $(LEXIN) $(BTABH) $(ASTH)
	flex $(LEXIN)

clean:
	rm -f $(LEXOUT) $(BALLOUT)

clean-all:
	rm -f $(LEXOUT) $(BALLOUT) $(EXEF)
