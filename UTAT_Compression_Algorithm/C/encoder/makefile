CC = gcc	# compiler
LD = gcc	# linker
CFLAGS = -Wall -I/usr/local/include -g -c	# compiler flags
LDFLAGS = -o	# linker flags
LIBS = -L/usr/local/lib -lgsl -lgslcblas -lm -Lbitfile

all: main.out test.out

main.out: main.o encoder.o bitfile/libbitfile.a
	$(LD) $^ $(LIBS) $(LDFLAGS) $@

test.out: test.o encoder.o bitfile/libbitfile.a
	$(LD) $^ $(LIBS) $(LDFLAGS) $@

main.o: main.c bitfile/bitfile.h
	$(CC) $(CFLAGS) $< 

encoder.o: encoder.c encoder.h bitfile/bitfile.h
	$(CC) $(CFLAGS) $< 

bitfile/libbitfile.a:
	cd bitfile && $(MAKE) libbitfile.a

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