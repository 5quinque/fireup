CC=gcc
FLAGS=-Wall -Wextra -Wshadow -std=c99 -D_POSIX_C_SOURCE=199309L -g
LIBS=-lncurses
BIN=/usr/bin/
OBJS=./obj/boxes.o

all: ./src/fireup.c ${OBJS}
	${CC} ./src/fireup.c ${OBJS} ${FLAGS} ${LIBS} -o ./bin/fireup

./obj/boxes.o: ./src/boxes.c
	${CC} -c ./src/boxes.c ${FLAGS} -o ./obj/boxes.o

install:
	cp ./bin/fireup /usr/local/bin/fireup

uninstall:
	rm -f /usr/local/bin/fireup

clean:
	rm ./bin/*
