#!/bin/bash

program="pt"
for i in `seq 1 3`;
do
    if [ -f $program$i ]
    then
        for matrix_size in `seq 10 10 1000`;
        do
            echo "Computing with $program$i for a matrix of size $matrix_size (out of 1000)"
            ./$program$i $matrix_size 0 > /dev/null
        done
    else
        echo "Error: \"$program$i\" file does't exist"
    fi
done
