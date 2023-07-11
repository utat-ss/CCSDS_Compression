/**
 * @file helper.h
 * @author Yong Da Li (yongdali314@gmail.com)
 * @brief helper functions, like clip and sign
 * @version 0.1
 * @date 2023-05-06
 *
 * @copyright Copyright (c) 2023
 *
 */

#ifndef HELPER_H
#define HELPER_H

float clip(float val, float min, float max);
int sgn(float val);
int sgn_star(float val);
float mod_star(int val, int base);

#endif /* HELPER_H */