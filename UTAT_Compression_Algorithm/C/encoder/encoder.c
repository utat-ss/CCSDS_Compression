/**
 * Yong Da Li
 * Saturday, June 4, 2022
 * 
 * section 5.4.3.2.2 Length-Limited Golomb-Power-of-2 Codeword of CCSDS123 standard
 * encoder
 */

#include "encoder.h"

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

