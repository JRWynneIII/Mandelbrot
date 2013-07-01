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
	int maxiter= 10000;			//max number of iterations
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
	double yorbit[maxiter+1];
	long huge = 100000;
	bool flag = false;
	const double overflow = 1e300;
	double delta = threshold*(xmax-xmin)/(double)(nx-1);

	for (iy=0; iy<ny; iy++)
	{
		cy = (ymin+iy*(ymax-ymin))/(double)(ny-1);
		for (ix = 0; ix<nx; ix++)
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
			cx = (xmin +ix*(xmax-xmin))/(double)(ny-1);

			for (iter = 0; iter<maxiter && (x2+y2 < 10000.0); iter++)
			{
				//Begin normal mandel level set process
				temp = x2-y2 +cx;
				y = 2.0*x*y+cy;
				x = temp;
				x2 = x*x;
				y2 = y*y;
				xorbit[iter]=x;
				xorbit[iter]=y;
			}
			//if the point escapes, find the distance from the set
			if (x2+y2>huge)
			{
				xder, yder = 0;
				i = 0;
				flag = false;

				for (i=0;i<iter && flag==false;i++)
				{
					temp = 2.0*(xorbit[i]*xder-yorbit[i]*yder)+1;
					yder = 2.0*(yorbit[i]*xder+xorbit[i]*yder);
					xder = temp;
					flag = fmax(fabs(xder), fabs(yder)) > overflow;
				}
				if (flag == false)
				{
					dist=(log(x2+y2)*sqrt(x2+y2))/(double)(sqrt(xder*xder+yder*yder)); 
					printf("DIST:%d\n", dist);
				}	

			}
			
			if (dist < delta)
				MSet[ix][iy] = 1;
			else
				MSet[ix][iy] = 0;
		
			printf("MSET:%d\n",MSet[ix][iy]);
		}
	}
	calc_pixel_value(nx,ny,MSet,maxiter);
}
/*
double MSetDist(double cx, double cy, int maxiter)
{
	int iter, i = 0;
	double x,y,x2,y2 = 0.0;
	double temp=0.0;
	double xder=0.0;
	double yder=0.0;
	double xorbit= 0.0;
	double yorbit= 0.0;
	double dist = 0.0;
	long huge = 100000;
	bool flag = false;
	const long overflow = 400000;

	while (iter<maxiter && (x2+y2 < huge) && flag!=true)
	{
		//Begin normal mandel set point in/out process
		temp = x2-y2 +cx;
		y = 2.0*x*y+cy;
		x = temp;
		x2 = x*x;
		y2 = y*y;
		if (x2+y2>huge)			//if the point escapes, find the distance from the set
		{
			temp = 2.0*(xorbit*xder-yorbit*yder)+1;
			yder = 2.0*(yorbit*xder+xorbit*yder);
			xder = temp;
			flag = fmax(fabs(xder), fabs(yder)) > overflow;
			xorbit = x;
			yorbit = y;
			dist = log(x2+y2)*sqrt(x2+y2)/sqrt(xder*xder+yder*yder);  
			printf("%d\n", flag);
			break;
		}
		iter++;
	}

	return dist;
}*/
