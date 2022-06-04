/**
 * Yong Da Li
 * Saturday, June 4, 2022
 * 
 * section 5.4.3.2.2 Length-Limited Golomb-Power-of-2 Codeword of CCSDS123 standard
 * encoder
 */

#include <stdio.h>
#include <stdlib.h>

#include "main.h"
#include "encoder.h"

void check_single_encode(void){
    unsigned int sample = 7;
    unsigned int k = 2;
    unsigned int encoded = encode_sample(sample, k);

    printf("===== parameters =====\n");
    printf("Golomb power of 2 [k]: %u\n", k);
    printf("divisor [M]: %u\n", (unsigned int) pow(2,k));


    printf("===== encoding =====\n");
    printf("sample: decimal = %u, \tbin = "BYTE_TO_BINARY_PATTERN", \thex = %x\n", sample, BYTE_TO_BINARY(sample), sample);
    printf("encoded: \t\tbin = "BYTE_TO_BINARY_PATTERN", \thex = %x\n", BYTE_TO_BINARY(encoded), encoded);
}

// TO DO
void check_single_decode(void){

}

int main(void){
    check_single_encode();
    
    return 0;
}