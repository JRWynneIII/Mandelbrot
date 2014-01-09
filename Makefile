all: 
	mpicc -openmp -g -ltiff DEmandel.c tiff.c -o a.out
	cp a.out $(MEMBERWORK)
clean: all
	rm *.o
