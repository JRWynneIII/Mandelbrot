Mandelbrot
==========

Creates an image (TIFF) of a mandelbrot fractal. Parallelized using MPI and OpenMP. The resolution is changed by editing the `nx` and `ny` variables. To run, compile using `make titanHybrid` (for hybrid openMP MPI version on titan. Substitute `make lensHybrid` for lens or rhea) then from an interactive job, run the command `aprun -n<number of processes> ./a.out`

NOTE:
  libTIFF must be compiled and availible in your home directory (for Titan) or on the machine (Lens and Rhea). If you need to build libTIFF yourself, use the Ruby script in the buildTiff directory. 
```chmod +x buildTiff.rb
```./buildTiff.rb
