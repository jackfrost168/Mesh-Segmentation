// VariationTest.cpp

#include "stdafx.h"
#include "Cluster.h"
#include "MathFunctions.h"
#include "Vector3D.h"


#include <gsl/gsl_eigen.h>
#include <gsl/gsl_permutation.h>
#include <gsl/gsl_linalg.h>
#include <gsl/gsl_vector.h>
#include <gsl/gsl_matrix.h>



void Cluster::DrawCube(float cubecenter[3], float dim)
{	
	glBegin(GL_QUADS);	  
	  glNormal3f(1.0, 0.0, 0.0);
	  glVertex3f(cubecenter[0]+dim, cubecenter[1]-dim, cubecenter[2]+dim);
	  glVertex3f(cubecenter[0]+dim, cubecenter[1]-dim, cubecenter[2]-dim);
	  glVertex3f(cubecenter[0]+dim, cubecenter[1]+dim, cubecenter[2]-dim);
	  glVertex3f(cubecenter[0]+dim, cubecenter[1]+dim, cubecenter[2]+dim);	  
	glEnd();

	glBegin(GL_QUADS);
	  glNormal3f(-1.0, 0.0, 0.0);
	  glVertex3f(cubecenter[0]-dim, cubecenter[1]-dim, cubecenter[2]-dim);
	  glVertex3f(cubecenter[0]-dim, cubecenter[1]-dim, cubecenter[2]+dim);
	  glVertex3f(cubecenter[0]-dim, cubecenter[1]+dim, cubecenter[2]+dim);
	  glVertex3f(cubecenter[0]-dim, cubecenter[1]+dim, cubecenter[2]-dim);
	glEnd();

	glBegin(GL_QUADS);
	  glNormal3f(0.0,-1.0, 0.0);
	  glVertex3f(cubecenter[0]+dim, cubecenter[1]-dim, cubecenter[2]-dim);
	  glVertex3f(cubecenter[0]+dim, cubecenter[1]-dim, cubecenter[2]+dim);
	  glVertex3f(cubecenter[0]-dim, cubecenter[1]-dim, cubecenter[2]+dim);
	  glVertex3f(cubecenter[0]-dim, cubecenter[1]-dim, cubecenter[2]-dim);	  
	glEnd();

	glBegin(GL_QUADS);
	  glNormal3f(0.0, 1.0, 0.0);
	  glVertex3f(cubecenter[0]+dim, cubecenter[1]+dim, cubecenter[2]+dim);
	  glVertex3f(cubecenter[0]+dim, cubecenter[1]+dim, cubecenter[2]-dim);
	  glVertex3f(cubecenter[0]-dim, cubecenter[1]+dim, cubecenter[2]-dim);
	  glVertex3f(cubecenter[0]-dim, cubecenter[1]+dim, cubecenter[2]+dim);
	glEnd();

	glBegin(GL_QUADS);
	  glNormal3f(0.0, 0.0, 1.0);
	  glVertex3f(cubecenter[0]+dim, cubecenter[1]-dim, cubecenter[2]+dim);
	  glVertex3f(cubecenter[0]+dim, cubecenter[1]+dim, cubecenter[2]+dim);
	  glVertex3f(cubecenter[0]-dim, cubecenter[1]+dim, cubecenter[2]+dim);
	  glVertex3f(cubecenter[0]-dim, cubecenter[1]-dim, cubecenter[2]+dim);
	glEnd();

    glBegin(GL_QUADS);
	  glNormal3f(0.0, 0.0,-1.0);
	  glVertex3f(cubecenter[0]+dim, cubecenter[1]+dim, cubecenter[2]-dim);
	  glVertex3f(cubecenter[0]+dim, cubecenter[1]-dim, cubecenter[2]-dim);
	  glVertex3f(cubecenter[0]-dim, cubecenter[1]-dim, cubecenter[2]-dim);
	  glVertex3f(cubecenter[0]-dim, cubecenter[1]+dim, cubecenter[2]-dim);	  
	glEnd();
}

void Cluster::DrawLocalRegion(GLMmodel *meshmodel)
{
	if (verticesrecord)
	{
		IndexList *p1;
		p1 = verticesrecord;

		int i;
		i = 0;

		while (p1)
		{
			if (i == 0)
			{
				glColor3f(1.0, 0.0, 0.0);
			}
			else
			{
				glColor3f(1.0, 1.0, 0.0);
			}

			DrawCube(meshmodel->vertices+3*p1->index, 0.01);

			p1 = p1->next;
			i++;
		}
	}
}

