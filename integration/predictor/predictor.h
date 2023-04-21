#ifndef PREDICTOR_H
#define PREDICTOR_H

#include "mymatrix.h"
#include "datacube.h"

float local_sum (mymatrix* mat, int x, int y);
myvector* compute_local_diff_vector(datacube* datacube, int x, int y, int z);

#endif /* PREDICTOR_H */