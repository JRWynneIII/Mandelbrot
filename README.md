Parallel Mandelbrot
---

###Introduction
---

This project creates an image of a [Mandelbrot fractal](http://en.wikipedia.org/wiki/Mandelbrot_set). This repository contains three seperate codes: a serial version, an OpenMP version, and a hybrid OpenMP and MPI version.

This project uses the [Distance Estimator](http://mrob.com/pub/muency/distanceestimator.html) method to calculate the Mandelbrot set. Each pixel requires its own independent unit of work. Because these units of work are indepenent, it is a prime canidate for parallelizaion. 

The code will perform an iterative calculation at each point that will determine weather if the point [escapes the set](http://en.wikipedia.org/wiki/Mandelbrot_set#Formal_definition). If the point doesn't escape, then it is considered part of the Mandelbrot set and the pixel is painted black.

The Distance Estimator comes into play if the point escapes very slowly (takes many iterations before it escapes). For visualization purposes, we consider these points to be part of the set. By doing this, it will "reveal" more of the set than the standard method. 


###Building libTIFF
---
libTIFF must be compiled and availible in your home directory (for Titan). You can skip this section if you are compiling and running on Rhea. If you need to build libTIFF yourself, use the Ruby script in the buildTiff directory.

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

again replacing < number of processes > with the number of processes you want to run. Running this will create a TIFF image in your $MEMBERWORK/< PROJID > directory.

The resolution can be changed by editing the `nx` and `ny` variables and changing `maxiter` will change how many iterations are performed before determining if the point escapes or not. 

The image will be called `output.tif` and will look like this:

![The Mandelbrot](https://raw2.github.com/JRWynneIII/Mandelbrot/master/example.png)

