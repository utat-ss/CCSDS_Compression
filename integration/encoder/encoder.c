/**
 * Yong Da Li
 * Saturday, June 4, 2022
 * 
 * section 5.4.3.2.2 Length-Limited Golomb-Power-of-2 Codeword of CCSDS123 standard
 * encoder
 */
#include <math.h>
#include <stdint.h>

#include "logger.h"
#include "encoder.h"
#include "gsl_extensions.h"

/**
 * returns the GOP2 code word for an unsigned integer as a char array 
 * @param  sample 	- input integer
 * @param  k		- sample/2**k parameter
 * @return 			- char array of GOP2 code word
 */
unsigned int encode_sample(unsigned int sample, unsigned int k){
	unsigned int ret;
	unsigned int unary = 0;	// unary portion, pad with 1's so initialize to 0
	unsigned int binary;	// binary portion

	// compute portions needed
	unsigned int divisor = (int) pow(2,k);
	unsigned int quotient = sample / divisor;
	unsigned int remainder = sample % divisor;

	/* debug messages
	printf("sample: %u\n", sample);
	printf("k: %u\n", k);
	printf("divisor: %u\n", divisor);
	printf("quotient: %u\n", quotient);
	printf("remainder: %u\n", remainder);
	*/

	// unary portion --> pad with 1's and stop character 0, since it makes debugging easier
	// 					can easily find start of unary portion in integer, since it's leading 1's
	while (quotient > 0){
		unary = (unary | 0x1) << 1;	// pad with 1's
		quotient--;
	}
	// don't need to add stop character since unary was initialized with 0's


	// binary portion
	unsigned int binary_bitmask = 0;
	int k_copy = k;
	while (k_copy>0){	// create bit mask
		binary_bitmask = (binary_bitmask << 1 | 0x1);
		k_copy--;
	}
	binary = remainder & binary_bitmask; // create binary portion


	// combine
	ret = (unary<<k) | binary; // length of bitmask is k

	/* debug messages
	printf("unary: \t\t"BYTE_TO_BINARY_PATTERN"\n", BYTE_TO_BINARY(unary));
	printf("binary: \t"BYTE_TO_BINARY_PATTERN"\n", BYTE_TO_BINARY(binary));
	*/

	return ret;
}

//Wrap this function in something that gives a diff k value.
uint32_t encode_sample_optimized(uint32_t sample, unsigned int k, unsigned int* num_bits_used){
	uint32_t ret;
	unsigned int M = (int) pow(2,k);
	unsigned int quotient;
	unsigned int remainder;
	unsigned int unary = 0;

	// calculate using bitwise operations
	quotient = sample >> k;
	remainder = sample & (M-1);

	// number of bits needed
	// quotient in unary + 1 for unary stop character + k bits for remainder
	// 		- actually don't need this since the output is like: 00100010
	// 		- go by "first 1", that's the start of the sample, stored in 32 bit integer
	 /**
	  * number of bits used
	  * - always +1 for the 0 stop character (0, 10, 110, 1110, etc)
	  * - +quotient for the unary portion
	  * - +k for the remainder
	  */
	if (quotient > 0){
		*num_bits_used = quotient + 1 + k;
	}
	else{	// if don't need unary portion, then just use k bits, always plus stop character
		*num_bits_used = k+1;
	}

	// create unary encoding of quotient
	while (quotient > 0){
		unary = (unary | 0x1) << 1;	// pad with 1's
		quotient--;
	}	

	// combine
	ret = (unary << k) | remainder;

	return ret;
}


uint32_t decode_sample(uint32_t code, unsigned int k){
	uint32_t ret;
	int i = 32; // indexing variable, start at bit 32 (left edge) and work backwards
	uint32_t quotient = 0; 
	uint32_t remainder = 0;

	// structure: 0000001111011
	// 			  ^^^^^^ 		not encoded values, padding
	// 			        ^^^^^	unary value + stop character 0
	// 			             ^^ last k bits are remainder 
	
	/**
	 * TODO: Thursday, June 30, 2022
	 * doesn't account for the case where there is no unary
	 * aka it goes directly into truncated binary
	 * ex. sample = 28, k=5 --> divisor = 32
	 * 	encoded = no unary + bin(28) = 0b11100
	 * 	notice binary is 5 bit long representation (same as k)
	 * 	
	 */

	// find the leading 1
	while( (code & (1<<i)) >> i != 1 ){
		i--;
	}

	// count number of 1's in unary, until stop character 0
	while ( (code & (1<<i)) >> i == 1 ){
		i--;
		quotient++;
	}

	// want to extract last k bits
	// shift all to left, then shift right to pull out leading 0's
	remainder = code << (32-k);
	remainder = remainder >> (32-k);

	// combine
	ret = (int) pow(2,k) * quotient + remainder;

	return ret;
}


