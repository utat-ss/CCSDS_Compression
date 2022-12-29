#include <time.h>
#include <stdlib.h>
#include <stdio.h>

#include "mymatrix.h"
#include "logger.h"

// TODO: make it work without malloc
mymatrix* create_matrix(int nrows, int ncols){
    mymatrix* mat = (mymatrix*) malloc(sizeof(mymatrix));
    mat->data = (float*) malloc(sizeof(float)*nrows*ncols);
    mat->nrows = nrows;
    mat->ncols = ncols;

    return mat;
}

myvector* create_vector(int size){
    myvector* vec = (myvector*) malloc(sizeof(myvector));
    vec->data = (float*) malloc(sizeof(float)*size);
    vec->size = size;

    return vec;
}

mymatrix* random_matrix(int nrows, int ncols, int min, int max) {
    mymatrix* mat = create_matrix(nrows, ncols);
    for (int i = 0; i < nrows; i++) {
        for (int j = 0; j < ncols; j++) {
            mat_set(mat, i, j, (float)((rand() % (max - min)) + min));
        }
    }
    return mat;
}


myvector* random_vector(int size, int min, int max){
    myvector* vec = create_vector(size);
    for (int i=0; i<size; i++){
        vec_set(vec, i, (float) ((rand() % (max-min)) + min) );
    }
    return vec;
}


// getters, assume row major format
float mat_get(mymatrix* mat, int i, int j){
    if (mat == NULL){
        logger("ERROR", "matrix is null");
        return -1;
    }
    else if (i>mat->nrows){
        logger("ERROR", "out of range, matrix size is (%d, %d), trying to access row=%d", mat->nrows, mat->ncols, i);
        return -1;
    }
    else if(j>mat->ncols){
        logger("ERROR", "out of range, matrix size is (%d, %d), trying to access col=%d", mat->nrows, mat->ncols, j);
        return -1;
    }

    // no errors
    return mat->data[i * mat->nrows + j];
}


float vec_get(myvector* vec, int i){
    if (vec == NULL) {
        logger("ERROR", "vector is null");
        return -1;
    } 
    else if (i > vec->size) {
        logger("ERROR", "out of range, vector size is (%d), trying to access row=%d", vec->size, i);
        return -1;
    }

    return vec->data[i];
}

// setters, assume row major
void mat_set(mymatrix* mat, int i, int j, float val){
    mat->data[i * mat->nrows + j] = val;
}

void vec_set(myvector* vec, int i, float val){
    vec->data[i] = val;
}


// display
void pretty_print_mat(mymatrix* mat) {
    int nrow = (int)mat->nrows;
    int ncol = (int)mat->ncols;

    printf("----- matrix ----- \n");
    printf("size: %d x %d = %d\n", nrow, ncol, nrow * ncol);

    int i, j;
    for (i = 0; i < nrow; i++) {
        for (j = 0; j < ncol; j++) {
            printf("%3.3f\t", mat_get(mat, i, j));
        }
        printf("\n");
    }
}

void pretty_print_vec(myvector* vec) {
    int size = (int)vec->size;

    printf("----- vector ----- \n");
    printf("size: %d\n", size);

    int i;
    for (i = 0; i < size; i++) {
        printf("%3.3f", vec_get(vec, i));
        printf("\n");
    }
}


// saving
void pretty_save_mat(mymatrix* mat, char* filepath){
    FILE* fptr;
    fptr = fopen(filepath, "w");

    int nrows = mat->nrows;
    int ncols = mat->ncols;

    fprintf(fptr, "----- matrix ----- \n");
    fprintf(fptr, "size: %d x %d\n", nrows, ncols);

    int i, j;
    for (i = 0; i < nrows; i++) {
        for (j = 0; j < ncols; j++) {
            fprintf(fptr, "%5.3f ", mat_get(mat, i, j));  // %3d, at least 3 wide
        }
        fprintf(fptr, "\n");
    }

    fclose(fptr);
}

void pretty_save_vec(myvector* vec, char* filepath){
    FILE* fptr;
    fptr = fopen(filepath, "w");

    int size = vec->size;

    fprintf(fptr, "----- vector ----- \n");
    fprintf(fptr, "size: %d\n", size);

    int i;
    for (i = 0; i < size; i++) {
        fprintf(fptr, "%5.3f\n", vec_get(vec, i));  // %3d, at least 3 wide
    }

    fclose(fptr);
}

// math
float vec_dot_prod(myvector* vecA, myvector* vecB){
    float result = 0;

    if (vecA->size != vecB->size){
        logger("ERROR", "dot product size mismatch, (%d) vs (%d)", vecA->size, vecB->size);
    }

    for (int i=0; i<vecA->size; i++){
        result = result + vec_get(vecA, i) * vec_get(vecB, i);
    }

    return result;
}