#include <stdio.h>
#include <stdlib.h>
#include "mpi.h"
#include "graph_checkerboard_io.h"

#define R 0
#define C 1

void error_out(int ret, int ID, MPI_Status *status)
{
    if (ret != MPI_SUCCESS)
    {
        char string[MPI_MAX_ERROR_STRING];
        int strsz;
        MPI_Error_string(ret, string, &strsz);
        debug("%d: error: %s\n", ID );
        MPI_Finalize();
        exit(-1);
    }
    ret = MPI_SUCCESS;
}

/*
 *   Function 'read_checkerboard_graph' reads a graph from
 *   a file. The first element of the file is an integer
 *   whose value is the number of nodes in the graph, and
 *   the size each size of the adjacencymatrix ('n' rows
 *   and 'n' columns). What follows are 'n'*'n' values
 *   representing the matrix elements stored in row-major
 *   order.  This function allocates blocks of the matrix to
 *   the MPI processes.
 *
 *   The number of processes must be a square number.
 *   You must pass in communicator with a 2D grid topology.
 *   This routines allocates and frees the 2D matrix partition
 */
 
void read_checkerboard_graph (
                char *s,              /* IN - File name */
                void ***subs,         /* OUT - 2D array */
                void **storage,       /* OUT - Array elements */
                MPI_Datatype dtype,   /* IN - Element type */
                int *N,               /* OUT - Array cols */
                MPI_Comm grid_comm)   /* IN - Communicator */
{
    int          datum_size = 0;      /* Bytes per elements */
    int          grid_coord[2] = {0, 0};  /* Process coords */
    int          grid_id = 0;         /* Process rank */
    int          grid_period[2] = {0, 0}; /* Wraparound */
    int          grid_size[2] = {0, 0};   /* Dimensions of grid */
    int          i, ID = 0, ret = MPI_SUCCESS;
    void       **lptr = NULL;         /* Pointer into 'subs' */
    void        *rptr = NULL;         /* Pointer into 'storage' */
    int          matsize[2] = {0, 0};
    int          subsizes[2] = {0, 0};
    int          starts[2] = {0, 0};
    MPI_Status   status;              /* Results of read */
    MPI_File     fh;
    MPI_Datatype subarray_t;

    MPI_Comm_rank (grid_comm, &grid_id); ID = grid_id;

    debug( "%d: inside read_checkboard_graph\n", ID ); fflush(stderr);

    MPI_Type_size(dtype, &datum_size);

    debug( "%d: grid_id = %d dtype_size = %d\n", ID, grid_id, datum_size ); fflush(stderr);

    ret = MPI_Cart_get (grid_comm, 2, grid_size, grid_period, grid_coord);
    error_out(ret, ID, NULL);

    debug( "%d: g_size[R] = %d, g_size[C] = %d\n", ID, grid_size[R], grid_size[C] );
    debug( "%d: g_peri[R] = %d, g_peri[C] = %d\n", ID, grid_period[R], grid_period[C] );
    debug( "%d: g_coor[R] = %d, g_coor[C] = %d\n", ID, grid_coord[R], grid_coord[C] );

    ret = MPI_File_open(grid_comm, s, MPI_MODE_RDONLY, MPI_INFO_NULL, &fh);
    debug( "%d: Open file: %s\n", ID, s ); fflush(stderr);
    error_out(ret, ID, NULL);

    if (grid_id == 0)
    {
        ret = MPI_File_read(fh, N, 1, MPI_INT, &status);
        error_out(ret, ID, &status);
        debug( "%d: Read size from file: %s\n", ID, s ); fflush(stderr);
    }
    MPI_Bcast(N, 1, MPI_INT, 0, grid_comm);
    debug( "%d: N = %d\n", ID, *N ); fflush(stderr);

    matsize[R] = *N;
    matsize[C] = *N;
    debug( "%d: matsize[R] = %d, matsize[C] = %d\n", ID, matsize[R], matsize[C] );

    subsizes[R] = BLOCK_SIZE(grid_coord[R], grid_size[R], matsize[R]);
    subsizes[C] = BLOCK_SIZE(grid_coord[C], grid_size[C], matsize[C]);
    debug( "%d: subsz[R] = %d, subsz[C] = %d\n", ID, subsizes[R], subsizes[C] );

    starts[R] = BLOCK_LOW(grid_coord[R], grid_size[R], matsize[R]);
    starts[C] = BLOCK_LOW(grid_coord[C], grid_size[C], matsize[C]);
    debug( "%d: starts[R] = %d, starts[C] = %d\n", ID, starts[R], starts[C] );

    /* Dynamically allocate two-dimensional matrix 'subs' */

    *storage = (void *) malloc (subsizes[R] * subsizes[C] * datum_size);
    *subs = (void **) malloc (subsizes[R] * sizeof(void *));
    lptr = (void **) *subs;
    rptr = (void *) *storage;
    for (i = 0; i < subsizes[R]; i++)
    {
        *(lptr++) = (void *) rptr;
        rptr += (subsizes[C] * datum_size);
    }

    debug( "%d: create_subarray\n", ID ); fflush(stderr);
    MPI_Type_create_subarray(2, matsize, subsizes, starts, MPI_ORDER_C, dtype, &subarray_t);

    debug( "%d: commit\n", ID );
    MPI_Type_commit(&subarray_t);
    
    debug( "%d: set_view\n", ID ); fflush(stderr);
    MPI_File_set_view(fh, sizeof(int), dtype, subarray_t, "native", MPI_INFO_NULL);

    debug( "%d: read file data\n", ID ); fflush(stderr);
    ret = MPI_File_read(fh, *storage, subsizes[R] * subsizes[C], dtype, &status);
    error_out(ret, ID, &status);

    debug( "%d: close\n", ID ); fflush(stderr);
    MPI_File_close(&fh);

    debug( "%d: free\n", ID );
    MPI_Type_free(&subarray_t);

    debug( "%d: read returning\n", ID ); fflush(stderr);
}

