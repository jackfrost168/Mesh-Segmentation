// zyDoc.cpp : implementation of the CZyDoc class
//

#include "stdafx.h"
#include "zy.h"

#include "zyDoc.h"
#include "zyView.h"
#include "MainFrm.h"
#include "MathFunctions.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CZyDoc

IMPLEMENT_DYNCREATE(CZyDoc, CDocument)

BEGIN_MESSAGE_MAP(CZyDoc, CDocument)
	//{{AFX_MSG_MAP(CZyDoc)
	ON_COMMAND(ID_FILE_OPEN, OnFileOpen)
	ON_COMMAND(ID_FILE_SAVE, OnFileSave)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CZyDoc construction/destruction

CZyDoc::CZyDoc()
{
	// TODO: add one-time construction code here
	meshmodel = NULL;
	verticesvindices = NULL;
	verticestindices = NULL;
	numofneighboringvertices = NULL;
	numofneighboringtriangles = NULL;
	reconstructednormals = NULL;
}

CZyDoc::~CZyDoc()
{
	if (meshmodel)
	{
		DeleteTopologyInformation(verticesvindices, verticestindices, meshmodel->numvertices,
  			                      numofneighboringvertices, numofneighboringtriangles);
		verticesvindices = NULL;
		verticestindices = NULL;
		numofneighboringvertices = NULL;
		numofneighboringtriangles = NULL;

		glmDelete(meshmodel);
		meshmodel = NULL;
	}

	if (reconstructednormals)
	{
		delete[]reconstructednormals;
		reconstructednormals = NULL;
	}
}

BOOL CZyDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
// CZyDoc serialization

void CZyDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}

/////////////////////////////////////////////////////////////////////////////
// CZyDoc diagnostics

#ifdef _DEBUG
void CZyDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CZyDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CZyDoc commands

void CZyDoc::OnFileOpen() 
{
	printf("Opening\n");
	// TODO: Add your command handler code here
	char szFileters[] = "Obj files(*.obj)|*.obj|All files(*.*)|*.*||";
	CFileDialog opendlg(TRUE,"obj","*.obj",OFN_FILEMUSTEXIST|OFN_HIDEREADONLY,szFileters);

	opendlg.m_ofn.lpstrTitle = "Open Mesh Model";

	if (opendlg.DoModal()==IDOK)
	{
 		CString str;
 		str = opendlg.GetPathName();

		char *filename = new char [strlen(str) + 1];
		strcpy(filename, (LPCTSTR)str);

		if (meshmodel)
		{
			glmDelete(meshmodel);
			meshmodel = NULL;
		}
		meshmodel = glmReadOBJ(filename);

		GetBoundingBox();
		UnitizeMesh();
		GetTopologyInformation();

		delete []filename;
		filename = NULL;

		CMainFrame *pMain = (CMainFrame*)AfxGetApp()->m_pMainWnd; 
        CZyView *pView = (CZyView*)pMain->GetActiveView();

		if (pView->pCluster)
		{
			pView->pCluster->DeleteClusters();
			pView->colorflag = false;
		}
		else
		{
			pView->pCluster = new Cluster;
		}

		pView->Draw();
	}
}

void CZyDoc::GetBoundingBox()
{
	for (int j = 0; j < 3; j++)
	{
		meshmax[j] = meshmodel->vertices[3+j];
		meshmin[j] = meshmodel->vertices[3+j];
	}

	for (int i = 2; i <= (int)meshmodel->numvertices; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			if (meshmodel->vertices[3*i+j] > meshmax[j])
			{
				meshmax[j] = meshmodel->vertices[3*i+j];
			}

			if (meshmodel->vertices[3*i+j] < meshmin[j])
			{
				meshmin[j] = meshmodel->vertices[3*i+j];
			}
		}
	}

	for (int j = 0; j < 3; j++)
	{
		boundingboxdim[j] = 0.5 * (meshmax[j] - meshmin[j]);
		boundingboxcenter[j] = 0.5 * (meshmax[j] + meshmin[j]);
	}

	for (int j = 0; j < 3; j++)
	{
		meshmaxrecord[j] = meshmax[j];
		meshminrecord[j] = meshmin[j];
	}
}

