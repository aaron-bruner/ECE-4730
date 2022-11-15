 /*  ECE-4730     : Introduction to Parallel Systems
     File         : mm-parallel.c
     Assignment 2 : Fall 2022
     ajbrune
     C16480080

     Description: Reads 2 input matrix files and computes the product of the matrix in input_file1 with the matrix
                  in input_file2 (remember in matrix multiply, order matters), and then writes resulting product into
                  the output file. This method will implement the Cannon’s matrix multiply discussed in the book
                  and in class. You may require the number of rows and columns of the matrices to be evenly
                  divided by the square of the number of processors (which should be square). You must use an
                  MPI 2D Cartesian topology to manage communication between tasks. Your 2D matrix should be
                  represented in memory as presented in class and in the book with an array of doubles and an array
                  of pointers to double.

     Example Usage: mpirun -np __ mm-parallel input_file1 input_file2 output_file
*/

#include "functions.h"

int main(int argc, char* argv[])
{
    FILE* fpt;
    char* matrixOneFile = NULL, * matrixTwoFile = NULL, * matrixOutputFile = NULL;
    int matrixOneR = 0, matrixOneC = 0, matrixTwoR = 0, matrixTwoC = 0, size = 0, rank = 0;
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
        fprintf(stdout, "Incorrect number of arguments.\nUsage: mpirun -np __ mm-parallel (input_file1) (input_file2) (output_file)\n");
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

#pragma region Setup MPI

    MPI_Init(&argc, &argv);

    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    // Ensure that the number of processors is a square
    if (!PerfectSquare(size))
    {
        fprintf(stdout, "Error: The number of processors (%d) is not a perfect square.\n", size);
        MPI_Finalize();
        abort();
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
double** readMatrix(char* file, int* row, int* col)
{
    double buffer = 0.0;
    FILE* fpt = fopen(file, "r"); fpt == NULL ? (fprintf(stderr, "Failed to open file %s\n", file), exit(0)) : fpt;
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

/// <summary>
/// Perfect Square algorithm discussed here:
/// https://www.johndcook.com/blog/2008/11/17/fast-way-to-test-whether-a-number-is-a-square/
/// </summary>
/// <param name="n">Number to check if it is a perfect square</param>
/// <returns>0 (Not Perfect) Or 1 (Perfect)</returns>
int PerfectSquare(int n)
{
    int h = n & 0xF; // last hexadecimal "digit"
    if (h > 9)
        return 0; // return immediately in 6 cases out of 16.

    // Take advantage of Boolean short-circuit evaluation
    if (h != 2 && h != 3 && h != 5 && h != 6 && h != 7 && h != 8)
    {
        // take square root if you must
        int t = (int)floor(sqrt((double)n) + 0.5);
        return t * t == n;
    }
    return 0;
}