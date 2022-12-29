/**
 * Yong Da Li
 * Saturday, June 4, 2022
 * 
 * section 5.4.3.2.2 Length-Limited Golomb-Power-of-2 Codeword of CCSDS123 standard
 * encoder
 */

#ifndef ENCODER_H
#define ENCODER_H
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "bitfile.h"
#include "mymatrix.h"


// single encode and decode functions
uint32_t encode_sample_optimized(uint32_t sample, unsigned int k, unsigned int *num_bits_used);
uint32_t decode_sample(uint32_t code, unsigned int k);

// streaming encode and decode functions
void encode_mymatrix(mymatrix *mat, int k, char* filename);
uint32_t decode_sample_bitfile(bit_file_t *stream, unsigned int k);

// checks
void check_single_encode(void);
void check_single_decode(void);
void check_read_write_to_binary_file(void);
void check_multiple_decode(char *filename, int k, uint32_t *decoded_array, uint32_t decoded_size);

// helpers
void view_binary_file(char *filename);
void view_binary_file_using_bitfile(char *filename);

#endif // ENCODER_H