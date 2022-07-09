parse: main.o parse.o
	gcc -L/usr/local/lib main.o parse.o -lgsl -lgslcblas -lm -o parse.out

main.o: main.c
	gcc -Wall -I/usr/local/include -g -c main.c -o main.o

parse.o: parse.c
	gcc -Wall -I/usr/local/include -g -c parse.c -o parse.o

clean:
	rm *.o
	rm *.out

run: parse.out
	./parse.out "../images/data_10_145_145.txt" "outfile"

debug: parse
	gdb --args parse.out "../images/data_10_145_145.txt" "outfile"