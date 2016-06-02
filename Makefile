# Makefile for periodic threads example programs
# Chris Simmonds
# chris@2net.co.uk

CFLAGS = -Wall -g -pthread -lrt
PROGS = itimer timer timerfd

all: $(PROGS)

timerfd: timerfd.c
	$(CC) $(CFLAGS) -o $@ $^ -lrt

timer: timer.c
	$(CC) $(CFLAGS) -o $@ $^ -lrt

itimer: itimer.c
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -f *.o
	rm -f $(PROGS)

install:
	cp $(PROGS) $(TARGETDIR)/bin
