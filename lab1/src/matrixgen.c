/*
Generate two uniformed random matrix with every element that has 
an absolute value less or equal than BD. You can change the value of 
BD in the macro.

It reads the first command line argument as the matrix size. Say if you 
want two 10 by 10 matrix, you can run it as
$matrixgen 10 
The output file is 
./data_input 
It will display the generated matrix if you pass any value to the second
command line argument, say
$matrixgen 10 p
*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MAX 100000
#define BD 10

int main (int argc, char* argv[])
{
	int i,j;
	int n;
	int temp;	
//	int A[MAX][MAX],B[MAX][MAX];
	FILE *op;
	if (argc==1)
	{
		printf("Please assign the matrix size!\n");
		return 1;
	}
	n=strtol(argv[1], NULL, 10);
	if (n>MAX)
	{
		printf("Exceed the maximum size\n");
		return 1;
	}
	op=fopen("data_input","w");
	if (op==NULL)
	{
		printf("Cant open a file!");
		return 1;
	}
		

	srand(time(NULL));
	fprintf(op,"%d\n\n",n);
	for (i=0;i<n;i++)
	{	
//			A[i][j]=rand()%(2*BD)-BD+1;
//			B[i][j]=rand()%(2*BD)-BD+1;
			for (j=0;j<n;j++)
			{
				temp=rand()%(2*BD)-BD+1;
				fprintf(op,"%d\t",temp);
			}
			fprintf(op,"\n");
	}
	fprintf(op,"\n");	
	for (i=0;i<n;i++)
	{	
//			A[i][j]=rand()%(2*BD)-BD+1;
//			B[i][j]=rand()%(2*BD)-BD+1;
			for (j=0;j<n;j++)
			{
				temp=rand()%(2*BD)-BD+1;
				fprintf(op,"%d\t",temp);
			}
			fprintf(op,"\n");
	}
	fclose(op);
	if (argc>2)
	{
		op=fopen("data_input","r");	
		if (op==NULL)
		{
			printf("Cant open a file!");		
			return 1;
		}
		fscanf(op,"%d\n",&n);
		printf("the matrix size is %d\n", n);
		printf("=====================================\n");
		printf("Matrix A is \n");			
		for (i=0;i<n;i++)
		{
			for (j=0;j<n;j++)
			{
				fscanf(op,"%d\t",&temp);
				printf("%d\t",temp);
			}
			printf("\n");
		}
		printf("=====================================\n");
		printf("Matrix B is \n");			
		for (i=0;i<n;i++)
		{
			for (j=0;j<n;j++)
			{
				fscanf(op,"%d\t",&temp);
				printf("%d\t",temp);
			}
			printf("\n");
		}
		fclose(op); 
	}
	return 0;
}
