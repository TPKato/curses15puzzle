# Makefile for 15puzzle

CC = gcc
CFLAGS = -g -O2
LDLIBS = -lcurses -ltermcap


15puzzle : 15puzzle.o 15puzzle.score
	$(CC) $(LDFLAGS) $< $(LDLIBS) -o $@

15puzzle.score :
	touch $@
	chmod 666 $@

clean :
	$(RM) *.o *.obj *~ *.bak
