/*
Serial Version Implementation and verification program. 

math.h is include, you need to add the -lm compile flag.

	In this program, the sizes of the matrix in the file "data_input" and "data_output" are checked at fisrt and will exit if those sizes are not match. Then the serial version of the Gaussian-Jordan Elimination is implemented on the data in the file "data_input" and gets the solution. It then calulate the 2-D norm of the difference between the solutions calculated by the serial program and stored in the file "data_output". Finally this norm is divided by the 2-D norm of the solution by serial code. The result is accepted if this quotient is less than the TOL defined in the macro. 
*/
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define TOL 0.0005

int LoadLab3(int n, float* D, int mode);

int main()
{
	int i,j,k,n;
	float* Au;
	float *X;
	float temp, error, Xnorm;
	int* index;
	FILE* fp;

	/*Load the datasize and verify it*/
	if ((fp=fopen("data_input","r"))==NULL)
	{
		printf("Fail to open the input data file!\n");
		return 1;
	}
	fscanf(fp, "%d\n\n", &n);
	fclose(fp);
	if ((fp=fopen("data_output","r"))==NULL)
	{
		printf("Fail to open the result data file!\n");
		return 2;
	}
	fscanf(fp, "%d\n\n", &i);
	if (i!=n)
	{
		printf("The problem size of the input file and result file does not match!\n");
		return -1;
	}
	/*Calculate the solution by serial code*/
	Au=malloc(n*(n+1)*sizeof(float));
	X=malloc(n*sizeof(float));
	index=malloc(n*sizeof(int));
	for (i=0; i<n; ++i)
		index[i]=i;

	LoadLab3(n, Au, 0);
	if (n==1)
		X[0]=Au[1]/Au[0];
	else
	{
		/*Gaussian elimination*/
		for (k=0; k<n-1; ++k)
		{
			/*Pivoting*/
			temp=0;
			j=0;
			for (i=k; i<n; ++i)
				if (temp<Au[index[i]*(n+1)+k]*Au[index[i]*(n+1)+k])
				{
					temp=Au[index[i]*(n+1)+k]*Au[index[i]*(n+1)+k];
					j=i;
				}
			if (j!=k)/*swap*/
				{
					i=index[j];
					index[j]=index[k];
					index[k]=i;
				}
			/*calculating*/
			for (i=k+1; i<n; ++i)
			{
				temp=Au[index[i]*(n+1)+k]/Au[index[k]*(n+1)+k];
				for (j=k; j<n+1; ++j)
					Au[index[i]*(n+1)+j]-=Au[index[k]*(n+1)+j]*temp;
			}		
		}
		/*Jordan elimination*/
		for (k=n-1; k>0; --k)
		{
			for (i=k-1; i>=0; --i )
			{
				temp=Au[index[i]*(n+1)+k]/Au[index[k]*(n+1)+k];
				Au[index[i]*(n+1)+k]-=temp*Au[index[k]*(n+1)+k];
				Au[index[i]*(n+1)+n]-=temp*Au[index[k]*(n+1)+n];
			} 
		}
		/*solution*/
		for (k=0; k<n; ++k)
			X[k]=Au[index[k]*(n+1)+n]/Au[index[k]*(n+1)+k];
	}

	/*compare the solution*/
	error=0;
	Xnorm=0;	
	for (i=0; i<n; ++i)
	{
		fscanf(fp, "%f\n", &temp);
		error+=(temp-X[i])*(temp-X[i]);
		Xnorm+=X[i]*X[i];
	}
	error=sqrt(error);
	Xnorm=sqrt(Xnorm);
	if (error/Xnorm<=TOL)
		printf("Congratulation! Your result is accepted!\n");
	else
		printf("Sorry, your result is wrong.\n");
	printf("The relative difference to the serial solution is %e\n", error/Xnorm);
	
	fclose(fp);
	free(X);
	free(Au);
	free(index);
	return 0;	
}

int LoadLab3(int n, float* D, int mode)
{	
	FILE* fp;
	int i,j;
	float temp;

	if ((fp=fopen("data_input","r"))==NULL)
	{
		printf("Fail to load the input data!\n");
		return 1;
	}
	fscanf(fp, "%d\n\n", &i);
	if (i!=n)
	{
		printf("Problem size does not match!\n");
		fclose(fp);
		return 2;
	}
	switch(mode)
	{
		case 0:/*Load the augemented matrix*/
		{
			for (i=0; i<n; ++i)
			{
				for (j=0; j<n; ++j)
				{
					fscanf(fp, "%f\t", &temp);
					D[i*(n+1)+j]=temp;
				}
				fscanf(fp, "\n");
			}
			fscanf(fp,"\n");
			for (i=0; i<n; ++i)
			{
				fscanf(fp, "%f\n", &temp);
				D[i*(n+1)+n]=temp;
			}
		}
			break;
		case 1:/*Load the A*/
		{
			for (i=0; i<n; ++i)
                        {
                                for (j=0; j<n; ++j)
                                {
                                        fscanf(fp, "%f\t", &temp);
                                        D[i*n+j]=temp;
                                }
                                fscanf(fp, "\n");
                        }
		}
			break;
		case 2:/*Load the b*/
		{
                        for (i=0; i<n; ++i)
                        {
                                for (j=0; j<n; ++j)
                                {
                                        fscanf(fp, "%f\t", &temp);
                                }
                                fscanf(fp, "\n");
                        }
                        fscanf(fp,"\n");
                        for (i=0; i<n; ++i)
                        {
                                fscanf(fp, "%f\n", &temp);
                                D[i]=temp;
                        }
                }
			break;
		default:
		{
			printf("Please specify the loading type!\n");
			fclose(fp);
			return -1;
		}
	}
	fclose(fp);
	return 0;
}

