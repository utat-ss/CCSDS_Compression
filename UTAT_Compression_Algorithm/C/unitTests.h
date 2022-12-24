#ifndef UNITTESTS
#define UNITTESTS

#include <gsl/gsl_vector.h>
#include "compression.h"

void testPredictionCalculation(double data, double localSum, int t, int z, int Nz, gsl_vector *local_d);
void testWeightUpdate(double dr_sample_value, double pred_sample, double pred_residual, int t, int z, int Nx, int Nz, gsl_vector *weight_prev, gsl_vector *local_d);
void test_dec_to_bin(int d, uint8_t width);
void test_gsl_vector_append(gsl_vector* base, gsl_vector* addition);

#endif