#include <stdlib.h>

#include "compression.h"

int main()
{

    // Unit test for Prediction Calculation
    printf("Creating a local difference vector of size 5 with all values equal to 5\n");
    printf("Creating a weights vector of size 5 with all values equal to 3\n ");
    gsl_vector *local = gsl_vector_calloc(5);
    gsl_vector *weights = gsl_vector_calloc(5);
    gsl_vector_set_all(local, 5);
    gsl_vector_set_all(weights, 3);
    printf("Setting t = 3, Local Sum = 13, Data = 16\n");
    int t = 3;
    int local_sum = 13;
    int data = 16.0;
    int dr_sample_value = 0;
    int pred_sample_value = 0;
    int pred_residual = predictionCalculation(local, weights, local_sum, t, data, &dr_sample_value, &pred_sample_value);
    printf("Calculated Predicted residual: %d\n", pred_residual);
    printf("Calculated double resolution and predicted sample value: %d %d\n", dr_sample_value, pred_sample_value);
    return 0;
}
