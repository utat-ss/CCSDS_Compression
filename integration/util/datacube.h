#ifndef DATACUBE_H
#define DATACUBE_H

#include "mymatrix.h"

// maximum row length for reading data
#define MAXCHAR 1000

typedef struct datacube{
    int depth;
    int nrows;
    int ncols;
    mymatrix** frames;    // array of mymatrix structs
} datacube;

// ========== constructors ============ 
datacube* create_datacube(int depth, int nrows, int ncols);
datacube* random_datacube(int depth, int nrows, int ncols, int min, int max);
datacube* ordered_datacube(int depth, int nrows, int ncols);

// ========== deconstructors ============
void del_datacube(datacube* cube);

// ========== accessors ==============
mymatrix* cube_get_frame(datacube* cube, int z);
float cube_get(datacube* cube, int z, int x, int y);
void cube_set(datacube* cube, int z, int x, int y, float val);
void cube_set_flat(datacube* cube, int z, int x, float val);

// ========== display and printing =========
void pretty_print_cube(datacube* cube);
void pretty_save_cube(datacube* cube, char* filepath);
void save_cube(datacube* cube, char* filepath);

// ========== parsing =========
datacube* parse_cube_from_file(char* filepath);

#endif /* DATACUBE_H */