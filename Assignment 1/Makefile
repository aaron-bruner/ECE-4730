#    ECE-4730     : Introduction to Parallel Systems
#    File         : Makefile
#    Assignment 1 : Fall 2022
#    ajbrune
#    C16480080
#
#    Description: This makefile is designed to make our lives much simpler and reduce typing the same commands over and over

CFLAGS = -g -Wall -Wstrict-prototypes  
PROGS = make-list print-list serial-add-list parallel-add-list
LDFLAGS = -lm  
CC = gcc  
MCC = mpicc 
  
all: $(PROGS)  
 
make-list: make-list.o

print-list: print-list.o

serial-add-list: serial-add-list.o

parallel-add-list: parallel-add-list.c MyMPI.c
	$(MCC) $(LDFLAGS) -o $@ $+

pbs:
	qsub pbs2.pbs
	qsub pbs4.pbs
	qsub pbs8.pbs
	qsub pbs16.pbs
	qsub pbs32.pbs

clean:  
	rm -f $(PROGS) *.o core* #ASG1_*

# make-list.c  
# MyMPI.c  
# parallel-add-list.c    includes global_sum() 
# print-list.c  
# serial-add-list.c 
# functions.h 
# MyMPI.h 
# Makefile 