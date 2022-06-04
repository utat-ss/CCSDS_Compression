#ifndef DATACUBE
#define DATACUBE

#include <gsl/gsl_matrix.h>

// Defines the 3D datacube that the predictor will operate on
// Each GSL matrix view object represents a band in the datacube
typedef struct dataCube
{
    int bands;
    gsl_matrix_view **data;
} dataCube;

/*Constructor for the datacube -> passed off to parser code so commented out here
dataCube *dataCube_init(int bands, int Nx, int Ny)
{
    if (bands < 1) {
        bands = 1;
    }

    dataCube *cube = malloc(sizeof(dataCube));
    cube->data = calloc(bands, sizeof(gsl_matrix_view));
    for (int i = 0; i < bands; i++)
    {
        cube->data[i] = gsl_matrix_alloc(Nx, Ny);
    }
    cube->bands = bands;

    return cube;
}

void dataCube_free(dataCube *data)
{
    for (int i = 0; i < data->bands; i++)
    {
        gsl_matrix_free(data->data[i]);
    }
    free(data->data);
    free(data);
}*/

#endif