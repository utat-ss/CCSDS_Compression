#ifndef PARSE_H
#define PARSE_H

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <gsl/gsl_matrix.h>

#define MAXCHAR 1000

// Parse and helper functions
gsl_matrix **parse(char *in_file, int *num_row, int *num_col, int *num_frame);
gsl_matrix *parse_into_gsl(double *arr, int num_row, int num_col, int num_frame);
double get_data(double *arr, int num_row, int num_col, int num_frame, int i, int j, int k);

#endif