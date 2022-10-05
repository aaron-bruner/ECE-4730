/*  ECE-4730     : Introduction to Parallel Systems
    File         : make-list.c
    Assignment 1 : Fall 2022
    ajbrune
    C16480080

    Description: This serial executable will generate the initial data file that will contain the list of data 
                 vales to be added. It takes two arguments “-n” that indicates the number of values to 
                 place in the list, and “-o” to indicate the name of the datafile to write the integers to. If 
                 one of the arguments is not specified at runtime, there should be defaults with n=100 and 
                 the output file called “default-list-file.dat”. The first integer in the file should be the 
                 number of integers being written to the list. Then directly following this, all the other n 
                 numbers should go back to back. (File should be written as binary, NOT ASCII). 
 */

#include "functions.h"
#define default_file "default-list-file.dat"
#define default_num 100;

void main(int argc, char* argv[])
{
    FILE* fpt;
    int randInt, c, n = 100; // Default n to 100
    char *o = NULL;

    // Handling multiple CLA
    while ((c = getopt(argc, argv, "n:o:")) != -1)
    {
        switch (c)
        {
        case 'n':
            n = atoi(optarg);
            break;
        case 'o':
            o = strdup(optarg);
            break;
        case '?':
            fprintf(stderr, "Unknown option character `\\x%x'.\n", optopt);
            return;
        default:
            abort();
        }
    }

    // If no CLA is provided we use the default file
    o == NULL ? (o = default_file) : o;

    fpt = fopen(o, "w"); fpt == NULL ? (fprintf(stderr, "Failed to open file %s\n", o), exit(0)) : fpt;

    // Seed RNG
    srand(time(0));

    // Number of integers being written
    fwrite(&n, 1, sizeof(int), fpt);

    // Write n number of random integers
    for (c = 0; c < n; c++)
    {
        randInt = rand() % 20;
        fwrite(&randInt, 1, sizeof(int), fpt);
    }

    printf("Number of integers written: %d\n", c);
    fclose(fpt);

	return;
}