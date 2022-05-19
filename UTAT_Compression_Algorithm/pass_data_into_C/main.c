/*
Yong Da Li
Saturday, May 14, 2022

how to pass a file into a c program
*/

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#define MAXCHAR 1000

/**
 * modified CSV format
 * -------------------
 *
 * 1st row: has 3 comma separated numbers that defined the dimensions of the data
 *  ex. 3, 145, 200 --> data will be 145 rows, 145 columns, and 200 spectral slices
 *
 * rest of the rows: line-by-line data
 */


/**
 * function: reads hyperspectral data from modified CSV file format
 * ----------------
 * in_file: string name of input modified CSV file
 * num_row, num_col, num_depth: return by reference parameters that define the datacube size
 *
 * returns: 3-D array that is the datacube
 */

int* parse(char* in_file, int* num_row, int* num_col, int* num_depth){
    // setup variables
    FILE *fp;
    char row[MAXCHAR];
    char *token;
    int i = 0;

    printf("reading from file: %s\n", in_file); 

    fp = fopen(in_file, "r");

    // grab header and immediately exit
    while (feof(fp) != true){
        fgets(row, MAXCHAR, fp);
        printf("Row: %s", row);

        token = strtok(row, ",");
        *num_row = atoi(token);

        token = strtok(NULL, ",");
        *num_col = atoi(token);

        token = strtok(NULL, ",");
        *num_depth = atoi(token);

        // only run 1x for the header
        break;
    }

    // read everything into a flat 1-D array
    // convert this array into a 3D array later
    int *flat_arr = (int*) malloc(sizeof(int) * (*num_row) * (*num_col) * (*num_depth));

    // load the rest of the data
    while (feof(fp) != true){  
        fgets(row, MAXCHAR, fp);

        // printf("Row: %s", row);

        token = strtok(row, ",");

        while(token != NULL){
            // printf("Token: %s \ti: %d\n", token, i);
            flat_arr[i] = atoi(token);
            token = strtok(NULL, ",");
            i++;
        }
    }

    free(fp);
    free(token);

    return flat_arr;
}

/**
 * given a flat array, get the x,y,z data element
 *
 * don't actually need the zlen, but include it for clarity
 *
 * passing in (0,2,3) will return the `o` in position (0,2) 
 * in slice 3
 * 
 * x, x, x, x
 * x, x, x, x
 * o, x, x, x
 * x, x, x, x
 */
int get_data(int* arr, int xlen, int ylen, int zlen, int x, int y, int z){
    return arr[ (z*(xlen*ylen)) + (y*xlen) + x];
}


/* 
 === arguments == 
 1. input file
 2. output file
*/
int main( int argc, char *argv[] )  {
    printf("==== start program ====\n");
    printf("Program name %s\n", argv[0]);

    if( argc == 3 ) {
      printf("The first argument is: %s\n", argv[1]);
      printf("The second argument is: %s\n", argv[2]);
    }
    else {
      printf("Please give 2 arguments.\n");
    }

    int* data;
    int num_row, num_col, num_depth;

    data = parse(argv[1], &num_row, &num_col, &num_depth);

    printf("num_row: %d\n", num_row);
    printf("num_col: %d\n", num_col);
    printf("num_depth: %d\n", num_depth);

    int i = 0;
    int j = 0;
    int k = 0;

    for (i=0; i<3; i++){
        for (j=0; j<3; j++){
            for (k=0; k<3; k++){
                printf("data[%d][%d][%d]: %d\n", i,j,k, get_data(data, num_row, num_col, num_depth, i, j, k));
            }
        }
    }
}



