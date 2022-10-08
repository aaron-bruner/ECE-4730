# ECE-4730 [Introduction to Parallel Systems]
Course material for ECE-4730 at Clemson University for the Fall semester of 2022.

## Course Description
Introduces parallel computer architectures and their programming. Includes an 
introduction to MPI and OpenMP and a number of engineering problems, including numerical 
simulations. Introduces scalability analysis.

## Learning Outcomes
- Students should be able to write working parallel programs with MPI 
- Students should be able to write working parallel programs with OpenMP 
- Students should be able to code important numerical parallel algorithms involving vectors, matrices, linear systems, etc. 
- Students should be able to code important non-numerical parallel algorithms involving graphs, game trees, sorts etc. 
- Students should be able to perform performance analysis of parallel algorithms run on typical parallel machines including speedup, efficiency, and isoefficiency in order to evaluate the applicability of parallelism to the given task 
- Students should be able to recognize and discuss general architecture features of parallel machines including processor and network design. 

###### Program 1 [Global Sum]
Write an MPI Program to compute the sum of a large quantity of integers and
associated helper executables, according to the specification given below. In the parallel
version, you will use a block decomposition so that each
processor will find a partial sum of its portion of the file. Then, the final summation of
the partial sums will be done using our the global_sum() function based on the technique
given in the text and in class.