void CZyDoc::UnitizeMesh()
{
	float dim = max(max(boundingboxdim[0], boundingboxdim[1]),boundingboxdim[2]);

	for (int i = 1; i <= (int)meshmodel->numvertices; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			meshmodel->vertices[3*i+j] -= boundingboxcenter[j];
			meshmodel->vertices[3*i+j] /= dim;
		}
	}

	for (int j = 0; j < 3; j++)
	{
		meshmax[j] -= boundingboxcenter[j];
		meshmin[j] -= boundingboxcenter[j];

		meshmax[j] /= dim;
		meshmin[j] /= dim;

		boundingboxcenter[j] = 0.0;
		boundingboxdim[j] /= dim;
	}
}

void CZyDoc::GetTopologyInformation()
{
	verticesvindices = new IndexList * [meshmodel->numvertices+1];
	verticestindices = new IndexList * [meshmodel->numvertices+1];
	numofneighboringvertices = new int [meshmodel->numvertices+1];
	numofneighboringtriangles = new int [meshmodel->numvertices+1];

	for (int i = 0; i <= (int)meshmodel->numvertices; i++)
	{
		verticesvindices[i] = NULL;
		verticestindices[i] = NULL;
		numofneighboringvertices[i] = 0;
		numofneighboringtriangles[i] = 0;
	}

	IndexList **verticesvindicestail = new IndexList * [meshmodel->numvertices+1];
	IndexList **verticestindicestail = new IndexList * [meshmodel->numvertices+1];

	for (int i = 0; i <= (int)meshmodel->numvertices; i++)
	{
		verticesvindicestail[i] = NULL;
		verticestindicestail[i] = NULL;
	}

	IndexList *p1, pIndexList;
	int indexv, indexvo, kk[2];

	for (int j = 0; j < (int)meshmodel->numtriangles; j++)
	{
		for (int k = 0; k < 3; k++)
		{
			indexv = meshmodel->triangles[j].vindices[k];

			if (!verticestindices[indexv])
			{
				verticestindices[indexv] = verticestindicestail[indexv] = new IndexList;
			}
			else
			{
				p1 = new IndexList;
				verticestindicestail[indexv]->next = p1;
				verticestindicestail[indexv] = p1;
			}	
			verticestindicestail[indexv]->index = j;
			verticestindicestail[indexv]->next = NULL;

			numofneighboringtriangles[indexv]++;

			if (k == 0)
			{
				kk[0] = 1;
				kk[1] = 2;
			}
			else if (k == 1)
			{
				kk[0] = 0;
				kk[1] = 2;
			}
			else
			{
				kk[0] = 0;
				kk[1] = 1;
			}

			for (int l = 0; l < 2; l++)
			{
				indexvo = meshmodel->triangles[j].vindices[kk[l]];

				if (!pIndexList.IsInList(indexvo, verticesvindices[indexv]))
				{
					if (!verticesvindices[indexv])
					{
						verticesvindices[indexv] = verticesvindicestail[indexv] = new IndexList;
					}
					else
					{
						p1 = new IndexList;
						verticesvindicestail[indexv]->next = p1;
						verticesvindicestail[indexv] = p1;
					}
					verticesvindicestail[indexv]->index = indexvo;
					verticesvindicestail[indexv]->next = NULL;

					numofneighboringvertices[indexv]++;
				}
			}
		}
	}

	for (int i = 0; i <= (int)meshmodel->numvertices; i++)
	{
		verticesvindicestail[i] = NULL;
		verticestindicestail[i] = NULL;
	}

	delete []verticesvindicestail;
	verticesvindicestail = NULL;
	delete []verticestindicestail;
	verticestindicestail = NULL;
}

