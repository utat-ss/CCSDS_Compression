/**
 * Yong Da Li
 * Saturday, June 4, 2022
 *
 * section 5.4.3.2.2 Length-Limited Golomb-Power-of-2 Codeword of CCSDS123 standard
 * encoder
 */
#include <math.h>
#include <stdint.h>
#include <assert.h>

#include "encoder.h"
#include "logger.h"
#include "mymatrix.h"

/* ============= single encode and decode functions =========== */

/**
 * @brief optimized Golomb rice encoder
 * https://michaeldipperstein.github.io/rice.html
 *
 * @param sample        sample to be encoded, note it's uint32_t size (will need to be increased)
 * @param k             control parameter, divisor M = 2^k
 * @param num_bits_used return by reference, how many bits were used
 * @return uint32_t     encoded sample, stored as 23-bit integer
 */
uint32_t encode_sample_optimized(uint32_t sample, unsigned int k, unsigned int *num_bits_used) {
    uint32_t ret;
    unsigned int M = (int)pow(2, k);
    unsigned int quotient;
    unsigned int remainder;
    unsigned int unary = 0;

    // calculate using bitwise operations
    quotient = sample >> k;
    remainder = sample & (M - 1);

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
    if (quotient > 0) {
        *num_bits_used = quotient + 1 + k;
    } else {  // if don't need unary portion, then just use k bits, always plus stop character
        *num_bits_used = k + 1;
    }

    // create unary encoding of quotient
    while (quotient > 0) {
        unary = (unary | 0x1) << 1;  // pad with 1's
        quotient--;
    }

    // combine
    ret = (unary << k) | remainder;

    return ret;
}


/**
 * @brief decodes a single sample, from a unsigned 32-bit unsigned integer
 *      assumes there will always be a unary bit
 * 
 * @param code          encoded value to be decoded
 * @param k             divisor control parameter M = 2^k
 * @return uint32_t 
 */
uint32_t decode_sample(uint32_t code, unsigned int k) {
    uint32_t ret;
    int i = 32;  // indexing variable, start at bit 32 (left edge) and work backwards
    uint32_t quotient = 0;
    uint32_t remainder = 0;

    // structure: 0000001111011
    // 			  ^^^^^^ 		not encoded values, padding
    // 			        ^^^^^	unary value + stop character 0
    // 			             ^^ last k bits are remainder

    /**
     * Yong Da Li: Thursday, June 30, 2022
     * doesn't account for the case where there is no unary
     * aka it goes directly into truncated binary
     * ex. sample = 28, k=5 --> divisor = 32
     * 	encoded = no unary + bin(28) = 0b11100
     * 	notice binary is 5 bit long representation (same as k)
     *
     * Yong Da Li: Thursday, December 29, 2022
     * solution was to used a "num_bits_used" field 
     */

    // find the leading 1
    while ((code & (1 << i)) >> i != 1) {
        i--;
    }

    // count number of 1's in unary, until stop character 0
    while ((code & (1 << i)) >> i == 1) {
        i--;
        quotient++;
    }

    // want to extract last k bits
    // shift all to left, then shift right to pull out leading 0's
    remainder = code << (32 - k);
    remainder = remainder >> (32 - k);

    // combine
    ret = (int)pow(2, k) * quotient + remainder;

    return ret;
}


/* ============= constant k: streaming encode and decode functions =========== */

/**
 * @brief decodes one sample per function call, from an open bitfile handle
 *
 * @param stream        open bitfile handle, will be read continually
 * @param k             must know divisor size for decoding, M = 2^k, k control parameter
 * @return uint32_t     decoded sample, held in a unsigned 32-bit container
 */
uint32_t decode_sample_bitfile(bit_file_t *stream, unsigned int k) {
    uint32_t decoded = 0;
    uint32_t c;
    uint32_t i = 0;
    uint32_t unary_count = 0;
    uint32_t truncated_binary = 0;

    c = BitFileGetBit(stream);

    // assume unary portion first
    if (c == 0) {
        // if first entry is 0, then the next k bits are the truncated binary
        for (i = 0; i < k; i++) {
            truncated_binary = truncated_binary | (c << (k - i));  // add in backwards order, since reading bits in backwards order
            c = BitFileGetBit(stream);                             // get next bit
        }
        truncated_binary = truncated_binary | (c << (k - i));  // need to do it for the last bit
    } else {
        // read the unary portion, count number of 1's
        while (c == 1) {
            unary_count++;
            c = BitFileGetBit(stream);
        }

        // then read the truncated binary portion
        for (i = 0; i < k; i++) {
            truncated_binary = truncated_binary | (c << (k - i));  // add in backwards order, since reading bits in backwards order
            c = BitFileGetBit(stream);                             // get next bit
        }
        truncated_binary = truncated_binary | (c << (k - i));  // need to do it for the last bit
    }

    // calculate value
    // printf("DEBUG: unary_count = %3d\n", unary_count);
    decoded = pow(2, k) * unary_count + truncated_binary;

    return decoded;
}


/**
 * @brief given input matrix, write it to an output bitfile
 *
 * @param mat
 * @param filename
 */
