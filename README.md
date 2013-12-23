Parallel Mandelbrot
==========

Creates an image (TIFF) of a mandelbrot fractal. Most code in this repository utilizes the Level Set or Distance Estimation method for calculating a Mandelbrot set. The two dimentional array `MSet` is used to store whether a point (pixel) is in the set or not. If it is, the point/pixel is assigned a 1, else its assigned a 0. The resolution of the image can be modified by changing the value of `nx` (for resolution in pixels in the x dimention) or `ny` (for resolution in the y dimention). This code is parallelized using MPI, OpenMP, and a combonation of both. Soon to have a GPU accelerated verion (CUDA and OpenACC). 

The libtiff library was used to create the tiff image. Included is a folder `buildTiff` in which there is an executable Ruby script that will download and build the libTIFF library in your $HOME directory. 

Make sure to use the correct flags to point to the library and headerfiles when compiling (see `MFTitan` file for an example make file. If compiling this code on Lens or Rhea, simply use the `-ltiff` flag to link in the library.

Before compiling with the makefiles (`make -f MFTitan` for the Titan supercomputer and `make -f MFLens` for the Lens or Rhea computer), edit the approriate makefile and append the project directory or other directory on ATLAS to the end of `cp a.out $(MEMBERWORK)` so that it reads something like `cp a.out $(MEMBERWORK)/<PROJID>`
