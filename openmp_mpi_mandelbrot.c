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

#define nx 9000		//Resolution in the X dimention
#define ny 9000		//Resolution in the Y dimention

int maxiter= 2000;			//max number of iterations to test for an escaping point
int myRank;
int commSize;

void calc_pixel_value(int calcny, int calcnx, int calcMSet[calcnx*calcny], int calcmaxiter);
void calcSet(int startIdx, int endIdx, int chunkSize);

int main(int argc, char *argv[])
{
	//First of all, initialize the MPI region. You can only call MPI functions (send, recv, gather, scatter, etc)
	//from inside this region (between MPI_Init() and MPI_Finalize())
	MPI_Init(&argc, &argv);
	//Here we create a 1 dimensional array that will be the total size of the image (in pixels). Because
	//this array can get so big, we need to declare it on the heap and NOT the stack (i.e. how one would normally
	//declare an array. So we use malloc();
	int *MSet = (int*)malloc(nx*ny*sizeof(int));
	memset(MSet, 0, nx*ny*sizeof(int));
	//Get rank number and total number of processes that were launched
	MPI_Comm_size(MPI_COMM_WORLD, &commSize);
	MPI_Comm_rank(MPI_COMM_WORLD, &myRank);
	//Check to see if the resolution and number of processes are compatable.
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
	//This only happens on the manager rank (rank 0). Basically it will compute the boundaries of the section of MSet each of the other
	//compute nodes will process and the size of the region and send them to the appropriate process. Then it will wait (block) until it recieves
	//the completed section of the array it was assigned. When its recieved it will copy it into the right place in MSet
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
		//Recieve the completed chunks and memcpy it onto MSet
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
	//This block is only exectued on the rest of the processes. First, it will recive the data from rank 0 then Do the same calculation on the smaller
	//set of points then send the completed region back to the manager rank.
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
		calcSet(myStart, myEnd, chunkSize);
	}
	MPI_Barrier(MPI_COMM_WORLD);
	//Once the final array (MSet) is completed, it will call the same tiff writing funcion from tiff.c and write the image out as output.tif
	if (myRank == 0)
	{
		printf("Starting to write image\n");
		calc_pixel_value(nx,ny,MSet,maxiter);
	}
	//End the MPI instance
	MPI_Finalize();
}

void calcSet(int startIdx, int endIdx, int chunkSize)
{
	int position = 0;
	printf("Calculating Mandelbrot chunk on process %d\n", myRank);
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
	double huge = 100000;
	bool flag = false;
	const double overflow = DBL_MAX;
	double delta = (threshold*(xmax-xmin))/(double)(nx-1);
	int rowSize = ny/(commSize-1);
	int *localMSet = (int*)malloc((chunkSize)*sizeof(int));
  int count = 0;
  #pragma omp parallel //firstprivate(iy,ix,iter,i,x,y,x2,y2,temp,xder,yder,dist,cy,cx,count)
  {
	  double xorbit[maxiter+1];
	  xorbit[0] = 0.0;
	  double yorbit[maxiter+1];
	  yorbit[0] = 0.0;
  	//Start OpenMP code
  	//We use a nested loop here to effectively traverse over each part of the grid (pixel of the image) in sequence. First, the complex values of the points are
  	//determined and then used as the basis of the computaion. Effectively, it will loop over each point (pixel) and according on how many iterations it takes for
  	//the value that the mathematical function returns on each iteration it will determine whether or not the point "escapes" to infinity (or an arbitrarily large
  	//number.) or not. If it takes few iterations to escape then it will decide that this point is NOT part of the Mandelbrot set and will put a 0 in that point's
  	//index in MSet. If it takes nearly all or all of the iterations to escape, then it will decide that the point/pixel is part of the Mandelbrot set and instead
  	//put a 1 in its place in MSet.
  	//The use of the OpenMP pragma here will divide up the iterations between threads and execute them in parallel
  	//This region is VERY easily parallelized because there is NO data shared between the loop iterations.
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
  			//This is the main loop that determins whether or not the point escapes or not. It breaks out of the loop when it escapes
  			for (iter =0; iter<maxiter; iter++)
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
  				localMSet[count*(nx)+ix] = 1;
  			else
  				localMSet[count*(nx)+ix] = 0;
  				
  		}
  		count++;
  	}
  }
	printf("Sending calculated set back to master from rank %d\n", myRank);
	//Send the array back to be memcpy'ed into MSet
	MPI_Send(localMSet, chunkSize, MPI_INT, 0, 3, MPI_COMM_WORLD);
}
