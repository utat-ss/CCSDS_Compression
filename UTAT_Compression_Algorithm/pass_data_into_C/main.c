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

int*** parse(char* in_file, int* num_row, int* num_col, int* num_depth){
    // setup variables
    FILE *fp;
    char row[MAXCHAR];
    char *token;
    int i = 0; // indexing
    int j = 0;
    int k = 0;

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


    int*** dataCube = (int***) malloc(sizeof(int ***) * (*num_row));

    // create rows
    for (i=0; i < *num_row; i++){
        dataCube[i] = (int**) malloc(sizeof(int*) * (*num_col));

        // create columns
        for (j=0; j < *num_col; j++){
            dataCube[i][j] = (int*) malloc(sizeof(int) * (*num_depth));

            // load data
            for(k=0; k < *num_depth; k++){
                dataCube[i][j][k] = flat_arr[i*(*num_row * *num_col) + j*(*num_col) + k];
            }
        }
    }

    free (flat_arr);
    free (fp);
    return dataCube;
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

    int*** dataCube;
    int num_row, num_col, num_depth;

    dataCube = parse(argv[1], &num_row, &num_col, &num_depth);

    printf("num_row: %d\n", num_row);
    printf("num_col: %d\n", num_col);
    printf("num_depth: %d\n", num_depth);

    int i = 0;
    int j = 0;
    int k = 0;

    for (i=0; i<3; i++){
        for (j=0; j<3; j++){
            for (k=0; k<3; k++){
                printf("dataCube[%d][%d][%d]: %d\n", i,j,k, dataCube[i][j][k]);
            }
        }
    }
}



