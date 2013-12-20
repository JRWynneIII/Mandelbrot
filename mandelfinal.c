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

#define nx 1000
#define ny 1000
#define maxiter 2000

double* xorbit;
double* yorbit;
int* MSet;
int* recievecounts;
int* displs;

void calc_pixel_value(int calcny, int calcnx, int calcMSet[calcnx*calcny], int calcmaxiter);

int main(int argc, char *argv[])
{
	MSet = (int*)malloc(nx*ny*sizeof(int));
	xorbit = (double*)malloc((maxiter+1)*sizeof(double));
	yorbit = (double*)malloc((maxiter+1)*sizeof(double));
	printf("Calculating Mandelbrot...\n");
	double start, end;
	MPI_Init(&argc, &argv);
	start = MPI_Wtime();
	int numMpiSections; 
	int myRank;
	MPI_Comm_size(MPI_COMM_WORLD,&numMpiSections);
	MPI_Comm_rank(MPI_COMM_WORLD,&myRank);
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
	xorbit[0] = 0.0;
	yorbit[0] = 0.0;
	double huge = 100000;
	bool flag = false;
	const double overflow = DBL_MAX;
	double delta = (threshold*(xmax-xmin))/(double)(nx-1);
	int yLowBound = 0;
	int yHighBound = 0;
	int *localMSet;
	recievecounts = malloc(sizeof(int)*numMpiSections);
	displs = malloc(sizeof(int)*numMpiSections);
	if (myRank == 0)
	{
		int sum = 0;
		for (i = 0; i < numMpiSections - 1; i++)
		{
			displs[i] = sum;//(i*recievecounts[i]+1);
			if (myRank == numMpiSections - 1 && myRank !=0)
				recievecounts[i] = (nx*ny)/(numMpiSections-1);// (ny-(ny/(numMpiSections-1)));
			else if (myRank == 0)
				recievecounts[i] = 0;
			else
				recievecounts[i] = (nx*ny)/(numMpiSections-1);// (ny-(ny/(numMpiSections-1)));
			sum += recievecounts[i];
		}
	}
	if (myRank > 0)
	{

		//Split by MPI rank
		int mpiSize = ny/(numMpiSections-1);
		
		if (myRank <= numMpiSections - 2)
		{
			yLowBound = (myRank-1)*mpiSize;
			yHighBound = yLowBound + mpiSize;
			yHighBound -= 1;
	
		}
		else if (myRank == numMpiSections-1)	//give last rank the rest | master should have none
		{
			yLowBound = (ny-mpiSize);	
			yHighBound = (ny);
		}
		localMSet = (int*)malloc(((nx*ny)/numMpiSections)*sizeof(int));
		//Split into 16 OMP pieces
		int ompSize = 0;
		int ompYHighBound = 0;
		int ompYLowBound = 0;
		ompYLowBound = yHighBound/16;
		if ((yHighBound%16) != 0)
		{
			int y = 0;
			y = (ompYLowBound*15);
			ompYHighBound = yHighBound - y;
		}
		else
		{
			ompYHighBound = (yHighBound/16);
		}
	
		int ompmax = 0;
		//Start OpenMP code
		#pragma omp parallel shared(localMSet) firstprivate(ompSize,iter,ompmax,cx,cy,ix,iy,i,x,y,x2,y2,temp,xder,yder,dist,yorbit,xorbit,flag) num_threads(16)
		{
			if (omp_get_thread_num() == 15)
			{
				ompmax = (ompYHighBound-1);
				ompSize = (omp_get_thread_num()*ompYHighBound);
			}
			else
			{
				ompmax = (omp_get_thread_num()+1)*ompYLowBound - 1;
				ompSize = omp_get_thread_num()*ompYLowBound;
			}
		
			for (iy=ompSize; iy<=ompmax; iy++)
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
						localMSet[ix+iy] = 1;
					else
						localMSet[ix+iy] = 0;
			
					//printf("MSET:%d\n",MSet[ix][iy]);
				}
			}
		}
		for (i = 0; i<(nx*ny)/numMpiSections; i++)
		{
			if (localMSet[i] != 0)
				printf("localMSet[%d]: %d\n", i, localMSet[i]);
		}
		free(xorbit);
		free(yorbit);
		MPI_Gatherv(localMSet, recievecounts[myRank], MPI_INT, MSet, recievecounts, recievecounts, MPI_INT,0, MPI_COMM_WORLD);
		printf("Passed Gather\n");
	}
	MPI_Barrier(MPI_COMM_WORLD);
	if (myRank == 0)
	{
		calc_pixel_value(nx,ny,MSet,maxiter);
		end = MPI_Wtime();
		int t = end-start;
		printf("Time elapsed: %d", t);
	}
	MPI_Finalize();
}
