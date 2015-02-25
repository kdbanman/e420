#!/bin/bash

# Kirby Banman
# ECE 420 Lab 3
# 26 Feb 2015
#
# File:  
#    experiment.sh
#
# Purpose:
#    gather timing test data for experimental analysis
#    
# Usage:
#    bash experiment.sh

sizes=( 50 50 50 50 50 1000 1000 1000 1000 1000 )

make gaussj
make gen

for size in "${sizes[@]}"
do
  echo "Generating matrix of size ${size}..."
  bin/gen $size
  echo "<thread count>\t<runtime>"
  for th_count in {1..50}
  do
    bin/gaussj $th_count $size | sed -E 's/(Elapsed time for )|( threads and [0-9]* size:)|(ms)//g' | sed 's/ /\t/g'
  done
done
