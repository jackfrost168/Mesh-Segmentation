#include"Cluster.h"

typedef struct mycluster
{
	int index;
	int order;
	int numMergedCluster;
	//float center[3];
	//float normal[3];

	float Internal_DiffLocation;
	float Internal_DiffNormal;
	float Internal_DiffSDF;
	float Internal_DiffAGD;
	float Internal_DiffCF;

	int belongsTo;
	
	//mycluster *nextcluster;
	//IndexList *pvertices;	//all vertex belong to this cluster
};

typedef struct EdgeNode
{
	int index;
	float *weight;	//location,normal,SDF,AGD,CF
	//cluster vertex;				//edge weight
	EdgeNode *Next;	//point to next adjacent vertex
}EdgeNode;

typedef struct VertexNode	
{
	int vertex;			//vertex information, list head	
	EdgeNode *firstEdge;	//head pointer
}VertexNode, AdjList;

typedef struct GraphAdjList
{
	int numVertex;	//num of vertex
	int numEdge;		//num of edge
	AdjList *adjlist;	//define "adjacency list"
}GraphAdjList;

typedef struct SortedList
{
	float weight[6];
	int index1;
	int index2;
	SortedList *Next;

};

typedef struct SegedCluster
{
	int index;
	int origin_index;
	int belongsto;
	bool is_seed;
};

//typedef struct ClusterOfV
//{
//	int cluster;
//	bool isseed;
//};

typedef struct List
{
	int index;
	struct List *nextnode;
};

typedef struct nodegraph
{
	int index;
	int order;
	bool marked;
	struct nodegraph *Next;
};