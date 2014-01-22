#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <omp.h>
#include <setjmp.h>
#include <tiffio.h>
#include <stdbool.h>
#include <float.h>

//Define x and y resolution
#define nx 1000
#define ny 1000

void calc_pixel_value(int calcny, int calcnx, int calcMSet[calcnx*calcny], int calcmaxiter);

int main(int argc, char *argv[])
{
	//Here we create a 1 dimensional array that will be the total size of the image (in pixels). Because
	//this array can get so big, we need to declare it on the heap and NOT the stack (i.e. how one would normally
	//declare an array. So we use malloc();
	int *MSet = (int*)malloc(nx*ny*sizeof(int));
	int maxiter= 2000;		
	//Here, you can change the minimum and maximum values for the "window" that the image sees. increase the values (in sync)
	//and you will zoom out. Decrease them, and you will effectively zoom into the image.
	int xmin=-3, xmax= 1; 
	int ymin=-2, ymax= 2;
	double threshold = 1.0;
	double dist = 0.0;
	int ix, iy;
	double cx, cy;
	int iter, i = 0;
	double x,y,x2,y2 = 0.0;
	double temp=0.0;
	double xder=0.0;
	double yder=0.0;
	double xorbit[maxiter+1];
	xorbit[0] = 0.0;
	double yorbit[maxiter+1];
	yorbit[0] = 0.0;
	double huge = 100000;
	bool flag = false;
	const double overflow = DBL_MAX;
	double delta = (threshold*(xmax-xmin))/(double)(nx-1);
	int size =0;
	//We use a nested loop here to effectively traverse over each part of the grid (pixel of the image) in sequence. First, the complex values of the points are
	//determined and then used as the basis of the computaion. Effectively, it will loop over each point (pixel) and according on how many iterations it takes for
	//the value that the mathematical function returns on each iteration it will determine whether or not the point "escapes" to infinity (or an arbitrarily large
	//number.) or not. If it takes few iterations to escape then it will decide that this point is NOT part of the Mandelbrot set and will put a 0 in that point's
	//index in MSet. If it takes nearly all or all of the iterations to escape, then it will decide that the point/pixel is part of the Mandelbrot set and instead
	//put a 1 in its place in MSet.
	//The use of the OpenMP pragma here will divide up the iterations between threads and execute them in parallel
	//This region is VERY easily parallelized because there is NO data shared between the loop iterations.
	#pragma omp for
	for (iy = 0; iy<ny; iy++)
	{	
		cy = ymin+iy*(ymax-ymin)/(double)(ny-1);
		for (ix = 0; ix<=(nx-1); ix++)
		{
			iter = 0;
			i = 0;
			x = 0.0;
			y = 0.0;
			x2 = 0.0;
			y2 = 0.0;
			temp = 0.0;
			xder = 0.0;
			yder = 0.0;
			dist = 0.0;
			cx = xmin +ix*(xmax-xmin)/(double)(ny-1);
			//This is the main loop that determins whether or not the point escapes or not. It breaks out of the loop when it escapes
			for (iter =0; iter<=maxiter; iter++)
			{
				temp = x2-y2 +cx;
				y = 2.0*x*y+cy;
				x = temp;
				x2 = x*x;
				y2 = y*y;
				xorbit[iter+1]=x;
				yorbit[iter+1]=y;
				if (x2+y2>huge) break;	//if point escapes then break to next loop
			}
			//if the point escapes, find the distance from the set, just incase its close to the set. if it is, it will make it part of the set.
			if (x2+y2>=huge)
			{
				xder, yder = 0;
				i = 0;
				flag = false;

				for (i=0;i<=iter && flag==false;i++)
				{
					temp = 2.0*(xorbit[i]*xder-yorbit[i]*yder)+1;
					yder = 2.0*(yorbit[i]*xder+xorbit[i]*yder);
					xder = temp;
					flag = fmax(fabs(xder), fabs(yder)) > overflow;
				}
				if (flag == false)
				{
					dist=(log(x2+y2)*sqrt(x2+y2))/sqrt(xder*xder+yder*yder); 
				}	

			}
			//Assign the appropriate values to MSet in the place relating to the point in question
			if (dist < delta)
				MSet[iy * ny + ix] = 1;
			else
				MSet[iy * ny + ix] = 0;
		}
	}
	//Finally write the image. This funcion is defined in tiff.c. Refer to that file for more indepth usage of libTiff in C.
	calc_pixel_value(nx,ny,MSet,maxiter);
}
