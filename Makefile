CC = gcc
CFLAGS = -Wall -pedantic -Werror
# LDFLAGS =

.PHONY: clean
all: covertsigs.double covertsigs.single
covertsigs.double: covertsigs.c
	gcc -std=c99 -o covertsigs.double covertsigs.c -D_GNU_SOURCE

covertsigs.single: covertsigs.c
	gcc -std=c99 -o covertsigs.single covertsigs.c -DSINGLE -D_GNU_SOURCE

clean:
	$(RM) covertsigs.single *.o
	$(RM) covertsigs.double *.o

