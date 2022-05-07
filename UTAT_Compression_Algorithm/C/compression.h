#ifndef COMPRESSION
#define COMPRESSION

double predictionCalculation(gsl_vector* local_d, gsl_vector* weight, double local_sum, int t, double data, double* dr_sample_value, double* pred_sample_value);
double localSum(int x, int y, int Nx, gsl_matrix* data);

#endif