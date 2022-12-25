#include "gsl_extensions.h"

#include <gsl/gsl_matrix.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>   

/**
 * creates a gsl matrix object with given size, filled with random integers from 0-range
 * @param  nrow [number of rows]
 * @param  ncol [number of columns]
 * @return      [gsl matrix object, integers]
 */
gsl_matrix_int* init_gsl_matrix(unsigned int nrow, unsigned int ncol, int range) {
    int i, j;
    srand((unsigned int)time(NULL));  // init random
    gsl_matrix_int* matrix = gsl_matrix_int_alloc(nrow, ncol);

    for (i = 0; i < nrow; i++) {
        for (j = 0; j < ncol; j++) {
            gsl_matrix_int_set(matrix, i, j, (int)rand() % range);
        }
    }

    return matrix;
}

/**
 * nicely prints out a gsl matrix (int only)
 * @param matrix [gsl_matrix_int type]
 */
void pretty_print_matrix(gsl_matrix_int* matrix) {
    int nrow = (int)matrix->size1;
    int ncol = (int)matrix->size2;

    printf("----- matrix ----- \n");
    printf("size: %d x %d = %d\n", nrow, ncol, nrow * ncol);

    int i, j;
    for (i = 0; i < nrow; i++) {
        for (j = 0; j < ncol; j++) {
            printf("%3d ", gsl_matrix_int_get(matrix, i, j));  // %3d, at least 3 wide
        }
        printf("\n");
    }
}

void pretty_save_matrix(gsl_matrix_int* matrix, char* filepath) {
    FILE* fptr;
    fptr = fopen(filepath, "w");

    int nrow = (int)matrix->size1;
    int ncol = (int)matrix->size2;
    fprintf(fptr, "----- matrix ----- \n");
    fprintf(fptr, "size: %d x %d\n", nrow, ncol);

    int i, j;
    for (i = 0; i < nrow; i++) {
        for (j = 0; j < ncol; j++) {
            fprintf(fptr, "%3d ", gsl_matrix_int_get(matrix, i, j));  // %3d, at least 3 wide
        }
        fprintf(fptr, "\n");
    }

    fclose(fptr);
}