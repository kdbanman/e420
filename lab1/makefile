# Default number of threads and the matrix dimension
NUM_TH="4"
SIZE="8"

TESTS=$(wildcard test/*)

gen: src/matrixgen.c
	gcc -Wall -Wextra -o bin/gen src/matrixgen.c

serial: src/serialtester.c
	gcc -Wall -Wextra -o bin/serial src/serialtester.c

matr: src/matr.c src/timer.h
	gcc -Wall -Wextra -lm -lpthread -o bin/matr src/matr.c

# vary parameters with, for example, make run NUM_TH="16" SIZE="1024"
run: matr gen serial
	# Generate matrix
	bin/gen $(SIZE)
	# Calculate output with parallel program
	bin/matr $(NUM_TH) $(SIZE)
	
# vary parameters with, for example, make run NUM_TH="16" SIZE="1024"
test_run: matr gen serial
	# Generate matrix
	bin/gen $(SIZE)
	# Calculate output with parallel program
	bin/matr $(NUM_TH) $(SIZE)
	# Verify output
	bin/serial

# Explicitly chosen test cases are run by the parallel and serial versions.
# The outputs are compared for each matrix.
test: matr serial $(TESTS)
	for test in test/*; do cp $$test data_input; bin/matr 4 8; bin/serial; done
	
clean:
	rm -rf bin/*
	rm data_*