void Cluster::VarianceTest(GLMmodel *meshmodel, IndexList **plist, int *pnum)
{
	IndexList *p1, *p2, *p3;
	verticesrecord = p1 = new IndexList;
	p1->index = 1;
	p1->next = NULL;

	IndexList *pvr, *pnl;
	pvr = verticesrecord;

	int numvertices = 1;

	float localcenter[3], localcenterrecord[3];
	localcenter[0] = meshmodel->vertices[3*verticesrecord->index];
	localcenter[1] = meshmodel->vertices[3*verticesrecord->index+1];
	localcenter[2] = meshmodel->vertices[3*verticesrecord->index+2];

	float **difmatrix;
	difmatrix = new float * [3];

	for (int i = 0; i < 3; i++)
	{
		difmatrix[i] = NULL;
	}

	double covariancematrix[9], eval[3], evec[3][3];
	float localvariance, threshold = 0.01;

	FILE *fp;
    char path[] = "E:\\September\\output.txt"; //the path should be right 
    fp = fopen(path, "w"); 
	
	while (pvr)
	{
		pnl = plist[pvr->index];

		while (pnl)
		{
			if(!IsInList(pnl->index, verticesrecord))
			{		
				localcenterrecord[0] = localcenter[0];
				localcenterrecord[1] = localcenter[1];
				localcenterrecord[2] = localcenter[2];

				Multiply(localcenter, numvertices);

				localcenter[0] += meshmodel->vertices[3*pnl->index];
				localcenter[1] += meshmodel->vertices[3*pnl->index+1];
				localcenter[2] += meshmodel->vertices[3*pnl->index+2];

				Multiply(localcenter, (float)1/(numvertices+1));

				for (int i = 0; i < 3; i++)
				{
					difmatrix[i] = new float [numvertices+1];
				}

				p3 = verticesrecord;
				int i = 0;

				while (p3)
				{
					difmatrix[0][i] = meshmodel->vertices[3*p3->index] - localcenter[0];
					difmatrix[1][i] = meshmodel->vertices[3*p3->index+1] - localcenter[1];
					difmatrix[2][i] = meshmodel->vertices[3*p3->index+2] - localcenter[2];

					p3 = p3->next;
					i++;
				}

				difmatrix[0][i] = meshmodel->vertices[3*pnl->index] - localcenter[0];
				difmatrix[1][i] = meshmodel->vertices[3*pnl->index+1] - localcenter[1];
				difmatrix[2][i] = meshmodel->vertices[3*pnl->index+2] - localcenter[2];

				for (int i = 0; i < 3; i++)
				{
					for (int j = 0; j < 3; j++)
					{
						covariancematrix[3*i+j] = 0.0;

						for (int k = 0; k <= numvertices; k++)
						{
							covariancematrix[3*i+j] += difmatrix[i][k] * difmatrix[j][k];
						}
					}
				}

				Eigen_value_vector(covariancematrix, 3, eval, evec);
				localvariance = eval[2]/(eval[0]+eval[1]+eval[2]);
				
				if (localvariance < threshold)
				{
				    fprintf(fp, "\n%f, %f, %f, %f", eval[0], eval[1], eval[2], localvariance);

					p2 = new IndexList;
					p2->index = pnl->index;

					p1->next = p2;
					p1 = p2;
					p1->next = NULL;

					numvertices++;
				}
				else
				{
					localcenter[0] = localcenterrecord[0];
					localcenter[1] = localcenterrecord[1];
					localcenter[2] = localcenterrecord[2];
				}

				for (int i = 0; i < 3; i++)
				{
					delete []difmatrix[i];
					difmatrix[i] = NULL;
				}								
			}

			pnl = pnl->next;
		}

		pvr = pvr->next;
	}

	printf("\nnumvertices = %d", numvertices);

	fclose(fp);
	
	delete []difmatrix;
	difmatrix = NULL;
}

void Cluster::Eigen_value_vector(double *data, int dimension, double peval[3], double pevec[3][3])
{
  gsl_matrix_view m
    = gsl_matrix_view_array(data, dimension, dimension);

  gsl_vector *eval = gsl_vector_alloc (dimension);
  gsl_matrix *evec = gsl_matrix_alloc (dimension, dimension);

  gsl_eigen_symmv_workspace * w =
    gsl_eigen_symmv_alloc (dimension);

  gsl_eigen_symmv (&m.matrix, eval, evec, w);

  gsl_eigen_symmv_free (w);

  gsl_eigen_symmv_sort (eval, evec,
                        GSL_EIGEN_SORT_ABS_DESC);

  for (int i = 0; i < dimension; i++)
  {
      peval[i] = gsl_vector_get(eval, i);

	  for (int j = 0; j < dimension; j++)
 	  {
		  pevec[i][j] = gsl_matrix_get(evec, j, i);
	  }
  }
}

