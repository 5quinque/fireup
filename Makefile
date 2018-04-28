CC=gcc
FLAGS=-Wall -Wextra -Wshadow -std=c99 -D_POSIX_C_SOURCE=199309L -g
LIBS=-lncurses
BIN=/usr/bin/
OBJS=

all: ./src/fireup.c ${OBJS}
	${CC} ./src/fireup.c ${OBJS} ${FLAGS} ${LIBS} -o ./bin/fireup

install:
	cp ./bin/fireup /usr/local/bin/fireup

uninstall:
	rm -f /usr/local/bin/fireup

clean:
	rm ./bin/*
