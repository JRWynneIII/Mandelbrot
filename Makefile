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

titanSerial: serial_mandelbrot.c tiff.c
	cc serial_mandelbrot.c tiff.c -ltiff -I=/ccs/home/$(USER)/lib/libtiff/include -L=/ccs/home/$(USER)/lib/libtiff/lib -o mandelbrot
	cp mandelbrot $(MEMBERWORK)/<PROJID>

rheaSerial: serial_mandelbrot.c tiff.c
	cc serial_mandelbrot.c tiff.c -ltiff -o mandelbrot
	cp mandelbrot $(MEMBERWORK)/<PROJID>

titanOMP: openmp_mandelbrot.c tiff.c
	cc $(CFLAGS) openmp_mandelbrot.c tiff.c -ltiff -I=/ccs/home/$(USER)/lib/libtiff/include -L=/ccs/home/$(USER)/lib/libtiff/lib -o mandelbrot
	cp mandelbrot $(MEMBERWORK)/<PROJID>

rheaOMP: openmp_mandelbrot.c tiff.c
	mpicc $(CFLAGS) openmp_mandelbrot.c tiff.c -ltiff -o mandelbrot
	cp mandelbrot $(MEMBERWORK)/<PROJID>

titanHybrid: openmp_mpi_mandelbrot.c tiff.c
	cc $(CFLAGS) openmp_mpi_mandelbrot.c tiff.c -ltiff -I=/ccs/home/$(USER)/lib/libtiff/include -L=/ccs/home/$(USER)/lib/libtiff/lib -o mandelbrot
	cp mandelbrot $(MEMBERWORK)/<PROJID>

rheaHybrid: openmp_mpi_mandelbrot.c tiff.c
	mpicc $(CFLAGS) openmp_mpi_mandelbrot.c tiff.c -ltiff -o mandelbrot
	cp mandelbrot $(MEMBERWORK)/<PROJID>

clean:
	rm -rf *.o
