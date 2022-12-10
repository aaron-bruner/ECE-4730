#    ECE-4730     : Introduction to Parallel Systems
#    File         : Makefile
#    Assignment 3 : Fall 2022
#    ajbrune
#    C16480080
#
#    Description: This makefile is designed to make our lives much simpler and reduce typing the same commands over and over

CFLAGS = -g -Wall -Wstrict-prototypes -Wno-unknown-pragmas
PROGS = Make-Matrix Make-Vector Print-Matrix Print-Vector RMS-Serial MatrixVector-Serial Jacobi-Parallel Sequential-Iterative
LDFLAGS = -lm  
CC = gcc  
MCC = mpicc 
  
all: $(PROGS)  
 
Make-Matrix: make-matrix.c
	$(MCC) -o $@ $+ $(LDFLAGS) $(CFLAGS)

Make-Vector: make-vector.c
	$(MCC) -o $@ $+ $(LDFLAGS) $(CFLAGS)

Print-Matrix: print-matrix.c
	$(MCC) -o $@ $+ $(LDFLAGS) $(CFLAGS)

Print-Vector: print-vector.c
	$(MCC) -o $@ $+ $(LDFLAGS) $(CFLAGS)

RMS-Serial: RMS-Serial.c
	$(MCC) -o $@ $+ $(LDFLAGS) $(CFLAGS)

MatrixVector-Serial: MatrixVector-Serial.c
	$(MCC) -o $@ $+ $(LDFLAGS) $(CFLAGS)

Sequential-Iterative: Sequential-Iterative.c
	$(MCC) -o $@ $+ $(LDFLAGS) $(CFLAGS)

Jacobi-Parallel: Jacobi-Parallel.c MyMPI.c
	$(MCC) -o $@ $+ $(LDFLAGS) $(CFLAGS)

pbs:
	qsub pbs2.pbs
	qsub pbs4.pbs
	qsub pbs8.pbs
	qsub pbs16.pbs
	qsub pbs32.pbs

clean:  
	rm -f $(PROGS) *.o core* *.dat