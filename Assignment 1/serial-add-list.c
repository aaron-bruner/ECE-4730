/*  ECE-4730     : Introduction to Parallel Systems
    File         : serial-add-list.c
    Assignment 1 : Fall 2022
    ajbrune
    C16480080

    Description: This serial executable will open the specified input file, and will determine the sum of all 
                 the numbers in it (excluding the first, which indicates how many there are). It will assume 
                 the file format is equal to that of above. It should also have a default input file should the 
                 -i not be specified. This default should be the same as specified above. This is essentially 
                 the non-parallel version of the parallel one described further down in this assignment. 
 */

#include "functions.h"

#define default_file "default-list-file.dat"

void main(int argc, char* argv[])
{
    FILE* fpt;
    int c, n, sum = 0;
    short x;
    char* i = NULL;

    // Handling multiple CLA
    while ((c = getopt(argc, argv, "i:")) != -1)
    {
        switch (c)
        {
        case 'i':
            i = strdup(optarg);
            break;
        case '?':
            fprintf(stderr, "Unknown option character `\\x%x'.\n", optopt);
            return;
        default:
            abort();
        }
    }

    // If no CLA is provided we use the default file
    i == NULL ? (i = default_file) : i;

    fpt = fopen(i, "r"); fpt == NULL ? (fprintf(stderr, "Failed to open file %s\n", i), exit(0)) : fpt;

    // Number of integers being read
    fread(&n, 1, sizeof(int), fpt);

    // Read n number of random integers
    while (fread(&x, 1, sizeof(short), fpt) != 0)
    {
        sum += x;
    }
    
    fclose(fpt);
    printf("Sum = %d\n", sum);

    return;
}