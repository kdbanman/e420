int LoadLab3(int n, float* D, int mode)
{	
	/*
	Load the data stored in the file "data_input". mode specifies the loading content. 

	n is the size of the problem. D is the pointer to the destination array. mode specifies the target data. When mode=0, the augmented matrix of size n by (n+1) will be stored in the array D[]; when mode=1, only the coefficient matrix of size n by n will be stored in the array D[]; when mode=2, only the constant vector will be stored in the array D[]. 

	The function will first compare the n to the size stored in the file "data_input" and will exit if they don't match.
	
	When mode=0, the loaded array is [A|b], i.e., an n by (n+1) matrix, the element in the i th row and j th column will be stored in D[(n+1)*i+j].
	When mode=1, the loaded array is A, i.e., an n by n matrix, the element in the i th row and j th column will be stored in D[n*i+j].
	When mode=2, the loaded array is b, i.e., an n by 1 matrix, the element int he i th row will be stored in D[i].
	*/
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

int SaveLab3(int n, float* b)
{
	/*
	Save the result in array b[] into the file "data_output"

	n is the problem size. b is the pointer to the to be stored array. 

	b[i] will be stored in the i th row of the vector written in the output file.
	*/
        int i;
        FILE* fp;

        if ((fp=fopen("data_output", "w"))==NULL)
        {
                printf("Fail to open the file!\n");
                return 1;
        }

        fprintf(fp, "%d\n\n", n);
        for (i=0; i<n; ++i)
                fprintf(fp, "%f\n", b[i]);

        fclose(fp);
        return 0;
}
