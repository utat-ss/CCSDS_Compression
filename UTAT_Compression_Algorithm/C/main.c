#include <stdlib.h>

#include <gsl/gsl_vector.h>
#include <gsl/gsl_matrix.h>

#include "compression.h"



int main(){
    gsl_vector* local = gsl_vector_calloc(5);
    gsl_vector* weights = gsl_vector_calloc(5);
    gsl_vector_set_all(local, 5);
    gsl_vector_set_all(weights, 3);
    int t = 3;
    double local_sum = 13;
    double data = 16.0;
    double dr_sample_value = 0;
    double pred_sample_value = 0;
    double pred_residual = predictionCalculation(local, weights, local_sum, t, data, &dr_sample_value, &pred_sample_value);

    return 0;
}
