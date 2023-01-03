CC = gcc	# compiler
LD = gcc	# linker

# make sure to have `-g` debug flag at the end or else it won't have debugging symbols
CFLAGS = -I/usr/local/include -Iutil -I -Wall -g -c	# compiler flags
LDFLAGS = -g # linker flags
# LIBS = -L/usr/local/lib -lgsl -lgslcblas -lm -Lbitfile
LIBS = -lm

SOURCES :=	\
	main.c	\
	encoder/encoder.c	\
	util/bitfile.c		\
	util/logger.c		\
	util/mymatrix.c	\

HEADERS := $(SOURCES:.c=.h)
OBJECTS := $(SOURCES:.c=.o)
DEPENDENCIES := $(SOURCES:.c=.d)

$(info $$SOURCES is [${SOURCES}])
$(info $$HEADERS is [${HEADERS}])
$(info $$OBJECTS is [${OBJECTS}])
$(info $$DEPENDENCIES is [${DEPENDENCIES}])

all: main.out

# link all objects together into single executable
main.out: $(OBJECTS)
	$(LD) $(LDFLAGS) $^ $(LIBS) -o $@

# for every item in objects list, it depends on the *.o -> *.c version of the code
$(OBJECTS): %.o:%.c
# produce a .d makefile syntax file that contains the header file dependencies
	$(CC) -MT $@ -MM $(CFLAGS) $< > $(<:%.c=%.d)
# for every .c file, compile a produce a .o file
	$(CC) $(CFLAGS) $< -o $@

# pull in dependency information for existing .o files
 -include $(OBJECTS:.o=.d)

.PHONY: clean
clean:
	rm -f $(OBJECTS)
	rm -f $(DEPENDENCIES)
	rm *.out
