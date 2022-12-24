#include "compression.h"

/*
Calculates the local sum at a given x,y point in the dataset
Input: x-coordinate, y-coordinate, total number of x-coordinates, and the band of the dataset to calculate the local sum
*/
void localSum(int x, int y, int Nx, gsl_matrix *data, double *local_sum){
    *local_sum = 0;
    if (y == 0 && x > 0){
        *local_sum = 4 * gsl_matrix_get(data, y, x - 1);
    }
    else if (y > 0){
        if (x == 0){
            *local_sum = 2 * (gsl_matrix_get(data, y - 1, x) + gsl_matrix_get(data, y - 1, x + 1));
        }

        else if (x == Nx - 1){
            *local_sum = gsl_matrix_get(data, y, x - 1) + gsl_matrix_get(data, y - 1, x - 1) + 2 * gsl_matrix_get(data, y - 1, x);
        }

        else{
            *local_sum = gsl_matrix_get(data, y, x - 1) + gsl_matrix_get(data, y - 1, x - 1) + gsl_matrix_get(data, y - 1, x) + gsl_matrix_get(data, y - 1, x + 1);
        }
    }
}

/*Calculates a vector of local differences at a specified pixel
Inputs: x,y,z coordinates, entire data cube, calculated local sum, total number of bands
Output: modified gsl_vector, which is assumed to be properly allocated in advance
*/
void localDifference(int x, int y, int z, dataCube *dataCube, double local_sum, gsl_vector *ld_vector, int Nz){
    // Calculated the differences between the surrounding three pixels
    double north_ld = 0;
    double west_ld = 0;
    double northwest_ld = 0;

    if (x == 0){
        north_ld = 4 * gsl_matrix_get(&(dataCube->data[z]->matrix), x, y - 1) - local_sum;
        west_ld = north_ld;
        northwest_ld = north_ld;
    }
    else if (y != 0){
        north_ld = 4 * gsl_matrix_get(&(dataCube->data[z]->matrix), x, y - 1) - local_sum;
        west_ld = 4 * gsl_matrix_get(&(dataCube->data[z]->matrix), x - 1, y) - local_sum;
        northwest_ld = 4 * gsl_matrix_get(&(dataCube->data[z]->matrix), x - 1, y - 1) - local_sum;
    }

    gsl_vector_set(ld_vector, 0, north_ld);
    gsl_vector_set(ld_vector, 1, west_ld);
    gsl_vector_set(ld_vector, 2, northwest_ld);

    // Calculate the difference at neighboring bands
    for (int band = 1; band < number_of_bands; band++){
        double central_ld = 0;
        if (z + band < Nz){ // make sure we're not going out of bounds
            central_ld = 4 * gsl_matrix_get(&(dataCube->data[z + band]->matrix), x, y) - local_sum;
        }
        gsl_vector_set(ld_vector, band + 2, central_ld);
    }
}

/*Initializes the weight vector at the beginning of each band to predetermined values
Inputs: current band, number of bands
Output: modified weight vector, which is assumed to be properly allocated
*/
void weightInitialization(gsl_vector *weight, int z, int Nz){
    // Surrounding vectors at the same band are set to 0
    gsl_vector_set(weight, 0, 0);
    gsl_vector_set(weight, 1, 0);
    gsl_vector_set(weight, 2, 0);

    double weight_one = 0;
    if (z != Nz - 1 && number_of_bands != 0){
        weight_one = (7 / 8) * (pow(2, weight_resolution));
    }
    gsl_vector_set(weight, 3, weight_one);

    // Subsequent weight vectors are dependednt on the previous value
    for (int band = 2; band < number_of_bands; band++){
        double weight_i = 0;
        if (z + band < Nz){
            weight_i = floor((1 / 8) * gsl_vector_get(weight, band + 1));
        }
        gsl_vector_set(weight, band + 2, weight_i);
    }
}

