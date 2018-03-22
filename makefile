target:
	mpicc src/openmpi_pram_jhouma.c -o bitonic_sort -lm

run:
	mpirun -np 8 ./bitonic_sort 400000
