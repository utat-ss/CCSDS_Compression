#ifndef HELPERLIB
#define HELPERLIB

#include <gsl/gsl_vector.h>
#include <string.h>
#include <stdint.h>

// helper functions
int sign(double x);
double clamp(double x, double min, double max);
gsl_vector* dec_to_bin(int d, uint8_t width);
void gsl_vector_append(gsl_vector* base, gsl_vector* addition);

#endif