/*  ECE-4730     : Introduction to Parallel Systems
	File         : MatrixVector-Serial.c
	Assignment 3 : Fall 2022
	ajbrune
	C16480080

	Description:

	Example Usage: MatrixVector-Serial MatrixA VectorX VectorBprime
 */

#include "functions.h"

int main(int argc, char* argv[])
{
	FILE* fpt;
	char* MatrixAfile = NULL;
	char* VectorXfile = NULL;
	int i = 0, j = 0;

	// Handle CLA
	if (argc == 4)
	{
		MatrixAfile = strdup(argv[1]);
		VectorXfile = strdup(argv[2]);
	}
	else
	{
		fprintf(stdout, "Incorrect number of arguments.\nUsage: MatrixVector-Serial MatrixA VectorX VectorBprime\n");
		abort();
	}

	/**************************************************************************************************************/
	fpt = fopen(MatrixAfile, "r"); fpt == NULL ? (fprintf(stderr, "Failed to open file %s\n", MatrixAfile), exit(0)) : fpt;

	// Read in first two integers as row and column
	int MatrixAr; fread(&MatrixAr, 1, sizeof(int), fpt);
	int MatrixAc; fread(&MatrixAc, 1, sizeof(int), fpt);

	// Read the double values until we hit end of file
	double** MatrixA = (double**)malloc(sizeof(double*) * MatrixAr);
	double* MatrixAStorage = (double*)calloc(sizeof(double), MatrixAr * MatrixAc);
	for (i = 0; i < MatrixAr; i++) MatrixA[i] = &MatrixAStorage[i * MatrixAc];
	i = 0;
	fread(*MatrixA, sizeof(double), MatrixAr * MatrixAc, fpt);
	fclose(fpt);
	/**************************************************************************************************************/
	fpt = fopen(VectorXfile, "r"); fpt == NULL ? (fprintf(stderr, "Failed to open file %s\n", VectorXfile), exit(0)) : fpt;

	// Read in first two integers as row and column
	int VectorXn; fread(&VectorXn, 1, sizeof(int), fpt);

	// Print the integers until we hit end of file
	double* VectorX; VectorX = malloc(sizeof(double) * VectorXn);
	i = 0;
	fread(VectorX, sizeof(double), VectorXn, fpt);
	fclose(fpt);
	/**************************************************************************************************************/
	(MatrixAc != MatrixAr || MatrixAr != VectorXn || MatrixAc != VectorXn) ? fprintf(stdout, "Invalid matrix and vector size...\n"), exit(0) : false;

	double* VectorBprime = calloc(sizeof(double), VectorXn);
	// y = A * x
	// https://cse.buffalo.edu/~erdem/cse331/support/matrix-vect/
	for (i = 0; i < MatrixAr; i++)
	{
		for (j = 0; j < VectorXn; j++)
		{
			//fprintf(stdout, "% 17.6lf % 17.6lf ", MatrixA[i][j], VectorX[j]);
			VectorBprime[i] += MatrixA[i][j] * VectorX[j];
		}
		//fprintf(stdout, "\n");
	}

	/**************************************************************************************************************/
	fpt = fopen(argv[3], "w"); fpt == NULL ? fprintf(stdout, "Failed to open file %s\n", argv[3]), exit(0) : fpt;
	//print_subvector(VectorBprime, MPI_DOUBLE, VectorXn, fpt);
	double dbl = 0.0;
	fwrite(&VectorXn, sizeof(int), 1, fpt);
	for (i = 0; i < VectorXn; i++) {
		//printf ("%6.3f ", ((double *)a)[i]);
		dbl = ((double*)VectorBprime)[i];
		fwrite(&dbl, sizeof(double), 1, fpt);
	}
	fclose(fpt);

	return 0;
}