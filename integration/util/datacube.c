#include <stdio.h>
#include <stdlib.h>

#include "datacube.h"

// ========== constructors ============
datacube* create_datacube(int depth, int nrows, int ncols){
    datacube* cube = (datacube*) malloc(sizeof(datacube));
    cube->depth = depth;
    cube->nrows = nrows;
    cube->ncols = ncols;

    cube->frames = (mymatrix**) malloc(sizeof(mymatrix*)*depth);

    for (int z=0; z<depth; z++){
        cube->frames[z] = create_matrix(nrows, ncols);
    }

    return cube;
}


datacube* random_datacube(int depth, int nrows, int ncols, int min, int max){
    datacube* cube = create_datacube(depth, nrows, ncols);
    for (int z=0; z<depth; z++){
        cube->frames[z] = random_matrix(nrows, ncols, min, max);
    }

    return cube;
}

// ========== deconstructors ============
void del_datacube(datacube* cube){
    for (int z=0; z<cube->depth; z++){
        del_matrix(cube->frames[z]);
    }

    free(cube->frames);
    free(cube);
}

// ========== accessors ==============
float cube_get(datacube* cube, int z, int x, int y){
    return mat_get(cube->frames[z], x, y);
}


void cube_set(datacube* cube, int z, int x, int y, float val){
    mat_set(cube->frames[z], x, y, val);
}

// ========== display and printing =========
void pretty_print_cube(datacube* cube){
    printf("===== datacube start ===== \n");
    for (int z=0; z<cube->depth; z++){
        printf("z=%d: ",z);
        pretty_print_mat(cube->frames[z]);
    }
    printf("===== datacube end ===== \n");
}

void pretty_save_cube(datacube* cube, char* filepath){
    FILE* fptr;
    fptr = fopen(filepath, "w");

    mymatrix* mat = cube->frames[0];
    int nrows = mat->nrows;
    int ncols = mat->ncols;

    fprintf(fptr, "===== datacube start ===== \n");

    for (int z=0; z<cube->depth; z++){
        fprintf(fptr, "z=%d: ",z);

        fprintf(fptr, "----- matrix ----- \n");
        fprintf(fptr, "size: %d x %d\n", nrows, ncols);

        mat = cube->frames[z];
        for (int i = 0; i < nrows; i++) {
            for (int j = 0; j < ncols; j++) {
                fprintf(fptr, "%5.3f ", mat_get(mat, i, j));  // %3d, at least 3 wide
            }
            fprintf(fptr, "\n");
        }
    }


    fprintf(fptr, "===== datacube end ===== \n");
    fclose(fptr);
}