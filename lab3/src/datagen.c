/*
Generate the coefficient matrix (float) and constant vector (float) as the problem input for Lab3

Input:
	The first command line input will be the size of the problem. The second one is optional and the program will display the generated data when more than one command line arguments are passed.

Output:
	The problem size, a coeffient matrix with data type float and a constant vector with data type float will be stored in the file "data_input". The coeffient matrix is guaranteed to be nonsingular.

Remarks:
	The program generates two auxiliary matrices A and T first, in which most of the elements are 0 and the nonzero ones have absolute value in [0, RANGE/DECIMAL] (you can change the setting in the macro). The final matrix is A*T. The elements in the constant vector b also have absolute value in [0, RANGE/DECIMAL].
*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define DECIMAL 100.0
#define RANGE 1000


int MatMul(int, float*, float*, float*);
int MatGen(int,int*,  float*, float*);



int main (int argc, char* argv[])
{	
	int i,j,n;
	int *flag; 
	float *A, *T, *S;
	float temp;
	FILE * fp;

	if (argc==1)
	{
		printf("Please assgin the problem size!\n");
		return 1;
	}
	n=strtol(argv[1], NULL, 10);

	if ((fp=fopen("data_input","w"))==NULL)
	{
		printf("Fail to open a file. \n");
		return 2;
	}
	
	/*Generate the data*/
	A=malloc(n*n*sizeof(float));
	T=malloc(n*n*sizeof(float));
	S=malloc(n*n*sizeof(float));	
	flag=malloc(n*sizeof(int));
	for (i=0; i<n; ++i)
		flag[i]=0;
	
	for (i=0; i<n; ++i)
		for (j=0; j<n; ++j)
		{
			A[i*n+j]=0;
			T[i*n+j]=0;
		}
	srand(time(NULL));
	MatGen(n, flag, A, T);
	MatMul(n, A, T, S);
	
	/*Output the data*/
	fprintf(fp, "%d\n\n", n);
	for (i=0; i<n; ++i)
	{
		for (j=0; j<n; ++j)
			fprintf(fp, "%f\t", S[i*n+j]);
		fprintf(fp, "\n");
	}
	fprintf(fp, "\n");
	for (i=0; i<n; ++i)
	{
		temp=(float)(rand()%RANGE)/DECIMAL;
		if (random()%2)
			temp*=-1;
		fprintf(fp, "%f\n", temp);
	}
	fclose(fp);
	/*Print the result if neccesary*/
	if (argc>2)
	{
		if ((fp=fopen("data_input","r"))==NULL)
                        {
                                printf("Fail to open the saved data to display!\n");
                                return 3;
                        }
                fscanf(fp, "%d\n\n", &n);
		printf("The problem size is %d.\n", n);
		printf("============\n The A is \n============\n");
		for (i=0; i<n; ++i)
                {
                        for (j=0; j<n; ++j)
                        {
                                fscanf(fp, "%f\t", &temp);
                                printf("%f\t", temp);
                        }
                        fscanf(fp,"\n");
                        printf("\n");
                }
		fscanf(fp ,"\n");
		printf("============\n The b is \n============\n");
		for (i=0; i<n; ++i)
		{
			fscanf(fp,"%f\n", &temp );
			printf("%f\n", temp);
		}
		fclose(fp);
	}

	free(A);
	free(T);
	free(S);
	free(flag);
	return 0;
}

int MatGen(int n, int* flag, float* A, float* T)
{
	int i,j;
	int mysize=0;
	int vindex;

	for (i=0; i<n; ++i)
		mysize+=flag[i];
	mysize=n-mysize;
	
	if (mysize==1)
	{
		i=0;
		while (flag[i]==1)
			++i;
		A[(n-1)*n+i]=1.0;
		do
			T[i*n+i]=(float)(random()%RANGE)/DECIMAL;
		while(T[i*n+i]==0);
		if (random()%2)
			T[i*n+i]*=-1;
	}
	else
	{
		/*generate a v and the real v*/
		vindex=random()%mysize;
		i=0;
		j=0;
		while (flag[i]==1 || j<vindex)
		{
			if (flag[i]==0)
				++j;
			++i;
		}
		vindex=i;
		/*fill A and T according to the real v*/
		for (i=n-mysize; i<n; ++i)
		{
			A[i*n+vindex]=(float)(random()%RANGE)/DECIMAL;
			if (random()%2)
				A[i*n+vindex]*=-1;
		}
		j=0;

		A[(n-mysize)*n+vindex]=1.0;
		for (i=0; i<n; ++i)
			if (flag[i]==0)
			{
				T[vindex*n+i]=(float)(random()%RANGE)/DECIMAL;
				if (random()%2)
					T[vindex*n+i]*=-1;
			}
		flag[vindex]=1;
		MatGen(n, flag, A, T);
	}
	return 0;
}

int MatMul(int n, float* A, float* T, float* S)
{
	int i,j,k;

	
	for (i=0; i<n; ++i)
		for (j=0; j<n; ++j)
		{
			S[i*n+j]=0;
			for (k=0; k<n; ++k)
				S[i*n+j]+=A[i*n+k]*T[k*n+j];
		}
	
	return 0;
}


