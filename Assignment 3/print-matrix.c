/*  ECE-4730     : Introduction to Parallel Systems
    File         : print-list.c
    Assignment 2 : Fall 2022
    ajbrune
    C16480080

    Description: Reads a matrix file created with 1) and prints in ascii.
    
    Example Usage: print-matrix input_file
 */

#include "functions.h"

int main(int argc, char* argv[])
{
    FILE* fpt;
    char* i = NULL;

    // Handle CLA
    if (argc == 1);
    else if (argc == 2)
    {
        i = strdup(argv[1]);
    }
    else
    {
        fprintf(stdout, "Incorrect number of arguments.\nUsage: print-matrix (input_file)\n");
        abort();
    }

    // If no CLA is provided we use the default file
    i == NULL ? (i = default_file) : i;

    fpt = fopen(i, "r"); fpt == NULL ? (fprintf(stderr, "Failed to open file %s\n", i), exit(0)) : fpt;

    // Read in first two integers as row and column
    int r; fread(&r, 1, sizeof(int), fpt);
    int c; fread(&c, 1, sizeof(int), fpt);

    // Print initial formatting which includes row and column count
    PRINT_HORIZONTAL_LINE(20);
    fprintf(stdout, " Matrix Data ");
    PRINT_HORIZONTAL_LINE(20);
    fprintf(stdout, " Rows = %d AND Columns = %d ", r, c);
    PRINT_HORIZONTAL_LINE(20);
    fprintf(stdout, "\n");

    // Print the integers until we hit end of file
    int rover = 0;
    double x;
    while(fread(&x, 1, sizeof(double), fpt) != 0)
    {
        fprintf(stdout, "% 17.6lf ", x);
        rover++;
        // If we've printed the number of columns then print a newline
        rover % c == 0 ? fprintf(stdout, "\n") : false;
    }

    fprintf(stdout, "\n");
    fclose(fpt);

    return 0;
}