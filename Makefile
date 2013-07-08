all:
	cc -g -ltiff DEmandel.c tiff.c -o a.out
	cp a.out /tmp/work/wyn
	qsub /tmp/work/wyn/lens.pbs
	showq -u wyn
	eog
