#ifndef COMPRESSION
#define COMPRESSION

#include <gsl/gsl_matrix.h>
#include <gsl/gsl_vector.h>
#include <gsl/gsl_blas.h>
#include <math.h>

#include "helperlib.h"

double localSum(int i, int j, int numRow, int numCol, gsl_matrix *data);
gsl_vector *localDifference(int i, int j, int k, gsl_matrix **data, double localSum, int numCol, int numFrame);
gsl_vector *weightInitialization(int k, int numFrame);
double predictionCalculation(gsl_vector *local_d, gsl_vector *weight, int localSum, int t, int data, double *drSampleValue, double *predSampleValue);
gsl_vector *weightUpdate(double drSampleValue, double predSample, double predResidual, int t, int k, int numCol, int numFrame, gsl_vector *weightVectorPrev, gsl_vector *ldVector);
unsigned int mapper(double predSamp, double predResidual, double drSamp, int t);

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