//  MathFunctions.h 
#include"assert.h"
#include <math.h>

#include <gsl/gsl_linalg.h>
#include <gsl/gsl_math.h>
#include <gsl/gsl_eigen.h>

float Dot(const float *x, const float *y);

double Dot(const double *x, const double *y);

float Dot(const float *point1, const float *point2, const float *point3);

double Dot(const double *point1, const double *point2, const double *point3);

float Det(const float *point1, const float *point2, const float *point3);

double Det(const double *point1, const double *point2, const double *point3);

float Det(float matrix[3][3]);

double Det(double matrix[3][3]);

double Det(double *matrix);

double Det(gsl_matrix *matrix);

void CrossProd(const float *point1, const float *point2, const float *point3, float *crossproduct);

void CrossProd(const double *point1, const double *point2, const double *point3, double *crossproduct);

void CrossProd(const float *vector1, const float *vector2, float *crossproduct);

void CrossProd(const double *vector1, const double *vector2, double *crossproduct);

float Length(const float *vec);

double Length(const double *vec);

void Normalize(float *vec);

void Normalize(double *vec);

void LinearOperation(const float *vector1, const float *vector2, float t, float *sum);

void LinearOperation(const double *vector1, const double *vector2, float t, double *sum);

void Multiply(float *vec, float t);

void Multiply(double *vec, double t);

void PrincipalComponentAnalysis(double *covariancematrix, double localframe[3][3]);

void SingularValueDecomposition(double *linearmatrix, double **rotationmatrix, double threshold);

void SortingDESC(double data[3][3], double jacobimatrix[3][3]);

void GetRotationMatrix(double *linearmatrix, double data[3][3], double jacobimatrix[3][3], double **rotationmatrix);

void FabsMaximum(double *fabsmax, int *row, int *column, double data[3][3]);

void JacobiZ(double data[3][3], double jacobimatrix[3][3]);

void JacobiY(double data[3][3], double jacobimatrix[3][3]);

void JacobiX(double data[3][3], double jacobimatrix[3][3]);
