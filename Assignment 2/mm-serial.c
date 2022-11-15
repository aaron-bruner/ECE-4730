/*  ECE-4730     : Introduction to Parallel Systems
    File         : mm-serial.c
    Assignment 2 : Fall 2022
    ajbrune
    C16480080

    Description: Reads 2 input matrix files and computes the product of the matrix in input_file1 with the matrix
                 in input_file2 (remember in matrix multiply, order matters), and then writes resulting product into
                 the output file. The columns of input_file1 and the rows of input_file2 must be equal, otherwise
                 print an error and stop. This may be implemented using the traditional algorithm or a blockwise
                 algorithm.

                 The below link talks about Matrix Multiplication in C which is what this code is based off of.
                 https://www.javatpoint.com/matrix-multiplication-in-c

    Example Usage: mm-serial input_file1 input_file2 output_file
 */

#include "functions.h"

int main(int argc, char* argv[])
{
    FILE* fpt;
    char* matrixOneFile = NULL, * matrixTwoFile = NULL, * matrixOutputFile = NULL;
    int matrixOneR = 0, matrixOneC = 0, matrixTwoR = 0, matrixTwoC = 0;
    double** matrixOne, ** matrixTwo, ** product;

#pragma region Handle CLA
    // Handle CLA (Not supporting no provided file names since predefined files may not exist)
    if (argc == 4)
    {
        matrixOneFile = strdup(argv[1]);
        matrixTwoFile = strdup(argv[2]);
        matrixOutputFile = strdup(argv[3]);
    }
    else
    {
        fprintf(stdout, "Incorrect number of arguments.\nUsage: mm-serial (input_file1) (input_file2) (output_file)\n");
        abort();
    }
#pragma endregion

#pragma region Read Matrix Data
    /*━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━*/
    // Open matrix one data and read it into matrixOne
    /*━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━*/
    matrixOne = readMatrix(matrixOneFile, &matrixOneR, &matrixOneC);

    /*━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━*/
    // Open matrix two data and read it into matrixTwo
    /*━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━*/
    matrixTwo = readMatrix(matrixTwoFile, &matrixTwoR, &matrixTwoC);
#pragma endregion
    
#pragma region Data Validation

    if (matrixOneC != matrixTwoR)
    {
        fprintf(stderr, "Number of columns in matrix one (%d) does not equal number of rows in matrix two (%d)\n", matrixOneC, matrixTwoR);
        abort();
    }

#pragma endregion

#pragma region Multiply Matrix Together

    // Allocate space in the product array
    product = (double **)malloc(matrixOneR * sizeof(double *));
    for (int a = 0; a < matrixOneR; a++)
    {
        product[a] = (double *)malloc(matrixTwoC * sizeof(double));
    }

    // Initialize result matrix to 0s
    for (int a = 0; a < matrixOneR; a++)
    {
        for (int b = 0; b < matrixTwoC; b++)
        {
            product[a][b] = 0;
        }
    }

    // Multiplying first and second matrices and storing it in result
    for (int a = 0; a < matrixOneR; ++a) 
    {
        for (int b = 0; b < matrixTwoC; ++b) 
        {
            for (int c = 0; c < matrixOneC; ++c) 
            {
                product[a][b] += matrixOne[a][c] * matrixTwo[c][b];
            }
        }
    }

#pragma endregion

#pragma region Output to File

    fpt = fopen(matrixOutputFile, "w"); fpt == NULL ? (fprintf(stderr, "Failed to open file %s\n", matrixOutputFile), exit(0)) : fpt;

    // Number of integers being written
    fwrite(&matrixOneR, 1, sizeof(int), fpt);
    fwrite(&matrixTwoC, 1, sizeof(int), fpt);

    // Write n number of random integers
    for (int a = 0; a < matrixOneR; a++)
    {
        for (int b = 0; b < matrixTwoC; b++)
        {
            fwrite(&product[a][b], sizeof(double), 1, fpt);
        }
    }

    fclose(fpt);

#pragma endregion

    return 0;
}

/// <summary>
/// Helper function to read in matrix data from specified file. This is to simplify repeated methods.
/// </summary>
/// <param name="file">File name for matrix data</param>
/// <param name="row">Number of rows in matrix data file (param 1)</param>
/// <param name="col">Number of cols in matrix data file (param 2)</param>
/// <returns></returns>
double** readMatrix(char* file, int *row, int *col)
{
    double buffer = 0.0;
    FILE *fpt = fopen(file, "r"); fpt == NULL ? (fprintf(stderr, "Failed to open file %s\n", file), exit(0)) : fpt;
    // Read in first two integers as row and column
    fread(&(*row), 1, sizeof(int), fpt);
    fread(&(*col), 1, sizeof(int), fpt);
    double** data = (double**)malloc((*row) * sizeof(double*));
    for (int i = 0; i < (*row); i++)
    {
        data[i] = (double*)malloc((*col) * sizeof(double));
    }

    // Read in all of the data
    for (int a = 0; a < (*row); a++)
    {
        for (int b = 0; b < (*col); b++)
        {
            fread(&buffer, 1, sizeof(double), fpt);
            data[a][b] = buffer;
        }
    }

    fclose(fpt);

    return data;
}