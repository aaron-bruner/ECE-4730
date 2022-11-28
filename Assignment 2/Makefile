#    ECE-4730     : Introduction to Parallel Systems
#    File         : Makefile
#    Assignment 2 : Fall 2022
#    ajbrune
#    C16480080
#
#    Description: This makefile is designed to make our lives much simpler and reduce typing the same commands over and over

CFLAGS = -g -Wall #-Wstrict-prototypes -Wno-unknown-pragmas
PROGS = make-matrix print-matrix mm-serial mm-parallel
LDFLAGS = -lm  
CC = gcc  
MCC = mpicc 
  
all: $(PROGS)  
 
make-matrix: make-matrix.c
	$(MCC) -o $@ $+ $(LDFLAGS)

print-matrix: print-matrix.c
	$(MCC) -o $@ $+ $(LDFLAGS)

mm-serial: mm-serial.c
	$(MCC) -o $@ $+ $(LDFLAGS)

mm-parallel: mm-parallel.c MyMPI.c
	$(MCC) -o $@ $+ $(LDFLAGS)

pbs:
	qsub pbs2.pbs
	qsub pbs4.pbs
	qsub pbs8.pbs
	qsub pbs16.pbs
	qsub pbs32.pbs

clean:  
	rm -f $(PROGS) *.o core* *.dat