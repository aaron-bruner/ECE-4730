/*  ECE-4730     : Introduction to Parallel Systems
    File         : make-matrix.c
    Assignment 2 : Fall 2022
    ajbrune
    C16480080

    Description: This program writes to output_file a binary file with an n by n (square) matrix representing one of 
                 the  matrices to be multiplied, where the value of each element of the matrix is randomly 
                 generated based on a uniform random variable 
                             -r <rows>  number of rows in the matrix 
                             -c <cols> number of columns in the matrix 
                             -l <low> lower bound on the values in the matrix 
                             -u <up> upper bound of the values in the matrix 
                             -o <fname> output file name 
                 Data is 64-bit double precision floating point.  For each value generate a random integer 
                 between ‘l’ and ‘u’, convert to double, and then divide by 1000.0 
                     double v = ((double) l + ((double)random() / (u – l))) / 1000.0; 
                 The first two 32-bit integer words of the file should be ‘r’ and ‘c’ the number of rows and 
                 columns of matrix, and the data should be stored in row major order. 

    Example Usage: make-matrix -r 100 -c 100 -l 50 -u 500000 -o output_file 
 */

#include "functions.h"

int main(int argc, char* argv[])
{
    FILE* fpt;
    int x, r = default_rows, c = default_cols, l = default_lower, u = default_upper;
    double v = 0.0;
    char *o = NULL;

    // Handling multiple CLA
    while ((x = getopt(argc, argv, "r:c:l:u:o:")) != -1)
    {
        switch (x)
        {
        case 'r':
            r = atoi(optarg);
            break;
        case 'c':
            c = atoi(optarg);
            break;
        case 'l':
            l = atoi(optarg);
            break;
        case 'u':
            u = atoi(optarg);
            break;
        case 'o':
            o = strdup(optarg);
            break;
        case '?':
            fprintf(stderr, "Unknown option character `\\x%x'.\n", optopt);
            return 0;
        default:
            abort();
        }
    }

    // If no CLA is provided we use the default file
    o == NULL ? (o = default_file) : o;

    fpt = fopen(o, "w"); fpt == NULL ? (fprintf(stderr, "Failed to open file %s\n", o), exit(0)) : fpt;

    // Seed RNG
    srand(time(0));

    // Number of integers being written
    fwrite(&r, 1, sizeof(int), fpt);
    fwrite(&c, 1, sizeof(int), fpt);

    // Write n number of random integers
    for (int a = 0; a < r; a++)
    {
        for (int b = 0; b < c; b++)
        {
            //v = ((double)l + ((double)random() / (u - l))) / 1000.0;
            v = u + ((double)random() / RAND_MAX) * (u - l);
            fwrite(&v, sizeof(double), 1, fpt);
        }
    }

    fclose(fpt);

	return 0;
}