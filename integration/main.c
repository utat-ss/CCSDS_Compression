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


int main(void){
    logger_init("output/encoder.log");
    srand((unsigned int)time(NULL));  // init random
    // check_single_encode();
    // check_single_decode();
    // check_read_write_to_binary_file();
    
    int nrow = 1;
    int ncol = 1;
    int min = 0;
    int max = min + (int)pow(2,8);
    // int range = 100;
    int k = 7;
    logger("INFO", "parameter k = %d\n", k);
    mymatrix * matrix = encode_rng_mymatrix(nrow, ncol, min, max, k);
    // view_binary_file("output/encoded.bin");
    // view_binary_file_using_bitfile("output/encoded.bin");

    // decode
    uint32_t* decoded_array = (uint32_t*) malloc(sizeof(uint32_t) * nrow*ncol);
    check_multiple_decode("output/encoded.bin", k, decoded_array, nrow*ncol);

    // myvector* vecA = random_vector(3, -5,5);
    // myvector* vecB = random_vector(3, -5,5);
    // pretty_print_vec(vecA);
    // pretty_print_vec(vecB);

    // float result = 0.0;
    // result = vec_dot_prod(vecA, vecB);
    // printf("result %f\n", result);

    // mymatrix* mat = random_matrix(5,5,0,10);
    // pretty_save_mat(mat, "output/random_matrix.txt");
    // pretty_save_vec(vecA, "output/random_vectorA.txt");
    // pretty_save_vec(vecB, "output/random_vectorB.txt");

    logger_finalize();
    
    return 0;
}