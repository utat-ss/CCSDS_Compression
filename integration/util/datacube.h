#ifndef DATACUBE_H
#define DATACUBE_H

#include "mymatrix.h"

typedef struct datacube{
    int depth;
    int nrows;
    int ncols;
    mymatrix** frames;    // array of mymatrix structs
} datacube;

// ========== constructors ============ 
datacube* create_datacube(int depth, int nrows, int ncols);
datacube* random_datacube(int depth, int nrows, int ncols, int min, int max);

// ========== deconstructors ============
void del_datacube(datacube* cube);

// ========== accessors ==============
float cube_get(datacube* cube, int z, int x, int y);
void cube_set(datacube* cube, int z, int x, int y, float val);

// ========== display and printing =========
void pretty_print_cube(datacube* cube);
void pretty_save_cube(datacube* cube, char* filepath);

#endif /* DATACUBE_H */