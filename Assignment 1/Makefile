#    ECE-4730     : Introduction to Parallel Systems
#    File         : Makefile
#    Assignment 1 : Fall 2022
#    ajbrune
#    C16480080
#
#    Description: This makefile is designed to make our lives much simpler and reduce typing the same commands over and over

CFLAGS = -g -Wall -Wstrict-prototypes  
PROGS = runmain  
OBJECTS = HW04_will_jones.o functions.o  
LDFLAGS = -lm  
CC = gcc  
MCC = mpicc 
  
all: $(PROGS)  
 
make-list: make-list.o

parallel-add-list: parallel-add-list.o MyMPI.c
	$(MCC) $(LDFLAGS) -o $@ $+

print-list: print-list.o

serial-add-list: serial-add-list.o
 
clean:  
	rm -f $(PROGS) *.o core*  

# make-list.c  
# MyMPI.c  
# parallel-add-list.c    includes global_sum() 
# print-list.c  
# serial-add-list.c 
# functions.h 
# MyMPI.h 
# Makefile 