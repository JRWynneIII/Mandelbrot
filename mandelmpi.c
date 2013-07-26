#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <omp.h>
#include <setjmp.h>
#include <tiffio.h>
#include <stdbool.h>
#include <float.h>
#include "mpi.h"
#define nx 900
#define ny 900

int MSet[nx][ny];
void calc_pixel_value(int calcnx, int calcny, int calcMSet[calcnx][calcny], int calcmaxiter);

void main(int argc, char *argv[])
{
	int xmin=-2;
        int xmax= 4; 		//low and high x-value of image window
	int ymin=-2;
        int ymax= 2;			//low and high y-value of image window
	int maxiter= 2000;			//max number of iterations
	double threshold = 1.0;
	double dist = 0.0;
	int ix, iy = 0;
	double cx, cy;
	int iter, i = 0;
	double x,y,x2,y2 = 0.0;
	double temp=0.0;
	double xder=0.0;
	double yder=0.0;
	double xorbit[maxiter+1];
	xorbit[0] = 0;
	double yorbit[maxiter+1];
	yorbit[0] = 0;
	double huge = 100000;
	bool flag = false;
	const double overflow = DBL_MAX;
	double delta = (threshold*(xmax-xmin))/(double)(nx-1);
	MPI_Request request;
	MPI_Status status;

	//Start MPI code
	int totalnodes;
	int rank;
	int size = 0;
	int ompmax = 0;
	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &totalnodes);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	int mpi_16 = ny/totalnodes;
	int mpi_16_last = 0;
	if ((ny%totalnodes-1) != 0)
	{
		int y = 0;
		y = (mpi_16*(totalnodes-1));
		mpi_16_last = (ny - y);
	}
	else
	{
		mpi_16_last = mpi_16;
	}
	

	if (rank == totalnodes-1)
	{
		ompmax = (ny-1);
		size = (rank*mpi_16);
	}
	else
	{	
		ompmax = ((rank+1)*mpi_16 -1);
		size = rank*mpi_16;
	}
	
	int recvcnts[totalnodes];
	
	int tempMSet[nx][ny];

	if (rank == 0)
	{
		for (i = 1; i<totalnodes; i++)
		{
			printf("Approaching Recv %d\n", i);
			MPI_Recv(&MSet, (nx*ny), MPI_INT, i, 0, MPI_COMM_WORLD, &status);
			printf("Recieved msg\n");
		}
	}

	else if (rank != 0)
	{
		for (i = 0; i<=totalnodes-1;i++)
		{
			if (i==rank)
				recvcnts[rank] = ompmax*(nx-1);
			else
					printf("");
		}
	
		for (iy=size; iy<=ompmax; iy++)
		{
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
				cy = ymin+iy*(ymax-ymin)/(double)(ny-1);
				cx = xmin +ix*(xmax-xmin)/(double)(ny-1);
				double tempcx = cx;
				//Make the points plot on the inverse complex plane
				//cx = cx/(cy*cy+cx*cx);
				//cy = -1*cy/(tempcx*tempcx+cy*cy);
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
						//printf("DIST:%d\n", dist);
					}	
	
				}
				
				if (dist < delta)
					tempMSet[ix][iy] = 1;
				else
					tempMSet[ix][iy] = 0;
					
				//printf("MSET:%d\n",MSet[ix][iy]);
			}
		}
	}
	if (rank == 0)
	{
	}
	if (rank != 0)
	{
		MPI_Ssend(&tempMSet, (nx*ny), MPI_INT, 0, 0, MPI_COMM_WORLD);
		//MPI_Wait(&request, &status);
		printf("Sent\n");
	}
	MPI_Finalize();
	printf("Printing tif");
	calc_pixel_value(nx,ny,MSet,maxiter);
}
