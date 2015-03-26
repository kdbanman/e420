
# Kirby Banman
# ECE 420 Lab 3
# April 2015
#
# File:  
#    makefile
#
# Purpose:
#    compile, run, and test lab program and all support programs

SMALL_SIZE=5000

TESTS = $(wildcard test/*)

DEPS = bin/util.o bin/io.o bin/graph.o

trim: src/datatrim.c
	gcc -Wall -Wextra -o bin/trim src/datatrim.c

bin/util.o: src/util.c src/util.h
	gcc -Wall -Wextra -o bin/util.o -c src/util.c

bin/io.o: src/io.c src/io.h
	gcc -Wall -Wextra -o bin/io.o -c src/io.c

bin/graph.o: src/graph.c src/graph.h bin/util.o
	gcc -Wall -Wextra -o bin/graph.o -c src/graph.c

rank: src/rank.c bin/util.o bin/io.o bin/graph.o
	gcc -Wall -Wextra -o bin/rank src/rank.c $(DEPS)

run_small: trim rank
	# Generate smaller graph
	bin/trim $(SMALL_SIZE) data/full.txt data/small.txt
	# Calculate result
	bin/rank data/small.txt data/small_out.txt

clean:
	rm -rf bin/*

