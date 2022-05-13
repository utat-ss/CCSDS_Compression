#ifndef COMPRESSION
#define COMPRESSION

#include <gsl/gsl_matrix.h>
#include <gsl/gsl_vector.h>
#include <gsl/gsl_blas.h>
#include <math.h>

#include "helperlib.h"

int predictionCalculation(gsl_vector* local_d, gsl_vector* weight, int local_sum, int t, int data, int* dr_sample_value, int* pred_sample_value);
int localSum(int x, int y, int Nx, gsl_matrix* data);

#endif