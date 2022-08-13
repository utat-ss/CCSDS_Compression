#include <stdlib.h>
#include <stdio.h>

#include "predictor/predict.h"
#include "parser/parse.h"
#include "unitTests/unitTests.h"

int main(int argc, char *argv[])
{
    printf("==== start compression ====\n");
    printf("Program name %s\n", argv[0]);

    if (argc == 2)
    {
        printf("The first argument is: %s\n", argv[1]);
    }
    else
    {
        printf("Please give 1 argument.\n");
    }

    int numRow, numCol, numFrame;
    gsl_matrix **dataCube;

    dataCube = parse(argv[1], &numRow, &numCol, &numFrame);
    gsl_matrix_uint *mappedVal = gsl_matrix_uint_alloc(1, numRow * numCol);

    for (int k = 0; k < numFrame; k++)
    {
        for (int i = 0; i < numRow; i++)
        {
            gsl_vector *weight = gsl_vector_alloc(number_of_bands + 3);
            gsl_matrix *curr = dataCube[k];

            for (int j = 0; j < numCol; j++)
            {
                int t = i * numCol + j;

                int data = gsl_matrix_get(curr, 0, t);

                double local = localSum(i, j, numRow, numCol, curr);

                gsl_vector *local_d = localDifference(i, j, k, dataCube, local, numCol, numFrame);

                if (t == 0)
                {
                    weight = weightInitialization(k, numFrame);
                }

                double drSamp = 0;
                double predSamp = 0;
                double predResidual = predictionCalculation(local_d, weight, local, t, data, &drSamp, &predSamp);

                unsigned int mapped = mapper(predSamp, predResidual, drSamp, t);
                if (k == 0)
                {
                    gsl_matrix_uint_set(mappedVal, 0, t, mapped);
                }

                weight = weightUpdate(drSamp, predSamp, predResidual, t, k, numCol, numFrame, weight, local_d);
            }
        }
    }

    for (int i = 0; i < numRow; i++)
    {
        for (int j = 0; j < numCol; j++)
        {
            printf("%i ", gsl_matrix_uint_get(mappedVal, 0, i * numCol + j));
        }
        printf("\n");
    }

    return 0;

    /* double data = 16;
    int t = 3;
    int localSum = 5;
    int z = 3;
    int Nz = 4;
    int Nx = 4;
    gsl_vector *local_d = gsl_vector_alloc(number_of_bands + 3);
    gsl_vector_set_all(local_d, 3);
    gsl_vector *weight = gsl_vector_alloc(number_of_bands + 3);
    gsl_vector_set_all(weight, 3);

    testPredictionCalculation(data, localSum, t, z, Nz, local_d);
    testWeightUpdate(3, 1, 15, t, z, Nx, Nz, weight, local_d);
    test_dec_to_bin(53, 8);
    */

    //test_gsl_vector_append
    gsl_vector* base = gsl_vector_alloc(5);
    gsl_vector* addition = gsl_vector_alloc(3);
    for(int i = 0; i < 5; ++i){
        gsl_vector_set(base, i, i + 1);
    }
    for(int i = 0; i < 3; ++i){
        gsl_vector_set(addition, i, i + 6);
    }
    test_gsl_vector_append(base, addition);

    return 0;
}