/*
 *   Function 'write_checkerboard_graph' writes a graph to
 *   a file. The first element of the file is an integer
 *   whose value is the number of nodes in the graph, and
 *   the size each size of the adjacency matrix ('n' rows
 *   and 'n' columns). What follows are n x n values
 *   representing the matrix elements stored in row-major
 *   order.  This function allocates frees the matrix for
 *   the MPI processes.
 *
 *   The number of processes must be a square number.
 */
 
void write_checkerboard_graph (
                char *s,              /* IN - File name */
                void ***subs,         /* OUT - 2D array */
                void **storage,       /* OUT - Array elements */
                MPI_Datatype dtype,   /* IN - Element type */
                int N,                /* OUT - Array cols */
                MPI_Comm grid_comm)   /* IN - Communicator */
{
    int          datum_size = 0;      /* Bytes per elements */
    int          grid_coord[2] = {0, 0};  /* Process coords */
    int          grid_id = 0;         /* Process rank */
    int          grid_period[2] = {0, 0}; /* Wraparound */
    int          grid_size[2] = {0, 0};   /* Dimensions of grid */
    int          i, ID = 0, ret = 0;
    void       **lptr = NULL;         /* Pointer into 'subs' */
    void        *rptr = NULL;         /* Pointer into 'storage' */
    int          matsize[2] = {0, 0};
    int          subsizes[2] = {0, 0};
    int          starts[2] = {0, 0};
    MPI_Status   status;              /* Results of read */
    MPI_File     fh;
    MPI_Datatype subarray_t;

    MPI_Comm_rank (grid_comm, &grid_id); ID = grid_id;

    debug( "%d: inside write_checkboard_graph\n", ID ); fflush(stderr);

    MPI_Type_size(dtype, &datum_size);

    debug( "%d: grid_id = %d dtype_size = %d\n", ID, grid_id, datum_size ); fflush(stderr);

    ret = MPI_Cart_get (grid_comm, 2, grid_size, grid_period, grid_coord);
    if (ret != MPI_SUCCESS)
    {
        char string[MPI_MAX_ERROR_STRING];
        int strsz;
        MPI_Error_string(ret, string, &strsz);
        debug("%d: error: %s\n", ID );
        MPI_Finalize();
        exit(-1);
    }

    debug( "%d: g_size[R] = %d, g_size[C] = %d\n", ID, grid_size[R], grid_size[C] );
    debug( "%d: g_peri[R] = %d, g_peri[C] = %d\n", ID, grid_period[R], grid_period[C] );
    debug( "%d: g_coor[R] = %d, g_coor[C] = %d\n", ID, grid_coord[R], grid_coord[C] );
    debug( "%d: N = %d\n", ID, N );

    ret = MPI_File_open(grid_comm, s, MPI_MODE_CREATE | MPI_MODE_WRONLY, MPI_INFO_NULL, &fh);
    error_out(ret, ID, NULL);
    debug( "%d: Open file: %s\n", ID, s ); fflush(stderr);

    if (grid_id == 0)
    {
        ret = MPI_File_write(fh, &N, 1, MPI_INT, &status);
        error_out(ret, ID, &status);
        debug( "%d: write size to file: %s\n", ID, s ); fflush(stderr);
    }

    matsize[R] = N;
    matsize[C] = N;
    debug( "%d: matsize[R] = %d, matsize[C] = %d\n", ID, matsize[R], matsize[C] );

    subsizes[R] = BLOCK_SIZE(grid_coord[R], grid_size[R], matsize[R]);
    subsizes[C] = BLOCK_SIZE(grid_coord[C], grid_size[C], matsize[C]);
    debug( "%d: subsz[R] = %d, subsz[C] = %d\n", ID, subsizes[R], subsizes[C] );

    starts[R] = BLOCK_LOW(grid_coord[R], grid_size[R], matsize[R]);
    starts[C] = BLOCK_LOW(grid_coord[C], grid_size[C], matsize[C]);
    debug( "%d: starts[R] = %d, starts[C] = %d\n", ID, starts[R], starts[C] );

    debug( "%d: create_subarray\n", ID ); fflush(stderr);
    MPI_Type_create_subarray(2, matsize, subsizes, starts, MPI_ORDER_C, dtype, &subarray_t);

    debug( "%d: commit\n", ID ); fflush(stderr);
    MPI_Type_commit(&subarray_t);
    
    debug( "%d: set_view\n", ID ); fflush(stderr);
    MPI_File_set_view(fh, sizeof(int), dtype, subarray_t, "native", MPI_INFO_NULL);

    debug( "%d: write data to file\n", ID ); fflush(stderr);
    ret = MPI_File_write(fh, *storage, subsizes[R] * subsizes[C], dtype, &status);
    error_out(ret, ID, &status);

    debug( "%d: close file\n", ID ); fflush(stderr);
    ret = MPI_File_close(&fh);
    error_out(ret, ID, NULL);

    debug( "%d: free subtype\n", ID );
    ret = MPI_Type_free(&subarray_t);
    error_out(ret, ID, NULL);

    free(*storage);
    *storage = NULL;
    free(*subs);
    *subs = NULL;

    debug( "%d: write returning\n", ID ); fflush(stderr);
}
