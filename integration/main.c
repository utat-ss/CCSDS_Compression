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
#include "util/datacube.h"
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


void check_encoder(int argc, char* argv[]){
    printf("=============\n");
    printf("check encoder\n");
    printf("=============\n");

    logger_init("output/encoder.log");
    srand((unsigned int)time(NULL));  // init random
    // check_single_encode();
    // check_single_decode();
    // check_read_write_to_binary_file();

    if (argc != 3) {
        printf("usage is [number of rows], [number of columns], expect 2 arguments\n");
        return;
    }

    // convert from string to integer
    int nrows = atoi(argv[1]);
    int ncols = atoi(argv[2]);
    int min = 0;
    int max = min + (int)pow(2, 8);
    // int range = 100;
    int k = 7;
    logger("INFO", "parameter k = %d\n", k);

    // create random matrix data
    mymatrix* matrix = random_matrix(nrows, ncols, min, max);
    pretty_print_mat(matrix);
    pretty_save_mat(matrix, "output/sample_mymatrix.txt");

    // encode
    adaptive_encode_mymatrix(matrix, "output/encoded.bin");
    // encode_mymatrix(matrix, k, "output/encoded.bin");
    // view_binary_file("output/encoded.bin");
    // view_binary_file_using_bitfile("output/encoded.bin");

    // decode
    mymatrix* decoded_mat = create_matrix(nrows, ncols);
    adaptive_decode_bitfile(decoded_mat, "output/encoded.bin");
    pretty_print_mat(decoded_mat);
    pretty_save_mat(decoded_mat, "output/decoded_mymatrix.txt");

    // uint32_t* decoded_array = (uint32_t*) malloc(sizeof(uint32_t) * nrows*ncols);
    // check_multiple_decode("output/encoded.bin", k, decoded_array, nrows*ncols);

    logger_finalize();
}

void check_mymatrix_operations(void) {
    int nrows = 2;
    int ncols = 2;

    // test matrix * matrix
    mymatrix* mat_identity = identity_matrix(nrows, ncols);
    mymatrix* mat_rand = random_matrix(nrows, ncols, 0, 10);

    mymatrix* mat_mat_mult_result = mat_mat_mult(mat_identity, mat_rand);

    printf("identity\n");
    pretty_print_mat(mat_identity);

    printf("rand\n");
    pretty_print_mat(mat_rand);

    printf("mat mat mult result\n");
    pretty_print_mat(mat_mat_mult_result);

    // test matrix * vector
    printf("\n============\n");
    myvector* vec_rand = random_vector(ncols, 0, 10);
    myvector* mat_vec_mult_result = mat_vec_mult(mat_rand, vec_rand);

    printf("mat rand\n");
    pretty_print_mat(mat_rand);

    printf("vector random\n");
    pretty_print_vec(vec_rand);

    printf("mat vec mult result\n");
    pretty_print_vec(mat_vec_mult_result);

    // clean up
    del_matrix(mat_identity);
    del_matrix(mat_rand);
    del_matrix(mat_mat_mult_result);
    del_vector(vec_rand);
    del_vector(mat_vec_mult_result);
}


void check_datacube(int argc, char* argv[]){
    if (argc !=4){
        printf("check datacube function:\n");
        printf("usage is 3 numbers: <depth> <nrows> <ncols>\n");
        return;
    }
    // convert from string to integer
    int depth = atoi(argv[1]);
    int nrows = atoi(argv[2]);
    int ncols = atoi(argv[3]);

    // locally defined
    int min = 0;
    int max = 10;
    datacube* cube = random_datacube(depth, nrows, ncols, min, max);
    pretty_print_cube(cube);

    char* filepath = "output/random_datacube.txt";
    pretty_save_cube(cube, filepath);

    return;
}

int main(int argc, char* argv[]){
    // check_encoder(argc, argv);
    // check_mymatrix_operations();
    check_datacube(argc, argv);
    
    return 0;
}