void encode_mymatrix(mymatrix *mat, int k, char *filename) {
    int nrows = mat->nrows;
    int ncols = mat->ncols;

    // setup encoding parameters
    unsigned int num_bits_used;

    // setup bitfile
    FILE *outFile;
    outFile = fopen(filename, "wb");

    bit_file_t *bOutFile; /* encoded output */
    bOutFile = MakeBitFile(outFile, BF_WRITE);

    int i, j;
    int bit = 0;
    int counter = 0;
    uint32_t sample, code;
    for (i = 0; i < nrows; i++) {
        for (j = 0; j < ncols; j++) {
            sample = mat_get(mat, i, j);
            code = encode_sample_optimized(sample, k, &num_bits_used);  // encode

            // debugging to see sample and encoded value
            logger("DEBUG", "(%d, %d): sample=%3d | code=%-8x | num_bits_used=%3d\n", i, j, sample, code, num_bits_used);
            print_binary_32(code);

            /**
             * Yong Da Li, Saturday, July 16, 2022
             * note that the bitfile library BitFilePutBits() (plural) gives some weird padding issue
             *     - breaks assumptions about binary file structure
             * falling back to use the singular BitFilePutBit() and looping through --> avoids padding issue
             */

            // write to file all the bits used
            while (counter < num_bits_used) {
                // write left-most bit first
                // 0b1101 0010
                //   ^ write this one first, then write the ones to the right
                bit = (code >> (num_bits_used - 1 - counter)) & 0x1;
                BitFilePutBit(bit, bOutFile);

                counter++;
            }
            counter = 0;
        }
    }

    /* pad fill with 1s so decode will run into EOF */
    BitFileFlushOutput(bOutFile, 1);

    fclose(outFile);
}

/* ============ sample adaptive: streaming encode and decode functions ================== */
void adaptive_encode_mymatrix(mymatrix *mat, char *filename){
    int nrows = mat->nrows;
    int ncols = mat->ncols;

    // setup encoding parameters

    // setup bitfile
    FILE *outFile;
    outFile = fopen(filename, "wb");

    bit_file_t *bOutFile; /* encoded output */
    bOutFile = MakeBitFile(outFile, BF_WRITE);

    // variables for normal encoder
    int i, j;
    int t = 0;  // sample index, using `t` to match CCSDS123 documentation
    int bit = 0;
    int bit_counter = 0;
    unsigned int num_bits_used = D;
    uint32_t sample, code;

    // variables for sample adaptive
    int k = 0;  // k=0 for the first sample t=0
    int counter =  pow(2, gamma_o);
    int accum = ((3 * pow(2, gamma_o+6) - 49) * counter) / (pow(2,7));
    int limit = 0;

    // debugging
    int limit_original = 0;     // copy of right side of equality
    int left_side = 0;          // left side of equality for k

    for (i = 0; i < nrows; i++) {
        for (j = 0; j < ncols; j++) {
            t = (i * mat->ncols) + j;
            sample = mat_get_flat(mat, t);

            // for the first pixel, don't do the adaptive stuff
            // just write the sample directly
            code = sample;
            if (t != 0){
                // ========= update sample adaptive variables =======
                if (counter < pow(2, gamma_star) -1){
                    counter = counter + 1;
                    accum = accum + sample;
                }
                // rescaling update
                else{
                    counter = (counter+1)/2;
                    accum = (accum + sample + 1)/2;
                }

                // ====== compute value of k ========
                limit = accum + (49 * counter) / pow(2,7);
                limit_original = limit;
                k = 0;
                while (counter <= limit){
                    k = k+1;
                    limit = limit >> 1; // shift right is divide by 2
                }
                k = k-1;    // went over the limit to exit the while loop, need to subtract 1
                left_side = counter*pow(2,k);
                assert(left_side<= limit_original);

                // =========== encode ==========
                code = encode_sample_optimized(sample, k, &num_bits_used);
            }
            

            // ========== write code to bitfile ==========
            while (bit_counter < num_bits_used) {
                // write left-most bit first
                // 0b1101 0010
                //   ^ write this one first, then write the ones to the right
                bit = (code >> (num_bits_used - 1 - bit_counter)) & 0x1;
                BitFilePutBit(bit, bOutFile);

                bit_counter++;
            }
            bit_counter = 0;

            // ====== debugging =========
            logger("DEBUG", "(%d, %d): sample=%3d | code=%-8x | num_bits_used=%3d\n", i, j, sample, code, num_bits_used);
            print_binary_32(code);
            logger("DEBUG", "counter=%3d \t accum=%3d \t k=%3d \t left_side=%3d \t limit=%3d\n",counter, accum, k, left_side, limit_original);
        }
    }

    /* pad fill with 1s so decode will run into EOF */
    BitFileFlushOutput(bOutFile, 1);

    fclose(outFile);
}


mymatrix *adaptive_decode_bitfile(char *filename){
    
}





/* =============== checks ============== */

/**
 * @brief checks one encode operation
 * 
 */
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


/**
 * @brief checks 1 decode operation
 * 
 */
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


/**
 * @brief encode, write to file, read from file, decode
 * 
 */
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


/**
 * @brief read the encoded contents from a file using bitfile, and decode them
 * 
 * @param filename          encoded binary file
 * @param k                 divisor control parameter M = 2^k
 * @param decoded_array     decoded array put into unsigned 32-bit array for easy access
 * @param decoded_size      input, number of samples is known a priori (from header file)
 */
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


/* ================= helpers ================== */

/**
 * @brief read binary file and print contents one-by-one
 * 
 * @param filename 
 */
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


/**
 * @brief read binary file using bitfile library, and print contents one-by-one
 * 
 * @param filename 
 */
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
