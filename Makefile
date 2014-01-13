all: clean titanSerial lensSerial titanOMP lensOMP titanHybrid lensHybrid

.PHONY: setFlags clean

CFLAGS=""

#setFlags:
ifdef CRAY_PRGENVGNU
	CFLAGS = -std=c99 -fopenmp
endif
ifdef CRAY_PRGENVINTEL
	CFLAGS = -openmp
endif
ifdef CRAY_PRGENVPGI
	CFLAGS = -mp
endif
ifdef CRAY_PRGENVCRAY
	CFLAGS = -h omp
endif

titanSerial: serialdemandel.c tiff.c
	cc serialdemandel.c tiff.c -ltiff -I=/ccs/home/$(USER)/lib/libtiff/include -L=/ccs/home/$(USER)/lib/libtiff/lib -o a.out
	cp a.out $(MEMBERWORK)/stf007

lensSerial: serialdemandel.c tiff.c
	cc serialdemandel.c tiff.c -ltiff -o a.out
	cp a.out $(MEMBERWORK)/stf007

titanOMP: mpmandel.c tiff.c
	cc $(CFLAGS) mpmandel.c tiff.c -ltiff -I=/ccs/home/$(USER)/lib/libtiff/include -L=/ccs/home/$(USER)/lib/libtiff/lib -o a.out
	cp a.out $(MEMBERWORK)/stf007

lensOMP: mpmandel.c tiff.c
	cc $(CFLAGS) mpmandel.c tiff.c -ltiff -o a.out
	cp a.out $(MEMBERWORK)/stf007

titanHybrid: mandel.c tiff.c
	cc $(CFLAGS) mandel.c tiff.c -ltiff -I=/ccs/home/$(USER)/lib/libtiff/include -L=/ccs/home/$(USER)/lib/libtiff/lib -o a.out
	cp a.out $(MEMBERWORK)/stf007

lensHybrid: mandel.c tiff.c
	cc $(CFLAGS) mandel.c tiff.c -ltiff -o a.out
	cp a.out $(MEMBERWORK)/stf007

clean:
	rm -rf *.o
