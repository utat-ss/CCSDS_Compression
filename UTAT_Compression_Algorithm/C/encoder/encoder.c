/**
 * Yong Da Li
 * Saturday, June 4, 2022
 * 
 * section 5.4.3.2.2 Length-Limited Golomb-Power-of-2 Codeword of CCSDS123 standard
 * encoder
 */

#include <stdint.h>
#include "encoder.h"
#include <math.h>


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
