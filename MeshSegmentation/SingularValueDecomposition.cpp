//  SingularValueDecomposition.cpp

#include "StdAfx.h"
#include <math.h>
#include "MathFunctions.h"

void FabsMaximum(double *fabsmax, int *row, int *column, double data[3][3])
{// data[][] is symmetric
	*fabsmax = 0.0;
	*row = -1;
	*column = -1;

	for (int i = 0; i < 3; i++)
 	{
 		for (int j = 0; j <= i-1; j++)
 		{
			if (*fabsmax < fabs(data[i][j]))
			{
				*fabsmax = fabs(data[i][j]);
				*row = i;
				*column = j;
			}
 		}
 	}
}

void JacobiZ(double data[3][3], double jacobimatrix[3][3])
{
	int i, j, k;
	double jacobiz[3][3];

	for (i = 0; i < 3; i++)
	{
		for (j = 0; j < 3; j++)
		{
			jacobiz[i][j] = 0.0;
		}
	}

	double cot = (data[0][0]-data[1][1])/(2*data[0][1]);

	int sign;
	if (cot >= 0.0)
	{
		sign = 1;
	}
	else
	{
		sign = -1;
	}

	double tan;
	tan = sign / (fabs(cot)+sqrt(1+cot*cot));

	jacobiz[0][0] = 1 / sqrt(1+tan*tan);
	jacobiz[0][1] = tan * jacobiz[0][0];

	jacobiz[1][1] = jacobiz[0][0];
	jacobiz[1][0] = -jacobiz[0][1];

	jacobiz[2][2] = 1.0;

	double result[3][3];

	for (i = 0; i < 3; i++)
	{
		for (j = 0; j < 3; j++)
		{
			result[i][j] = 0.0;

			for (k = 0; k < 3; k++)
			{
				result[i][j] += jacobiz[i][k] * data[k][j];
			}
		}
	}

	for (i = 0; i < 3; i++)
	{
		for (j = 0; j < 3; j++)
		{
			data[i][j] = 0.0;

			for (k = 0; k < 3; k++)
			{
				data[i][j] += result[i][k] * jacobiz[j][k];
			}
		}
	}

	for (i = 0; i < 3; i++)
	{
		for (j = 0; j < 3; j++)
		{
			result[i][j] = jacobimatrix[i][j];
		}
	}
	
	for (i = 0; i < 3; i++)
	{
		for (j = 0; j < 3; j++)
		{
			jacobimatrix[i][j] = 0.0;

			for (k = 0; k < 3; k++)
			{
				jacobimatrix[i][j] += result[i][k] * jacobiz[j][k];
			}
		}
	}
}

void JacobiY(double data[3][3], double jacobimatrix[3][3])
{
	int i, j, k;
	double jacobiy[3][3];

	for (i = 0; i < 3; i++)
	{
		for (j = 0; j < 3; j++)
		{
			jacobiy[i][j] = 0.0;
		}
	}

	double cot = (data[0][0]-data[2][2])/(2*data[0][2]);

	int sign;
	if (cot >= 0.0)
	{
		sign = 1;
	}
	else
	{
		sign = -1;
	}

	double tan;
	tan = sign / (fabs(cot)+sqrt(1+cot*cot));

	jacobiy[0][0] = 1 / sqrt(1+tan*tan);
	jacobiy[0][2] = tan * jacobiy[0][0];

	jacobiy[2][2] = jacobiy[0][0];
	jacobiy[2][0] = -jacobiy[0][2];
	jacobiy[1][1] = 1.0;

	double result[3][3];

	for (i = 0; i < 3; i++)
	{
		for (j = 0; j < 3; j++)
		{
			result[i][j] = 0.0;

			for (k = 0; k < 3; k++)
			{
				result[i][j] += jacobiy[i][k] * data[k][j];
			}
		}
	}

	for (i = 0; i < 3; i++)
	{
		for (j = 0; j < 3; j++)
		{
			data[i][j] = 0.0;

			for (k = 0; k < 3; k++)
			{
				data[i][j] += result[i][k] * jacobiy[j][k];
			}
		}
	}

	for (i = 0; i < 3; i++)
	{
		for (j = 0; j < 3; j++)
		{
			result[i][j] = jacobimatrix[i][j];
		}
	}

	for (i = 0; i < 3; i++)
	{
		for (j = 0; j < 3; j++)
		{
			jacobimatrix[i][j] = 0.0;

			for (k = 0; k < 3; k++)
			{
				jacobimatrix[i][j] += result[i][k] * jacobiy[j][k];
			}
		}
	}
}

