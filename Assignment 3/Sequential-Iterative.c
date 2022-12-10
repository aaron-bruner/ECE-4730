/*  ECE-4730     : Introduction to Parallel Systems
    File         : Sequential-Iterative.c
    Assignment 3 : Fall 2022
    ajbrune
    C16480080

    Description:

    Example Usage: Sequential-Iterative -d 100 MatrixA VectorX VectorB
 */

#include "functions.h"

int main(int argc, char* argv[])
{
    FILE* fpt;
    char* MatrixAFile = NULL;
    char* VectorXFile = NULL;
    char* VectorBFile = NULL;
    int x_opt, d = -1, i = 0;

    // Handling multiple CLA
    while ((x_opt = getopt(argc, argv, "d:")) != -1)
    {
        switch (x_opt)
        {
        case 'd':
            d = atoi(optarg);
            break;
        case '?':
            fprintf(stderr, "Unknown option character `\\x%x'.\n", optopt);
            return 0;
        default:
            abort();
        }
    }

    // If we didn't get the -d flag then exit
    if (d == -1) exit(1);

    MatrixAFile = argv[3];
    VectorXFile = argv[4];
    VectorBFile = argv[5];

    /**************************************************************************************************************/
    fpt = fopen(MatrixAFile, "r"); fpt == NULL ? (fprintf(stderr, "Failed to open file %s\n", MatrixAFile), exit(0)) : fpt;

    // Read in first two integers as row and column
    int MatrixAr; fread(&MatrixAr, 1, sizeof(int), fpt);
    int MatrixAc; fread(&MatrixAc, 1, sizeof(int), fpt);

    // Read the double values until we hit end of file
    double** MatrixA = (double**)malloc(sizeof(double*) * MatrixAr);
    double* MatrixAStorage = (double*)calloc(sizeof(double), MatrixAr * MatrixAc);
    for (i = 0; i < MatrixAr; i++) MatrixA[i] = &MatrixAStorage[i * MatrixAc];
    fread(*MatrixA, sizeof(double), MatrixAr * MatrixAc, fpt);
    fclose(fpt);
    /**************************************************************************************************************/
    fpt = fopen(VectorBFile, "r"); fpt == NULL ? (fprintf(stderr, "Failed to open file %s\n", VectorBFile), exit(0)) : fpt;

    // Read in first two integers as row and column
    int VectorBn; fread(&VectorBn, 1, sizeof(int), fpt);

    // Print the integers until we hit end of file
    double* VectorB; VectorB = malloc(sizeof(double) * VectorBn);
    fread(VectorB, sizeof(double), VectorBn, fpt);
    fclose(fpt);
    /**************************************************************************************************************/
    /*
    Jacobi Method:
    a[0..n — 1, 0..n — 1] — coefficient matrix
    b[0..1] - constant vector
    new[0..n - 1] - new value of result vector
    sum - accumulates partial results
    x[0..n-1] - result vector

    for i <~ 0 to n - 1 do
        x[i] <~ 0
    endfor
    repeat
        for j <~ 0 to n-1 do
            sum <~ 0
            for k <~ 0 to n - 1 do
                if k != j then
                    sum <~ sum + a[j,k] x x[k]
                endif
            endfor
            new[j] <~ (1/a[j,j]) x (b[j] – sum)
        endfor
    for j <~ 0 to n - 1 do
        x[j] <~ new[j]
    endfor
    until values in x converge
    */
    double* ptr;
    double* x0 = calloc(sizeof(double), VectorBn);
    double* new = malloc(sizeof(double) * VectorBn);

    double sum = 0.0;
    for (int rover = 0; rover < d; rover++)
    {
        for (int j = 0; j < VectorBn; j++)
        {
            sum = 0.0;
            for (int k = 0; k < VectorBn; k++)
            {
                if (k != j)
                {
                    sum += MatrixA[j][k] * x0[k];
                }
            }
            new[j] = (1 / MatrixA[j][j]) * (VectorB[j] - sum);
        }
        ptr = x0;
        x0 = new;
        new = ptr;
    }
    /**************************************************************************************************************/
    fpt = fopen(VectorXFile, "w"); fpt == NULL ? fprintf(stdout, "Failed to open file %s\n", VectorXFile), exit(0) : fpt;
    //print_subvector(VectorB, MPI_DOUBLE, VectorXn, fpt);
    double dbl = 0.0;
    fwrite(&VectorBn, sizeof(int), 1, fpt);
    for (i = 0; i < VectorBn; i++) {
        //printf ("%6.3f ", ((double *)a)[i]);
        dbl = ((double*)VectorB)[i];
        fwrite(&dbl, sizeof(double), 1, fpt);
    }
    fclose(fpt);

    return 0;
}