/*
This file implements helper functions used in the compression and 
decompression steps.
*/
#include <math.h>
#include <stdlib.h>
#include <gsl/gsl_vector_int.h>

//Returns the sign of an integer, with 0 returned as a positive
int sign(int x) {
    if (x >= 0) return 1;
    else return -1;
}

//Returns a binary array from a decimal
//Binary arrays are stored as GSL integer vectors
gsl_vector_int* dec_to_bin(int num, int width){
    gsl_vector_int* bin = gsl_vector_int_alloc(width);

    int i = 0;
    while (i < width){
        
        gsl_vector_int_set(bin, i, num %2);
        
        num = num / 2;
        
        i++;
    }
    
    return bin;
}

//Returns a decimal converted from a binary array
int bin_to_dec(gsl_vector_int* bin) {
    int dec = 0;
    
    for (int i = 0; i < bin->size; i++){
        dec += pow(2, i) * gsl_vector_int_get(bin, i);
    }

    return dec;
}

