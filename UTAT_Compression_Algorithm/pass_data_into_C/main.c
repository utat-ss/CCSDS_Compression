// Sample parse program, reads input file and generates a dataCube array

#include "parse.h"

/*
 === arguments ==
 1. input file
 2. output file
*/
int main(int argc, char *argv[])
{
    printf("==== start program ====\n");
    printf("Program name %s\n", argv[0]);

    if (argc == 3)
    {
        printf("The first argument is: %s\n", argv[1]);
        printf("The second argument is: %s\n", argv[2]);
    }
    else
    {
        printf("Please give 2 arguments.\n");
    }

    int num_row, num_col, num_frame;
    gsl_matrix **dataCube;
    dataCube = parse(argv[1], &num_row, &num_col, &num_frame);

    int frame = 0;
    gsl_matrix *mat = dataCube[frame];

    printf("Printing frame %d\n", frame);
    for (int row = 0; row < num_row; row++)
    {
        for (int col = 0; col < num_col; col++)
        {
            printf("\t%3.1f", gsl_matrix_get(mat, 0, row * num_col + col));
        }
        printf("\n");
    }

    return 0;
}