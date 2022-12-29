/**
 * @file mymatrix.h
 * @author Yong Da Li
 * @brief brief implementation of matrix and vectors
 * @version 0.1
 * @date 2022-12-24
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include <stdint.h>

#ifndef MYMATRIX_H
#define MYMATRIX_H


typedef struct mymatrix{
    int nrows;
    int ncols;
    float* data;
} mymatrix;

typedef struct myvector{
    int size;
    float* data;
} myvector;

// constructors
mymatrix* create_matrix(int nrows, int ncols);
myvector* create_vector(int size);

mymatrix* random_matrix(int nrows, int ncols, int min, int max);
myvector* random_vector(int size, int min, int max);


// accessors
float mat_get(mymatrix* mat, int i, int j);
float vec_get(myvector* vec, int i);

void mat_set(mymatrix* mat, int i, int j, float val);
void vec_set(myvector* vec, int i, float val);

// display
void pretty_print_mat(mymatrix* mat);
void pretty_print_vec(myvector* vec);

//saving
void pretty_save_mat(mymatrix* mat, char* filepath);
void pretty_save_vec(myvector* vec, char* filepath);

// math
float vec_dot_prod(myvector* vecA, myvector* vecB);

#endif /* MYMATRIX_H */