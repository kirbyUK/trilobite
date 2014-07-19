CC=g++
FLAGS=-Wall -c
LIBS=-lncurses
DESTDIR=/
PREFIX=$(DESTDIR)/usr/local
BIN=trilobite
OBJ=trilobite.o diskItem.o file.o directory.o

all: $(BIN)

$(BIN): $(OBJ)
	$(CC) $(LIBS) $(OBJ) -o $(BIN)

trilobite.o: trilobite.cpp
	$(CC) $(FLAGS) trilobite.cpp 

diskItem.o: diskItem.h diskItem.cpp
	$(CC) $(FLAGS) diskItem.cpp

file.o: file.h file.cpp 
	$(CC) $(FLAGS) file.cpp

directory.o: directory.h directory.cpp
	$(CC) $(FLAGS) directory.cpp

deinstall: uninstall
uninstall:
	rm $(PREFIX)/bin/$(BIN)
	rm $(PREFIX)/share/man/man1/$(BIN).1

install:
	if [ ! -d $(PREFIX)/bin ]; then mkdir -p $(PREFIX)/bin; fi
	install -m 0755 $(BIN) $(PREFIX)/bin/
	if [ ! -d $(PREFIX)/share/man/man1 ]; then mkdir -p $(PREFIX)/share/man/man1; fi
	install -m 0644 $(BIN).1 $(PREFIX)/share/man/man1/

clean:
	rm -f $(OBJ) $(BIN)