void JacobiX(double data[3][3], double jacobimatrix[3][3])
{
	int i, j, k;
	double jacobix[3][3];

	for (i = 0; i < 3; i++)
	{
		for (j = 0; j < 3; j++)
		{
			jacobix[i][j] = 0.0;
		}
	}

	double cot = (data[1][1]-data[2][2])/(2*data[1][2]);

	int sign;
	if (cot >= 0.0)
	{
		sign = 1;
	}
	else
	{
		sign = -1;
	}

	double tan;
	tan = sign / (fabs(cot)+sqrt(1+cot*cot));

	jacobix[1][1] = 1 / sqrt(1+tan*tan);
	jacobix[1][2] = tan * jacobix[1][1];

	jacobix[2][2] = jacobix[1][1];
	jacobix[2][1] = -jacobix[1][2];
	jacobix[0][0] = 1.0;

	double result[3][3];

	for (i = 0; i < 3; i++)
	{
		for (j = 0; j < 3; j++)
		{
			result[i][j] = 0.0;

			for (k = 0; k < 3; k++)
			{
				result[i][j] += jacobix[i][k] * data[k][j];
			}
		}
	}

	for (i = 0; i < 3; i++)
	{
		for (j = 0; j < 3; j++)
		{
			data[i][j] = 0.0;

			for (k = 0; k < 3; k++)
			{
				data[i][j] += result[i][k] * jacobix[j][k];
			}
		}
	}

	for (i = 0; i < 3; i++)
	{
		for (j = 0; j < 3; j++)
		{
			result[i][j] = jacobimatrix[i][j];
		}
	}

	for (i = 0; i < 3; i++)
	{
		for (j = 0; j < 3; j++)
		{
			jacobimatrix[i][j] = 0.0;

			for (k = 0; k < 3; k++)
			{
				jacobimatrix[i][j] += result[i][k] * jacobix[j][k];
			}
		}
	}
}

void SortingDESC(double data[3][3], double jacobimatrix[3][3])
{
	double temp;

	int k;
	for (int i = 0; i < 2; i++)
	{
		k = i;
		for (int j = i+1; j < 3; j++)
		{
			if (data[k][k] < data[j][j])
			{
				k = j;
			}
		}

		temp = data[i][i];
		data[i][i] = data[k][k];
		data[k][k] = temp;

		for (int l = 0; l < 3; l++)
		{
			temp = jacobimatrix[l][i];
			jacobimatrix[l][i] = jacobimatrix[l][k];
			jacobimatrix[l][k] = temp;
		}
	}
}

