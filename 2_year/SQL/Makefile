COMPILER=g++
COMPILER2=gcc
FLAGS= -O2 -ftrapv -fsanitize=undefined,address -Werror -Wignored-qualifiers -Winit-self -Wswitch-default -Wfloat-equal -Wshadow -Wpointer-arith -Wtype-limits -Wempty-body -Wmissing-field-initializers -Wcast-qual -Wwrite-strings -lm -Wno-c++11-extensions
CFLAGS=-O2 -ftrapv -fsanitize=undefined -Wall -Werror -Wformat-security -Wignored-qualifiers -Winit-self -Wswitch-default -Wfloat-equal -Wshadow -Wpointer-arith -Wtype-limits -Wempty-body -Wlogical-op -Wstrict-prototypes -Wold-style-declaration -Wold-style-definition -Wmissing-parameter-type -Wmissing-field-initializers -Wnested-externs -Wno-pointer-sign -std=gnu11 -lm

all: main

server:
	$(COMPILER2) $(CFLAGS) -c tables.h tables.c
	$(COMPILER) $(FLAGS) -c Scanner.cpp
	$(COMPILER) $(FLAGS) -c Table.cpp
	$(COMPILER) $(FLAGS) -c Poliz.cpp
	$(COMPILER) $(FLAGS) -c Parser.cpp
	$(COMPILER) $(FLAGS) -c Server.cpp
	$(COMPILER) $(FLAGS) tables.o Table.o Scanner.o Poliz.o Parser.o Server.cpp -o server.out

client:
	$(COMPILER2) $(CFLAGS) -c tables.h tables.c
	$(COMPILER) $(FLAGS) -c Client.cpp
	$(COMPILER) $(FLAGS) -c Table.cpp
	$(COMPILER) $(FLAGS) tables.o Client.cpp -o client.out

clean:
	-rm *.o
	-rm *.gch
	-rm *.out