void CZyDoc::DeleteTopologyInformation(IndexList **pverticesvindices, IndexList **pverticestindices, int pnumvertices,
									   int *pnumofneighboringvertices, int *pnumofneighboringtriangles)
{
	IndexList *p1, *p2;

	if (pverticesvindices)
	{
		for (int i = 1; i <= (int)pnumvertices; i++)
		{
			p1 = pverticesvindices[i];

			while (p1)
			{
				p2 = p1->next;
				delete p1;
				p1 = p2;
			}

			pverticesvindices[i] = NULL;
		}

		delete []pverticesvindices;
		pverticesvindices = NULL;
	}

	if (pverticestindices)
	{
		for (int i = 1; i <= (int)pnumvertices; i++)
		{
			p1 = pverticestindices[i];

			while (p1)
			{
				p2 = p1->next;
				delete p1;
				p1 = p2;
			}

			pverticestindices[i] = NULL;
		}

		delete []pverticestindices;
		pverticestindices = NULL;
	}

	if (pnumofneighboringvertices)
	{
		delete []pnumofneighboringvertices;
		pnumofneighboringvertices = NULL;
	}

	if (pnumofneighboringtriangles)
	{
		delete []pnumofneighboringtriangles;
		pnumofneighboringtriangles = NULL;
	}
}

float CZyDoc::ComputeTriangleArea(float *point1, float *point2, float *point3)
{
	float area;

	float crossproduct[3];
	CrossProd(point1, point2, point3, crossproduct);

	area = 0.5 * Length(crossproduct);

	return area;
}

void CZyDoc::ReconstructVertexNormals()
{
	glmFacetNormals(meshmodel);

	if (reconstructednormals)
	{
		delete[]reconstructednormals;
		reconstructednormals = NULL;
	}
	reconstructednormals = new float[3 * ((int)meshmodel->numvertices + 1)];

	IndexList *p1;
	float facetnormssum[3], weightsum, weightfacet;
	int indexf;
	int indexv1, indexv2, indexv3;

	for (int i = 1; i <= (int)meshmodel->numvertices; i++)
	{
		p1 = verticestindices[i];
		weightsum = 0.0;
		facetnormssum[0] = 0.0;
		facetnormssum[1] = 0.0;
		facetnormssum[2] = 0.0;

		while (p1)
		{
			indexf = meshmodel->triangles[p1->index].findex;

			indexv1 = meshmodel->triangles[p1->index].vindices[0];
			indexv2 = meshmodel->triangles[p1->index].vindices[1];
			indexv3 = meshmodel->triangles[p1->index].vindices[2];

			weightfacet = ComputeTriangleArea(meshmodel->vertices + 3 * indexv1,
				meshmodel->vertices + 3 * indexv2,
				meshmodel->vertices + 3 * indexv3);
			weightsum += weightfacet;

			facetnormssum[0] += weightfacet * meshmodel->facetnorms[3 * indexf];
			facetnormssum[1] += weightfacet * meshmodel->facetnorms[3 * indexf + 1];
			facetnormssum[2] += weightfacet * meshmodel->facetnorms[3 * indexf + 2];

			p1 = p1->next;
		}

		facetnormssum[0] /= weightsum;
		facetnormssum[1] /= weightsum;
		facetnormssum[2] /= weightsum;
		Normalize(facetnormssum);

		reconstructednormals[3 * i] = facetnormssum[0];
		reconstructednormals[3 * i + 1] = facetnormssum[1];
		reconstructednormals[3 * i + 2] = facetnormssum[2];
	}
}

void CZyDoc::OnFileSave() 
{
	// TODO: Add your command handler code here
	char szFileters[] = "Obj file(*.obj)|*.obj|All files(*.*)|*.*||";
	CFileDialog savedlg(FALSE,"obj","*.obj",OFN_FILEMUSTEXIST|OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,szFileters);

    savedlg.m_ofn.lpstrTitle = "Save Mesh Model";

	if (savedlg.DoModal()==IDOK)
	{
		CString str;
		str = savedlg.GetPathName();

		char *filename = new char [strlen(str) + 1];
		strcpy(filename, (LPCTSTR)str);

		if (meshmodel)
		{
			glmWriteOBJ(meshmodel, filename, GLM_FLAT);
		}

		delete []filename;
		filename = NULL;

		CMainFrame *pMain = (CMainFrame*)AfxGetApp()->m_pMainWnd; 
        CZyView *pView = (CZyView*)pMain->GetActiveView();
		pView->Draw();
	}
}
