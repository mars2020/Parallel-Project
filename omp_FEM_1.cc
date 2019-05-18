#ifdef _OPENMP
#include <omp.h>
#endif
#include <iostream>
#include <fstream>
#include <sys/time.h>
#include <time.h>
#include <stdlib.h>
#include <math.h>
#include <stdio.h>

using namespace std;

// getting accurate excecution time of code
double mysecond()
{
   struct timeval tp;
   struct timezone tzp;
   int i;
   i = gettimeofday(&tp,&tzp);
   return ( (double) tp.tv_sec + (double) tp.tv_usec * 1.e-6 );
}


int main()
{
	srand(time(0));
	int nt = 1;

	#ifdef _OPENMP
	#pragma omp parallel private(nt)
	{ nt = omp_get_num_threads(); if(nt<1) printf("NO print, OMP warmup.\n"); }
	#endif

	// Printing threads
	#ifdef _OPENMP
	#pragma omp parallel 
	{ printf("thread :%d\n",omp_get_thread_num());}
	#endif

	double start,end,time_disp,time_init,time_reorder,time_total;  // timing varialbes	
	int dofi,dofj,i;
	int dim = 3;	// degrees of freedom and dimension
	int nodes = 10000;	// Total nodes (10,000 and 100,000)
	int total_dof = nodes*dim;	// total degrees of freedom
	int* gcon = (int*) malloc(nodes*dim*sizeof(int));	// global ID's
	int known_disp = (nodes*dim)/10;	// constraints on structure
	int* disp = (int*) malloc(known_disp*sizeof(int)); // array of known nodes
	
	// Checking if there was an error with the allocation	
	if(gcon == NULL || disp == NULL)
	{
		printf("Memory not allocated\n");
	  	exit(0);
	}	
	
	// Creating array with known diplacements
 	start = mysecond();	
	#pragma omp parallel for schedule(runtime) private(i) 
	for(int i=0;i<known_disp;i++)
	{
		disp[i] = rand()%total_dof + 1; // generates the global ID 
		//cout << disp[i] << endl;
	}
	end = mysecond();
	time_disp = end-start;
	
	// Creating the global node ID's
	start = mysecond();
	#pragma omp parallel for schedule(runtime) private(i) 
	for(int i=0;i<nodes;i++)
	{  
		gcon[i*dim + 0] = 3*(i+1)-2;
		gcon[i*dim + 1] = 3*(i+1)-1;
		gcon[i*dim + 2] = 3*(i+1);
	}
	end = mysecond();
	time_init = end-start;

	// Reordering gcon with known displacements
	start = mysecond();
	#pragma omp parallel for schedule(runtime) private(i)
	for(int i=0;i<known_disp;i++){
		dofi = gcon[disp[i]-1];	// getting ID of known displacments
		for(int j=0;j<nodes;j++){
			for(int k=0;k<dim;k++){
				dofj = gcon[j*dim+k]; // checking all global ID's
				if(dofj > dofi) gcon[j*dim+k] = gcon[j*dim+k]-1; // shifting unkown global ID to the beginning of array
			}
		}
		gcon[disp[i]-1] = total_dof; // moving known disp of global ID to the end of array
	}
	end = mysecond();
	time_reorder = end-start;

	// total time	
	time_total = time_init + time_reorder;

	// Printing Timing
	printf("-------------------------------------------------\n");
	printf("Initialization of known displacements :: %1.3e\n",time_disp);
	printf("Initializing gcon		      :: %1.3e\n",time_init);
	printf("Reordering gcon			      :: %1.3e\n",time_reorder);
	printf("Total gcon time			      :: %1.3e\n",time_total);


	// appending times to file for plotting	
	ofstream myfile;
        myfile.open("chunk_default.txt",ios::out | ios::app);
    	myfile << time_init << " " << time_reorder  << " " << endl;
        myfile.close();
    
	// free memory allocation
	delete[] gcon;

	return(0);
}



