#!/bin/bash

N=9
P=2

if [ ! -f "multiply_seq" ]
then
    echo "Nu exista binarul multiply_seq"
    exit
fi

if [ ! -f "multiply" ]
then
    echo "Nu exista binarul multiply"
    exit
fi

./multiply_seq $N > seq.txt
./strassen $N $P > par.txt

diff seq.txt par.txt

rm -rf seq.txt par.txt