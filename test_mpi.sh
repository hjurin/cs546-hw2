#!/bin/bash

program="mpi1"
nb_processus="4"
if [ -f $program ]
then
    for matrix_size in `seq 10 10 1000`;
    do
        echo "Computing with $program for a matrix of size $matrix_size (out of 1000)"
        mpiexec -n $nb_processus ./$program $matrix_size 0 > /dev/null
    done
else
    echo "Error: \"$program\" file does't exist"
fi
