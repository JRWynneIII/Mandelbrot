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

titanSerial: serial_mandel.c tiff.c
	cc serial_mandel.c tiff.c -ltiff -I=/ccs/home/$(USER)/lib/libtiff/include -L=/ccs/home/$(USER)/lib/libtiff/lib -o mandelbrot
	cp mandelbrot $(MEMBERWORK)/<PROJID>

rheaSerial: serial_mandel.c tiff.c
	cc serial_mandel.c tiff.c -ltiff -o mandelbrot
	cp mandelbrot $(MEMBERWORK)/<PROJID>

titanOMP: mp_mandel.c tiff.c
	cc $(CFLAGS) mp_mandel.c tiff.c -ltiff -I=/ccs/home/$(USER)/lib/libtiff/include -L=/ccs/home/$(USER)/lib/libtiff/lib -o mandelbrot
	cp mandelbrot $(MEMBERWORK)/<PROJID>

rheaOMP: mp_mandel.c tiff.c
	mpicc $(CFLAGS) mp_mandel.c tiff.c -ltiff -o mandelbrot
	cp mandelbrot $(MEMBERWORK)/<PROJID>

titanHybrid: mandel.c tiff.c
	cc $(CFLAGS) mp_mpi_mandel.c tiff.c -ltiff -I=/ccs/home/$(USER)/lib/libtiff/include -L=/ccs/home/$(USER)/lib/libtiff/lib -o mandelbrot
	cp mandelbrot $(MEMBERWORK)/<PROJID>

rheaHybrid: mandel.c tiff.c
	mpicc $(CFLAGS) mp_mpi_mandel.c tiff.c -ltiff -o mandelbrot
	cp mandelbrot $(MEMBERWORK)/<PROJID>

clean:
	rm -rf *.o
