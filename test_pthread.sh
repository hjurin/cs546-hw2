#!/bin/bash
if [ $# -ne 1 ]
then
    echo "Error: need to provide which solution to test (1, 2 or 3)"
    exit
fi

program="pt"
if [ -f $program$1 ]
then
    for matrix_size in `seq 10 10 1000`;
    do
        echo "Computing with $program$1 for a matrix of size $matrix_size (out of 1000)"
        ./$program$1 $matrix_size 0 > /dev/null
    done
else
    echo "Error: \"$program$1\" file does't exist"
fi
