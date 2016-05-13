LEXIN=small_lang.lex
LEXOUT=lex.yy.c

BISONIN=small_lang.y
BTABH=small_lang.tab.h
BTABC=small_lang.tab.c
BALLOUT=$(BTABH) $(BTABC)

ASTH=small_ast.h

EXEF=small_parser.exe

parser: $(EXEF) $(LEXOUT) $(BALLOUT) $(ASTH)
	g++ -g -o $(EXEF) $(BTABC) $(LEXOUT)

lexer: $(LEXOUT)

bison: $(BALLOUT)

$(BALLOUT): $(BISONIN)
	bison -d $(BISONIN)

$(LEXOUT): $(LEXIN) $(BTABH)
	flex $(LEXIN)

clean:
	rm -f $(LEXOUT) $(BALLOUT)

clean-all:
	rm -f $(LEXOUT) $(BALLOUT) $(EXEF)
