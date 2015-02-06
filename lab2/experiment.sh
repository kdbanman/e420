#!/bin/bash

# Kirby Banman
# ECE 420 Lab 1
# 29 Jan 2015
#
# File:  
#    experiment.sh
#
# Purpose:
#    gather timing test data for experimental analysis
#    
# Usage:
#    bash experiment.sh

sizes=( 50 50 50 50 50 500 500 500 500 500 )

make fwarshall
make gen

for size in "${sizes[@]}"
do
  echo "Generating matrix of size ${size}..."
  bin/gen $size
  echo "<thread count>\t<runtime>"
  for th_count in {1..50}
  do
    bin/fwarshall $th_count $size | sed -E 's/(Elapsed time for )|( threads and [0-9]* cities:)|(ms)//g' | sed 's/[0-9]* //g'
  done
done
