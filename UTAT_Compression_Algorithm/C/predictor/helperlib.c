/*
This file implements helper functions used in the compression and
decompression steps.
*/
#include "helperlib.h"

// Returns the sign of an integer, with 0 returned as a positive
int sign(double x){
    if (x >= 0){
        return 1;
    }
    else{
        return -1;
    }
}

double clamp(double x, double min, double max){
    double result = x;
    if (x < min){
        result = min;
    }
    else if (x > max){
        result = max;
    }

    return result;
}

gsl_vector* dec_to_bin(int d, uint8_t width)
{
    gsl_vector* b = gsl_vector_alloc(width);
    for(int8_t i = width - 1; i >= 0; --i)
    {
        gsl_vector_set(b, i, (d & 1));
        d = d >> 1;
    }
    return b;
}

void gsl_vector_append(gsl_vector* base, gsl_vector* addition)
{
    //Resize data element
    base->data = (double *) realloc(base->data, (base->size + addition->size) * sizeof(double));

    //Resize block element
    free(base->block);
    base->block = gsl_block_alloc(base->size + addition->size);
    base->block->data = base->data;
        
    //Append data
    memcpy(base->data + base->size, addition->data, addition->size * sizeof(double));
    
    //Update size element
    base->size += addition->size;
}