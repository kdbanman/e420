
# Kirby Banman
# ECE 420 Lab 3
# 26 February 2015
#
# File:  
#    makefile
#
# Purpose:
#    compile, run, and test lab program and all support programs
#    
# Usage:
#    EX: make run
#    EX: make run NUM_TH="16" SIZE="1024"
#    EX: make test_run
#    EX: make test_run NUM_TH="16" SIZE="1024"
#    EX: make test
#    EX: make clean


# Default number of threads and the problem size
NUM_TH="4"
SIZE="128"

TESTS=$(wildcard test/*)

gen: src/datagen.c
	gcc -Wall -Wextra -o bin/gen src/datagen.c

serial: src/serialtester.c
	gcc -Wall -Wextra -o bin/serial src/serialtester.c -lm 

gaussj: src/gaussj.c src/timer.h
	gcc -Wall -Wextra -fopenmp -o bin/gaussj src/gaussj.c -lm

# vary parameters with, for example, make run NUM_TH="16" SIZE="1024"
run: gaussj gen
	# Generate matrix
	bin/gen $(SIZE)
	# Calculate output with parallel program
	bin/gaussj $(NUM_TH) $(SIZE)

# vary parameters with, for example, make run NUM_TH="16" SIZE="1024"
test_run: gaussj gen serial
	# Generate matrix
	bin/gen $(SIZE)
	# Calculate output with parallel program
	bin/gaussj $(NUM_TH) $(SIZE)
	# Verify output
	bin/serial

clean:
	rm -rf bin/*
	rm data_*

