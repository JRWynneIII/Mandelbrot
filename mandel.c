#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <omp.h>

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
	int pixelset[nx*ny];
	int counter = 0;

	FILE *pFile = fopen("output.ppm", "w");
	fprintf(pFile, "P6\n%d %d\n255\n",nx, ny);
	fclose(pFile);

	FILE *f = fopen("output.ppm", "a");

	for (iy = 0; iy <= (ny-1); iy++)
	{		
		cy = (ymin + iy * (ymax - ymin) /(double)(ny-1));
		for (ix = 0; ix <= (nx-1); ix++)
		{
				
			x = 0.0;
			y = 0.0;
			x2 = 0.0;
			y2 = 0.0;
			iter = 0;

			cx = (xmin + ix * (xmax - xmin) / (double)(nx-1));

			for (iter = 1; iter<maxiter; iter++)
			{
				temp =  x2-y2 +cx;
				y = 2.0*x*y+cy;
				x = temp;
				x2 = x*x;
				y2 = y*y;
				if (x*x+y*y>10000.0) break;
			}

			
			
			if (iter == maxiter)
			{
				fputc((char)0, f);
				fputc((char)0, f);
				fputc((char)0, f);
			}	//pixelset[ix] = 1;
			else if (iter <= maxiter-1 && iter > 75)
			{
				fputc((char)255, f);
				fputc((char)0, f);
				fputc((char)0, f);
			}//pixelset[ix] = 2;
			else if (iter <= 74 && iter > 50)
			{
				fputc((char)0, f);
				fputc((char)255, f);
				fputc((char)0, f);
			}//pixelset[ix] = 3;
			else if (iter <= 49 && iter > 25)
			{		fputc((char)255, f);
				fputc((char)255, f);
				fputc((char)255, f);
			}	//pixelset[ix] = 4;
			else if (iter <= 24 && iter > 20)
			{
				fputc((char)0, f);
				fputc((char)255, f);
				fputc((char)255, f);
			}//	pixelset[ix] = 5;
			else if (iter <= 19 && iter > 15)
			{
				fputc((char)100, f);
				fputc((char)0, f);
				fputc((char)0, f);
			}//	pixelset[ix] = 6;
			else if (iter <= 14 && iter > 10)
			{
				fputc((char)0, f);
				fputc((char)100, f);
				fputc((char)0, f);
			}//	pixelset[ix] = 7;
			else if (iter <= 9 && iter > 5)
			{
				fputc((char)0, f);
				fputc((char)0, f);
				fputc((char)100, f);
			}//	pixelset[ix] = 8;
			else if (iter <= 4 && iter > 2)
			{
				fputc((char)0, f);
				fputc((char)100, f);
				fputc((char)100, f);
			}//	pixelset[ix] = 9;
			else if (iter <= 2 && iter > 0)
			{
				fputc((char)100, f);
				fputc((char)100, f);
				fputc((char)100, f);
			}//	pixelset[ix] = 0;
			else
			{
				fputc((char)0, f);
				fputc((char)0, f);
				fputc((char)255, f);
			}	//pixelset[ix] = 11;
			counter++;
			if (counter <= 500)
			{
				counter = 0;
				fflush(f);	
			}
		}

	}
	fclose(f);
/*
	for (int a = 0; a<(nx*ny); a++)
	{
		printf("%d\n",pixelset[a]);

		if (pixelset[a] == 1)
		{
			fputc((char)0, f);
			fputc((char)0, f);
			fputc((char)0, f);
		}	//colorpoint(0,0,0);			
		else if (pixelset[a] == 2)
		{
			fputc((char)255, f);
			fputc((char)0, f);
			fputc((char)0, f);
		}	//colorpoint(255,0,0);
		else if (pixelset[a] == 3)
		{
			fputc((char)0, f);
			fputc((char)255, f);
			fputc((char)0, f);
		}	//colorpoint(0,255,0);
		else if (pixelset[a] == 4)
		{
			fputc((char)255, f);
			fputc((char)255, f);
			fputc((char)255, f);
		}	//colorpoint(255,255,255);
		else if (pixelset[a] == 5)
		{
			fputc((char)0, f);
			fputc((char)255, f);
			fputc((char)255, f);
		}	//colorpoint(0,255,255);
		else if (pixelset[a] == 6)
		{
			fputc((char)100, f);
			fputc((char)0, f);
			fputc((char)0, f);
		}	//colorpoint(100,0,0);
		else if (pixelset[a] == 7)
		{
			fputc((char)0, f);
			fputc((char)100, f);
			fputc((char)0, f);
		}	//colorpoint(0,100,0);
		else if (pixelset[a] == 8)
		{
			fputc((char)0, f);
			fputc((char)0, f);
			fputc((char)100, f);
		}	//colorpoint(0,0,100);
		else if (pixelset[a] == 9)
		{
			fputc((char)0, f);
			fputc((char)100, f);
			fputc((char)100, f);
		}	//colorpoint(0,100,100);
		else if (pixelset[a] == 0)
		{
			fputc((char)100, f);
			fputc((char)100, f);
			fputc((char)100, f);
		}//	colorpoint(100,100,100);
		else if (pixelset[a] == 11)
		{
			fputc((char)0, f);
			fputc((char)0, f);
			fputc((char)255, f);
		}	//colorpoint(0,0,255);
		counter++;
		if (counter <= 500)
		{
			counter = 0;
			fflush(f);	
		}
	}
*/
}
