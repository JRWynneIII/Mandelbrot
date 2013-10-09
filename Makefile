all: 
	mpicc -openmp -g -ltiff DEmandel.c tiff.c -o a.out
	cp a.out /tmp/work/wyn
clean: all
	rm *.o
