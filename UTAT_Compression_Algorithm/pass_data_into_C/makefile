parse: main.c
	gcc -g main.c -o parse

clean:
	rm *.o

run: parse
	./parse "../images/data_10_145_220.txt" "outfile"

debug: parse
	gdb --args parse "../images/data_10_145_220.txt" "outfile"