/*Calculates a predicted residual from the generated local difference and weight vectors.
Inputs: local difference vector, weight vector, calculated local sum, t (position in dataset), data value at position
Output: returns predicted residual, and also modifies dr sample value and predicted sample value for weight update
*/
//Yup
void predictionCalculation(gsl_vector *local_d, gsl_vector *weight, int local_sum, int t, int data, double *dr_sample_value, double *pred_sample_value,
                           double *pred_residual){
    int s_min = (-1 * (pow(2, dynamic_range - 1)));
    int s_max = pow(2, dynamic_range - 1);

    double pred_difference = 0;

    gsl_blas_ddot(local_d, weight, &pred_difference);

    // Compute the hr predicted sample value (Equation 37)
    int tempOne = pred_difference + pow(2, weight_resolution) * (local_sum - 4 * s_mid);
    int tempTwo = fmod((tempOne + pow(2, register_size - 1)), pow(2, register_size)) - pow(2, register_size - 1);
    int tempThree = tempTwo + pow(2, (weight_resolution + 2)) * s_mid + pow(2, (weight_resolution + 1));

    int minimum = pow(2, (weight_resolution + 2)) * s_min;
    int maximum = pow(2, (weight_resolution + 2)) * s_max + pow(2, (weight_resolution + 1));

    int hr_pred_sample_value = clamp(tempThree, minimum, maximum);

    // Modify dr and pred sample value
    if (t == 0){
        *dr_sample_value = 2 * s_mid;
    }

    else{
        *dr_sample_value = floor(hr_pred_sample_value / (pow(2, weight_resolution + 1)));
    }

    *pred_sample_value = floor(*dr_sample_value / 2);

    // Calculate predicted residual
    *pred_residual = data - *pred_sample_value;
}

/*Updates the weight vector for a specified band before moving onto the next pixel
Inputs: double resolution sample value, predicted sample value, predicted residual, t, z, number of x pixels,
        number of z pixels, and the previous local difference and weight vectors
Output: new weight vector, which is assumed to properly allocated beforehand.
        The function also frees the previous weight vectors and local difference vectors from memory
*/
//Yup
void weightUpdate(double dr_sample_value, double pred_sample, double pred_residual, int t, int z, int Nx, int Nz,
                  gsl_vector *weight_vector_prev, gsl_vector *local_d, gsl_vector *weight){

    double s_min = (-1 * (pow(2, dynamic_range - 1)));
    double s_max = pow(2, dynamic_range - 1);

    double w_min = -pow(2, (weight_resolution + 1));
    double w_max = pow(2, (weight_resolution + 2)) - 1;

    // Calculate prediction error
    double clipped_quant = clamp(pred_sample + (pred_residual * (2 * max_error + 1)), s_min, s_max);
    double prediction_error = 2 * clipped_quant - dr_sample_value;

    // Calculate weight update scaling exponent
    double temp = v_min + floor((t - Nx) / t_inc);
    double weight_exponent = clamp(temp, v_min, v_max) + dynamic_range - weight_resolution;

    double base = sign(prediction_error) * (pow(2, -(weight_exponent + intraband)));

    // Set north, west, and northwest vectors based on previous
    double north = gsl_vector_get(weight_vector_prev, 0) + floor((1 / 2) * (base * gsl_vector_get(local_d, 0) + 1));
    double west = gsl_vector_get(weight_vector_prev, 1) + floor((1 / 2) * (base * gsl_vector_get(local_d, 1) + 1));
    double northwest = gsl_vector_get(weight_vector_prev, 2) + floor((1 / 2) * (base * gsl_vector_get(local_d, 2) + 1));

    north = clamp(north, w_min, w_max);
    west = clamp(west, w_min, w_max);
    northwest = clamp(northwest, w_min, w_max);

    gsl_vector_set(weight, 0, north);
    gsl_vector_set(weight, 1, west);
    gsl_vector_set(weight, 2, northwest);

    // Set remaining points in vector
    for (int band = 1; band < number_of_bands; band++){
        double weight_i = 0;

        double base_two = sign(prediction_error) * (pow(2, -(weight_exponent + interband)));

        if (z + band < Nz){
            weight_i = gsl_vector_get(weight_vector_prev, band + 2) + floor((1 / 2) * (base_two * gsl_vector_get(local_d, band + 2) + 1));
            weight_i = clamp(weight_i, w_min, w_max);
        }

        gsl_vector_set(weight, band + 2, weight_i);
    }

    // Free the vectors for the previous pixel, no longer needed
    gsl_vector_free(weight_vector_prev);
    gsl_vector_free(local_d);
}

