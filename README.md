Parallel Mandelbrot
---

###Introduction
---

This project creates an image (TIFF) of a Mandelbrot fractal. This code is parallelized using MPI and OpenMP. The resolution is changed by editing the `nx` and `ny` variables and changing `maxiter` will change how many iterations are performed before determining if the point escapes or not. The project is split up into three different files: a serial version (serial\_mandel.c), an OpenMP parallelized verion (mp\_mandel.c), and a hybrid OpenMP and MPI parallelized version (mp\_mpi\_mandel.c). The algorithm used to calculate the Mandelbrot set is the Distance Estimation method. This is a point by point calculation where each point maps to one pixel in the final image. The code will iterate over each point and run it through a set of mathematical instructions that will determine weather if the point "escapes" (takes few iterations before the value is over the escape "threshold") or weather it doesn't. If it doesn't escape, then it is considered part of the Mandelbrot set and the pixel is painted black. If the point escapes very slowly (takes many iterations to escape) then that point will be labeled as part of the fractal. This is the Distance Estimation part. By doing this, it will "reveal" more of the set than the standard method without the Distance Estimation.

For more information about the algorithm used in this code, refer to http://mrob.com/pub/muency/distanceestimator.html or http://en.wikipedia.org/wiki/Mandelbrot_set#Distance_estimates

###Building libTIFF
---
libTIFF must be compiled and availible in your home directory (for Titan). You can skip this section if you're compiling and running on Rhea. If you need to build libTIFF yourself, use the Ruby script in the buildTiff directory.

```
./buildTiff.rb
```

###Compiling on Titan
---
Before you can compile on Titan, the makefile needs to be edited and < PROJID > needs to be changed to the appropriate project id. To compile run...

`make titanSerial` (for serial version)

`make titanOMP` (for OpenMP parallelized version)

`make titanHybrid` (for hybrid OpenMP and MPI parallelized version)

###Compiling on Rhea
---
Before you can compile on Rhea, the makefile needs to be edited and < PROJID > needs to be changed to the appropriate project id. To compile on Rhea run...

`make rheaSerial` (for serial version)

`make rheaOMP` (for OpenMP parallelized version)

`make rheaHybrid` (for hybrid OpenMP and MPI parallelized verion)

NOTE: You can use the rules for compiling on Rhea on other general clusters that have libTIFF already installed in the default location.

###Running
---
This can be run from either an interactive job or by submitting a batch script. Make sure to use the correct batch script on the appropriate machine (titan.pbs for titan and rhea.pbs for rhea). Before submitting either batch script, edit the script and change the 2 instances of < PROJID > to your appropriate project id. After doing that, save and then run

`qsub titan.pbs`

for Titan. To run on Rhea, simply run

`qsub rhea.pbs`

This can also be run from an interactive job. To do this, run the command

```
qsub -I -V -A< PROJID > -lnodes=< number of nodes needed >,walltime=2:00:00
```
Don't forget to replace `< PROJID >` and `< number of nodes needed >` with your project ID and number of nodes, respectively. When in your interactive job, run

```
aprun -n<number of processes> ./mandelbrot
```

or on Rhea, run

```
mpirun -n<number of processes> mandelbrot
```

again replacing < number of processes > with the number of processes you want to run. Running this will create a TIFF image in your $MEMBERWORK/< PROJID > directory. The image will be called `output.tif` and will look like this:

![The Mandelbrot](https://raw2.github.com/JRWynneIII/Mandelbrot/master/example.png)