void Cluster::ApproximationEvaluation(GLMmodel *mesh1, GLMmodel *mesh2)
{
	int samplingrate = 10;
		
	numsamplingvertices = (int)mesh1->numvertices
		                 +(int)(0.5*(samplingrate-2)*(samplingrate-1)*mesh1->numtriangles);

	float *samplingvertices;	
	samplingvertices = new float [3*numsamplingvertices];

	int i;
	for(i = 0; i < (int)mesh1->numvertices; i++)  
	{
		for(int k = 0; k < 3; k++)
		{
			samplingvertices[3*i+k] = mesh1->vertices[3*(i+1)+k];
		}		
	}

	int indexv[3];
	for(int j = 0; j < (int)mesh1->numtriangles && i < numsamplingvertices; j++)  
	{
		for(int k = 0; k < 3; k++)
		{
			indexv[k] = mesh1->triangles[j].vindices[k];
		}

		for (int l = 1; l < samplingrate; l++)
		{
			for (int m = 1; m < samplingrate-l; m++)
			{
				for(int k = 0; k < 3; k++)
				{
					samplingvertices[3*i+k] = i*mesh1->vertices[3*indexv[0]+k]
						                     +j*mesh1->vertices[3*indexv[1]+k]
											 +(samplingrate-i-j)*mesh1->vertices[3*indexv[2]+k];
				}
				i++;
			}
		}
	}

	ConstructKdtree(samplingvertices, numsamplingvertices);

	int *validnofneighbour, **neighbourhoods;

	validnofneighbour = new int [(int)mesh2->numvertices+1];
	neighbourhoods = new int * [(int)mesh2->numvertices+1];

	float errorvec[3];

	for(int i = 1; i <= (int)mesh2->numvertices; i++)
	{
		kdtree->queryPosition(Vector3D(mesh2->vertices+3*i));

		validnofneighbour[i] = kdtree->getNOfFoundNeighbours();     
	    if (validnofneighbour[i] > 5)  validnofneighbour[i] = 5;

	    neighbourhoods[i] = new int [validnofneighbour[i]];

		for(int j = 0; j < validnofneighbour[i]; j++)
		{
			neighbourhoods[i][j] = kdtree->getNeighbourPositionIndex(j);
		}

		for(int j = 0; j < 3; j++)
		{
			errorvec[j] = mesh2->vertices[3*i+j] - samplingvertices[3*neighbourhoods[i][0]+j];			
		}

		approximationerror += Length(errorvec)*Length(errorvec);
	}

	if (kdtree)
	{
		delete []kdtree;
		kdtree = NULL;
	}
}

void Cluster::ConstructKdtree(float *vertices, int numvertices)
{
	Vector3D *positionrecord;
	positionrecord = new Vector3D [numvertices];

	for(int i = 0; i < numvertices; i++)  
	{
        positionrecord[i].p[0] = vertices[3*i];
		positionrecord[i].p[1] = vertices[3*i+1];
		positionrecord[i].p[2] = vertices[3*i+2];
	}

	kdtree = new KdTree(positionrecord, numvertices, 10);
    kdtree->setNOfNeighbours(5);

	delete []positionrecord;
	positionrecord = NULL;
}

bool Cluster::IsInTriangle(GLMmodel *pmesh, int indexv1, int indexv2,
						   int *indexv3, int indext)
{
	bool flag = true;

	if (((int)pmesh->triangles[indext].vindices[0] == indexv1 && (int)pmesh->triangles[indext].vindices[1] == indexv2)
	   ||((int)pmesh->triangles[indext].vindices[0] == indexv2 && (int)pmesh->triangles[indext].vindices[1] == indexv1))
	{
		*indexv3 = (int)pmesh->triangles[indext].vindices[2];
	}
	else if (((int)pmesh->triangles[indext].vindices[0] == indexv1 && (int)pmesh->triangles[indext].vindices[2] == indexv2)
	        ||((int)pmesh->triangles[indext].vindices[0] == indexv2 && (int)pmesh->triangles[indext].vindices[2] == indexv1))
	{
		*indexv3 = (int)pmesh->triangles[indext].vindices[1];
	}
	else if (((int)pmesh->triangles[indext].vindices[1] == indexv1 && (int)pmesh->triangles[indext].vindices[2] == indexv2)
	        ||((int)pmesh->triangles[indext].vindices[1] == indexv2 && (int)pmesh->triangles[indext].vindices[2] == indexv1))
	{
		*indexv3 = (int)pmesh->triangles[indext].vindices[0];
	}
	else
	{
		flag = false;
	}

	return flag;
}

