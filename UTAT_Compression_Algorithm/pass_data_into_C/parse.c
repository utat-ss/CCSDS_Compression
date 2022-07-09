#include "parse.h"

/**
 * @brief Reads hyperspectral data from a modified CSV file format and stores it into an array of GSL matricies
 *
 * @param in_file modified CSV format:
 *
 * 1st row: has 3 comma separated numbers that defined the dimensions of the data
 *  ex. 3, 145, 200 --> data will be 3 rows, 145 columns, and 200 spectral slices
 *
 * rest of the rows: line-by-line data
 *
 * @param num_row
 * @param num_col
 * @param num_frame
 * @return gsl_matrix*
 */
gsl_matrix *parse(char *in_file, int *num_row, int *num_col, int *num_frame)
{
    // setup variables
    FILE *fp;
    char row[MAXCHAR];
    char *token;
    int i = 0;

    printf("reading from file: %s\n", in_file);

    fp = fopen(in_file, "r");

    // grab header and immediately exit
    while (feof(fp) != true)
    {
        fgets(row, MAXCHAR, fp);
        printf("Row: %s", row);

        token = strtok(row, ",");
        *num_row = atoi(token);

        token = strtok(NULL, ",");
        *num_col = atoi(token);

        token = strtok(NULL, ",");
        *num_frame = atoi(token);

        // only run 1x for the header
        break;
    }

    /*int pixels_per_frame = (*num_col) * (*num_row);
    int k = 0;

    // allocate return array of GSL matrices
    gsl_matrix *items = (gsl_matrix_view *)malloc((*num_frame) * sizeof(gsl_matrix_view));

    */
    // allocate temporary array that will be filled with each frame's values
    double *flat_arr = (double *)malloc(sizeof(double) * (*num_row) * (*num_col) * (*num_frame));

    while (feof(fp) != true)
    {
        // When a new frame is reached, reset array

        fgets(row, MAXCHAR, fp);
        // printf("Row: %s\n", row);

        token = strtok(row, ",");

        while (token != NULL)
        {
            // printf("Token: %s \ti: %d\n", token, i);
            flat_arr[i] = atoi(token);
            token = strtok(NULL, ",");
            i++;
        }

        /* if (i % pixels_per_frame == 0)
        {
            i = 0;
            // items[k] = malloc(sizeof(gsl_matrix));
            gsl_matrix_view temp = gsl_matrix_view_array(flat_arr, 1, (*num_row) * (*num_col));
            items[k] = temp.matrix;
            if (k > 0)
            {
                printf("%lf \n", gsl_matrix_get(&items[k - 1], 0, 0));
            }
            k++;
        } */
    }

    gsl_matrix *dataCube;
    dataCube = parse_into_gsl(flat_arr, *num_row, *num_col, *num_frame);

    return dataCube;
}

/**
 * @brief Parses flat array into array of GSL matricies
 *
 * @param arr
 * @param num_row
 * @param num_col
 * @param num_frame
 * @return gsl_matrix*
 */
gsl_matrix *parse_into_gsl(double *arr, int num_row, int num_col, int num_frame)
{

    gsl_matrix *items = (gsl_matrix *)malloc(num_frame * sizeof(gsl_matrix));
    for (int k = 0; k < num_frame; k++)
    {
        double *mat = malloc(num_row * num_col * sizeof(double));
        for (int i = 0; i < num_row; i++)
        {
            for (int j = 0; j < num_col; j++)
            {
                mat[(i * num_col) + j] = get_data(arr, num_row, num_col, num_frame, i, j, k);
            }
        }
        items[k] = gsl_matrix_view_array(mat, 1, num_row * num_col).matrix;
    }

    return items;
}

/**
 * @brief given a flat array, get the i,j,k data element in row-major order
 *
 * don't actually need the num_frame, but include it for clarity
 *
 * passing in (0,2,3) will return the `o` in position (0,2)
 * in frame 3
 *
 * x, x, x, x
 * x, x, x, x
 * o, x, x, x
 * x, x, x, x
 */
double get_data(double *arr, int num_row, int num_col, int num_frame, int i, int j, int k)
{
    return arr[k * (num_col * num_row) + i * num_col + j];
}