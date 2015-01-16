# Default number of threads and the matrix dimension
NUM_TH="16"
SIZE="32"

gen: src/matrixgen.c
	gcc -Wall -Wextra -o bin/gen src/matrixgen.c

serial: src/serialtester.c
	gcc -Wall -Wextra -o bin/serial src/serialtester.c

matr: src/matr.c
	gcc -Wall -Wextra -lm -lpthread -o bin/matr src/matr.c

# vary params with, for example, make run NUM_TH="4" SIZE="1024"
run: matr gen
	# Generate matrix
	bin/gen $(SIZE)
	# Calculate output
	bin/matr $(NUM_TH) $(SIZE)

clean:
	rm -rf bin/*
	rm data_*

# Explicitly chosen test cases are run by the parallel and serial versions.
# The outputs are compared for each matrix.
# TODO test: matr serial
