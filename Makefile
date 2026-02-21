CC=gcc
CFLAGS=-O2 -Wall -Wextra
LDFLAGS=-lgpiod

all: pulse_test

pulse_test: pulse_test.c
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

clean:
	rm -f pulse_test
