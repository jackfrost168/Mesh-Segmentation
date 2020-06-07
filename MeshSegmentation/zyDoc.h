// zyDoc.h : interface of the CZyDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_ZYDOC_H__7E2859D0_A576_40E9_8790_C4D685A35663__INCLUDED_)
#define AFX_ZYDOC_H__7E2859D0_A576_40E9_8790_C4D685A35663__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "glm.h"
#include "SharedStructures.h"

class CZyDoc : public CDocument
{
protected: // create from serialization only
	CZyDoc();
	DECLARE_DYNCREATE(CZyDoc)

// Attributes
public:

// Operations
public:
	GLMmodel *meshmodel;
	float    meshmax[3];
	float    meshmin[3];
	float    meshmaxrecord[3];
	float    meshminrecord[3];
	float    boundingboxcenter[3];
	float    boundingboxdim[3];
	IndexList **verticesvindices;
	IndexList **verticestindices;
	int      *numofneighboringvertices;
	int      *numofneighboringtriangles;
	float    *reconstructednormals;

	void     GetBoundingBox();
    void     UnitizeMesh();
	void     GetTopologyInformation();
	void     DeleteTopologyInformation(IndexList **pverticesvindices, IndexList **pverticestindices,
		                               int pnumvertices, int *pnumofneighboringvertices,
									   int *pnumofneighboringtriangles);
	void     ReconstructVertexNormals();
	float    ComputeTriangleArea(float *point1, float *point2, float *point3);
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CZyDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CZyDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CZyDoc)
	afx_msg void OnFileOpen();
	afx_msg void OnFileSave();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ZYDOC_H__7E2859D0_A576_40E9_8790_C4D685A35663__INCLUDED_)