void GetRotationMatrix(double *linearmatrix, double data[3][3], double jacobimatrix[3][3], double **rotationmatrix)
{
	int i, j, k;
	bool nonzero[3];	

	SortingDESC(data, jacobimatrix);

	for(i = 0; i < 3; i++)
	{
		nonzero[i] = false;

		if(data[i][i] >= 0.000001)
 		{
			nonzero[i] = true;
		}
	}

	if(!nonzero[0] || !nonzero[1] || !nonzero[2])
	{// 需要扩充为一组单位正交基
		double u[3][3];

		for(i = 0; i < 3; i++)
		{
     		if(nonzero[i])
 			{
				for(j = 0; j < 3; j++)  
				{
					u[j][i] = 0;

					for(k = 0; k < 3; k++)
					{
						u[j][i] += linearmatrix[3*j+k] * jacobimatrix[k][i];
					}

					u[j][i] /= sqrt(data[i][i]);
				}
 			}
		}

		double len;

		if(!nonzero[0])
		{
			u[0][1] = 1;
			u[1][1] = 0;
			u[2][1] = 0;

//            printf("\nsomething wrong with singularvalue");
		}

		if(!nonzero[1])
		{
			if(fabs(u[1][0]) > 0.1 || fabs(u[2][0]) > 0.1)
			{				
   			    u[0][1] = 1 - u[0][0] * u[0][0];
			    u[1][1] = 0 - u[0][0] * u[1][0]; 
			    u[2][1] = 0 - u[0][0] * u[2][0];
			}
			else 
			{
			    u[0][1] = 0 - u[1][0] * u[0][0];
			    u[1][1] = 1 - u[1][0] * u[1][0]; 
			    u[2][1] = 0 - u[1][0] * u[2][0]; 
			}

			len = (double)sqrt(u[0][1]*u[0][1] + u[1][1]*u[1][1] + u[2][1]*u[2][1]);

			u[0][1] /= len;
			u[1][1] /= len;
			u[2][1] /= len;
		}

        if(!nonzero[2])
		{
			if(fabs(u[1][0] * u[2][1] - u[1][1] * u[2][0]) > 0.1)
			{
			    u[0][2] = 1 - u[0][0] * u[0][0] - u[0][1] * u[0][1];
			    u[1][2] = 0 - u[0][0] * u[1][0] - u[0][1] * u[1][1];
			    u[2][2] = 0 - u[0][0] * u[2][0] - u[0][1] * u[2][1];
			}
			else if(fabs(u[0][0] * u[2][1] - u[0][1] * u[2][0]) > 0.1)
			{
			    u[0][2] = 0 - u[1][0] * u[0][0] - u[1][1] * u[0][1];
			    u[1][2] = 1 - u[1][0] * u[1][0] - u[1][1] * u[1][1];
			    u[2][2] = 0 - u[1][0] * u[2][0] - u[1][1] * u[2][1];
			}
			else 
			{
			    u[0][2] = 0 - u[2][0] * u[0][0] - u[2][1] * u[0][1];
			    u[1][2] = 0 - u[2][0] * u[1][0] - u[2][1] * u[1][1];
			    u[2][2] = 1 - u[2][0] * u[2][0] - u[2][1] * u[2][1];
			}

			len = (double)sqrt(u[0][2]*u[0][2] + u[1][2]*u[1][2] + u[2][2]*u[2][2]);

			u[0][2] /= len;
			u[1][2] /= len;
			u[2][2] /= len;
		}

		if (Det(linearmatrix)< 0.0)
		{
			for (int i = 0; i < 3; i++)
			{
				jacobimatrix[i][2] *= -1;
			}
		}

		for(i = 0; i < 3; i++)
		{
			for(j = 0; j < 3; j++)
			{
				rotationmatrix[i][j] = 0;

				for(k = 0; k < 3; k++)
				{
					rotationmatrix[i][j] += u[i][k] * jacobimatrix[j][k]; 
				}
			}
		}
	}
	else
	{
		double record[3][3];

		for (i = 0; i < 3; i++)
		{
			for (j = 0; j < 3; j++)
			{
				record[i][j] = 0.0;

				for (k = 0; k < 3; k++)
				{
					record[i][j] += linearmatrix[3*i+k] * jacobimatrix[k][j];
				}

				record[i][j] /= sqrt(data[j][j]);
			}
		}

		if (Det(linearmatrix)< 0.0)
		{
			for (int i = 0; i < 3; i++)
			{
				jacobimatrix[i][2] *= -1;
			}
		}

		for (i = 0; i < 3; i++)
		{
			for (j = 0; j < 3; j++)
			{
				rotationmatrix[i][j] = 0.0;

				for (k = 0; k < 3; k++)
				{
					rotationmatrix[i][j] += record[i][k] * jacobimatrix[j][k];
				}
			}
		}
	}
}

void SingularValueDecomposition(double *linearmatrix, double **rotationmatrix, double threshold)
{
	int i, j, k;
	double data[3][3], jacobimatrix[3][3];

	for (i = 0; i < 3; i++)
	{
		for (j = 0; j < 3; j++)
		{
			data[i][j] = 0.0;

			for (k = 0; k < 3; k++)
			{
				data[i][j] += linearmatrix[3*k+i] * linearmatrix[3*k+j];
			}
		}
	}

	for (i = 0; i < 3; i++)
	{
		for (j = 0; j < 3; j++)
		{
			if (i == j)
			{
				jacobimatrix[i][j] = 1.0;
			}
			else
			{
				jacobimatrix[i][j] = 0.0;
			}
		}
	}

	double fabsmax;
	int row, column/*, count = 0*/;

	FabsMaximum(&fabsmax, &row, &column, data);

	while (fabsmax > threshold)
	{
		if ((row == 0 && column == 1) || (row == 1 && column == 0))
		{
			JacobiZ(data, jacobimatrix);
		}
		else if ((row == 0 && column == 2) || (row ==2 && column == 0))
		{
			JacobiY(data, jacobimatrix);
		}
		else if((row == 2 && column == 1) || (row == 1 && column == 2))
		{
			JacobiX(data, jacobimatrix);
		}

		FabsMaximum(&fabsmax, &row, &column, data);

//		count++;
	}

//	printf("\n%f, %f, %d, %f, %f, %f", threshold, fabsmax, count, sqrt(data[0][0]), sqrt(data[1][1]), sqrt(data[2][2]));

	GetRotationMatrix(linearmatrix, data, jacobimatrix, rotationmatrix);
}
