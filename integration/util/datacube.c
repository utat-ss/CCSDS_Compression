#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "datacube.h"
#include "logger.h"

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
mymatrix* cube_get_frame(datacube* cube, int z){
    return cube->frames[z];
}

float cube_get(datacube* cube, int z, int x, int y){
    return mat_get(cube->frames[z], x, y);
}

void cube_set(datacube* cube, int z, int x, int y, float val){
    mat_set(cube->frames[z], x, y, val);
}

/**
 * @brief given a frame in the datacube, set the direct flat array `i` value
 * 
 * @param cube 
 * @param z 
 * @param x 
 * @param val 
 */
void cube_set_flat(datacube* cube, int z, int x, float val){
    mat_set_flat(cube->frames[z], x, val);
}

// ========== display and printing =========
void pretty_print_cube(datacube* cube){
    printf("===== datacube start ===== \n");
    printf("depth=%d, nrows=%d, ncols=%d\n", cube->depth, cube->nrows, cube->ncols);
    for (int z=0; z<cube->depth; z++){
        printf("z=%d: ",z);
        pretty_print_mat(cube->frames[z]);
    }
    printf("===== datacube end ===== \n");
}

void pretty_save_cube(datacube* cube, char* filepath){
    FILE* fptr;
    fptr = fopen(filepath, "w");

    int depth = cube->depth;
    int nrows = cube->nrows;
    int ncols = cube->ncols;

    fprintf(fptr, "===== datacube start ===== \n");
    fprintf(fptr, "depth=%d, nrows=%d, ncols=%d", depth, nrows, ncols);

    for (int z=0; z<cube->depth; z++){
        fprintf(fptr, "z=%d: ",z);

        fprintf(fptr, "----- matrix ----- \n");
        fprintf(fptr, "size: %d x %d = %d\n", nrows, ncols, nrows*ncols);

        for (int i = 0; i < nrows; i++) {
            for (int j = 0; j < ncols; j++) {
                fprintf(fptr, "%5.3f ", cube_get(cube, z, i, j));  // %3d, at least 3 wide
            }
            fprintf(fptr, "\n");
        }
    }


    fprintf(fptr, "===== datacube end ===== \n");
    fclose(fptr);
}

/**
 * @brief save datacube in specific format
 *      first row contains dimensions <depth> <nrows> <ncols>
 *      rest contains the data line-by-line in CSV format
 *      up to the parser to keep track of when new frames happen 
 * @param cube 
 * @param filepath 
 */
void save_cube(datacube* cube, char* filepath){
    FILE* fptr;
    fptr = fopen(filepath, "w");

    int depth = cube->depth;
    int nrows = cube->nrows;
    int ncols = cube->ncols;

    // write header
    fprintf(fptr, "%d, %d, %d\n", depth, nrows, ncols);

    // write matrix data sequentially
    for (int z=0; z<depth; z++){
        for (int i=0; i<nrows; i++){
            // write out an entire row
            for (int j=0; j<ncols; j++){
                fprintf(fptr, "%f,", cube_get(cube, z, i, j));
            }

            // go to next row
            fprintf(fptr, "\n");
        }
    }

    fclose(fptr);
}

// ========== parsing =========

/**
 * @brief read in a correctly formatted datacube file
 *  example: 5-frames of a 2-row by 4-column data cube
 *      header contains <depth> <nrows> <ncols>
 *      then the data is written line-by-line
 *      it's up to the parse to keep track of when new frames happen
 *          in this case, we create a new frame every 2 rows
 * 
 *  5,    2,    4
    3.000000,6.000000,7.000000,5.000000,
    3.000000,5.000000,6.000000,2.000000,
    9.000000,1.000000,2.000000,7.000000,
    0.000000,9.000000,3.000000,6.000000,
    0.000000,6.000000,2.000000,6.000000,
    1.000000,8.000000,7.000000,9.000000,
    2.000000,0.000000,2.000000,3.000000,
    7.000000,5.000000,9.000000,2.000000,
    2.000000,8.000000,9.000000,7.000000,
    3.000000,6.000000,1.000000,2.000000,
 *
 * @param filepath
 * @return datacube*
 */
datacube* parse_cube_from_file(char* filepath){
    FILE* fptr;
    fptr = fopen(filepath, "r");

    char* token;

    int depth = 0;
    int nrows = 0;
    int ncols = 0;

    char temp_row[MAXCHAR];

    // grab header and immediately exit
    while (!feof(fptr)){
        // read the first row into temp_row
        fgets(temp_row, MAXCHAR, fptr);

        token = strtok(temp_row, ",");
        depth = atoi(token);

        token = strtok(NULL, ",");
        nrows = atoi(token);

        token = strtok(NULL, ",");
        ncols = atoi(token);

        break;
    }
    
    // now that we read the size, we allocate the appropriately size struct in memory
    datacube* cube = create_datacube(depth, nrows, ncols);

    int z = 0;
    int i = 0;
    float val = 0;

    // continue until end of file
    while (!feof(fptr)){
        // check if we went over
        if (z >= depth){
            break;
        }

        // get row and split it up by commas
        fgets(temp_row, MAXCHAR, fptr);
        token = strtok(temp_row, ",");

        // for the row, get pixel value
        while (i < (nrows*ncols)){
            // atof returns doubles, cast to float
            cube_set_flat(cube, z, i, (float) atof(token));
            token = strtok(NULL, ",");
            if (token == NULL){
                break;
            }
            i++;
        }

        // check if new frame is reached
        if (i % (nrows*ncols) == 0){
            // logger("DEBUG", "frame reset i=%d, z=%d\n", i, z);
            // reset counters
            i = 0;

            // go to next frame
            z++;
        }
    }

    fclose(fptr);
    return cube;
}
