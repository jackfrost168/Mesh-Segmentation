//  MathFunctions.cpp

#include "stdafx.h"
#include "MathFunctions.h"


float Dot(const float *x, const float *y)
{
	return x[0]*y[0] + x[1]*y[1] + x[2]*y[2];
}

double Dot(const double *x, const double *y)
{
	return x[0]*y[0] + x[1]*y[1] + x[2]*y[2];
}

float Dot(const float *point1, const float *point2, const float *point3)
{
	float dot = (point3[0]-point1[0])*(point2[0]-point1[0])
		       +(point3[1]-point1[1])*(point2[1]-point1[1])
			   +(point3[2]-point1[2])*(point2[2]-point1[2]);
	return dot;
}

double Dot(const double *point1, const double *point2, const double *point3)
{
	double dot = (point3[0]-point1[0])*(point2[0]-point1[0])
		        +(point3[1]-point1[1])*(point2[1]-point1[1])
			    +(point3[2]-point1[2])*(point2[2]-point1[2]);
	return dot;
}

float Det(const float *point1, const float *point2, const float *point3)
{
	float det;

	det = point1[0] * (point2[1]*point3[2]-point2[2]*point3[1])
		 -point1[1] * (point2[0]*point3[2]-point2[2]*point3[0])
		 +point1[2] * (point2[0]*point3[1]-point2[1]*point3[0]);
		
	return det;
}

double Det(const double *point1, const double *point2, const double *point3)
{
	double det;

	det = point1[0] * (point2[1]*point3[2]-point2[2]*point3[1])
		 -point1[1] * (point2[0]*point3[2]-point2[2]*point3[0])
		 +point1[2] * (point2[0]*point3[1]-point2[1]*point3[0]);
	
	return det;
}

float Det(float matrix[3][3])
{
 	float det;

	det = matrix[0][0] * (matrix[1][1] * matrix[2][2] - matrix[1][2] * matrix[2][1])
 	     -matrix[1][0] * (matrix[0][1] * matrix[2][2] - matrix[0][2] * matrix[2][1])
 	     +matrix[2][0] * (matrix[0][1] * matrix[1][2] - matrix[0][2] * matrix[1][1]);

 	return det;
}

double Det(double matrix[3][3])
{
 	double det;

	det = matrix[0][0] * (matrix[1][1] * matrix[2][2] - matrix[1][2] * matrix[2][1])
 	     -matrix[1][0] * (matrix[0][1] * matrix[2][2] - matrix[0][2] * matrix[2][1])
 	     +matrix[2][0] * (matrix[0][1] * matrix[1][2] - matrix[0][2] * matrix[1][1]);

 	return det;
}

double Det(double *matrix)
{
 	double det;

	det = matrix[3*0+0] * (matrix[3*1+1] * matrix[3*2+2] - matrix[3*1+2] * matrix[3*2+1])
 	     -matrix[3*1+0] * (matrix[3*0+1] * matrix[3*2+2] - matrix[3*0+2] * matrix[3*2+1])
 	     +matrix[3*2+0] * (matrix[3*0+1] * matrix[3*1+2] - matrix[3*0+2] * matrix[3*1+1]);

 	return det;
}

double Det(gsl_matrix *matrix)
{
	double det;

	det = gsl_matrix_get (matrix, 0, 0)*(gsl_matrix_get (matrix, 1, 1)*gsl_matrix_get (matrix, 2, 2)
		                                -gsl_matrix_get (matrix, 1, 2)*gsl_matrix_get (matrix, 2, 1))
		 -gsl_matrix_get (matrix, 1, 0)*(gsl_matrix_get (matrix, 0, 1)*gsl_matrix_get (matrix, 2, 2)
		                                -gsl_matrix_get (matrix, 0, 2)*gsl_matrix_get (matrix, 2, 1))
		 +gsl_matrix_get (matrix, 2, 0)*(gsl_matrix_get (matrix, 0, 1)*gsl_matrix_get (matrix, 1, 2)
		                                -gsl_matrix_get (matrix, 0, 2)*gsl_matrix_get (matrix, 1, 1));

	return det;
}

void CrossProd(const float *point1, const float *point2, const float *point3,
			   float *crossproduct)
{	
	float vector1[3], vector2[3];

	vector1[0] = point2[0] - point1[0];
	vector1[1] = point2[1] - point1[1];
	vector1[2] = point2[2] - point1[2];

	vector2[0] = point3[0] - point1[0];
	vector2[1] = point3[1] - point1[1];
	vector2[2] = point3[2] - point1[2];

	crossproduct[0] = vector1[1]*vector2[2] - vector1[2]*vector2[1];
	crossproduct[1] = vector1[2]*vector2[0] - vector1[0]*vector2[2];
	crossproduct[2] = vector1[0]*vector2[1] - vector1[1]*vector2[0];
}

