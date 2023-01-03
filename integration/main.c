/**
 * Yong Da Li
 * Saturday, June 4, 2022
 * 
 * section 5.4.3.2.2 Length-Limited Golomb-Power-of-2 Codeword of CCSDS123 standard
 * encoder
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>

// technically don't need the "util/" prefixes, since I handle it in the GCC include paths -I argument
// but including it for clarity
#include "util/bitfile.h"
#include "util/logger.h"
#include "util/mymatrix.h"
#include "encoder/encoder.h"
#include "main.h"

/**
 * Yong Da Li
 * Saturday, July 16, 2022
 * TODO:
 * - need to do accumulator + counter to change the K value as index progresses
 *
 * structure encode:
 * - function to generate RNG GSL matrix
 * - feed GSL matrix to wrapper function in encoder.c
 * - wrapper function handles accumulator+counter
 * - calls encode_optimized() and gives it the appropriate value of k
 * - returns nothing, writes encoded values directly to binary file
 *
 * structure decode:
 * - wrapper function in encoder.c that handles accumulator+counter
 *     - input is bitfile stream
 * - calls decode_sample() and gives it the appropriate value of k
 * - returns (via parameter) uint32_t array of decoded samples
 * 
 */


int main(int argc, char* argv[]){
    logger_init("output/encoder.log");
    srand((unsigned int)time(NULL));  // init random
    // check_single_encode();
    // check_single_decode();
    // check_read_write_to_binary_file();

    if (argc != 3){
        printf("usage is [number of rows], [number of columns], expect 2 arguments\n");
        return -1;
    }
    
    // convert from string to integer
    int nrows = atoi(argv[1]);
    int ncols = atoi(argv[2]);
    int min = 0;
    int max = min + (int)pow(2,8);
    // int range = 100;
    int k = 7;
    logger("INFO", "parameter k = %d\n", k);

    // create random matrix data
    mymatrix* matrix = random_matrix(nrows, ncols, min, max);
    pretty_print_mat(matrix);
    pretty_save_mat(matrix, "output/sample_mymatrix.txt");

    adaptive_encode_mymatrix(matrix, "output/encoded.bin");
    // encode_mymatrix(matrix, k, "output/encoded.bin");
    // view_binary_file("output/encoded.bin");
    // view_binary_file_using_bitfile("output/encoded.bin");

    // decode
    // uint32_t* decoded_array = (uint32_t*) malloc(sizeof(uint32_t) * nrows*ncols);
    // check_multiple_decode("output/encoded.bin", k, decoded_array, nrows*ncols);

    logger_finalize();
    
    return 0;
}