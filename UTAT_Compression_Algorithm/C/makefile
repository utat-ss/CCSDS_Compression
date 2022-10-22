compress.out: main.o compression.o helperlib.o unitTests.o
	gcc -L/usr/local/lib main.o compression.o helperlib.o unitTests.o -lgsl -lgslcblas -lm -o compress.out

# compile with debug symbols enabled (-g option)
main.o: main.c
	gcc -Wall -I/usr/local/include -g -c main.c -o main.o

helperlib.o: helperlib.c helperlib.h
	gcc -Wall -I/usr/local/include -g -c helperlib.c -o helperlib.o

compression.o: compression.c compression.h
	gcc -Wall -I/usr/local/include -g -c compression.c -o compression.o

unitTests.o: unitTests.c unitTests.h
	gcc -Wall -I/usr/local/include -g -c unitTests.c -o unitTests.o

clean:
	rm *.o

debug: compress.out
	gdb compress.out