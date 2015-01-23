#!/bin/bash

sizes=( 60 120 180 240 300 480 960 )
th_counts=( 1 4 9 16 25)

for th_count in "${th_counts[@]}"
do
  echo "Thread Count ${th_count}"
  for size in "${sizes[@]}"
  do
    bin/gen $size
    bin/matr $th_count $size
  done
done
