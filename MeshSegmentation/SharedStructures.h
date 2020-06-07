// ShareStructures.h


#ifndef ShareStructures_H
#define ShareStructures_H

#define PI 3.14159265


class IndexList
{
public:
    int index;
    IndexList *next;

	bool     IsInList(int index, IndexList *plist);
	void     DeleteIndexList(IndexList **plist, int len);
	void     DeleteIndexList(IndexList *plist);
};

/*
struct IndexList
{
	int index;
	IndexList *next;
};

struct PointList
{
	int x;
	int y;
	PointList *next;
};*/

struct NeighborOfVertice
{
	int index;
	NeighborOfVertice *next;
	int m;
	double dist;
};

class SparseMatrix
{
public:
	float        data;
	int          column;
    SparseMatrix *next;

	void         ConstructSparseMatrix(SparseMatrix *p, float data, int column);
//	void         ConstructTransposedSparseMatrix(SparseMatrix **coefmatrix, SparseMatrix **transposedcoefmatrix, int row);
	void         ConstructTransposedSparseMatrix(SparseMatrix **coefmatrix, SparseMatrix **transposedcoefmatrix, int row, int column);
	void         ConstructNormalSparseMatrix(SparseMatrix **transposedcoefmatrix, SparseMatrix **normalcoefmatrix, int size);
	void         ConstructNormalBvector(SparseMatrix **transposedcoefmatrix, double *bvector, double *normalbvector, int size);

    void		 InsertElement(SparseMatrix *a, SparseMatrix *p);
    void		 DeleteElement(SparseMatrix *a, SparseMatrix *p);

	void		 DeleteSparseMatrix(SparseMatrix **sparsematrix, int row);

	void         AddSparseMatrices(SparseMatrix **sparsematrix1, SparseMatrix **sparsematrix2, int row);
	SparseMatrix **MultiplySparseMatrices(SparseMatrix **matrix1, SparseMatrix **transposedmatrix2, int row, int column);

	void         PrintSparseMatrix(SparseMatrix **sparsematrix, int row, int column);
	void         OutputSparseMatrix(char *filename, SparseMatrix **sparsematrix, int row, int column);

protected:
private:
};

#endif