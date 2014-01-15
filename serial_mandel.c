#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <omp.h>
#include <setjmp.h>
#include "tiffio.h"
#include <stdbool.h>
#include <float.h>

//Define x and y resolution
#define nx 5000
#define ny 5000

void calc_pixel_value(int calcny, int calcnx, int calcMSet[calcnx*calcny], int calcmaxiter);

void main(int argc, int *argv[])
{
	//MSet is where the info for the image is stored. 1 means its part of the fractal, 0 
	//means it isn't
	int *MSet = (int*)malloc(nx*ny*sizeof(int));
	int maxiter= 2000;			//max number of iterations
	int xmin=-3, xmax= 1; 		//low and high x-value of image window
	int ymin=-2, ymax= 2;			//low and high y-value of image window
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

	//Use a nested for loop here because we're technically working with a 2d array (image/plane)
	for (iy=0; iy<=(ny-1); iy++)
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
			//Determine if the point escapes or not
			for (iter =0; iter<=maxiter; iter++)
			{
				//Begin normal mandel level set process
				temp = x2-y2 +cx;
				y = 2.0*x*y+cy;
				x = temp;
				x2 = x*x;
				y2 = y*y;
				xorbit[iter+1]=x;
				yorbit[iter+1]=y;
				if (x2+y2>huge) break;	//if point escapes then break to next loop
			}
			//if the point escapes, find the distance from the set
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

			if (dist < delta)
				MSet[iy * ny + ix] = 1;
			else
				MSet[iy * ny + ix] = 0;
		}
	}
	//Finally write the image
	calc_pixel_value(nx,ny,MSet,maxiter);
}
