#ifndef MAIN_H
#define MAIN_H

#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#define MAX(a, b) (((a) > (b)) ? (a) : (b))

#define PARAM_D 14 // dynamic range (or bit-depth resolution)
#define PARAM_P                                                                \
  5 // number of preceding spectral bands used for prediction at current band

const extern float s_min;
const extern float s_mid;
const extern float s_max;

// weight parameter Omega (notice capitals Omega = param, omega = weight)
// min weight value omega_min = -2^(Omega+2)
// max weight value omega_max = 2^(Omega+2)-1

// NOTE – Increasing the number of bits used to represent weight values(i.e.,
// using a larger value of Ω) provides
//     increased resolution in the prediction calculation.This
//     Recommended Standard does not address the tradeoffs associated with
//     selecting the value of Ω.Reference[D1] presents some examples.
// ref D1 =  https://public.ccsds.org/Pubs/120x2g2.pdf
#define PARAM_OMEGA 4

// register size
// range: max{32, D + Ω + 2} ≤ R ≤ 64
// Increasing the register size R reduces the chance of an overflow occurring in
// the
//  calculation of a high - resolution predicted sample value.
#define PARAM_R MAX(32, PARAM_D + PARAM_OMEGA + 2)

#endif // MAIN_H