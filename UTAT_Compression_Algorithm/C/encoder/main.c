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

#include "bitfile/bitfile.h"
#include "logger/logger.h"
#include "main.h"
#include "encoder.h"

/**
 * creates a gsl matrix object with given size, filled with random integers from 0-range
 * @param  nrow [number of rows]
 * @param  ncol [number of columns]
 * @return      [gsl matrix object, integers]
 */
gsl_matrix_int* init_gsl_matrix(unsigned int nrow, unsigned int ncol, int range){
  int i, j; 
  srand( (unsigned int) time(NULL) );    // init random
  gsl_matrix_int* matrix = gsl_matrix_int_alloc (nrow, ncol);

  for (i = 0; i < nrow; i++){
    for (j = 0; j < ncol; j++){
      gsl_matrix_int_set (matrix, i, j, (int) rand()%range);
      }
  }

  return matrix;
}


/**
 * nicely prints out a gsl matrix (int only)
 * @param matrix [gsl_matrix_int type]
 */
void pretty_print_matrix(gsl_matrix_int* matrix){
    int nrow = (int) matrix->size1;
    int ncol = (int) matrix->size2;

    printf("----- matrix ----- \n");
    printf("size: %d x %d = %d\n", nrow, ncol, nrow*ncol);

    int i, j;
    for (i=0; i<nrow; i++){
        for (j=0; j<ncol; j++){
            printf("%3d ", gsl_matrix_int_get(matrix, i, j));  // %3d, at least 3 wide
        }
        printf("\n");
    }
}

void pretty_save_matrix(gsl_matrix_int* matrix, char* filepath){
    FILE* fptr;
    fptr = fopen(filepath, "w");

    int nrow = (int) matrix->size1;
    int ncol = (int) matrix->size2;
    fprintf(fptr, "----- matrix ----- \n");
    fprintf(fptr, "size: %d x %d\n", nrow, ncol);


    int i, j;
    for (i=0; i<nrow; i++){
        for (j=0; j<ncol; j++){
            fprintf(fptr, "%3d ", gsl_matrix_int_get(matrix, i, j));  // %3d, at least 3 wide
        }
        fprintf(fptr, "\n");
    }

    fclose(fptr);
}


void check_single_encode(void){
    unsigned int sample = 19;
    unsigned int k = 5;
    unsigned int num_bits_used; 
    // unsigned int encoded = encode_sample(sample, k);
    unsigned int encoded = encode_sample_optimized(sample, k, &num_bits_used);

    logger("INFO", "===== encode =====\n");
    logger("INFO", "---- parameters ----\n");
    logger("INFO", "Golomb power of 2 [k]: %u\n", k);
    logger("INFO", "divisor [M]: %u\n", (unsigned int) pow(2,k));
    logger("INFO", "sample: %u\n", sample);


    logger("INFO", "---- encoding ----\n");
    print_binary_32(sample);
    print_binary_32(encoded);
    logger("INFO", "input sample uses %d bits\n", (int)sizeof(sample)*8);
    logger("INFO", "encoded sample uses %d bits\n", num_bits_used);
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

    logger("INFO:", "===== decode =====\n");
    print_binary_32(code); 
    logger("INFO:", "decoded: %d\n", decode_sample(code, k));
}

void check_read_write_to_binary_file(void){
    uint32_t sample = 242;
    unsigned int k = 6;
    unsigned int num_bits_used;

    uint32_t code = encode_sample_optimized(sample, k, &num_bits_used);

    // write binary mode
    FILE *fptr;
    fptr = fopen("output/encoded.bin","wb");
    /**
     * https://www.tutorialspoint.com/c_standard_library/c_function_fwrite.htm
     * size_t fwrite(const void *ptr, size_t size, size_t nmemb, FILE *stream)
     * ptr − This is the pointer to the array of elements to be written.
     * size − This is the size in bytes of each element to be written.
     * nmemb − This is the number of elements, each one with a size of size bytes.
     * stream − This is the pointer to a FILE object that specifies an output stream.
     */
    fwrite(&code, 1, sizeof(code), fptr);
    printf("written:\t");
    print_binary_32(code);
    // need to close the file in write mode, before trying to read it back
    // if you don't close it, the read back value will be wrong
    fclose(fptr);           


    // read binary mode
    fptr = fopen("output/encoded.bin", "rb");
    uint32_t read_code;
    fread(&read_code, 1, sizeof(read_code), fptr);
    printf("read:\t\t");
    print_binary_32(read_code);
    fclose(fptr);
}

