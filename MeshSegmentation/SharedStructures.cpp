// SharedStructures.cpp

#include "stdafx.h"
#include "SharedStructures.h"
#include <math.h>


void SparseMatrix::ConstructSparseMatrix(SparseMatrix *p, float data, int column)
{
	p->data = data;
	p->column = column;	
	p->next = NULL;
}
/*
void SparseMatrix::ConstructTransposedSparseMatrix(SparseMatrix **coefmatrix, SparseMatrix **transposedcoefmatrix, int row)
{
	SparseMatrix *p1, *p2, *p3;

	for (int i = 0; i < row; i++)
	{
		p1 = coefmatrix[i];

		while (p1)
		{
			if (!transposedcoefmatrix[p1->column])
			{
			    transposedcoefmatrix[p1->column] = p2 = new SparseMatrix;
			}
			else
			{
				p2 = transposedcoefmatrix[p1->column];
				
				while (p2->next)
				{
					p2 = p2->next;
				}

				p3 = new SparseMatrix;
				p2->next = p3;
				p2 = p3;
			}
			ConstructSparseMatrix(p2, p1->data, i);

			p1 = p1->next;
		}
	}
}
*/
void SparseMatrix::ConstructTransposedSparseMatrix(SparseMatrix **coefmatrix, SparseMatrix **transposedcoefmatrix, int row, int column)
{
	SparseMatrix *p1, *p2, **ptail = new SparseMatrix * [column];

	for (int i = 0; i < row; i++)
	{
		p1 = coefmatrix[i];

		while (p1)
		{
			if (!transposedcoefmatrix[p1->column])
			{
			    transposedcoefmatrix[p1->column] = ptail[p1->column] = new SparseMatrix;
			}
			else
			{
				p2 = new SparseMatrix;
				ptail[p1->column]->next = p2;
				ptail[p1->column] = p2;
			}
			ConstructSparseMatrix(ptail[p1->column], p1->data, i);

			p1 = p1->next;
		}
	}
}

void SparseMatrix::ConstructNormalSparseMatrix(SparseMatrix **transposedcoefmatrix,
											   SparseMatrix **normalcoefmatrix, int size)
{
	SparseMatrix *p3, *p4, *p5, *p6, *p7;
	float matrixdata;

	for (int i = 0; i < size; i++)
	{
		for (int j = i; j < size; j++)
		{
			matrixdata = 0.0;

			p3 = transposedcoefmatrix[i];
			p4 = transposedcoefmatrix[j];

			while (p3)
			{
				while (p4 && p4->column < p3->column)
				{
					p4 = p4->next;
				}

				if (!p4)
				{
					break;
				}

				if (p4->column == p3->column)
				{
					matrixdata += p3->data * p4->data;
				}

				p3 = p3->next;
			}

			if (fabs(matrixdata) > 0.000001) // this threshold should be the same
				                             // as the threshold in SolveSparseSystem()
			{
				if (!normalcoefmatrix[i])
				{
					normalcoefmatrix[i] = p5 = new SparseMatrix;
				}
				else
				{
					p5 = normalcoefmatrix[i];

					while (p5->next)
					{
						p5 = p5->next;
					}

					p6 = new SparseMatrix;
					p5->next = p6;
					p5 = p6;
				}
				ConstructSparseMatrix(p5, matrixdata, j); // a[i][j]

				if (i != j)
				{
					if (!normalcoefmatrix[j])
					{
						normalcoefmatrix[j] = p7 = new SparseMatrix;
					}
					else
					{
						p7 = normalcoefmatrix[j];

						while (p7->next)
						{
							p7 = p7->next;
						}

						p6 = new SparseMatrix;
						p7->next = p6;
						p7 = p6;
					}
					ConstructSparseMatrix(p7, matrixdata, i); // a[j][i]
				}
			}
		}
	}
}

void SparseMatrix::ConstructNormalBvector(SparseMatrix **transposedcoefmatrix, double *bvector,
										  double *normalbvector, int size)
{
	SparseMatrix *p3;

	for (int i = 0; i < size; i++)
	{
		normalbvector[i] = 0.0;
		p3 = transposedcoefmatrix[i];

		while (p3)
		{
			normalbvector[i] += p3->data * bvector[p3->column];

			p3 = p3->next;
		}		
	}
}

void SparseMatrix::DeleteElement(SparseMatrix *a, SparseMatrix *p)
{
	while (a->next && a->next->column != p->column)
	{
		a = a->next;
	}

	a->next = p->next;

	delete p;
	p = NULL;
}

void SparseMatrix::InsertElement(SparseMatrix *a, SparseMatrix *p)
{
	while (a->next && a->next->column < p->column)
	{
		a = a->next;
	}

	p->next = a->next;
	a->next = p;
}

void SparseMatrix::DeleteSparseMatrix(SparseMatrix **sparsematrix, int row)
{
	if (sparsematrix)
	{
		SparseMatrix *p1, *p2;

		for (int i = 0; i < row; i++)
		{
			p1 = sparsematrix[i];

			while (p1)
			{
			    p2 = p1->next;
				delete p1;
				p1 = p2;
			}

			sparsematrix[i] = NULL;
		}

		delete []sparsematrix;
		sparsematrix = NULL;
	}
}

