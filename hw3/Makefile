CFLAGS = -Wall -Werror -Wpedantic
LFLAGS = 

OBJECTS = obj/main.o obj/alloc.o

build: init $(OBJECTS)
	$(CC) $(LFLAGS) $(OBJECTS) -o bin/hw

run: build
	echo
	bin/hw

init:
	mkdir -p bin
	mkdir -p obj

clean:
	-rm -f obj/*
	-rm -f bin/*

obj/main.o: src/main.c src/alloc.h
	$(CC) $(CFLAGS) -c src/main.c -o $@

obj/alloc.o: src/alloc.c src/alloc_internal.h src/alloc.h
	$(CC) $(CFLAGS) -c src/alloc.c -o $@
