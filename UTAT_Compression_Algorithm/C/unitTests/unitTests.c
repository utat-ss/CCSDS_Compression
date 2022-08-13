#include "unitTests.h"

// Calculates the predicted value at a pixel t given a localSum, localDifference vector, and dataPoint
void testPredictionCalculation(double data, double localSum, int t, int k, int numFrame, gsl_vector *local_d)
{
    printf("Local difference vector is initialized to: \n");
    for (int i = 0; i < local_d->size; i++)
    {
        printf("%.2lf ", local_d->data[i]);
    }

    printf("\n");
    gsl_vector *weight = gsl_vector_alloc(number_of_bands + 3);
    weight = weightInitialization(k, numFrame);
    printf("Initializing weights to their top of band value\n\n");
    double drSamp = 0;
    double predSamp = 0;
    double predResidual = predictionCalculation(local_d, weight, localSum, t, data, &drSamp, &predSamp);

    printf("After prediction, predicted residual calculated as: %.2lf\n", predResidual);
    printf("Predicted sample calculated as %.2lf\n", predSamp);
    printf("Double resolution sample calculated as %.2lf\n", drSamp);

    return;
}

// Calculates and displays a weight update given input parameters
void testWeightUpdate(double drSampleValue, double predSample, double predResidual, int t, int k, int numCol, int numFrame, gsl_vector *weightPrev, gsl_vector *local_d)
{
    printf("Updating weights with dr_sample_value, pred_sample, and pred_residual set to : %.2lf %.2lf %.2lf\n", drSampleValue, predSample, predResidual);

    printf("Previous weight vector set to: ");
    for (int i = 0; i < weightPrev->size; i++)
    {
        printf("%.2lf ", weightPrev->data[i]);
    }
    printf("\n\n");

    gsl_vector *weightNew = weightUpdate(drSampleValue, predSample, predResidual, t, k, numCol, numFrame, weightPrev, local_d);

    printf("New weight vector set to: ");
    for (int i = 0; i < weightNew->size; i++)
    {
        printf("%.2lf ", weightNew->data[i]);
    }
    printf("\n");
}
