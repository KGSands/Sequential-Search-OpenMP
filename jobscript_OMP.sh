#!/bin/bash

# Execute job from current working directory
#$ -cwd

# Gives the name for output of execution
#$ -o programoutput.$JOB_ID

# Ask the scheduler for allocating 2 cores
#$ -pe smp-verbose 4

# Run 1 thread per core
export OMP_NUM_THREADS=4

# Prints date
date

# Printing starting job
echo "Start new job"

# Execute job
./execute_OMP small-inputs

# Sort and print differences
sort -k 1,1n -k 2,2n -k 3,3n result_OMP.txt > sorted_OMP.txt
diff sorted_OMP.txt small-inputs.sorted

# Prints finished job
echo "Finished job"

# Prints date
date
