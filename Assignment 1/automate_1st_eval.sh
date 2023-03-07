#!/bin/bash
mpic++ ./2021201074/3/3.cpp -o 3
for n in {1..12}; 
do
    time mpirun -np $n ./3 > output_file
done