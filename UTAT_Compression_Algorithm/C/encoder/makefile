CC = gcc	# compiler
LD = gcc	# linker
CFLAGS = -Wall -I/usr/local/include -g -c	# compiler flags
LDFLAGS = -o	# linker flags
# LIBS = -L/usr/local/lib -lgsl -lgslcblas -lm -Lbitfile
LIBS = -L/usr/local/lib -lgsl -lgslcblas -lm

all: main.out test.out

main.out: main.o encoder.o bitfile/bitfile.o
	$(LD) $^ $(LIBS) $(LDFLAGS) $@

test.out: test.o encoder.o bitfile/bitfile.o
	$(LD) $^ $(LIBS) $(LDFLAGS) $@

main.o: main.c main.h bitfile/bitfile.h bitfile/bitfile.c
	$(CC) $(CFLAGS) $< 

encoder.o: encoder.c encoder.h bitfile/bitfile.h bitfile/bitfile.c
	$(CC) $(CFLAGS) $< 

bitfile/bitfile.o: bitfile/bitfile.c bitfile/bitfile.h
	cd bitfile && $(MAKE) bitfile.o

clean:
	rm -f *.o
	rm -f *.out
	cd bitfile && $(MAKE) clean

run: main.out
	./$<

debug: main.out
	gdb $<

test: test.out
	./$<
