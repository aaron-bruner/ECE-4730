/*  ECE-4730     : Introduction to Parallel Systems
    File         : parallel-add-list.c
    Assignment 1 : Fall 2022
    ajbrune
    C16480080

    Description: This is the parallel version of the list adder. Defaults as specified above. As mentioned
                 above, it will use block decomposition. This function must be executed by all tasks in MPI_COMM_WORLD and computes the
                 sum of all the my_value's and returns the sum of all the tasks individual 'my_value's via
                 the result pointer. (All processors must know the global sum after the call returns)
                 Note, this is similar to MPI_Allreduce with addition does, except I only expect you
                 to deal with scalar local my_values (i.e. not the reductions of arrays of values).
                 Note, for convenience, you may assume that the number of processors you run this
                 program is a 'power of 2', i.e. 2, 4, 8, 16, etc. You must put in some error checking code
                 to see ensure your code only executes if the number of processors matches this
                 assumption and if not, prints a message and immediately exits.
 */

#include "functions.h"
#define default_file "default-list-file.dat"

// MPI Headers
#include <mpi.h>
#include "MyMPI.h"

void global_sum(int* result, int rank, int size, long int my_value);

int main(int argc, char* argv[])
{
    FILE* fpt;
    int c, rank, size, elements, sum = 0, globalSum = 0;
    char* i = NULL;
    void* out_vector;

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

    // Creation of parallel processes
    MPI_Init(&argc, &argv);
    
    // Process ID and number of processes
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // Validate that we have a power of 2 number of processors
    if ((size != 0) && ((size & (size - 1)) == 0))
    {
        // Is a power of 2
    }
    else
    {
        // Is not a power of 2
        rank == 0 ? fprintf(stderr, "Error: The number of processors must be a power of 2\n") : rank;

        MPI_Finalize();
        exit(0);
    }

    // Block Decomposition
    //void read_block_vector(char *, void **, MPI_Datatype, int*, MPI_Comm);
    read_block_vector(i, &out_vector, MPI_INT, (long long int*)(&elements), MPI_COMM_WORLD);

    // Calculate partial sum
    for (c = 0; c < BLOCK_SIZE(rank, size, elements); c++) sum += ((int*)out_vector)[c];
    
    // Validate decomposition by printing out partial sums for each processor
    fprintf(stdout, "[%2d] Partial sum = %d\n", rank, sum);

    // Get the global sum by having each processor send it's sum to the root
    global_sum(&globalSum, rank, size, sum);

    // Make it *pretty* by waiting for all processes to return from global_sum() to print the global sum value
    MPI_Barrier(MPI_COMM_WORLD);

    // Have the root display total value
    fprintf(stdout, "[%2d] Global sum  = %d\n", rank, globalSum);

    // Clear all MPI state before process exits
    MPI_Finalize();

    return 0;
}

// The below link is a good discussion regarding sending and receiving in MPI
// https://stackoverflow.com/questions/39967886/how-to-send-receive-in-mpi-using-all-processors
void global_sum(int *result, int rank, int size, long int my_value)
{
    MPI_Status status;

    if (rank == size-1) // Root process
    {
        // Root already calculated it's partial value so we just add that to the result
        *result += my_value;

        // Total up partial sums as they come in
        for (int i = 0; i < size-1; i++)
        {
            //int MPI_Recv(void *buf, int count, MPI_Datatype datatype, int source, int tag,
            //              MPI_Comm comm, MPI_Status* status)
            MPI_Recv(&my_value, 1, MPI_DOUBLE, i , DATA_MSG, MPI_COMM_WORLD, &status);
            *result += my_value;
        }

        // All processes must know the global sum before we display the result
        for (int i = 0; i < size-1; i++)
        {
            //int MPI_Send(const void *buf, int count, MPI_Datatype datatype, int dest, int tag, 
            //              MPI_Comm comm)
            MPI_Send(result, 1, MPI_DOUBLE, i, DATA_MSG, MPI_COMM_WORLD);
        }
    }
    else // All processors but root
    {
        // Tell root our partial sum value
        MPI_Send(&my_value, 1, MPI_DOUBLE, size-1, DATA_MSG, MPI_COMM_WORLD);

        // Wait for root to tell us the global sum
        MPI_Recv(result, 1, MPI_DOUBLE, size-1, DATA_MSG, MPI_COMM_WORLD, &status);
    }
}