TARGET=main
CC=gcc
CFLAGS=-O2 -ftrapv -fsanitize=undefined -Wall -Werror -Wformat-security -Wignored-qualifiers -Winit-self -Wswitch-default -Wfloat-equal -Wshadow -Wpointer-arith -Wtype-limits -Wempty-body -Wlogical-op -Wstrict-prototypes -Wold-style-declaration -Wold-style-definition -Wmissing-parameter-type -Wmissing-field-initializers -Wnested-externs -Wno-pointer-sign -std=gnu11 -lm
OBJECTS=main.o polis.o stack.o funcs.o vars.o

all:$(TARGET)

clean:
	rm -f *.o $(TARGET) core
$(TARGET): $(OBJECTS)
	$(CC) -o $(TARGET) $(CFLAGS) $(OBJECTS)
.c.o:
	$(CC) -c $(CFLAGS) $<
#dependency from h files
polis.o: polis.h
stack.o: stack.h
funcs.o: funcs.h
vars.o: vars.h
