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

#define nx 5000		//Resolution in the X dimention
#define ny 5000		//Resolution in the Y dimention

int maxiter= 2000;			//max number of iterations to test for an escaping point
int myRank;
int commSize;

void calc_pixel_value(int calcny, int calcnx, int calcMSet[calcnx*calcny], int calcmaxiter);
void calcSet(int startIdx, int endIdx, int chunkSize);

int main(int argc, char *argv[])
{
	MPI_Init(&argc, &argv);
	int *MSet = (int*)malloc(nx*ny*sizeof(int));
	memset(MSet, 0, nx*ny*sizeof(int));
	MPI_Comm_size(MPI_COMM_WORLD, &commSize);
	MPI_Comm_rank(MPI_COMM_WORLD, &myRank);
	if (myRank == 0)
	{
		if ((nx*ny)%(commSize-1) != 0)
		{
			printf("Incompatable number of processes requested\nExiting...\n");
			exit(EXIT_FAILURE);
		}
	}
	int totalRes = (nx*ny);
	int chunkSize = totalRes/(commSize-1);

	if (myRank == 0)
		printf("Starting Calculation of Mandelbrot set...\n");

	int i;
	MPI_Barrier(MPI_COMM_WORLD);
	//calculate exactly 1/commSize to be processed on each node; send; wait for completed data
	if (myRank == 0)
	{
		int myStart = 0, myEnd = 0;
		int* tempMSet = (int*)malloc((chunkSize)*sizeof(int));
		for (i = 1; i<commSize; i++)
		{
			myStart = (chunkSize/ny)*(i-1);
			myEnd = (myStart + (chunkSize/ny));
		
			//send to each node
			//tag 0 = startIdx
			//tag 1 = endIdx
			//tag 2 = chunkSize
			
			MPI_Send(&myStart, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
			MPI_Send(&myEnd, 1, MPI_INT, i, 1, MPI_COMM_WORLD);
			MPI_Send(&chunkSize, 1, MPI_INT, i, 2, MPI_COMM_WORLD);
			printf("Sent chunk data to node %d\n", i);
		}
		int sender = -1;
		MPI_Status status;
		for (i = 1; i<commSize; i++)
		{
			MPI_Recv(tempMSet, chunkSize, MPI_INT, MPI_ANY_SOURCE, 3, MPI_COMM_WORLD, &status);
			sender = status.MPI_SOURCE;
			printf("Recieved chunk from %d\n",sender);
			memcpy(MSet+(chunkSize*(sender-1)), tempMSet, chunkSize*sizeof(int));
			printf("Memcpy'd data from rank %d into MSet\n", sender);
			sender = -1;
		}
	}	
	else if (myRank != 0)
	{
		int myStart = 0;
		int myEnd = 0;
		int chunkSize = 0;
		MPI_Status status;
		MPI_Recv(&myStart, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
		MPI_Recv(&myEnd, 1, MPI_INT, 0, 1, MPI_COMM_WORLD, &status);
		MPI_Recv(&chunkSize, 1, MPI_INT, 0, 2, MPI_COMM_WORLD, &status);
		//DO THE MANDEL
		//printf("Rank: %d\tmyStart: %d\tmyEnd:%d\tchunkSize:%d\n", myRank, myStart, myEnd, chunkSize);
		calcSet(myStart, myEnd, chunkSize);
	}
	MPI_Barrier(MPI_COMM_WORLD);
	if (myRank == 0)
	{
		printf("Starting to write image\n");
		calc_pixel_value(nx,ny,MSet,maxiter);
	}
	MPI_Finalize();
}

void calcSet(int startIdx, int endIdx, int chunkSize)
{
	int position = 0;
	printf("Calculating Mandelbrot chunk on process %d\n", myRank);
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
	int rowSize = ny/(commSize-1);
	int *localMSet = (int*)malloc((chunkSize)*sizeof(int));
	//Start OpenMP code
	int count = 0;
	#pragma omp for  
	for (iy = startIdx; iy<endIdx; iy++)
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
			//printf("localMSet[%d]\n", (ix));	
			
			if (dist < delta)
				localMSet[count*(nx)+ix] = 1;
			else
				localMSet[count*(nx)+ix] = 0;
				
			//printf("MSET:%d\n",MSet[ix][iy]);
		}
		count++;
	}
	printf("Sending calculated set back to master from rank %d\n", myRank);
	MPI_Send(localMSet, chunkSize, MPI_INT, 0, 3, MPI_COMM_WORLD);
}
