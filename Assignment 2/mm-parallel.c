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

	right = rank + 1 == size ? 0 : rank + 1; // Find rank to the right
	left = rank - 1 < 0 ? size-1 : rank - 1; // Find rank to the left
    
	read_row_striped_matrix(matrixOneFile				/* IN - File name */
							,(void***)&matrixOne		/* OUT - 2D submatrix indices */
							,(void**)&matrixOneStorage	/* OUT - Submatrix stored here */
							,MPI_DOUBLE					/* IN - Matrix element type */
							,&matrixOneRows				/* OUT - Matrix rows */
							,&matrixOneCols				/* OUT - Matrix cols */
							,MPI_COMM_WORLD);			/* IN - Communicator */
	read_row_striped_matrix(matrixTwoFile				/* IN - File name */
							,(void***)&matrixTwo		/* OUT - 2D submatrix indices */
							,(void**)&matrixTwoStorage	/* OUT - Submatrix stored here */
							,MPI_DOUBLE					/* IN - Matrix element type */
							,&matrixTwoRows				/* OUT - Matrix rows */
							,&matrixTwoCols				/* OUT - Matrix cols */
							,MPI_COMM_WORLD);			/* IN - Communicator */
	/*==================================================================================================================================================*/

	productRows = BLOCK_SIZE(rank, size, matrixOneRows);	// Number of rows per processor based on matrix one
	productCols = matrixTwoCols;							// Number of columns in product
	currentRow = BLOCK_LOW(rank, size, matrixOneRows);		// What row each processor starts on
	product = (double*)calloc(productRows * productCols * sizeof(MPI_DOUBLE), sizeof(MPI_DOUBLE)); // Allocate space for product per processor

	for (int rover = 0; rover < size; rover++) // For each processor calculate the partial product
	{
		for (int processRow = 0; processRow < productRows; processRow++) // Number of rows for processor to process
		{
			/// We need to get the 2D coordinates based on the processor and what the last row we processed was
			/// Using a 3x3 we can follow this logic:
			/// 
			/// processRow (x) = Number of rows per processor ( for a 4x4 matrix with 4 processors this would go to 1 | for 16x16 with 4 processors this would go to 4 )
			/// r = Current row
			/// c = Current Column
			/// 
			/// x=0 r=0 c=0    x=0 r=0 c=1    x=0 r=0 c=2    x=0 r=1 c=0    x=0 r=1 c=1 
			///  X   2   3      1   X   3      1   2   X      1   2   3      1   2   3  
			///  4   5   6  =>  4   5   6  =>  4   5   6  =>  X   5   6  =>  4   X   6  . . .
			///  7   8   9      7   8   9      7   8   9      7   8   9      7   8   9  
			/// 
			/// x=1 r=1 c=0    x=1 r=1 c=1    x=1 r=1 c=2    x=1 r=2 c=0    x=1 r=2 c=1 
			///  1   2   3      1   2   3      1   2   3      1   2   3      1   2   3  
			///  X   5   6  =>  4   X   6  =>  4   5   X  =>  4   5   6  =>  4   5   6  . . .
			///  7   8   9      7   8   9      7   8   9      X   8   9      7   X   9  
			/// Following this algorithm we reference every values for each matrix and evenly distribute the rows among processors
			for (int r = processRow * productCols + currentRow; r < processRow * productCols + (currentRow + productRows); r++) // Product Rows - Changes per process
				for (int c = (r % productRows) * productCols; c < (r % productRows) * productCols + productCols; c++)			// Product Cols - Changes per process
					product[processRow * productCols + (c % productCols)] += matrixOneStorage[r] * matrixTwoStorage[c];
		}
		MPI_Sendrecv(matrixTwoStorage, productRows * productCols, MPI_DOUBLE, left, 1,							/* SEND */
					matrixTwoStorage, productRows * productCols, MPI_DOUBLE, right, 1, MPI_COMM_WORLD, &status);/* RECV */
		currentRow = (currentRow + productRows) % matrixOneRows;
	}

	/*==================================================================================================================================================*/
	// Output result to file (only if ROOT)
	outputMatrix = (double*)malloc(matrixOneRows * matrixTwoCols * sizeof(double));
	MPI_Gather(product, (productRows * productCols), MPI_DOUBLE, rank == ROOT ? outputMatrix : NULL, (productRows * productCols), MPI_DOUBLE, 0, MPI_COMM_WORLD);

	if (rank == ROOT)
	{
		fpt = fopen(matrixOutputFile, "w");
		fwrite(&matrixOneRows, sizeof(int), 1, fpt);
		fwrite(&matrixTwoCols, sizeof(int), 1, fpt);
		fwrite(outputMatrix, sizeof(double), matrixOneRows * matrixTwoCols, fpt);
		fclose(fpt);
	}
	/*==================================================================================================================================================*/
	// Free memory
	free(product);
	free(outputMatrix);
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