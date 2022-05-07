#include <gsl/gsl_matrix.h>
#include <stdlib.h>

/*
Calculates the local sum at a given x,y point in the dataset
Input: x-coordinate, y-coordinate, total number of x-coordinates, and the band of the dataset to calculate the local sum
*/ 
double localSum(int x, int y, int Nx, gsl_matrix* data){
    double localSum = 0;
    if (y == 0 && x > 0){
        localSum = 4*gsl_matrix_get(data,y,x-1);
    }
    else if(y > 0){
        if (x == 0)
            localSum = 2*(gsl_matrix_get(data,y-1,x) + gsl_matrix_get(data,y-1,x+1));
        else if (x == Nx-1)
            localSum = gsl_matrix_get(data,y,x-1) + gsl_matrix_get(data,y-1,x-1) + 2*gsl_matrix_get(data,y-1,x);

        else
            localSum = gsl_matrix_get(data,y,x-1) + gsl_matrix_get(data,y-1,x-1) + gsl_matrix_get(data,y-1,x) + gsl_matrix_get(data,y-1,x+1);
    }
    return localSum;
}

