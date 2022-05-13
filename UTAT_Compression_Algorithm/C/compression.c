#include "compression.h"

#define dynamic_range 16
#define weight_resolution 4
#define s_mid 0
#define register_size 32

/*
Calculates the local sum at a given x,y point in the dataset
Input: x-coordinate, y-coordinate, total number of x-coordinates, and the band of the dataset to calculate the local sum
*/ 
int localSum(int x, int y, int Nx, gsl_matrix* data){
    int localSum = 0;
    if (y == 0 && x > 0){
        localSum = 4*gsl_matrix_get(data,y,x-1);
    }
    else if(y > 0){
        if (x == 0){
            localSum = 2*(gsl_matrix_get(data,y-1,x) + gsl_matrix_get(data,y-1,x+1));
        }

        else if (x == Nx-1){
            localSum = gsl_matrix_get(data,y,x-1) + gsl_matrix_get(data,y-1,x-1) + 2*gsl_matrix_get(data,y-1,x);
        }

        else{
            localSum = gsl_matrix_get(data,y,x-1) + gsl_matrix_get(data,y-1,x-1) + gsl_matrix_get(data,y-1,x) + gsl_matrix_get(data,y-1,x+1);
        }
    }
    return localSum;
}

/*Calculates a predicted residual from the generated local difference and weight vectors.
Inputs: local difference vector, weight vector, calculated local sum, t (position in dataset), data value at position
Also modifies dr sample value and predicted sample value for weight update
*/
int predictionCalculation(gsl_vector* local_d,gsl_vector* weight, int local_sum, int t, int data, int* dr_sample_value, int* pred_sample_value){
    int s_min = (-1*(pow(2,dynamic_range-1)));
    int s_max = pow(2, dynamic_range - 1);

    double pred_difference = 0;

    gsl_blas_ddot(local_d, weight, &pred_difference);

    //Compute the hr predicted sample value (Equation 37)
    int tempOne = pred_difference + pow(2,weight_resolution)*(local_sum-4*s_mid);
    int tempTwo = fmod((tempOne+ pow(2,register_size - 1)), pow(2, register_size)) - pow(2, register_size - 1);
    int tempThree = tempTwo + pow(2,(weight_resolution+2))*s_mid + pow(2,(weight_resolution+1));
    
    int minimum = pow(2,(weight_resolution +2))*s_min;
    int maximum = pow(2,(weight_resolution +2))*s_max + pow(2,(weight_resolution+1));
    
    int hr_pred_sample_value = clamp(tempThree, minimum, maximum);
    
    //Modify dr and pred sample value
    if (t==0){
        *dr_sample_value = 2*s_mid;
    }
    
    else {
        *dr_sample_value = floor(hr_pred_sample_value/(pow(2,weight_resolution+1)));
    }

    *pred_sample_value = floor(*dr_sample_value/2);

    //Calculate predicted residual
    int pred_residual = data - *pred_sample_value;

    return pred_residual;
}
