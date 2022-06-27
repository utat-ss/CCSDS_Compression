# need to link math library `-lm`
main.out: main.o encoder.o
	gcc -L/usr/local/lib main.o encoder.o -lgsl -lgslcblas -lm -o main.out

# debug symbols turned on `-g`, has to be before `-c` compile
main.o: main.c 
	gcc -Wall -I/usr/local/include -g -c main.c -o main.o

encoder.o: encoder.c encoder.h
	gcc -Wall -I/usr/local/include -g -c encoder.c -o encoder.o

test.o: test.c
	gcc -Wall -I/usr/local/include -g -c test.c -o test.o

test.out: test.o encoder.o
	gcc -L/usr/local/lib test.o encoder.o -lgsl -lgslcblas -lm -o test.out

clean:
	rm *.o
	rm *.out

run: main.out
	./main.out

debug: main.out
	gdb main.out

test: test.out
	./test.out