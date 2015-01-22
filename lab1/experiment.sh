#!/bin/bash

sizes=( 60 120 180 240 300 480 960 )
th_counts=( 1 4 9 16 25)

for size in "${sizes[@]}"
do
  echo "Generating matrices of size ${size}..."
  bin/gen $size
  for th_count in "${th_counts[@]}"
  do
    bin/matr $th_count $size
  done
done
