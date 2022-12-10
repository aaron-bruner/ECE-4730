/*  ECE-4730     : Introduction to Parallel Systems
    File         : functions.h
    Assignment 3 : Fall 2022
    ajbrune
    C16480080

    Description: This is the header file for all of our custom files.
 */


// For mpi.h
#include <mpi.h>
#include "MyMPI.h"
// From MyMPI.c
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
/*----------------*/
#include <stdbool.h>
#include <time.h>
#include <unistd.h>
#include <math.h>
#define SQR(x) ((x)*(x))

/*----------------*/
/* make-matrix.c  */
/*----------------*/
#define default_rows 100
#define default_cols 100
#define default_lower 50
#define default_upper 500000
#define default_file "matrix.dat"

/*----------------*/
/* print-matrix.c */
/*----------------*/
#define PRINT_HORIZONTAL_LINE(x) for (int __Z__ = 0; __Z__ < x; __Z__++) fprintf(stdout, "━");

/*-------------------*/
/* Jacobi-Parallel.c */
/*-------------------*/
#define ROOT 0