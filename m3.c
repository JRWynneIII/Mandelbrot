#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <omp.h>
#include <setjmp.h>
#include <tiffio.h>
#include <stdbool.h>
#include <float.h>
#include <mpi.h>
#include <string.h>

#define nx 2000
#define ny 2000

int maxiter= 2000;			//max number of iterations
int myRank;
int commSize;

void calc_pixel_value(int calcny, int calcnx, int calcMSet[calcnx*calcny], int calcmaxiter);
void calcSet(int chunkSize, int* localMSet);

int main(int argc, char *argv[])
{
	int *MSet = (int*)malloc(nx*ny*sizeof(int));
	memset(MSet, 0, nx*ny*sizeof(int));
	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &commSize);
	MPI_Comm_rank(MPI_COMM_WORLD, &myRank);
	int recvcnts[commSize];
	int i;

	int *localMSet = (int*)malloc((nx*ny)/(commSize-1)*sizeof(int));
	int chunkSize = 0;
	if (myRank != 0)
	{
		calcSet(chunkSize, localMSet);
	}
	MPI_Barrier(MPI_COMM_WORLD);
       	MPI_Status status;
	if (myRank == 0)
	{
		for (i = 1; i<=(commSize-1); i++)
		{
			MPI_Recv(MSet,(nx*ny), MPI_INT, i, i, MPI_COMM_WORLD, &status);
			printf("Recieved from node %d\n", i);
		}
		//MPI_Gatherv(localMSet, (nx*ny)/(commSize-1), MPI_INT, MSet, recvcnts, 0, MPI_INT, 0, MPI_COMM_WORLD);
		calc_pixel_value(nx,ny,MSet,maxiter);
	}
	MPI_Barrier(MPI_COMM_WORLD);
	MPI_Finalize();
}
void calcSet(int chunkSize, int* localMSet)
{
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
	int size =0;
	int max_16 = (ny/16);
	int max_16_last = 0;
	//break into 16 parts 1/core
	if ((ny%16) != 0)
	{
		int y = 0;
		y = (max_16*15);
		max_16_last = (ny - y);	//
	}
	else
	{
		max_16_last = max_16;
	}
	int ompmax = 0;
	//Start OpenMP code
	#pragma omp parallel shared(localMSet) firstprivate(size,iter,ompmax,cx,cy,ix,iy,i,x,y,x2,y2,temp,xder,yder,dist,yorbit,xorbit,flag) num_threads(16)
	{
		if (omp_get_thread_num() == 15)
		{
			ompmax = (ny-1);
			size = (omp_get_thread_num()*max_16_last);
		}
		else
		{
			ompmax = (omp_get_thread_num()+1)*max_16 - 1;
			size = omp_get_thread_num()*max_16;
		}
	
		for (iy=size; iy<=ompmax; iy++)
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
					localMSet[ix * (nx-1) + iy] = 1;
				else
					localMSet[ix * (nx-1) + iy] = 0;
		
				//printf("MSET:%d\n",MSet[ix][iy]);
			}
		}
	}
	MPI_Request * request;
	MPI_Isend(localMSet, (nx*ny)/(commSize-1), MPI_INT, 0, myRank, MPI_COMM_WORLD, request);
}
