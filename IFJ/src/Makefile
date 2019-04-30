# /******************** Makefile ********************\
# |***************************************************|
# |team: 053/1                                        |
# |author: Martin Gaňo, xganom00                      |
# |FIT VUT                                            |
# |***************************************************|
# |***************************************************/

CC=gcc
CFLAGS= -std=c11 -Wall -Wextra -pedantic -O3

HODOLO = parser.o scanner.o  my_string.o semantic.o symtable.o stack.o expres.o

all: build

build: $(HODOLO) err_codes.h my_token.h
	$(CC) $(CFLAGS) $(HODOLO) -o ifj18

parser.o: parser.c scanner.o my_string.o semantic.o expres.o
	$(CC) $(CFLAGS) -c parser.c

scanner.o: scanner.c my_string.o
	$(CC) $(CFLAGS) -c scanner.c

my_string.o: my_string.c my_string.h
	$(CC) $(CFLAGS) -c my_string.c

semantic.o:  semantic.c symtable.h
	$(CC) $(CFLAGS) -c  semantic.c

symboltab.o:  symtable.c stack.h
	$(CC) $(CFLAGS) -c  symtable.c

stack.o:  stack.c
	$(CC) $(CFLAGS) -c stack.c

expres.o:  expres.c
	$(CC) $(CFLAGS) -c expres.c



clean:
	rm *.o ifj18

test: 
	./test_semantic
	./test_scanner
	./test_stack
	./test_symbol_tab	
	./test_generator
