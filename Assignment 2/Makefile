#    ECE-4730     : Introduction to Parallel Systems
#    File         : Makefile
#    Assignment 2 : Fall 2022
#    ajbrune
#    C16480080
#
#    Description: This makefile is designed to make our lives much simpler and reduce typing the same commands over and over

CFLAGS = -g -Wall -Wstrict-prototypes  
PROGS = make-matrix print-matrix mm-serial mm-parallel
LDFLAGS = -lm  
CC = gcc  
MCC = mpicc 
  
all: $(PROGS)  
 
make-matrixx: make-matrix.o

print-matrix: print-matrix.o

mm-serial: mm-serial.o

mm-parallel: mm-parallel.c MyMPI.c
	$(MCC) $(LDFLAGS) -o $@ $+

pbs:
	qsub pbs2.pbs
	qsub pbs4.pbs
	qsub pbs8.pbs
	qsub pbs16.pbs
	qsub pbs32.pbs

clean:  
	rm -f $(PROGS) *.o core*