/*  ECE-4730     : Introduction to Parallel Systems
    File         : Jacobi-Parallel.c
    Assignment 3 : Fall 2022
    ajbrune
    C16480080

    Description: 

    Example Usage: mpirun -n _ ./Jacobi-Parallel -d 100 MatrixA VectorX VectorB
 */

#include "functions.h"

int main(int argc, char* argv[])
{
    FILE* fpt;
    char* MatrixAFile = NULL;
    char* VectorXFile = NULL;
    char* VectorBFile = NULL;
    int x_opt, d = -1, i = 0, rover, j, k, size, rank, MatrixAr, MatrixAc, VectorBn;
    double** MatrixA, * MatrixAStorage, * VectorB, sum = 0.0;

    /**************************************************************************************************************/
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

    if (argc == 6 && d != -1)
    {
        MatrixAFile = argv[3];
        VectorXFile = argv[4];
        VectorBFile = argv[5];
    }
    else
    {
        fprintf(stdout, "Incorrect number of arguments.\nUsage: mpirun -n _ ./Jacobi-Parallel -d 100 MatrixA VectorX VectorB\n");
        abort();
    }
    /**************************************************************************************************************/
    // Setup MPI
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    /**************************************************************************************************************/
    // Read in data
    read_row_striped_matrix(
        MatrixAFile,            /* IN - File name */
        (void***)&MatrixA,      /* OUT - 2D submatrix indices */
        (void**)&MatrixAStorage,/* OUT - Submatrix stored here */
        MPI_DOUBLE,             /* IN - Matrix element type */
        &MatrixAr,              /* OUT - Matrix rows */
        &MatrixAc,              /* OUT - Matrix cols */
        MPI_COMM_WORLD);        /* IN - Communicator */

    read_block_vector(
        VectorBFile,        /* IN - File name */
        (void**)&VectorB,   /* OUT - Subvector */
        MPI_DOUBLE,         /* IN - Element type */
        &VectorBn,          /* OUT - Vector length */
        MPI_COMM_WORLD);    /* IN - Communicator */
    /**************************************************************************************************************/
    (MatrixAc != MatrixAr || MatrixAr != VectorBn || MatrixAc != VectorBn) ? fprintf(stdout, "Invalid matrix and vector size...\n"), exit(0) : false;
    /**************************************************************************************************************/
    //Jacobi's Method
    //    Solving Ax = b
    //    Guess a series of values of xk
    //    Evaluate Axk = b'
    //    Compare b and b'
    //    Refine xk into xk + 1
    //    Repeat until b - b' < threshold
    //    For each iteration k :
    //Then Allgather xk + 1 for next iteration
    int N = VectorBn / size; // Number of rows per processor
    double* x = (double*)calloc(sizeof(double), VectorBn);
    double* new = (double*)malloc(sizeof(double) * VectorBn / size);

    for (rover = 0; rover < d; rover++) 
    {
        for (j = 0; j < N; j++) 
        {
            sum = 0.0;
            for (k = 0; k < VectorBn; k++)
            {
                if (k != j + N * rank)
                {
                    sum += MatrixA[j][k] * x[k];
                }
            }
            new[j] = (1 / MatrixA[j][j + N * rank]) * (VectorB[j] - sum);
        }
        /*for (j = 0; j < VectorBn; j++)
        {
            x[j] = new[j];
        }*/
        MPI_Allgather(new, N, MPI_DOUBLE, x, N, MPI_DOUBLE, MPI_COMM_WORLD);
    }
    /**************************************************************************************************************/
    if (rank == ROOT)
    {
        fpt = fopen(VectorXFile, "w"); fpt == NULL ? fprintf(stdout, "Failed to open file %s\n", VectorXFile), exit(0) : fpt;
        //print_subvector(VectorB, MPI_DOUBLE, VectorXn, fpt);
        double dbl = 0.0;
        fwrite(&VectorBn, sizeof(int), 1, fpt);
        for (i = 0; i < VectorBn; i++) {
            //printf ("%6.3f ", ((double *)a)[i]);
            dbl = ((double*)x)[i];
            fwrite(&dbl, sizeof(double), 1, fpt);
        }
        fclose(fpt);
    }
    /**************************************************************************************************************/
    MPI_Finalize();

    return 0;
}