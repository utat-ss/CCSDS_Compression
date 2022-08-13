compression.out: main.o parse.o predict.o helperlib.o unitTests.o
	gcc -L/usr/local/lib main.o parse.o predict.o helperlib.o -lgsl -lgslcblas -lm -o compression.out

main.o: main.c
	gcc -Wall -I/usr/local/include -g -c main.c -o main.o

parse.o: parser/parse.c parser/parse.h
	gcc -Wall -I/usr/local/include -g -c parser/parse.c -o parse.o

predict.o: predictor/predict.c predictor/predict.h
	gcc -Wall -I/usr/local/include -g -c predictor/predict.c -o predict.o

helperlib.o: predictor/helperlib.c predictor/helperlib.h
	gcc -Wall -I/usr/local/include -g -c predictor/helperlib.c -o helperlib.o

unitTests.o: unitTests/unitTests.c unitTests/unitTests.h
	gcc -Wall -I/usr/local/include -g -c unitTests/unitTests.c -o unitTests.o

clean:
	rm *.o
	rm *.out

run: compression.out
	./compression.out "../images/data_10_145_145.txt"

debug: compression.out
	gdb --args compression.out "../images/data_10_145_145.txt"