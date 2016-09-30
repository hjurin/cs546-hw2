#!/bin/bash

program="normal"
if [ -f $program ]
then
    for matrix_size in `seq 10 10 1000`;
    do
        echo "Computing with $program for a matrix of size $matrix_size (out of 1000)"
        ./$program $matrix_size 0 > /dev/null
    done
else
    echo "Error: \"$program\" file does't exist"
fi
