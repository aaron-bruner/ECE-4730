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
    char* matrixOneFile = NULL, * matrixTwoFile = NULL, * matrixOutputFile = NULL;
	int rank, size;
	int matrixOneCols, matrixOneRows, matrixTwoCols, matrixTwoRows, productRows, productCols, i, right, left, currentRow;
	double** matrixOne, * matrixOneStorage;
	double** matrixTwo, * matrixTwoStorage;
	double* product, * outputMatrix;
	MPI_Status status;
	FILE* fpt;

	/*==================================================================================================================================================*/
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
        return 0;
    }

	/*==================================================================================================================================================*/
	// Setup MPI and read in matrix data
	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	// Ensure we have a square number of processors
	if (!PerfectSquare(size) || size == 1)
	{
		if (rank == ROOT) fprintf(stdout, "Invalid number of processors (%d)...\n", size);
		return 0;
	}
  
	// Read in Matrix One
	read_row_striped_matrix(matrixOneFile				/* IN - File name */
							,(void***)&matrixOne		/* OUT - 2D submatrix indices */
							,(void**)&matrixOneStorage	/* OUT - Submatrix stored here */
							,MPI_DOUBLE					/* IN - Matrix element type */
							,&matrixOneRows				/* OUT - Matrix rows */
							,&matrixOneCols				/* OUT - Matrix cols */
							,MPI_COMM_WORLD);			/* IN - Communicator */
	// Read in Matrix Two
	read_row_striped_matrix(matrixTwoFile				/* IN - File name */
							,(void***)&matrixTwo		/* OUT - 2D submatrix indices */
							,(void**)&matrixTwoStorage	/* OUT - Submatrix stored here */
							,MPI_DOUBLE					/* IN - Matrix element type */
							,&matrixTwoRows				/* OUT - Matrix rows */
							,&matrixTwoCols				/* OUT - Matrix cols */
							,MPI_COMM_WORLD);			/* IN - Communicator */

	if (rank == ROOT)
	{

	}
	/*==================================================================================================================================================*/
	// Free memory
	free(matrixOne);
	free(matrixOneStorage);
	free(matrixTwo);
	free(matrixTwoStorage);

	// Clean up MPI
	MPI_Finalize();
	/*==================================================================================================================================================*/

	return(0);
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