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
	int matrixOneCols, matrixOneRows, matrixTwoCols, matrixTwoRows, rank, size;
	int dims[] = { 0, 0 };
	int period[] = { 1, 1 };
	double** matrixOne, * matrixOneStorage;
	double** matrixTwo, * matrixTwoStorage;
	double** product, * productStorage;
	MPI_Comm comm_cart;
	int cart_rank, cart_size, left, right, up, down, srcRank, dstRank;
	int coords[2];
	MPI_Status status;

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

	// Set up the Cartesian topology
	dims[0] = dims[1] = sqrt(size);
	// Create the Cartesian topology with rank reordering enabled
	MPI_Cart_create(MPI_COMM_WORLD, 2, dims, period, 1, &comm_cart);
	/*==================================================================================================================================================*/
	// Read in Matrix One
	read_checkerboard_matrix(matrixOneFile				/* IN - File name */
							,(void***)&matrixOne		/* OUT - 2D submatrix indices */
							,(void**)&matrixOneStorage	/* OUT - Submatrix stored here */
							,MPI_DOUBLE					/* IN - Matrix element type */
							,&matrixOneRows				/* OUT - Matrix rows */
							,&matrixOneCols				/* OUT - Matrix cols */
							,comm_cart);				/* IN - Communicator */
	// Read in Matrix Two
	read_checkerboard_matrix(matrixTwoFile				/* IN - File name */
							,(void***)&matrixTwo		/* OUT - 2D submatrix indices */
							,(void**)&matrixTwoStorage	/* OUT - Submatrix stored here */
							,MPI_DOUBLE					/* IN - Matrix element type */
							,&matrixTwoRows				/* OUT - Matrix rows */
							,&matrixTwoCols				/* OUT - Matrix cols */
							,comm_cart);				/* IN - Communicator */
	/*==================================================================================================================================================*/
	// Ensure that # of rows in matrix one equals number of columns in matrix two

	// You may require the number of rows and columns of the matrices to be evenly
	// divided by the square of the number of processors(which should be square).
	//fprintf(stdout, "[%d] Validating matrix sizes...\n", rank);
	if (matrixOneRows != matrixOneCols || // Ensure Matrix One is Square
		matrixTwoRows != matrixTwoCols || // Ensure Matrix Two is Square
		matrixOneRows != matrixTwoCols || // Ensure Matrix One Rows matches Matrix Two Columns
		matrixOneRows % (int)sqrt(size) != 0 // Ensure # of ROWS and COLS are evenly distributed
		)								   // We only need to check one since they're square and equal
	{
		fprintf(stdout, "[%d] Invalid matrix sizes...\n", rank);
		return 0;
	}
	/*==================================================================================================================================================*/
	// N is the number of rows per processor. Since we require that the matrix me 
	// square we don't have to worry about different rows and columns
	int local_rows = matrixOneRows / dims[0];
	int local_cols = matrixTwoCols / dims[1];

	// Create enough storage for the product array
	//fprintf(stdout, "[%d] Allocating space for product...\n", rank);
	product        = (double **)malloc(matrixOneRows * sizeof(double*));
	productStorage = (double *)calloc(sizeof(double), matrixOneRows * matrixTwoCols);
	for (int i = 0; i < matrixTwoCols; i++) product[i] = &productStorage[i * matrixTwoCols];

	/*==================================================================================================================================================*/
	// Get the rank, size and coordinates with respect to the new topology
	//fprintf(stdout, "[%d] Getting cart rank, size and coordinates...\n", rank);
	MPI_Comm_rank(comm_cart, &cart_rank);
	MPI_Comm_size(comm_cart, &cart_size);
	MPI_Cart_coords(comm_cart, cart_rank, 2, coords);
	//fprintf(stdout, "\t\t%d %d %d %d\n", cart_rank, cart_size, coords[0], coords[1]);

	// Move rank reference up and left
	//fprintf(stdout, "[%d] Moving rank reference up and left...\n", rank);
	MPI_Cart_shift(comm_cart, 1, -1, &right, &left);
	MPI_Cart_shift(comm_cart, 0, -1, &down, &up);

	// Next we need to align matrix A and matrix B
	//fprintf(stdout, "[%d] Aligning Matrix A and B\n", rank);
	MPI_Cart_shift(comm_cart, 1, -coords[0], &srcRank, &dstRank);
	MPI_Sendrecv_replace(matrixOneStorage, local_rows * local_cols, MPI_DOUBLE, dstRank, 1, srcRank, 1, comm_cart, &status);
	MPI_Cart_shift(comm_cart, 0, -coords[1], &srcRank, &dstRank);
	MPI_Sendrecv_replace(matrixTwoStorage, local_rows * local_cols, MPI_DOUBLE, dstRank, 1, srcRank, 1, comm_cart, &status);
		
	/*==================================================================================================================================================*/
	// Do the matrix multiplication sqrt(processors) number of times
	//fprintf(stdout, "[%d] Starting matrix multiplication...\n", rank);
	for (int i = 0; i < (int)sqrt(size); i++)
	{
		//fprintf(stdout, "\t[%d] ", rank);
		MatrixMultiply(local_rows, local_cols, matrixOne, matrixTwo, product); // Matrix Multiplication ( matrixOne x matrixTwo = product )
		MPI_Sendrecv_replace(matrixOneStorage, local_rows * local_cols, MPI_DOUBLE, left, 1, right, 1, comm_cart, &status); // Shift LEFT
		MPI_Sendrecv_replace(matrixTwoStorage, local_rows * local_cols, MPI_DOUBLE, up, 1, down, 1, comm_cart, &status);	// Shift UP
	}

	/*==================================================================================================================================================*/
	// Improved print_checkerboard_matrix to write to file instead of stdout
	//fprintf(stdout, "[%d] Outputting to file...\n", rank);
	print_checkerboard_matrix(matrixOutputFile, (void**)product, MPI_DOUBLE, matrixOneRows, matrixTwoCols, comm_cart);
	/*==================================================================================================================================================*/

	MPI_Finalize();

	return 0;
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

/// <summary>
/// This is a simple function that takes in two matrices and multiplies them 
/// together and then stores the result in matrix C
/// </summary>
/// <param name="n">Number of ROWS/COLS for each matrix (ALL SQUARE)</param>
/// <param name="a"></param>
/// <param name="b"></param>
/// <param name="c">Result Matrix</param>
void MatrixMultiply(int row, int col, double** a, double** b, double** c)
{
	for (int i = 0; i < row; i++)
	{
		for (int j = 0; j < col; j++)
		{
			for (int k = 0; k < col; k++)
			{
				c[i][j] += a[i][k] * b[k][j];
			}
		}
	}
}