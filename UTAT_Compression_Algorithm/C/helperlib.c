/*
This file implements helper functions used in the compression and 
decompression steps.
*/
#include <math.h>
#include <stdlib.h>

//Returns the sign of an integer, with 0 returned as a positive
int sign(int x) {
    if (x >= 0) return 1;
    else return -1;
}


//Type definition for a binary array
typedef struct {
    int* array;
    int size;

} binArray;

binArray* binArray_init(int size)
{
    if(size < 1)
    {
        size = 1;
    }

    binArray* array = malloc(sizeof(binArray));
    array->array = calloc(size, sizeof(int));
    array->size  = size;

    return array;
}

int binArray_free(binArray* bin){
    free(bin->array);
    free(bin);

    return 0;
}


//Returns a binary array from a decimal
binArray* dec_to_bin(int num, int width){
    binArray* bin = binArray_init(width);

    int i = 0;
    while (i < width){
        
        bin->array[i] = num % 2;
        
        num = num / 2;
        
        i++;
    }
    
    return bin;
}

//Returns a decimal converted from a binary array
int bin_to_dec(binArray* bin) {
    int dec = 0;
    
    for (int i = 0; i < bin->size; i++){
        dec += pow(2, i) * bin->array[i];
    }

    return dec;
}

