# Makefile for 15puzzle

CC = gcc
CFLAGS = -g -O2
LDLIBS = -lcurses -ltermcap


15puzzle : 15puzzle.o
	$(CC) $(LDFLAGS) $< $(LDLIBS) -o $@

clean :
	$(RM) *.o *.obj *~ *.bak
