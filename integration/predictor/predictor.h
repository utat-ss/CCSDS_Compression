#ifndef PREDICTOR_H
#define PREDICTOR_H

#include "mymatrix.h"


/**
 * @brief neighbour oriented local sum
 * 
 * @param x 
 * @param y 
 * @param mat 
 * @return float 
 */
float local_sum (mymatrix* mat, int x, int y);

#endif /* PREDICTOR_H */