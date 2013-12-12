all:
	cc -g -ltiff DEmandel.c tiff.c -o a.out
	cp a.out $(MEMBERWORK)
	qsub $(MEMBERWORK)/lens.pbs

