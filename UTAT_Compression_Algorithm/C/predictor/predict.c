#include "predict.h"

/**
 * @brief Calculates the local sum at a given i,j point in the dataset
 *
 * @param i
 * @param j
 * @param numRow
 * @param numCol
 * @param data
 * @return localSum in a double
 */
double localSum(int i, int j, int numRow, int numCol, gsl_matrix *data)
{
    double localSum = 0;
    if (i == 0 && j > 0)
    {
        localSum = 4 * gsl_matrix_get(data, 0, j - 1);
    }
    else if (i > 0)
    {
        if (j == 0)
        {
            localSum = 2 * (gsl_matrix_get(data, 0, (i - 1) * numCol + j) + gsl_matrix_get(data, 0, (i - 1) * numCol + j + 1));
        }

        else if (j == numCol - 1)
        {
            localSum = gsl_matrix_get(data, 0, i * numCol + j - 1) + gsl_matrix_get(data, 0, (i - 1) * numCol + j - 1) + 2 * gsl_matrix_get(data, 0, (i - 1) * numCol + j);
        }

        else
        {
            localSum = gsl_matrix_get(data, 0, i * numCol + j - 1) + gsl_matrix_get(data, 0, (i - 1) * numCol + j - 1) + gsl_matrix_get(data, 0, (i - 1) * numCol + j) + gsl_matrix_get(data, 0, (i - 1) * numCol + j + 1);
        }
    }
    return localSum;
}

/**
 * @brief Calculates a vector of local differences at a specified pixel
 *
 * @param i
 * @param j
 * @param k
 * @param data complete data array
 * @param localSum
 * @param numCol
 * @param numFrame
 *
 * @return gsl_vector of local differences
 *
 */
gsl_vector *localDifference(int i, int j, int k, gsl_matrix **data, double localSum, int numCol, int numFrame)
{
    gsl_vector *ldVector = gsl_vector_alloc(number_of_bands + 3);

    // Calculated the differences between the surrounding three pixels
    double north_ld = 0;
    double west_ld = 0;
    double northwest_ld = 0;

    if (j == 0 && i != 0)
    {
        north_ld = 4 * gsl_matrix_get(data[k], 0, (i - 1) * numCol + j) - localSum;
        west_ld = north_ld;
        northwest_ld = north_ld;
    }
    else if (i != 0)
    {
        north_ld = 4 * gsl_matrix_get(data[k], 0, (i - 1) * numCol + j) - localSum;
        west_ld = 4 * gsl_matrix_get(data[k], 0, i * numCol + j - 1) - localSum;
        northwest_ld = 4 * gsl_matrix_get(data[k], 0, (i - 1) * numCol + j - 1) - localSum;
    }

    gsl_vector_set(ldVector, 0, north_ld);
    gsl_vector_set(ldVector, 1, west_ld);
    gsl_vector_set(ldVector, 2, northwest_ld);

    // Calculate the difference at neighboring bands
    for (int band = 1; band <= number_of_bands; band++)
    {
        double central_ld = 0;
        if (k + band < numFrame)
        { // make sure we're not going out of bounds
            central_ld = 4 * gsl_matrix_get(data[k + band], 0, i * numCol + j) - localSum;
        }
        gsl_vector_set(ldVector, band + 2, central_ld);
    }
    return ldVector;
}

/**
 * @brief Initializes the weight vector at the beginning of each band to predetermined values
 *
 * @param k
 * @param numFrame
 *
 * @return gsl_vector containing weight values
 */
gsl_vector *weightInitialization(int k, int numFrame)
{
    gsl_vector *weight = gsl_vector_alloc(number_of_bands + 3);
    // Surrounding vectors at the same band are set to 0
    gsl_vector_set(weight, 0, 0);
    gsl_vector_set(weight, 1, 0);
    gsl_vector_set(weight, 2, 0);

    double weight_one = 0;
    if (k != numFrame - 1 && number_of_bands != 0)
    {
        weight_one = (7 / 8) * (1 << weight_resolution);
    }
    gsl_vector_set(weight, 3, weight_one);

    // Subsequent weight vectors are dependent on the previous value
    for (int band = 2; band <= number_of_bands; band++)
    {
        double weight_i = 0;
        if (k + band < numFrame)
        {
            weight_i = floor((1 / 8) * gsl_vector_get(weight, band + 1));
        }
        gsl_vector_set(weight, band + 2, weight_i);
    }
    return weight;
}

/**
 * @brief Calculates a predicted residual from the generated local difference and weight vectors.
 *
 * @param local_d
 * @param weight
 * @param localSum
 * @param t
 * @param data at current t position (i,j,k)
 * @param drSampleValue pointer to variable for assignment
 * @param predSampleValue pointer to variable for assignment
 *
 * @return predicted residual at position i,j,k
 */
