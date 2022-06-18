parse: main.o
	gcc -L/usr/local/lib main.o -lgsl -lgslcblas -lm -o parse.out

main.o: main.c
	gcc -Wall -I/usr/local/include -g -c main.c -o main.o

clean:
	rm *.o
	rm *.out

run: parse.out
	./parse.out "../images/data_10_145_220.txt" "outfile"

debug: parse
	gdb --args parse.out "../images/data_10_145_220.txt" "outfile"
