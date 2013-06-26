#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <omp.h>
#include <setjmp.h>
#include "tiffio.h"


void calc_pixel_value(int nx, int ny, int coordArray[nx][ny], int maxiter);

void colorpoint(int red, int green, int blue)
{
	FILE *f = fopen("output.ppm", "a");
	fputc((char)red, f);
	fputc((char)green, f);
	fputc((char)blue, f);
}

void main()
{
	int nx=1000, ny=1000;			//Image resolution: x,y
	int xmin=-3, xmax= 1; 		//low and high x-value of image window
	int ymin=-2, ymax= 2;			//low and high y-value of image window
	int maxiter= 100;			//max number of iterations
	int ix, iy;
	double cx, cy;
	int mlevel;
	int iter;
	double x, y, x2, y2;		//point coordinates and squares
	double temp;
	double magnify=1.0;
	int coordArray[nx][ny];
	int counter = 0;

	for (iy = 0; iy <= (ny-1); iy++)
	{		
		cy = ymin + iy * (ymax - ymin) /(double)(ny-1);
		for (ix = 0; ix <= (nx-1); ix++)
		{
				
			x = 0.0;
			y = 0.0;
			x2 = 0.0;
			y2 = 0.0;
			iter = 0;

			cx = xmin + ix * (xmax - xmin) / (double)(nx-1);

			for (iter = 1; iter<maxiter; iter++)
			{
				temp =  x2-y2 +cx;
				y = 2.0*x*y+cy;
				x = temp;
				x2 = x*x;
				y2 = y*y;
				if (x*x+y*y>10000.0) break;
			}
			coordArray[ix][iy]=iter;
		}

	}
	calc_pixel_value(nx, ny, coordArray, maxiter);

}
