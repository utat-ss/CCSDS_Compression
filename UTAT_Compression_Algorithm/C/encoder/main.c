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
#include <gsl/gsl_matrix.h>

#include "main.h"
#include "encoder.h"

/**
 * creates a gsl matrix object with given size, filled with random integers from 0-99
 * @param  nrow [number of rows]
 * @param  ncol [number of columns]
 * @return      [gsl matrix object, integers]
 */
gsl_matrix_int* init_gsl_matrix(unsigned int nrow, unsigned int ncol){
  int i, j; 
  srand( (unsigned int) time(NULL) );    // init random
  gsl_matrix_int * matrix = gsl_matrix_int_alloc (nrow, ncol);

  for (i = 0; i < nrow; i++){
    for (j = 0; j < ncol; j++){
      gsl_matrix_int_set (matrix, i, j, (int) rand()%100);
      }
  }

  return matrix;
}


void pretty_print_matrix(gsl_matrix_int* matrix){
    int nrow = (int) matrix->size1;
    int ncol = (int) matrix->size2;
    printf("----- matrix ----- \n");
    printf("size: %d x %d\n", nrow, ncol);

    int i, j;
    for (i=0; i<nrow; i++){
        for (j=0; j<ncol; j++){
            printf("%d ", gsl_matrix_int_get(matrix, i, j));
        }
        printf("\n");
    }
}


void check_single_encode(void){
    unsigned int sample = 324;
    unsigned int k = 7;
    unsigned int num_bits_used;
    // unsigned int encoded = encode_sample(sample, k);
    unsigned int encoded = encode_sample_optimized(sample, k, &num_bits_used);

    printf("===== encode =====\n");
    printf("---- parameters ----\n");
    printf("Golomb power of 2 [k]: %u\n", k);
    printf("divisor [M]: %u\n", (unsigned int) pow(2,k));


    printf("---- encoding ----\n");
    printf("sample: dec = %u, \tbin = "\
        BYTE_TO_BINARY_PATTERN " " \
        BYTE_TO_BINARY_PATTERN " " \
        BYTE_TO_BINARY_PATTERN " " \
        BYTE_TO_BINARY_PATTERN \
        ", \thex = %x\n", sample, \
        BYTE_TO_BINARY(sample>>24), \
        BYTE_TO_BINARY(sample>>16), \
        BYTE_TO_BINARY(sample>>8), \
        BYTE_TO_BINARY(sample), sample);

    printf("encoded: \t\tbin = "\
        BYTE_TO_BINARY_PATTERN " " \
        BYTE_TO_BINARY_PATTERN " " \
        BYTE_TO_BINARY_PATTERN " " \
        BYTE_TO_BINARY_PATTERN \
        ", \thex = %x\n", \
        BYTE_TO_BINARY(encoded>>24), \
        BYTE_TO_BINARY(encoded>>16), \
        BYTE_TO_BINARY(encoded>>8), \
        BYTE_TO_BINARY(encoded), encoded);
    printf("input sample uses %d bits\n", (int)sizeof(sample)*8);
    printf("encoded sample uses %d bits\n", num_bits_used);
}

void check_single_decode(void){
    /**
     * ===== parameters =====
     * Golomb power of 2 [k]: 4
     * divisor [M]: 16
     * ===== encoding =====
     * sample: decimal = 18,   bin = 00000000 00000000 00000000 00010010,      hex = 12
     * encoded:                bin = 00100010,         hex = 22
     */
    uint32_t code = 0x344;
    unsigned int k = 7;

    printf("===== decode =====\n");
    printf("code: hex = %X, \tbin = "\
        BYTE_TO_BINARY_PATTERN " " \
        BYTE_TO_BINARY_PATTERN " " \
        BYTE_TO_BINARY_PATTERN " " \
        BYTE_TO_BINARY_PATTERN \
        ", \thex = %x\n", code, \
        BYTE_TO_BINARY(code>>24), \
        BYTE_TO_BINARY(code>>16), \
        BYTE_TO_BINARY(code>>8), \
        BYTE_TO_BINARY(code), code);   

    printf("decoded: %d\n", decode_sample(code, k));
}


void print_binary_32(uint32_t word){
    printf("word: hex = %X, \tbin = "\
        BYTE_TO_BINARY_PATTERN " " \
        BYTE_TO_BINARY_PATTERN " " \
        BYTE_TO_BINARY_PATTERN " " \
        BYTE_TO_BINARY_PATTERN \
        ", \thex = %x\n", word, \
        BYTE_TO_BINARY(word>>24), \
        BYTE_TO_BINARY(word>>16), \
        BYTE_TO_BINARY(word>>8), \
        BYTE_TO_BINARY(word), word);  
}

void check_multiple_encode_decode(void){
    printf("==== muliple encode and decode ====\n");
    int nrow = 5;
    int ncol = 3;
    gsl_matrix_int *matrix = init_gsl_matrix(nrow,ncol);
   
    // pretty_print_matrix(matrix);
    
    uint32_t sample = 242;
    unsigned int k = 6;
    unsigned int num_bits_used;

    uint32_t code = encode_sample_optimized(sample, k, &num_bits_used);

    // write binary mode
    FILE *fptr;
    fptr = fopen("output/encoded.bin","wb");
    fwrite(&code, 4, 1, fptr);
    printf("written:\t");
    print_binary_32(code);
    // need to close the file in write mode, before trying to read it back
    // if you don't close it, the read back value will be wrong
    fclose(fptr);           


    // read binary mode
    fptr = fopen("output/encoded.bin", "rb");
    uint32_t read_code;
    fread(&read_code, 4, 1, fptr);
    printf("read:\t\t");
    print_binary_32(read_code);
    fclose(fptr);


    gsl_matrix_int_free(matrix);
}

int main(void){
    check_single_encode();
    check_single_decode();
    // check_multiple_encode_decode();
    
    return 0;
}