void CrossProd(const double *point1, const double *point2, const double *point3,
			   double *crossproduct)
{
	double vector1[3], vector2[3];

	vector1[0] = point2[0] - point1[0];
	vector1[1] = point2[1] - point1[1];
	vector1[2] = point2[2] - point1[2];

	vector2[0] = point3[0] - point1[0];
	vector2[1] = point3[1] - point1[1];
	vector2[2] = point3[2] - point1[2];

	crossproduct[0] = vector1[1]*vector2[2] - vector1[2]*vector2[1];
	crossproduct[1] = vector1[2]*vector2[0] - vector1[0]*vector2[2];
	crossproduct[2] = vector1[0]*vector2[1] - vector1[1]*vector2[0];
}

void CrossProd(const float *vector1, const float *vector2, float *crossproduct)
{
	crossproduct[0] = vector1[1]*vector2[2] - vector1[2]*vector2[1];
	crossproduct[1] = vector1[2]*vector2[0] - vector1[0]*vector2[2];
	crossproduct[2] = vector1[0]*vector2[1] - vector1[1]*vector2[0];
}

void CrossProd(const double *vector1, const double *vector2, double *crossproduct)
{
	crossproduct[0] = vector1[1]*vector2[2] - vector1[2]*vector2[1];
	crossproduct[1] = vector1[2]*vector2[0] - vector1[0]*vector2[2];
	crossproduct[2] = vector1[0]*vector2[1] - vector1[1]*vector2[0];
}

float Length(const float *vec)
{
	float len;

	len = (float)sqrt(vec[0]*vec[0]+vec[1]*vec[1]+vec[2]*vec[2]);

	return len;
}

double Length(const double *vec)
{
	double len;

	len = sqrt(vec[0]*vec[0]+vec[1]*vec[1]+vec[2]*vec[2]);

	return len;
}

void Normalize(float *vec)
{
	float len = Length(vec);

// 	if (len < 0.000001)
// 	{
// 		printf("\nNormalize:length too short");
// 		
// 		printf("\n%f, %f, %f", vec[0], vec[1], vec[2]);	
// 	}

	vec[0] /= len;
	vec[1] /= len;
	vec[2] /= len;
}

void Normalize(double *vec)
{
	double len = Length(vec);

// 	if (len < 0.0001)
// 	{
// 		printf("\nNormalize:length too short");
// 	}

	vec[0] /= len;
	vec[1] /= len;
	vec[2] /= len;
}

void LinearOperation(const float *vector1, const float *vector2, float t, float *sum)
{
	sum[0] = vector1[0] + t * vector2[0];
	sum[1] = vector1[1] + t * vector2[1];
	sum[2] = vector1[2] + t * vector2[2];
}

void LinearOperation(const double *vector1, const double *vector2, double t, double *sum)
{
	sum[0] = vector1[0] + t * vector2[0];
	sum[1] = vector1[1] + t * vector2[1];
	sum[2] = vector1[2] + t * vector2[2];
}

void Multiply(float *vec, float t)
{
	vec[0] *= t;
	vec[1] *= t;
    vec[2] *= t;
}

void Multiply(double *vec, double t)
{
	vec[0] *= t;
	vec[1] *= t;
    vec[2] *= t;
}

void PrincipalComponentAnalysis(double *covariancematrix, double localframe[3][3])
{
	gsl_matrix_view m = gsl_matrix_view_array (covariancematrix, 3, 3);

	gsl_vector *eval = gsl_vector_alloc (3);
	gsl_matrix *evec = gsl_matrix_alloc (3, 3);

	gsl_eigen_symmv_workspace *w = gsl_eigen_symmv_alloc (3);

	gsl_eigen_symmv (&m.matrix, eval, evec, w);

	gsl_eigen_symmv_free (w);

	gsl_eigen_symmv_sort (eval, evec, GSL_EIGEN_SORT_VAL_ASC);

	for (int i = 0; i < 3; i++)
	{
//		double eval_i = gsl_vector_get (eval, i);
//	    printf("\neigenvalue = %g", eval_i);

	    for (int j = 0; j < 3; j++)
		{
			localframe[i][j] = gsl_matrix_get (evec, j, i);
		}
	}

	gsl_vector_free (eval);
	gsl_matrix_free (evec);
}