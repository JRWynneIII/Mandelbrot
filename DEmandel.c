#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <omp.h>
#include <setjmp.h>
#include "tiffio.h"
#include <stdbool.h>

double MSetDist(double cx, double cy, int maxiter);

void main()
{
	int nx=1000, ny=1000;			//Image resolution: x,y
	int MSet[nx][ny];
	int xmin=-3, xmax= 1; 		//low and high x-value of image window
	int ymin=-2, ymax= 2;			//low and high y-value of image window
	int maxiter= 100;			//max number of iterations
	int threshold;
	double dist;
	int ix, iy;
	double cx, cy;
	int iter;
	double x, y, x2, y2;		//point coordinates and squares
	double temp;
		
	int delta = threshold*(xmax-xmin)/(nx-1);
	for (iy=0; iy<ny; iy++)
	{
		cy = ymin+iy*(ymax-ymin)/(ny-1);
		for (ix = 0; ix<nx; ix++)
		{
			cx = xmin +ix*(xmax-xmin)/(ny-1);
			dist = MSetDist(cx,cy,maxiter);
			if (dist <= delta)
				MSet[ix][iy] = 1;
			else
				MSet[ix][iy] = 0;
			printf("%d\n", MSet[ix][iy]);
		}
	}
	calc_pixel_value(nx,ny,MSet,maxiter);
}

double MSetDist(double cx, double cy, int maxiter)
{
	int iter, i = 0;
	double x,y,x2,y2 = 0.0;
	int temp;
	int xder;
	double yder;
	double xorbit[maxiter+1];
	double yorbit[maxiter+1];
	double dist = 0.0;
	long huge = 100000;
	bool flag;
	const long overflow;

	while (iter<maxiter && (x2+y2 < huge))
	{
		temp = x2-y2 +cx;
		y = 2*x*y+cy;
		x = temp;
		x2 = x*x;
		y2 = y*y;
		iter++;
		xorbit[iter] = x;
		yorbit[iter] = y;
	}

	if (x2+y2>huge)
	{
		xder = yder = 0;
		i = 0;
		flag = false;
		while (i<iter && flag!=true)
		{
			temp = 2*(xorbit[i]*xder-yorbit[i]*yder)+1;
			yder = 2*(yorbit[i]*xder+xorbit[i]*yder);
			xder = temp;
			flag = fmax(fabs(xder), fabs(yder)) > overflow;
			i++;
		}
		if (x2+y2>huge)
		{
			dist = log(x2+y2)*sqrt(x2+y2)/sqrt(xder*xder+yder*yder);  
		}	
	}
	return dist;
}