uint32_t decode_sample_bitfile(bit_file_t *stream, unsigned int k){
	uint32_t decoded = 0;
	uint32_t c;
	uint32_t i = 0;
	uint32_t unary_count = 0;
	uint32_t truncated_binary = 0;

	c = BitFileGetBit(stream);
	// printf("DEBUG: first character=%d\n", c);


	// assume unary portion first
	if (c == 0){	// if first entry is 0, then the next k bits are the truncated binary
		// printf("DEBUG: no unary portion, only truncated binary\n");
		for (i=0; i<k; i++){
			truncated_binary = truncated_binary | (c << (k-i));	// add in backwards order, since reading bits in backwards order
			c = BitFileGetBit(stream);	// get next bit
		}
		truncated_binary = truncated_binary | (c << (k-i));	// need to do it for the last bit

		// printf("DEBUG: finished truncated binary only = %3d\n", truncated_binary);
	}
	else {
		// printf("DEBUG: unary and truncated binary\n");
		// read the unary portion, count number of 1's
		while (c == 1){
			unary_count++;
			c = BitFileGetBit(stream);
		}

		// then read the truncated binary portion
		for (i=0; i<k; i++){
			truncated_binary = truncated_binary | (c << (k-i));	// add in backwards order, since reading bits in backwards order
			c = BitFileGetBit(stream);	// get next bit
		}
		truncated_binary = truncated_binary | (c << (k-i));	// need to do it for the last bit
	}

	// calculate value
	// printf("DEBUG: unary_count = %3d\n", unary_count);
	decoded = pow(2,k)*unary_count + truncated_binary;

	return decoded;
}

void check_single_encode(void) {
        unsigned int sample = 19;
        unsigned int k = 5;
        unsigned int num_bits_used;
        // unsigned int encoded = encode_sample(sample, k);
        unsigned int encoded = encode_sample_optimized(sample, k, &num_bits_used);

        logger("INFO", "===== encode =====\n");
        logger("INFO", "---- parameters ----\n");
        logger("INFO", "Golomb power of 2 [k]: %u\n", k);
        logger("INFO", "divisor [M]: %u\n", (unsigned int)pow(2, k));
        logger("INFO", "sample: %u\n", sample);

        logger("INFO", "---- encoding ----\n");
        print_binary_32(sample);
        print_binary_32(encoded);
        logger("INFO", "input sample uses %d bits\n", (int)sizeof(sample) * 8);
        logger("INFO", "encoded sample uses %d bits\n", num_bits_used);
}

void check_single_decode(void) {
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

void check_read_write_to_binary_file(void) {
        uint32_t sample = 242;
        unsigned int k = 6;
        unsigned int num_bits_used;

        uint32_t code = encode_sample_optimized(sample, k, &num_bits_used);

        // write binary mode
        FILE *fptr;
        fptr = fopen("output/encoded.bin", "wb");
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

gsl_matrix_int* encode_rng_gsl_matrix(int nrow, int ncol, int range, int k) {
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

        bit_file_t *bOutFile; /* encoded output */
        bOutFile = MakeBitFile(outFile, BF_WRITE);

        int i, j;
        int bit = 0;
        int counter = 0;
        uint32_t sample, code;
        for (i = 0; i < nrow; i++) {
                for (j = 0; j < ncol; j++) {
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
                        while (counter < num_bits_used) {
                            bit = (code >> (num_bits_used - 1 - counter)) & 0x1;
                            BitFilePutBit(bit, bOutFile);

                            counter++;  // increment counter
                        }
                        counter = 0;  // reset counter

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
        // gsl_matrix_int_free(matrix);
        return matrix;
}

void check_multiple_decode(char *filename, int k, uint32_t *decoded_array, uint32_t decoded_size) {
        uint32_t counter = 0;  //  number of currently decoded samples, amount of samples is known a priori (aka in file header)
        uint32_t decoded;

        logger("INFO", "==== multiple decode ====\n");
        bit_file_t *bfp;

        logger("INFO", "==== opening file using bitfile: %s ===\n", filename);
        bfp = BitFileOpen(filename, BF_READ);

        while (counter < decoded_size) {
                decoded = decode_sample_bitfile(bfp, (unsigned int)k);
                decoded_array[counter] = decoded;
                logger("DEBUG", "decoded[%2d] = %3d\n", counter, decoded_array[counter]);

                counter++;
        }

        BitFileClose(bfp);
}

void view_binary_file(char *filename) {
        int c;
        FILE *fp;
        logger("INFO", "==== reading file: %s ===\n", filename);
        fp = fopen(filename, "r");
        while (1) {
                c = fgetc(fp);
                if (feof(fp)) {
                        break;
                }
                logger("INFO", "%x\n", c);
        }
        fclose(fp);

        logger("INFO", "\n==== end file ===\n");
}

void view_binary_file_using_bitfile(char *filename) {
        uint32_t i = 0;
        bit_file_t *bfp;
        int value;

        logger("INFO", "==== reading file using bitfile: %s ===\n", filename);
        bfp = BitFileOpen(filename, BF_READ);

        // read until end of file
        while (1) {
                value = BitFileGetBit(bfp);
                if (value == EOF) {  // end of file
                        logger("INFO", "end of file\n");
                        return;
                } else {
                        logger("INFO", "read bit [%2d] = %d\n", i, value);
                }
                i++;
        }
        logger("INFO", "\n==== end bitfile ===\n");
        BitFileClose(bfp);
}
