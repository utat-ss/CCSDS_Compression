/*
This file implements helper functions used in the compression and
decompression steps.
*/

// Returns the sign of an integer, with 0 returned as a positive
int sign(int x)
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

int clamp(int x, int min, int max)
{
    int result = x;
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
