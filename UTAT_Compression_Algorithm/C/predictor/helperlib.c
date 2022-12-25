/*
This file implements helper functions used in the compression and
decompression steps.
*/
#include "helperlib.h"

/**
 * @brief return sign of an integer, with 0 returned as a positive
 * 
 * @param x 
 * @return int 
 */
int sign(double x){
    if (x >= 0){
        return 1;
    }
    else{
        return -1;
    }
}

/**
 * @brief maps x in-between min and max values
 * 
 * @param x 
 * @param min 
 * @param max 
 * @return double 
 */
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