/* If you Multiply A with B, you should provide A and transposed B */
SparseMatrix ** SparseMatrix::MultiplySparseMatrices(SparseMatrix **matrix1, SparseMatrix **transposedmatrix2, int row, int column)
{
	SparseMatrix **multipliedmatrix = new SparseMatrix * [row];
	for (int i = 0; i < row; i++)
	{
		multipliedmatrix[i] = NULL;
	}

	SparseMatrix *p3, *p4, *p5, *p6;
	float matrixdata;

	for (int i = 0; i < row; i++)
	{
		for (int j = 0; j < column; j++)
		{
			matrixdata = 0.0;

			p3 = matrix1[i];
			p4 = transposedmatrix2[j];

			while (p3)
			{
				while (p4 && p4->column < p3->column)
				{
					p4 = p4->next;
				}

				if (!p4)
				{
					break;
				}

				if (p4->column == p3->column)
				{
					matrixdata += p3->data * p4->data;
				}

				p3 = p3->next;
			}

			if (fabs(matrixdata) > 0.000001) // this threshold should be the same as the threshold in SolveSparseSystem()
			{
				if (!multipliedmatrix[i])
				{
					multipliedmatrix[i] = p5 = new SparseMatrix;
				}
				else
				{
					p6 = new SparseMatrix;
					p5->next = p6;
					p5 = p6;
				}
				ConstructSparseMatrix(p5, matrixdata, j);
			}
		}
	}

	return multipliedmatrix;
}

void SparseMatrix::AddSparseMatrices(SparseMatrix **sparsematrix1, SparseMatrix **sparsematrix2, int row)
{
	SparseMatrix *p1, *p2, *q, *qq;

	for (int i = 0; i < row; i++)
	{
		if (sparsematrix1[i])
		{
			p1 = sparsematrix1[i];
			q = p1->next;

			p2 = sparsematrix2[i];
			while (p2)
			{
				if (p2->column < p1->column)
				{
					qq = new SparseMatrix;
					ConstructSparseMatrix(qq, p2->data, p2->column);

					sparsematrix1[i] = qq;
					qq->next = p1;

					p1 = qq;
					q = p1->next;
				}
				else if (p2->column == p1->column)
				{
					p1->data += p2->data;
				}
				else
				{
					while (q && q->column < p2->column)
					{
						p1 = q;
						q = q->next;
					}

					if (q && q->column == p2->column)
					{
						q->data += p2->data;
					}
					else
					{
						qq = new SparseMatrix;
						ConstructSparseMatrix(qq, p2->data, p2->column);

						p1->next = qq;
						qq->next = q;

						q = qq;
					}
				}

				p2 = p2->next;
			}
		}
		else
		{
			p2 = sparsematrix2[i];
			while (p2)
			{
				if (!sparsematrix1[i])
				{
					p1 = sparsematrix1[i] = new SparseMatrix;
				}
				else
				{
					qq = new SparseMatrix;
					p1->next = qq;
					p1 = qq;
				}
				ConstructSparseMatrix(p1, p2->data, p2->column);

				p2 = p2->next;
			}
		}
	}
}

void SparseMatrix::PrintSparseMatrix(SparseMatrix **sparsematrix, int row, int column)
{
	SparseMatrix *p1, *p2;

	for (int i = 0; i < row; i++)
	{
		printf("\n\n");

		p1 = sparsematrix[i];
		p2 = p1->next;

		if (p1->column != 0)
		{
			for (int j = 0; j < p1->column; j++)
			{
				printf("%f ", 0.0);
			}
		}

		while (p1)
		{
			printf("%f %d ", p1->data, p1->column);

			for (int j = p1->column+1; p2 && j < p2->column; j++)
			{
				printf("%f ", 0.0);
			}

			if (!p2)
			{
				for (int j = p1->column+1; j < column; j++)
				{
					printf("%f ", 0.0);
				}
			}

			p1 = p1->next;

			if (p2)
			{
				p2 = p2->next;
			}
		}
	}
}

void SparseMatrix::OutputSparseMatrix(char *filename, SparseMatrix **sparsematrix, int row, int column)
{
	FILE *fp;
    fp = fopen(filename, "w");

	SparseMatrix *p1, *p2;

	for (int i = 0; i < row; i++)
	{
		fprintf(fp, "\n\n");

		if (sparsematrix[i])
		{
			p1 = sparsematrix[i];
			p2 = p1->next;

			if (p1->column != 0)
			{
				for (int j = 0; j < p1->column; j++)
				{
					fprintf(fp, "%f ", 0.0);
				}
			}

			while (p1)
			{
				fprintf(fp, "%f %d ", p1->data, p1->column);

				for (int j = p1->column+1; p2 && j < p2->column; j++)
				{
					fprintf(fp, "%f ", 0.0);
				}

				if (!p2)
				{
					for (int j = p1->column+1; j < column; j++)
					{
						fprintf(fp, "%f ", 0.0);
					}
				}

				p1 = p1->next;

				if (p2)
				{
					p2 = p2->next;
				}
			}
		}
		else
		{
			fprintf(fp, "\n");
		}
	}

	fclose(fp);
}

void IndexList::DeleteIndexList(IndexList **plist, int len)
{
	if (plist)
	{
		for (int i = 0; i < len; i++)
		{
			DeleteIndexList(plist[i]);
			plist[i] = NULL;
		}

		delete []plist;
		plist = NULL;
	}
}

void IndexList::DeleteIndexList(IndexList *plist)
{
	if (plist)
	{
		IndexList *p1 = plist, *p2;

		while (p1)
		{
			p2 = p1->next;
			delete p1;
			p1 = p2;
		}

		plist = NULL;
	}
}

bool IndexList::IsInList(int index, IndexList *plist)
{
	bool flag = false;

	while (plist)
	{
		if (plist->index == index)
		{
			flag = true;
			break;
		}

		plist = plist->next;
	}

	return flag;
}
