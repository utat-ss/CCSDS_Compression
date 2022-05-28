#ifndef UNITTESTS
#define UNITTESTS

#include <gsl/gsl_vector.h>
#include "dataCube.h"
#include "compression.h"

void testPredictionCalculation(double data, double localSum, int t, int z, int Nz, gsl_vector *local_d);
void testWeightUpdate(double dr_sample_value, double pred_sample, double pred_residual, int t, int z, int Nx, int Nz, gsl_vector *weight_prev, gsl_vector *local_d);

#endif