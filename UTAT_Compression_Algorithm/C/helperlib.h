#ifndef HELPERLIB
#define HELPERLIB

//helper functions
int sign(int x);
double clamp(double x, double min, double max);
gsl_vector_int* dec_to_bin(int num, int width);
int bin_to_dec(gsl_vector_int* bin);

#endif