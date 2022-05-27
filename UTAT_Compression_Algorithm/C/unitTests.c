#include "unitTests.h"

// Calculates the predicted value at a pixel t given a localSum, localDifference vector, and dataPoint
void testPredictionCalculation(double data, double localSum, int t, int z, int Nz, gsl_vector *local_d)
{
    printf("Local difference vector is initialized to: \n");
    for (int i = 0; i < local_d->size; i++)
    {
        printf("%.2lf ", local_d->data[i]);
    }

    printf("\n");
    gsl_vector *weight = gsl_vector_alloc(number_of_bands + 3);
    weightInitialization(weight, z, Nz);
    printf("Initializing weights to their top of band value\n\n");
    double dr_samp = 0;
    double pred_samp = 0;
    double pred_resid = 0;
    predictionCalculation(local_d, weight, localSum, t, data, &dr_samp, &pred_samp, &pred_resid);

    printf("After prediction, predicted residual calculated as: %.2lf\n", pred_resid);
    printf("Predicted sample calculated as %.2lf\n", pred_samp);
    printf("Double resolution sample calculated as %.2lf\n", dr_samp);

    return;
}

// Calculates and displays a weight update given input parameters
void testWeightUpdate(double dr_sample_value, double pred_sample, double pred_residual, int t, int z, int Nx, int Nz, gsl_vector *weight_prev, gsl_vector *local_d)
{
    printf("Updating weights with dr_sample_value, pred_sample, and pred_residual set to : %.2lf %.2lf %.2lf\n", dr_sample_value, pred_sample, pred_residual);

    printf("Previous weight vector set to: ");
    for (int i = 0; i < weight_prev->size; i++)
    {
        printf("%.2lf ", weight_prev->data[i]);
    }
    printf("\n\n");

    gsl_vector *weight_new = gsl_vector_alloc(number_of_bands + 3);
    weightUpdate(dr_sample_value, pred_sample, pred_residual, t, z, Nx, Nz, weight_prev, local_d, weight_new);
    printf("New weight vector set to: ");
    for (int i = 0; i < weight_new->size; i++)
    {
        printf("%.2lf ", weight_new->data[i]);
    }
    printf("\n");
}
