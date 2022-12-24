CC = gcc	# compiler
LD = gcc	# linker
CFLAGS = -Wall -I/usr/local/include -g -c	# compiler flags
LDFLAGS = 	# linker flags
# LIBS = -L/usr/local/lib -lgsl -lgslcblas -lm -Lbitfile
LIBS = -L/usr/local/lib -lgsl -lgslcblas -lm

SOURCES :=	\
	main.c	\
	encoder.c	\
	bitfile/bitfile.c	\
	logger/logger.c		\

HEADERS := $(SOURCES:.c=.h)
OBJECTS := $(SOURCES:.c=.o)

$(info $$SOURCES is [${SOURCES}])
$(info $$HEADERS is [${HEADERS}])
$(info $$OBJECTS is [${OBJECTS}])

all: main.out

# link all objects together into single executable
main.out: $(OBJECTS)
	$(LD) $^ $(LDFLAGS) $(LIBS) -o $@

# for every item in objects list, it depends on the *.o -> *.c version of the code
$(OBJECTS): %.o:%.c
# produce a .d makefile syntax file that contains the header file dependencies
	$(CC) -MT $@ -MM $(CFLAGS) $< > $(<:%.c=%.d)
# for every .c file, compile a produce a .o file
	$(CC) $(CFLAGS) $< -o $@

# pull in dependency information for existing .o files
 -include $(OBJECTS:.o=.d)

clean:
	rm -f *.o
	rm -f *.d
	rm -f *.out
	cd bitfile && make clean
	cd logger && make clean

run: main.out
	./$<

debug: main.out
	gdb $<

test: test.out
	./$<
