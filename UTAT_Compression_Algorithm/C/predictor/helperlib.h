#ifndef HELPERLIB
#define HELPERLIB

#include <gsl/gsl_vector.h>
#include <string.h>
#include <stdint.h>

// helper functions
int sign(double x);
double clamp(double x, double min, double max);

#endif