SRC = $(wildcard *.c)
TARGETS = $(SRC:%.c=%)

CC = gcc
CFLAGS = -g -Wall -pthread
LDFLAGS = -pthread
LIBS = 

all: $(TARGETS)

%.o: %.c Makefile
	$(CC) $(CFLAGS) -c -o $@ $<

$(TARGETS): %: %.o
	$(CC) $(LDFLAGS) -o $@ $^ $(LIBS)

.PHONY: clean

clean:
	-rm $(TARGETS) $(SRC:%.c=%.o)

