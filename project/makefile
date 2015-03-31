
# Kirby Banman
# ECE 420 Lab 3
# April 2015
#
# File:  
#    makefile
#
# Purpose:
#    compile, run, and test lab program and all support programs
#

NUM_HOSTS = 5
NUM_PROCS = 40
CURRENT_MPI = bin/mpi_rank

SMALL_SIZE = 5500

TESTS = $(wildcard test/*)

DEPS = bin/util.o bin/io.o bin/graph.o

DIR = $(shell pwd)

bin/trim: src/datatrim.c
	gcc -Wall -Wextra -o bin/trim src/datatrim.c

bin/util.o: src/util.c src/util.h
	gcc -Wall -Wextra -o bin/util.o -c src/util.c

bin/io.o: src/io.c src/io.h
	gcc -Wall -Wextra -o bin/io.o -c src/io.c

bin/graph.o: src/graph.c src/graph.h bin/util.o
	gcc -Wall -Wextra -o bin/graph.o -c src/graph.c

bin/rank: src/rank.c bin/util.o bin/io.o bin/graph.o
	gcc -Wall -Wextra -o bin/rank src/rank.c $(DEPS)

run_small: bin/trim bin/rank
	# Generate smaller graph
	bin/trim $(SMALL_SIZE) data/full.txt data/small.txt
	# Calculate result
	bin/rank data/small.txt data/small_out.txt

run_full: bin/trim bin/rank
	bin/rank data/full.txt data/full_out.txt

# MPI cluster deployment
#
# run from ece420g2@e5-05-xx.ece.ualberta.ca
# 
# make hostfile [NUM_HOSTS=N]
# PASS=***** make hosts
# make run_mpi [NUM_PROCS=M]

hostfile:
	rm -f data/hostfile.txt
	for i in `shuf -i 2-20 -n $(NUM_HOSTS)`; do echo "e5-05-`if test $$i -lt 10; then echo 0; fi`$$i.ece.ualberta.ca"; done > data/hostfile.txt
	cat data/hostfile.txt

hosts:
	for host in `cat data/hostfile.txt`;  do ./deploy.exp $$PASS $$host $(CURRENT_MPI) ; done

bin/whois: src/whois.c
	mpicc -o bin/whois src/whois.c

run_whois: bin/whois
	mpirun -np $(NUM_PROCS) --hostfile data/hostfile.txt $(DIR)/whois

bin/mpi_rank: src/mpi_rank.c bin/util.o bin/io.o bin/graph.o
	mpicc -Wall -Wextra -o bin/mpi_rank src/mpi_rank.c

run_mpi: $(CURRENT_MPI)
	mpirun -np $(NUM_PROCS) --hostfile data/hostfile.txt $(DIR)/$(CURRENT_MPI) 

clean:
	rm -rf bin/*

