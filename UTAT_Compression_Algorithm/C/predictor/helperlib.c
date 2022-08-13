/*
This file implements helper functions used in the compression and
decompression steps.
*/
#include "helperlib.h"

// Returns the sign of an integer, with 0 returned as a positive
int sign(double x)
{
    if (x >= 0)
    {
        return 1;
    }
    else
    {
        return -1;
    }
}

double clamp(double x, double min, double max)
{
    double result = x;
    if (x < min)
    {
        result = min;
    }
    else if (x > max)
    {
        result = max;
    }

    return result;
}
