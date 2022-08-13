#ifndef UNITTESTS
#define UNITTESTS

#include <gsl/gsl_vector.h>
#include "../predictor/predict.h"

void testPredictionCalculation(double data, double localSum, int t, int z, int Nz, gsl_vector *local_d);
void testWeightUpdate(double drSampleValue, double predSample, double predResidual, int t, int k, int numCol, int numFrame, gsl_vector *weightPrev, gsl_vector *local_d);

#endif