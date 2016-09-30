CC=gcc
MPICC=mpicc
CFLAGS=-std=c99 -Wall
MPICCFLAGS=$(CFLAGS)

.PHONY: default all clean mrproper strats

default: build

build: normal mpi pthread

normal:
	$(CC) $(CFLAGS) gauss_normal.c -o normal

mpi:
	$(MPICC) $(MPICCFLAGS) gauss_mpi1.c -o mpi1

pthread:
	$(CC) $(CFLAGS) -pthread gauss_pt1.c -o pt1
	$(CC) $(CFLAGS) -pthread gauss_pt2.c -o pt2
	$(CC) $(CFLAGS) -pthread gauss_pt3.c -o pt3

mrproper:
	rm -rf *.csv
	rm -rf normal*
	rm -rf mpi*
	rm -rf pt*
	rm -rf *~
	rm -rf \#*
