#include "predictor.h"

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