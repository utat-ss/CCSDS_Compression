/**
 * Yong Da Li
 * Saturday, June 4, 2022
 * 
 * section 5.4.3.2.2 Length-Limited Golomb-Power-of-2 Codeword of CCSDS123 standard
 * encoder
 */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdint.h>
#include <time.h>

// technically don't need the "util/" prefixes, since I handle it in the GCC include paths -I argument
// but including it for clarity
#include "util/bitfile.h"
#include "util/logger.h"
#include "util/mymatrix.h"
#include "util/datacube.h"
#include "encoder/encoder.h"
#include "predictor/predictor.h"
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
    logger("INFO", "input matrix\n");
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
    logger("INFO", "decoded matrix\n");
    pretty_print_mat(decoded_mat);
    pretty_save_mat(decoded_mat, "output/decoded_mymatrix.txt");

    // uint32_t* decoded_array = (uint32_t*) malloc(sizeof(uint32_t) * nrows*ncols);
    // check_multiple_decode("output/encoded.bin", k, decoded_array, nrows*ncols);

    logger_finalize();

    return;
}

void check_mymatrix_operations(void) {
    int nrows = 2;
    int ncols = 2;

    logger_init("output/check_mymatrix_operations.log");

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

    logger_finalize();

    return;
}


void check_datacube(int argc, char* argv[]){
    if (argc !=4){
        printf("check datacube function:\n");
        printf("usage is 3 numbers: <depth> <nrows> <ncols>\n");
        return;
    }

    logger_init("output/check_datacube.log");

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

    filepath = "output/random_datacube_formatted.txt";
    printf("saved to file: %s\n", filepath);
    save_cube(cube, filepath);

    datacube* read_cube = parse_cube_from_file(filepath);
    printf("read from file: %s\n", filepath);
    pretty_print_cube(read_cube);

    logger_finalize();

    return;
}


void check_local_sum(){
    logger_init("output/check_local_sum.log");

    int nrows = 3;
    int ncols = 4;

    mymatrix* mat_test = ordered_matrix(nrows, ncols);
    pretty_save_mat(mat_test, "output/check_local_sum_ordered_matrix.txt");

    pretty_print_mat(mat_test);

    float sum = 0;
    int index;

    // loop through array and check local sum
    for (int i=0; i<nrows; i++){
        for (int j=0; j<ncols; j++){
            sum = local_sum(mat_test, i, j);
            logger("INFO", "local sum from (%d, %d) is %f\n", i, j, sum);

            index = i*ncols + j;  // walk row-major to check correctness
            switch(index){
                // case 0 is (0,0) --> never predicted, always kept as original value
                case 1:
                    assert(sum == 0);
                    break;
                case 2:
                    assert(sum == 4);
                    break;
                case 3:
                    assert(sum == 8);
                    break;
                case 4:
                    assert(sum == 2);
                    break;
                case 5:
                    assert(sum == 7);
                    break;
                case 6:
                    assert(sum == 11);
                    break;
                case 7:
                    assert(sum == 14);
                    break;
                case 8:
                    assert(sum == 18);
                    break;
                case 9:
                    assert(sum == 23);
                    break;
                case 10:
                    assert(sum == 27);
                    break;
                case 11:
                    assert(sum == 30);
                    break;

            }
        }
    }

    logger_finalize();
}


void check_local_diff_vector(){
    logger_init("output/check_local_diff.log");

    int nrows = 3;
    int ncols = 4;
    int depth = 5;

    // created data cube with 0,1,2,3,4, ... , N
    datacube* cube = ordered_datacube(depth, nrows, ncols);
    pretty_save_cube(cube, "output/check_local_diff_ordered_datacube.txt");
    pretty_print_cube(cube);

    // only check the local vector on the (0,0,1) value
    // the (0,0,0) value is always kept as the raw value, since it's used for reference to decode everything else
    myvector* vec_compute = compute_local_diff_vector(cube, 0, 0, 1);
    pretty_save_vec(vec_compute, "output/check_local_diff_ordered_vec.txt");
    pretty_print_vec(vec_compute);

    // check that the vector matches hand calculations
    myvector* vec_ref = create_vector(vec_compute->size);
    vec_set(vec_ref, 0, 0);
    vec_set(vec_ref, 1, 0);
    vec_set(vec_ref, 2, 0);
    vec_set(vec_ref, 3, 4);
    vec_set(vec_ref, 4, 4);
    vec_set(vec_ref, 5, 4);
    vec_set(vec_ref, 6, 4);
    vec_set(vec_ref, 7, 0);

    int compare = vec_compare(vec_compute, vec_ref);
    assert(compare == 1);

    logger_finalize();
}


void check_predictor(){
    logger_init("output/check_predictor.log");

    int nrows = 3;
    int ncols = 4;
    int depth = 5;

    // created data cube with 0,1,2,3,4, ... , N
    datacube* cube = ordered_datacube(depth, nrows, ncols);
    pretty_save_cube(cube, "output/check_predictor_datacube.txt");
    pretty_print_cube(cube);

    for (int z=0; z<depth; z++){
        for (int x=0; x<ncols; x++){
            for (int y=0; y<nrows; y++){
                // skip the (0,0,0) pixel since it's never encoded
                if (x==0 && y==0 && z==0){
                    continue;
                }
                else{
                    logger("INFO", "predict (%d, %d, %d) = %f\n", z,x,y, predict_calc(cube,z,x,y));
                }
            }
        }
    }

    logger_finalize();
}


void check_data_parsing(void){
    char filename[] = "/Users/liyongda/Documents/GitHub/CCSDS_Compression/integration/images/indian_pines.txt";
    // char filename[] = "/Users/liyongda/Documents/GitHub/CCSDS_Compression/integration/output/random_datacube_formatted.txt";

    datacube* cube = parse_cube_from_file(filename);
    // pretty_print_cube(cube);
    save_cube(cube, "/Users/liyongda/Documents/GitHub/CCSDS_Compression/integration/output/parse_check_indian_pines.txt");
}

int main(int argc, char* argv[]){
    // check_datacube(argc, argv);
    // check_encoder(argc, argv);
    // check_mymatrix_operations();
    // check_datacube(argc, argv);
    // check_local_sum();
    // check_local_diff_vector();

    // check_predictor();

    check_data_parsing();

    return 0;
}