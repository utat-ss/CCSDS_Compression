# need to link math library `-lm`
encoder.out: main.o encoder.o
	gcc main.o encoder.o -lm -o encoder.out

main.o: main.c 
	gcc -g -c main.c -o main.o

encoder.o: encoder.c encoder.h
	gcc -g -c encoder.c -o encoder.o

test.o: test.c
	gcc -g -c test.c -o test.o

test.out: test.o encoder.o
	gcc test.o encoder.o -lm -o test.out

clean:
	rm *.o
	rm *.out

run: encoder.out
	./encoder.out

debug: encoder.out
	gdb encoder.out

test: test.out
	./test.out