/*  ECE-4730     : Introduction to Parallel Systems
    File         : RMS-Serial.c
    Assignment 3 : Fall 2022
    ajbrune
    C16480080

    Description:

    Example Usage: RMS-Serial VectorB VectorBprime
 */

#include "functions.h"

int main(int argc, char* argv[])
{
    FILE* fpt;
    char* vectorBfile = NULL;
    char* vectorBprimefile = NULL;
    int i = 0;

    // Handle CLA
    if (argc == 2)
    {
        vectorBfile = strdup(argv[1]);
        vectorBprimefile = strdup(argv[2]);
    }
    else
    {
        fprintf(stdout, "Incorrect number of arguments.\nUsage: RMS-Serial VectorB VectorBprime\n");
        abort();
    }

    /**************************************************************************************************************/
    fpt = fopen(vectorBfile, "r"); fpt == NULL ? (fprintf(stderr, "Failed to open file %s\n", vectorBfile), exit(0)) : fpt;

    // Read in first two integers as row and column
    int vectorBn; fread(&vectorBn, 1, sizeof(int), fpt);

    // Print the integers until we hit end of file
    double* vectorB; vectorB = calloc(sizeof(double), vectorBn);
    double x;
    while (fread(&x, 1, sizeof(double), fpt) != 0)
        vectorB[++i] = x;

    fclose(fpt);
    /**************************************************************************************************************/
    fpt = fopen(vectorBprimefile, "r"); fpt == NULL ? (fprintf(stderr, "Failed to open file %s\n", vectorBprimefile), exit(0)) : fpt;

    // Read in first two integers as row and column
    int vectorBprimen; fread(&vectorBprimen, 1, sizeof(int), fpt);

    // Print the integers until we hit end of file
    double* vectorBprime; vectorBprime = calloc(sizeof(double), vectorBprimen);
    i = 0;
    while (fread(&x, 1, sizeof(double), fpt) != 0)
        vectorBprime[++i] = x;

    fclose(fpt);
    /**************************************************************************************************************/
    vectorBn != vectorBprimen ? fprintf(stdout, "Invalid vector sizes."), exit(0) : false;

    double sum = 0.0;
    for (i = 0; i < vectorBn; i++)
        sum += pow((vectorB[i] - vectorBprime[i]), 2.0);
    sum /= vectorBn;
    sum = sqrt(sum);
    printf("RMSD = %lf\n", sum);
    /**************************************************************************************************************/

    return 0;
}