double predictionCalculation(gsl_vector *local_d, gsl_vector *weight, int localSum, int t, int data, double *drSampleValue, double *predSampleValue)
{
    int sMin = -1 * (1 << (dynamic_range - 1));
    int sMax = 1 << (dynamic_range - 1);

    double pred_difference = 0;

    gsl_blas_ddot(local_d, weight, &pred_difference);

    // Compute the hr predicted sample value (Equation 37)
    double tempOne = pred_difference + (1 << weight_resolution) * (localSum - 4 * s_mid);
    double tempTwo = fmod((tempOne + pow(2, register_size - 1)), pow(2, register_size)) - pow(2, register_size - 1);
    double tempThree = tempTwo + (1 << (weight_resolution + 2)) * s_mid + (1 << (weight_resolution + 1));

    double minimum = (1 << (weight_resolution + 2)) * sMin;
    double maximum = (1 << (weight_resolution + 2)) * sMax + (1 << (weight_resolution + 1));

    int hrPredSampleValue = clamp(tempThree, minimum, maximum);

    // Modify dr and pred sample value
    if (t == 0)
    {
        *drSampleValue = 2 * s_mid;
    }

    else
    {
        *drSampleValue = floor(hrPredSampleValue / (1 << (weight_resolution + 1)));
    }

    *predSampleValue = floor(*drSampleValue / 2);

    // Calculate predicted residual
    double pred_residual = data - *predSampleValue;
    return pred_residual;
}

/**
 * @brief Updates the weight vector for a specified band before moving onto the next pixel
 *
 * @param drSampleValue
 * @param predSample
 * @param predResidual
 * @param t = i*numCol + j
 * @param k
 * @param numCol
 * @param numFrame
 * @param weightVectorPrev current weight vector
 * @param ldVector  current local difference vector
 *
 * @return updated weight vector as a gsl_vector
 */
gsl_vector *weightUpdate(double drSampleValue, double predSample, double predResidual, int t, int k, int numCol, int numFrame,
                         gsl_vector *weightVectorPrev, gsl_vector *ldVector)
{
    gsl_vector *weight = gsl_vector_alloc(number_of_bands + 3);
    int sMin = -1 * (1 << (dynamic_range - 1));
    int sMax = 1 << (dynamic_range - 1);

    double wMin = -(1 << (weight_resolution + 1));
    double wMax = (1 << (weight_resolution + 2)) - 1;

    // Calculate prediction error
    double clippedQuant = clamp(predSample + (predResidual * (2 * max_error + 1)), sMin, sMax);
    double predictionError = 2 * clippedQuant - drSampleValue;

    // Calculate weight update scaling exponent
    double temp = v_min + floor((t - numCol) / t_inc);
    double weightExponent = clamp(temp, v_min, v_max) + dynamic_range - weight_resolution;

    double base = sign(predictionError) * (pow(2, -(weightExponent + intraband)));

    // Set north, west, and northwest vectors based on previous
    double north = gsl_vector_get(weightVectorPrev, 0) + floor((1 / 2) * (base * gsl_vector_get(ldVector, 0) + 1));
    double west = gsl_vector_get(weightVectorPrev, 1) + floor((1 / 2) * (base * gsl_vector_get(ldVector, 1) + 1));
    double northwest = gsl_vector_get(weightVectorPrev, 2) + floor((1 / 2) * (base * gsl_vector_get(ldVector, 2) + 1));

    north = clamp(north, wMin, wMax);
    west = clamp(west, wMin, wMax);
    northwest = clamp(northwest, wMin, wMax);

    gsl_vector_set(weight, 0, north);
    gsl_vector_set(weight, 1, west);
    gsl_vector_set(weight, 2, northwest);

    // Set remaining points in vector
    for (int band = 1; band <= number_of_bands; band++)
    {
        double weightI = 0;

        double baseTwo = sign(predictionError) * (pow(2, -(weightExponent + interband)));

        if (k + band < numFrame)
        {
            weightI = gsl_vector_get(weightVectorPrev, band + 2) + floor((1 / 2) * (baseTwo * gsl_vector_get(ldVector, band + 2) + 1));
            weightI = clamp(weightI, wMin, wMax);
        }

        gsl_vector_set(weight, band + 2, weightI);
    }

    // Free the vectors for the previous pixel, no longer needed
    gsl_vector_free(weightVectorPrev);
    gsl_vector_free(ldVector);

    return weight;
}

/**
 * @brief Maps predicted residuals to unsigned integers: final compression step
 *
 * @param predSamp
 * @param predResidual
 * @param drSamp
 * @param t = i*numCol + j
 *
 * @return unsigned int mapped
 */
unsigned int mapper(double predSamp, double predResidual, double drSamp, int t)
{
    unsigned int mapped = 0;

    double theta = 0;
    int sMin = -1 * (1 << (dynamic_range - 1));
    int sMax = 1 << (dynamic_range - 1);

    // Calculate theta
    if (t == 0)
    {
        theta = fmin(predSamp - sMin, sMax - predSamp);
    }
    else
    {
        theta = fmin(floor((predSamp - sMin + max_error) / (2 * max_error + 1)), floor((sMax - predSamp + max_error) / (2 * max_error + 1)));
    }

    // Determine mapped value
    if (abs(predResidual > theta))
    {
        mapped = (unsigned int)abs(predResidual) + theta;
    }
    else if (((int)drSamp % 2 == 0 && predResidual >= 0) || ((int)drSamp % 2 != 0 && predResidual <= 0))
    {
        mapped = (unsigned int)2 * abs(predResidual);
    }
    else
    {
        mapped = (unsigned int)2 * abs(predResidual) - 1;
    }

    return mapped;
}