void encode_rng_gsl_matrix(int nrow, int ncol, int range, int k){
    logger("INFO", "==== multiple encode ====\n");

    // setup gsl matrix with random data
    gsl_matrix_int *matrix = init_gsl_matrix(nrow, ncol, range);
    pretty_print_matrix(matrix);
    pretty_save_matrix(matrix, "output/sample.txt");
    
    // setup encoding parameters
    unsigned int num_bits_used;

    // setup bitfile
    FILE *outFile;
    outFile = fopen("output/encoded.bin", "wb");

    bit_file_t *bOutFile;               /* encoded output */
    bOutFile = MakeBitFile(outFile, BF_WRITE);

    int i, j;
    int bit = 0;
    int counter = 0;
    uint32_t sample, code;
    for(i=0; i<nrow; i++){
        for(j=0; j<ncol; j++){
            sample = gsl_matrix_int_get(matrix, i, j);
            code = encode_sample_optimized(sample, k, &num_bits_used);

            logger("DEBUG", "(%d, %d): sample=%3d | code=%-8x | num_bits_used=%3d\n", i, j, sample, code, num_bits_used);
            print_binary_32(code);

           /**
            * Yong Da Li, Saturday, July 16, 2022
            * note that the bitfile library BitFilePutBits() (plural) gives some weird padding issue
            *     - breaks assumptions about binary file structure
            * falling back to use the singular BitFilePutBit() and looping through --> avoids padding issue
            */
            while (counter < num_bits_used){
                bit = (code >> (num_bits_used - 1 - counter)) & 0x1;
                BitFilePutBit(bit, bOutFile);

                counter++;  // increment counter
            }
            counter = 0;    // reset counter

            /**
             * Saturday, July 9, 2022
             * DON'T USE `hexdump` to view it
             * If the bytes on disk are (ABCD), hexdump will display it like (BADC)
             * A,B,C,D = 8-bit blocks
             * () = 1 byte = 4 * 8-bit blocks
             *
             * instead, use view_binary_file() to print it out using C
             */
        }
    }

    /* pad fill with 1s so decode will run into EOF */
    BitFileFlushOutput(bOutFile, 1);

    fclose(outFile);
    gsl_matrix_int_free(matrix);
}


void check_multiple_decode(char* filename, int k, uint32_t* decoded_array, uint32_t decoded_size){
    uint32_t counter = 0; //  number of currently decoded samples, amount of samples is known a priori (aka in file header)
    uint32_t decoded;

    logger("INFO", "==== multiple decode ====\n");
    bit_file_t *bfp;

    logger("INFO", "==== opening file using bitfile: %s ===\n", filename);
    bfp = BitFileOpen(filename, BF_READ);


    while(counter < decoded_size){
        decoded = decode_sample_bitfile(bfp, (unsigned int) k);
        decoded_array[counter] = decoded;
        logger("DEBUG", "decoded[%2d] = %3d\n", counter, decoded_array[counter]);

        counter++;
    }

    BitFileClose(bfp);

}


void view_binary_file(char* filename){
   int c;
   FILE *fp;
   logger("INFO", "==== reading file: %s ===\n", filename);
   fp = fopen(filename,"r");
   while(1) {
      c = fgetc(fp);
      if( feof(fp) ) {
         break ;
      }
      logger("INFO", "%x\n", c);
   }
   fclose(fp);

   logger("INFO", "\n==== end file ===\n");
}


void view_binary_file_using_bitfile(char* filename){
    uint32_t i = 0;
    bit_file_t *bfp;
    int value;

    logger("INFO", "==== reading file using bitfile: %s ===\n", filename);
    bfp = BitFileOpen(filename, BF_READ);

    // read until end of file
    while(1){
        value = BitFileGetBit(bfp);
        if (value == EOF){  // end of file
            logger("INFO", "end of file\n");
            return;
        }
        else{
            logger("INFO", "read bit [%2d] = %d\n", i, value);
        }
        i++; 
    }
    logger("INFO", "\n==== end bitfile ===\n");
    BitFileClose(bfp);
}

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
    logger_init("encoder.log");
    // check_single_encode();
    // check_single_decode();
    // check_read_write_to_binary_file();
    
    int nrow = 1;
    int ncol = 5;
    int range = (int)pow(2,8);
    // int range = 100;
    int k = 7;
    logger("INFO", "parameter k = %d\n", k);
    encode_rng_gsl_matrix(nrow, ncol, range, k);
    // view_binary_file("output/encoded.bin");
    // view_binary_file_using_bitfile("output/encoded.bin");

    // decode
    uint32_t* decoded_array = (uint32_t*) malloc(sizeof(uint32_t) * nrow*ncol);
    check_multiple_decode("output/encoded.bin", k, decoded_array, nrow*ncol);

    logger_finalize();
    
    return 0;
}