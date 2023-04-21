#include "predictor.h"
#include "mymatrix.h"
#include "datacube.h"
#include "../main.h"

/**
 * @brief neighbour oriented local sum
 *
 * @param x     indices of a single matrix
 * @param y     
 * @param mat
 * @return float
 */
float local_sum (mymatrix* mat, int x, int y){
    float west = 0;
    float northwest = 0;
    float north = 0;
    float northeast = 0;

    float sum = 0;

    /**
     * indexing for (x,y) in mat_get(mat, x, y)
     * (0,0)    (0,1)       (0,2)
     * (1,0)    (1,1)       (1,2)
     */


    // ------ main case ----
    if (y>0 && x>0 && y<mat->ncols-1){
        west = mat_get(mat, x, y-1);
        northwest = mat_get(mat, x-1, y-1);
        north = mat_get(mat, x-1, y);
        northeast = mat_get(mat, x-1, y+1);
        sum = northwest + north + northeast + west;
    }

    // ------ edge cases ----
    // top row
    else if (x==0 && y>0){
        west = mat_get(mat, x, y-1);
        sum  = 4*west;
    }

    // left side
    else if (x > 0 && y == 0) {
        north = mat_get(mat, x-1, y);
        northeast = mat_get(mat, x-1, y+1);
        
        sum = 2*(north + northeast);
    }

    // right side
    else if (y == mat->ncols - 1) {
        west = mat_get(mat, x, y-1);
        north = mat_get(mat, x-1, y);
        northwest = mat_get(mat, x-1, y-1);

        sum = west + northwest + 2*north;
    }

    else {
        // should never get here, but have an else catch-all just in case
        sum = 42069;      // set a meme value to indicate error
    }

    return sum;
}

/**
 * @brief returns local difference vector
 * 
 * @param mat 
 * @param x 
 * @param y 
 * @return myvector* 
 */
myvector* compute_local_diff_vector(datacube* cube, int x, int y, int z){
    mymatrix* mat = cube_get_frame(cube, z);
    float ls = local_sum(mat, x, y);            // can't name the variable `local_sum` since it conflicts with function definition??

    // local difference values, edge case is they're 0 if on the top row of the frame (y==0)
    float d_n = 0;
    float d_w = 0;  
    float d_nw = 0;

    // left side, but not on top row of frame
    if (x==0 & y>0){
        d_n = 4*mat_get(mat, x, y-1) - ls;
        d_w = 4*mat_get(mat, x, y-1) - ls;       // on left side, can't go West, so go North
        d_nw = 4 * mat_get(mat, x, y - 1) - ls;  // on left side, can't go West, so go North
    }
    // base case, not on left side and not on top of frame
    else{
        d_n = 4*mat_get(mat, x, y-1) - ls;
        d_w = 4*mat_get(mat, x-1, y) - ls;
        d_nw = 4*mat_get(mat, x-1, y-1) - ls;
    }

    // put calculation into vector
    myvector* vec = create_vector(PARAM_P + 3);
    vec_set(vec, 0, d_n);
    vec_set(vec, 1, d_w);
    vec_set(vec, 2, d_nw);

    // fill the rest of the vector with central differences
    int z_idx = 0;
    float central_diff = 0;
    for (int i = 3; i<PARAM_P + 3; i++){
        z_idx = i-2;

        // if we run out of spectral bands, set it to 0
        if ((z-z_idx) > cube->depth){
            central_diff = 0;     
        }
        else{
            mat = cube_get_frame(cube, z-z_idx);    // grab previous spectral frame
            ls = local_sum(mat, x, y);
            central_diff = 4 * mat_get(mat, x, y) - ls;
        }

        vec_set(vec, i, central_diff);
    }
    return vec;
}
