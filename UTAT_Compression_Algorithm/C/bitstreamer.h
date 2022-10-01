#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>

//Runtime parameters
#define BUFFER_SIZE 24 //In bytes

//Debug macros
#define PRINT_BITSTREAM for(int i = size - 1; i >= 0; --i) printf("%016llx ", bitstream[i]); printf("\n");

//Global variables
uint8_t  used;
uint8_t  read;
uint8_t  padding;
uint16_t size;
uint64_t file_cursor;

//Bitstreamer Function Definitions
uint64_t* init_bitstream(void);

void init_bitstreamer_variables(void);

void write_to_disk(uint64_t* bitstream);

void read_from_disk(uint64_t* bitstream);

void write_to_bitstream(uint64_t value, uint64_t* bitstream, uint8_t width);

uint64_t read_from_bitstream(uint64_t* bitstream, uint8_t width);

void pad_bitstream(uint64_t* bitstream);

void depad_bitstream(uint64_t* bitstream);

uint16_t get_size(void);

void reset_bin_file(void);