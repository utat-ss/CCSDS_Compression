#include <stdlib.h>

#include "compression.h"
#include "unitTests.h"
#include "dataCube.h"

int main(int argc, char* argv[]){
    double data = 16;
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

    return 0;
}