/*Maps predicted residuals to unsigned integers: final compression step
Input: predicted sample value, predicted residual value, double resolution sample value, t

Output: mapped value
*/
void mapper(double pred_samp, double pred_residual, double dr_samp, int t, unsigned int *mapped){
    double theta = 0;
    double s_min = (-1 * (pow(2, dynamic_range - 1)));
    double s_max = pow(2, dynamic_range - 1);

    // Calculate theta
    if (t == 0){
        theta = fmin(pred_samp - s_min, s_max - pred_samp);
    }
    else{
        theta = fmin(floor((pred_samp - s_min + max_error) / (2 * max_error + 1)), floor((s_max - pred_samp + max_error) / (2 * max_error + 1)));
    }

    // Determine mapped value
    if (abs(pred_residual > theta)){
        *mapped = (unsigned int)abs(pred_residual) + theta;
    }
    else if (((int)dr_samp % 2 == 0 && pred_residual >= 0) || ((int)dr_samp % 2 != 0 && pred_residual <= 0)){
        *mapped = (unsigned int)2 * abs(pred_residual);
    }
    else{
        *mapped = (unsigned int)2 * abs(pred_residual) - 1;
    }
}

void encoder(double*** delta, int Nx, int Ny, int Nz, uint8_t initial_count_exp, uint8_t k_zprime, uint8_t u_max)
{
    gsl_vector* encoded = gsl_vector_alloc(0);
    for(int z = 0; z < Nz; ++z)
    {
        //Set initial counter and accumulator values for the band
        uint8_t counter = 1 << initial_count_exp;
        int accum_value = (int) ((1 / (1 << 7))*(3 * (2 << (k_zprime + 6)) - 49)*counter); //Equaiton 58
        for(int y = 0; y < Ny; ++y)
        {
            for(int x = 0; x < Nx; ++x)
            {
                int t = y * Nx + x;
                //At the first pixel, the endoced value is just the D-bit representation of delta
                if(t == 0)
                {
                    gsl_vector* code = dec_to_bin(delta[z][y][x], dynamic_range);
                    gsl_vector_append(encoded, code);
                }
                else
                {
                    //Using the adaptive code statistics, set the code parameter, according to equation 62 in section 5.4.3.2.4
                    int condition = accum_value + (int) ((49 / (1 << 7)) * counter);
                    uint8_t code_param = 0;
                    if(2*counter > condition)
                        code_param = 0;
                    else
                        for(int i = dynamic_range; i > 0; --i)
                        {
                            if(counter * (1 << i) <= condition)
                            {
                                code_param = i;
                                break;
                            }
                                
                        }
                    //Use golomb power of two code words to write a binary codeword, based on the user-defined unary length limit
                    int unary_length_limit = (int)(delta[z][y][x] / (1 << code_param));
                    if(unary_length_limit < u_max)
                    {
                        //Write unary code
                        gsl_vector* u = gsl_vector_calloc(unary_length_limit);
                        gsl_vector_add_constant(u, 1);
                        gsl_vector* one = gsl_vector_calloc(1);
                        gsl_vector_append(u, one);

                        //Write remainder code

                    }
                }
            }
        }
    }
}

//Notes
/*
Replace all ints with uint32_t and stuff. Make all variable types explicit in terms of bit size.
*/