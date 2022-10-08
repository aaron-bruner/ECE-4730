/*  ECE-4730     : Introduction to Parallel Systems
    File         : print-list.c
    Assignment 1 : Fall 2022
    ajbrune
    C16480080

    Description: This serial executable will open the specified input file 
                 and will print the contents to the screen in one long column.
 */

#include "functions.h"
#define default_file "default-list-file.dat"

int main(int argc, char* argv[])
{
    FILE* fpt;
    int c;
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
            return 0;
        default:
            abort();
        }
    }

    // If no CLA is provided we use the default file
    i == NULL ? (i = default_file) : i;

    fpt = fopen(i, "r"); fpt == NULL ? (fprintf(stderr, "Failed to open file %s\n", i), exit(0)) : fpt;

    // Print the integers until we hit end of file
    while(fread(&c, 1, sizeof(int), fpt) != 0)
    {
        printf("%d\n", c);
    }

    fclose(fpt);

    return 0;
}