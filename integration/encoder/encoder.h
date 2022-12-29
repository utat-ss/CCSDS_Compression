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
#include <gsl/gsl_matrix.h>

#include "bitfile.h"
#include "mymatrix.h"

#define BYTE_TO_BINARY_PATTERN "%c%c%c%c%c%c%c%c"
#define BYTE_TO_BINARY(byte)  \
  (byte & 0x80 ? '1' : '0'), \
  (byte & 0x40 ? '1' : '0'), \
  (byte & 0x20 ? '1' : '0'), \
  (byte & 0x10 ? '1' : '0'), \
  (byte & 0x08 ? '1' : '0'), \
  (byte & 0x04 ? '1' : '0'), \
  (byte & 0x02 ? '1' : '0'), \
  (byte & 0x01 ? '1' : '0')

void check_single_encode(void);
void check_single_decode(void);
void check_read_write_to_binary_file(void);
mymatrix* encode_rng_mymatrix(int nrows, int ncols, int min, int max, int k);
gsl_matrix_int* encode_rng_gsl_matrix(int nrows, int ncols, int range, int k);
void check_multiple_decode(char *filename, int k, uint32_t *decoded_array, uint32_t decoded_size);
void view_binary_file(char *filename);
void view_binary_file_using_bitfile(char *filename);

// Yong Da's naive implementation
unsigned int encode_sample(unsigned int sample, unsigned int k);

/**
 * way more optimized implementation of encoder
 * https://michaeldipperstein.github.io/rice.html
 * @param  sample        [thing to be compressed]
 * @param  k             [divisor = M = 2^k, to control size of divisor]
 * @param  num_bits_used [how many bits of 32 bit integer is actually used in the compressed sample]
 * @return               [compressed value, stored as 32-bit unsigned integer]
 */
uint32_t encode_sample_optimized(uint32_t sample, unsigned int k, unsigned int* num_bits_used);

/**
 * decoder assumes input is 32 bit unsigned integer
 * and it'll find the leading 1, to figure out when does the compressed sample actually start within the 32 bits
 * @param  code [encoded sample]
 * @param  k    [control parameter for size of divisor, must know the k value used for encoding]
 * @return      [decoded 32-bit unsigned sample]
 */
uint32_t decode_sample(uint32_t code, unsigned int k);

/**
 * decodes a file bit-by-bit, returning each complete code
 * @param  stream [bitfile stream, will be read continually]
 * @param  k      [control parameter for size of divisor]
 * @return        [decoded sample, held in a unsigned 32-bit int container]
 */
uint32_t decode_sample_bitfile(bit_file_t *stream, unsigned int k);


#endif // ENCODER_H