#pragma once
#include "glm.h"
#include "KdTree.h"
#include "SharedStructures.h"

struct _cluster
{
	int index;
	int order;	//num of vertices 
	//int numMergedCluster = 1;
	bool iscluster;
	float center[3];
	float normal[3];
	float SDF;
	float AGD;
	float CF;
	//float HKS[101];
	double variation;
	_cluster *nextcluster;
	IndexList *pvertices;	//all vertex belong to this cluster
};

struct _node
{
	int order;
	int *vertices;
	float center[3];
	float normal[3];
	double variation;
	_node *pleft;
	_node *pright;
};

class Cluster
{

public:
	int numCluster, *cluOfV, maxsize;//cluOfV:vertex belongs to whcih cluster
	float *colorOfClu,weight;
	_cluster *pcluster;
	IndexList **neighborclusters;	//Adjacency Matrix
	_node *ptree;
	double maxvar,maxangle;
	GLMmodel *pnewmodel;


public:
	Cluster();
	~Cluster();
	void K_Means(GLMmodel *meshmodel,float maxerror,float *normalofvertices,float w);
	void K_Random(GLMmodel *meshmodel,float *normalofvertices);
	void Clustering(GLMmodel *meshmodel,float *normalofvertices,float w);
	float reCenter(GLMmodel *meshmodel,float *normalofvertices);
	void Color(int num);
	void Hierar(GLMmodel *meshmodel, IndexList **neighborvertices, int numcluster,float *normalofvertices,float w);
	void Merge(_cluster *pi, _cluster *pj, GLMmodel *meshmodel,float *normalofvertices);
	void insertneighbor(int index, IndexList **clusteri);
	void deleteneighbor(IndexList **clusteri, int index);
	void AdvancedHierar(GLMmodel *meshmodel,float *normalofvertices,IndexList **verticesvindices,
		                      int *numofneighboringvertices, IndexList **verticestindices);
	void Split(_node *pnode, GLMmodel *meshmodel,float *normalofvertices);
	void Tree2Cluster(_node *pnode);
	void Regiongrow(GLMmodel *meshmodel,IndexList **neighborvertices,float *normalofvertices,
					int *numofneighboringvertices, IndexList **verticestindices);
	void Growfrom(_cluster *pi,GLMmodel *meshmodel,IndexList **neighborvertices,float *normalofvertices);
	void Distribute(GLMmodel *meshmodel,float *normalofvertices);
	void deleteIndexList(IndexList *pp);
	void GetNeighbors(IndexList **neighborvertices,int numcluster);
	void GenerateNewModel(IndexList **neighborvertices);
	bool IsInList(int index, IndexList *pl);
	void GetNormals(float *normalofvertices);
	void DeleteClusters();
	void GMM(GLMmodel *meshmodel,float *normalofvertices);
	float Gaussian(float *x,float *u,float o);
	float PostProbability(int i,float *x,float *w,float *o);
	void MeanShift(GLMmodel *meshmodel,IndexList **neighborvertices,float radius,float maxerror,float *normalofvertices);
	void meanshift(_cluster *pi,GLMmodel *meshmodel,IndexList **neighborvertices,float *normalofvertices);
	void ReconstructMesh(GLMmodel *originmesh,GLMmodel *deformedmesh,IndexList *handles,IndexList *editregion,IndexList *boundary);
	void ComputeRotationMatrixPDSVD(double *LSMmatrix, int dimension,double **rotationmatrix);
	void ConstructVertexNormals(GLMmodel *meshmodel,float *normalofvertices);
	void ConstructDeformMatrix(GLMmodel *meshmodel,float *normalofdeformedmesh,float *normalofsimplifiedmesh,double **a);
	void ConstructRotationMatrix(GLMmodel *meshmodel,double ***ratationmatrix);	
	void ConstructLocalFrame(GLMmodel *meshmodel,float *normalofvertices,IndexList **neighborvertices,float **localcoordinates);
	void ReconstructMesh(GLMmodel *originmesh,GLMmodel *simplifiedmesh,GLMmodel *deformedmesh,
		                 IndexList *handles,IndexList *editregion,IndexList *boundary);
	void QuadricCenter(GLMmodel *meshmodel, GLMmodel *simplifiedMesh);
	void QuadricCenter2(GLMmodel *meshmodel, GLMmodel *simplifiedMesh);
	void SaveClusterInfo(char *filename,IndexList **neighborvertices);
	void ReadClusterInfo(char * filename,int numvertices);
	float ComputeTriangleArea(float *point1, float *point2, float *point3);
	
	//
	IndexList *verticesrecord;
	KdTree *kdtree;
	float approximationerror;
	int numsamplingvertices;
	float *meancurvature;
	IndexList *highcurvaturevertices;
	float maxcurvature;
	float mincurvature;
	float averagecurvature;
	void DrawCube(float cubecenter[3], float dim);
	void DrawLocalRegion(GLMmodel *meshmodel);
	void VarianceTest(GLMmodel *meshmodel, IndexList **plist, int *pnum);
	void Eigen_value_vector(double *data, int dimension, double peval[3], double pevec[3][3]);
	void ApproximationEvaluation(GLMmodel *mesh1, GLMmodel *mesh2);
	void ConstructKdtree(float *vertices, int numvertices);
	void Centroid(GLMmodel *meshmodel);
	void Centroid(GLMmodel *meshmodel, GLMmodel *simplifiedMesh);
	void ComputeMeanCurvature(GLMmodel *meshmodel, IndexList **verticesvindices,
		                      int *numofneighboringvertices, IndexList **verticestindices);
	void ComputeCotangentWeight(float *pcotangentweight, int indexv, int *indexcw,
		                        GLMmodel *meshmodel, IndexList **verticesvindices, 
								int *numofneighboringvertices, IndexList **verticestindices);
	void ConstructLaplacianCoordinatesCotangentWeight(float *plc, float *pcw, int *indexcw, int indexv,
		                                              IndexList **verticesvindices, float *vertices);
	bool IsInTriangle(GLMmodel *pmesh, int indexv1, int indexv2, int *indexv3, int indext);
	void DrawHighCurvatureVertices(GLMmodel *meshmodel);

};