void Cluster::ComputeCotangentWeight(float *pcotangentweight, int indexv, int *indexcw,
									 GLMmodel *meshmodel, IndexList **verticesvindices,
									 int *numofneighboringvertices, IndexList **verticestindices)
{
	int indexlr, k;
	float dot, crossprod[3], sum = 0.0;
	IndexList *p2, *p3;

	p2 = verticesvindices[indexv];		
	while (p2)
	{
		pcotangentweight[*indexcw] = 0.0;			
		k = 0;

		p3 = verticestindices[indexv];
		while (p3)
		{
			if (IsInTriangle(meshmodel, indexv, p2->index, &indexlr, p3->index))
			{
				dot = Dot(meshmodel->vertices+3*indexlr,
						  meshmodel->vertices+3*indexv,
						  meshmodel->vertices+3*p2->index);

				CrossProd(meshmodel->vertices+3*indexlr,
						  meshmodel->vertices+3*indexv,
						  meshmodel->vertices+3*p2->index,
						  crossprod);				

				pcotangentweight[*indexcw] += dot/Length(crossprod);
				
				k++;

				if (k == 2)
				{						
					break;						
				}
			}
							
			p3 = p3->next;
		}						

		sum += pcotangentweight[*indexcw];

		p2 = p2->next;
		(*indexcw)++;
	}

	for (k = (*indexcw-numofneighboringvertices[indexv]); k < *indexcw; k++)
	{
		pcotangentweight[k] /= sum;
	}
}

void Cluster::ConstructLaplacianCoordinatesCotangentWeight(float *plc, float *pcw, int *indexcw, int indexv,
														   IndexList **verticesvindices, float *vertices)
{
	for (int i = 0; i < 3; i++)
	{
		plc[i] = vertices[3*indexv+i];
	}
	
	IndexList *p1 = verticesvindices[indexv];

	while (p1)
	{
		for (int i = 0; i < 3; i++)
		{
			plc[i] -= pcw[*indexcw] * vertices[3*p1->index+i];
		}

		p1 = p1->next;
		(*indexcw)++;
	}	
}

void Cluster::ComputeMeanCurvature(GLMmodel *meshmodel, IndexList **verticesvindices,
								   int *numofneighboringvertices, IndexList **verticestindices)
{
	int numcotangentweight = 0, indexcw;
	float *cotangentweight, *laplaciancoordinates;

	for (int i = 1; i <= (int)meshmodel->numvertices; i++)
	{
		numcotangentweight += numofneighboringvertices[i];
	}

	cotangentweight = new float [numcotangentweight];

	indexcw = 0;
	for (int i = 1; i <= (int)meshmodel->numvertices; i++)
	{
		ComputeCotangentWeight(cotangentweight, i, &indexcw, meshmodel, verticesvindices,
			                   numofneighboringvertices, verticestindices);
	}

	laplaciancoordinates = new float [3*(int)meshmodel->numvertices];
	meancurvature = new float [(int)meshmodel->numvertices+1];

	indexcw = 0;
	for (int i = 1; i <= (int)meshmodel->numvertices; i++)
	{
		ConstructLaplacianCoordinatesCotangentWeight(laplaciancoordinates+3*(i-1),
			                                         cotangentweight, &indexcw, i,
													 verticesvindices, meshmodel->vertices);

		meancurvature[i] = Length(laplaciancoordinates+3*(i-1));
/*
		if (meancurvature[i] > maxcurvature)
		{
			maxcurvature = meancurvature[i];
		}

		if (meancurvature[i] < mincurvature)
		{
			mincurvature = meancurvature[i];
		}

		averagecurvature += meancurvature[i];*/
	}
/*
	float threshold;
	threshold = mincurvature + 0.5*(maxcurvature-mincurvature);

//	printf("\n%f, %d", averagecurvature, (int)meshmodel->numvertices);

	averagecurvature /= (int)meshmodel->numvertices;

//	printf("\n%f, %f, %f, %f", maxcurvature, mincurvature, threshold, averagecurvature);
	
	IndexList *p1, *p2;

	for (i = 1; i <= (int)meshmodel->numvertices; i++)
	{
		if (meancurvature[i] > 2.0*averagecurvature)
		{
			if (!highcurvaturevertices)
			{
				highcurvaturevertices = p1 = new IndexList;
			}
			else
			{
				p2 = new IndexList;

				p1->next = p2;
				p1 = p2;
			}

			p1->index = i;
			p1->next = NULL;
		}
	}
*/
	delete []cotangentweight;
	delete []laplaciancoordinates;
}

void Cluster::DrawHighCurvatureVertices(GLMmodel *meshmodel)
{
	if (highcurvaturevertices)
	{
		IndexList *p1;
		
		glColor3f(1.0, 0.0, 0.0);

		p1 = highcurvaturevertices;
		
		while (p1)
		{
			DrawCube(meshmodel->vertices+3*p1->index, 0.01);

			p1 = p1->next;
		}
	}
}