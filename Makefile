all: clean titanSerial rheaSerial titanOMP rheaOMP titanHybrid rheaHybrid

.PHONY: setFlags clean

CFLAGS=""

#Set CFLAGS for titan
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
#Set CFLAGS for rhea
ifdef PGI
	CFLAGS = -mp
endif
ifdef GCC
	CFLAGS = -fopenmp
endif
ifdef INTEL_LICENSE_FILE
	CFLAGS = -openmp
endif

titanSerial: serialdemandel.c tiff.c
	cc serialdemandel.c tiff.c -ltiff -I=/ccs/home/$(USER)/lib/libtiff/include -L=/ccs/home/$(USER)/lib/libtiff/lib -o a.out
	cp a.out $(MEMBERWORK)/stf007

rheaSerial: serialdemandel.c tiff.c
	cc serialdemandel.c tiff.c -ltiff -o a.out
	cp a.out $(MEMBERWORK)/stf007

titanOMP: mpmandel.c tiff.c
	cc $(CFLAGS) mpmandel.c tiff.c -ltiff -I=/ccs/home/$(USER)/lib/libtiff/include -L=/ccs/home/$(USER)/lib/libtiff/lib -o a.out
	cp a.out $(MEMBERWORK)/stf007

rheaOMP: mpmandel.c tiff.c
	mpicc $(CFLAGS) mpmandel.c tiff.c -ltiff -o a.out
	cp a.out $(MEMBERWORK)/stf007

titanHybrid: mandel.c tiff.c
	cc $(CFLAGS) mandel.c tiff.c -ltiff -I=/ccs/home/$(USER)/lib/libtiff/include -L=/ccs/home/$(USER)/lib/libtiff/lib -o a.out
	cp a.out $(MEMBERWORK)/stf007

rheaHybrid: mandel.c tiff.c
	mpicc $(CFLAGS) mandel.c tiff.c -ltiff -o a.out
	cp a.out $(MEMBERWORK)/stf007

clean:
	rm -rf *.o
