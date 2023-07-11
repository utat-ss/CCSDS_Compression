#include "helper.h"
#include <math.h>

/**
 * @brief clips value between min and max real values
 *
 * @param val
 * @param min
 * @param max
 * @return float
 */
float clip(float val, float min, float max) {
  if (val < min) {
    return min;
  } else if (val > max) {
    return max;
  } else {
    return val;
  }
}

/**
 * @brief return the sign of a number, 0 is allowed
 *
 * @param val
 * @return int
 */
int sgn(float val) {
  if (val < 0) {
    return -1;
  } else if (val > 0) {
    return 1;
  } else {
    return 0;
  }
}

/**
 * @brief sign of number, but 0 is not allowed,
 *  returns 1 if x>=0, else returns -1
 *
 * @param val
 * @return int
 */
int sgn_star(float val) {
  if (val < 0) {
    return -1;
  } else {
    return 1;
  }
}

/**
 * @brief R-bit two's complement of (val modulo R)
 *
 * @param val
 * @param R
 * @return float
 */
float mod_star(int val, int R) {
  int arg = val + (2 << (R - 1));
  int ret = (arg % (2 << R)) - (2 << (R - 1));

  return ret;
}
