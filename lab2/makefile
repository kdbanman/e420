
# Kirby Banman
# ECE 420 Lab 1
# 29 Jan 2015
#
# File:  
#    makefile
#
# Purpose:
#    compile, run, and test fwarshall.c and all support programs
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
SIZE="64"

TESTS=$(wildcard test/*)

gen: src/datagen.c
	gcc -Wall -Wextra -o bin/gen src/datagen.c

serial: src/serialtester.c
	gcc -Wall -Wextra -o bin/serial src/serialtester.c

fwarshall: src/fwarshall.c src/timer.h src/barrier.c
	gcc -Wall -Wextra -lpthread -o bin/fwarshall src/fwarshall.c

fwarshall_async: src/fwarshall_async.c src/timer.h src/barrier.c
	gcc -Wall -Wextra -lpthread -o bin/fwarshall_async src/fwarshall_async.c

# vary parameters with, for example, make run NUM_TH="16" SIZE="1024"
run: fwarshall gen
	# Generate matrix
	bin/gen $(SIZE)
	# Calculate output with parallel program
	bin/fwarshall $(NUM_TH) $(SIZE)

run_async: fwarshall_async gen
	# Generate matrix
	bin/gen $(SIZE)
	# Calculate output with parallel program
	bin/fwarshall_async $(NUM_TH) $(SIZE)
	
# vary parameters with, for example, make run NUM_TH="16" SIZE="1024"
test_run: fwarshall gen serial
	# Generate matrix
	bin/gen $(SIZE)
	# Calculate output with parallel program
	bin/fwarshall $(NUM_TH) $(SIZE)
	# Verify output
	bin/serial

# vary parameters with, for example, make run NUM_TH="16" SIZE="1024"
test_run_async: fwarshall_async gen serial
	# Generate matrix
	bin/gen $(SIZE)
	# Calculate output with parallel program
	bin/fwarshall_async $(NUM_TH) $(SIZE)
	# Verify output
	bin/serial

# Explicitly chosen test cases are run by the parallel and serial versions.
# The outputs are compared for each matrix.
# ALL TESTS RUN ASSUME PROBLEM SIZE 8.
test: matr serial $(TESTS)
	for test in test/*; do cp $$test data_input; bin/fwarshall 4 8; bin/serial; done
	
clean:
	rm -rf bin/*
	rm data_*

