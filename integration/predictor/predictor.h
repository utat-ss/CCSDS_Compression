#ifndef PREDICTOR_H
#define PREDICTOR_H

#include "datacube.h"
#include "mymatrix.h"

float local_sum(mymatrix *mat, int x, int y);
myvector *compute_local_diff_vector(datacube *datacube, int z, int x, int y);

myvector *initialize_weight_vector();

float predict_calc(datacube *datacube, int z, int x, int y);

#endif /* PREDICTOR_H */