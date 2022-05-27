#ifndef COMPRESSION
#define COMPRESSION

#include <gsl/gsl_matrix.h>
#include <gsl/gsl_vector.h>
#include <gsl/gsl_blas.h>
#include <math.h>

#include "helperlib.h"
#include "dataCube.h"

void localSum(int x, int y, int Nx, gsl_matrix *data, double *local_sum);
void localDifference(int x, int y, int z, dataCube *data, double local_sum, gsl_vector *ld_vector, int Nz);
void weightInitialization(gsl_vector *weight, int z, int Nz);
void predictionCalculation(gsl_vector *local_d, gsl_vector *weight, int local_sum, int t, int data, double *dr_sample_value, double *pred_sample_value, double *pred_residual);
void weightUpdate(double dr_sample_value, double pred_sample, double pred_residual, int t, int z, int Nx, int Nz, gsl_vector *weight_vector_prev, gsl_vector *local_d, gsl_vector *weight);

// Local parameters for compression
#define dynamic_range 16
#define weight_resolution 4
#define s_mid 0
#define register_size 32
#define number_of_bands 2
#define max_error 0
#define v_min -6
#define v_max 9
#define t_inc 16
#define intraband 1
#define interband 1

#endif