#include <gsl/gsl_matrix.h>

#ifndef GSL_EXTENSIONS_H
#define GSL_EXTENSIONS_H
gsl_matrix_int* init_gsl_matrix(unsigned int nrow, unsigned int ncol, int range);
void pretty_print_matrix(gsl_matrix_int* matrix);
void pretty_save_matrix(gsl_matrix_int* matrix, char* filepath);

#endif /* GSL_EXTENSIONS_H */