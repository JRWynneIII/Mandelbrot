Parallel Mandelbrot
---

Creates an image (TIFF) of a mandelbrot fractal. This code is parallelized using MPI and OpenMP. The resolution is changed by editing the `nx` and `ny` variables and changing `maxiter` will change how many iterations are performed before determining if the point escapes or not. 

###Building libTIFF
---
libTIFF must be compiled and availible in your home directory (for Titan). You can skip this section if you're compiling and running on Rhea. If you need to build libTIFF yourself, use the Ruby script in the buildTiff directory. 

```
chmod +x buildTiff.rb
./buildTiff.rb
```

###Compiling on Titan
---
To compile run...

`make titanSerial` (for serial version)

`make titanOMP` (for OpenMP parallelized version)

`make titanHybrid` (for hybrid OpenMP and MPI parallelized version)

###Compiling on Rhea 
---
To compile on Rhea run...

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
aprun -n<number of processes> ./a.out
```

or on Rhea, run 

```
mpirun -n<number of processes> a.out
```

again replacing < number of processes > with the number of processes you want to run. Running this will create a TIFF image in your $MEMBERWORK/< PROJID > directory. The image will look like this:

<p align="center"><img src="https://raw2.github.com/JRWynneIII/Mandelbrot/master/example.png" alt="A 9000x9000 pixel image of a Mandelbrot fractal" height="400px" width="400px"/></p>
  
