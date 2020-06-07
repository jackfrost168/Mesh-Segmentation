// zyView.h : interface of the CZyView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_ZYVIEW_H__253F8CB7_7588_47EA_9CF4_B1D4295B0AA2__INCLUDED_)
#define AFX_ZYVIEW_H__253F8CB7_7588_47EA_9CF4_B1D4295B0AA2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "glm.h"
#include "SharedStructures.h"
#include "Cluster.h"
#include "Graph.h"

#define axisx -1
#define axisy -2
#define axisz -3

#define FLATSHADING 1
#define SMOOTHSHADING 2
#define WIREFRAME 3
#define POINTS 4
#define MESHNULL 5

#define MAX(a,b) ((a)>(b) ? (a):(b))
#define MIN(a,b) ((a)<(b) ? (a):(b))

class CZyView : public CView
{
protected: // create from serialization only
	CZyView();
	DECLARE_DYNCREATE(CZyView)

// Attributes
public:
	CZyDoc* GetDocument();

// Operations
public:

	GLfloat  viewangle;
	GLfloat  rx;
	GLfloat  ry;
	GLfloat  rz;
	GLfloat  tx;
	GLfloat  ty;
	GLfloat  tz;

	float    bgcolor[3];
	HDC      m_hDC;
	HGLRC    m_hRC;

	GraphAdjList G;//
	SortedList *SortList;//
	int numofweight = 1;

	Cluster  *pCluster;
	bool     flagglobalframe;
	bool     flaghandlesframe;
	bool	 flagobjectmove;
	bool	 flagobjectrotation;
	bool	 flagobjectzoom;
	bool     flagobjecthandles;
	bool     flagobjectboundary;
	bool     flaghandlesinteraction;
	bool     flaghandlesrotation;
	bool     islbpressed;

	bool     colorflag;

	bool     objecthandle;
	bool     objectboundary;
	bool     objecthandles;
	bool     objectinteraction;
	bool     addcube;
	bool     islpressed;

	bool	 handlesframeflag;
	bool     globalframeflag;

	int      renderingmode;
	int      renderingmodeCM;
	int      renderingmodeDM;
	int      renderingmodeSM;

	int      prepointx;
	int      prepointy;
	int      curpointx;
	int      curpointy;
	int      prepointxrecord;
	int      prepointyrecord;
	IndexList *handleslist;   // 控制点的链表
	IndexList *handlestail;
	IndexList *boundaryverticeslist;   // 边界点的链表
	IndexList *boundarytail;
	IndexList *ROIlist;  // 待编辑区域点的链表
	IndexList *ROItail;
	IndexList pIndexList;
	int      numhandles;   // 控制点的个数
	int      numboundaryvertices;   // 边界点的个数
    int      numROI;   // 待编辑区域点的个数
	double   handlescenter[3];
	double   handlesframe[3][3];
	double   handlescovariancematrix[9];
	bool     *isdeformed;
	int      rotationaxis;
	float    totalangle;
	bool     flagdrawwireframe;
	int      *deformationindex;   // 控制点、边界点、待编辑区域点的顺序

	void     InitGL();
	void     Draw();
	void     myDraw();
	void     mytestDraw(int *a);
	void     DrawGlobalFrame(float meshmax[3], float meshmin[3]);
	void     DrawHandlesFrame();
	void	 DrawFlatShading(GLMmodel *pmesh);
	void     DrawFlatShading(GLMmodel *pmesh, int *cluster, float *color);
	
	void     DrawWireFrame(GLMmodel *pmesh);
	void     ComputeFacetNormals(GLMmodel *pmesh);
	void     ObjectMove(GLMmodel *pmesh, int prepointx, int prepointy, int curpointx, int curpointy, float displacement[3]);
	float	 ObjectRotation(int prepointxy, int curpointxy);
	float	 ObjectZoom(int prepointx, int prepointy, int curpointx, int curpointy);
	void	 GetPointFromSpace(float spacex, float spacey, float spacez, GLdouble screenpoint[3]);
	void	 GetPointFromScreen(int screenx, int screeny, GLdouble depth, GLdouble spacepoint[3]);
	void     DrawScreenRectangle(int prepointx, int prepointy, int curpointx, int curpointy);
	void     ObjectHandles(GLMmodel *pmesh, int rectmaxx, int rectminx, int rectmaxy, int rectminy, bool flag);
	void     DrawDeformedVertices(GLMmodel *pmesh, IndexList *pIndexList, GLfloat red, GLfloat green, GLfloat blue);
	void     DrawSphere(float spherecenter[3], float radius);
	void     DrawCube(float cubecenter[3], float dim);
	void     DrawCube(double cubecenter[3], double dim);
	void     RegionGrowing(IndexList *pindex);
    void     HandlesRotation(GLMmodel *pmesh, int prepointx, int prepointy, int curpointx, int curpointy);
	void	 HandlesInteraction(GLMmodel *pmesh, int prepointx, int prepointy, int curpointx, int curpointy);
	void	 HandlesFrameRotation();
	void     Initwindow();
	void     Weight(_cluster *cluster, int index, float *weight);
	void     InsertSort(SortedList *SortList, float *weight, int index1, int index2);
	IndexList *FindSeeds(_cluster *cluster, CZyDoc* pDoc);
	void GenerateNodeGraph(nodegraph *graph, _cluster *cluster1, _cluster *cluster2, int *copy_cluOfV, CZyDoc* pDoc);
	void GenerateSegedGraph(nodegraph *Segedgraph, _cluster *segmentedCluster, int *copy_cluOfV, CZyDoc* pDoc);
	void MergeSmall(nodegraph *Segedgraph, _cluster *segmentedCluster, int numSegedCluster, int *copy_cluOfV);
	void graphcut(nodegraph *graph, IndexList *SeedsS, IndexList *SeedsT, _cluster *cluster1, _cluster *cluster2, 
		GLMmodel *meshmodel, IndexList **verticestindices, int *FunctionOri2NewIndex, int *copy_cluOfV, int *ori_cluOfV);
	void Compute_VertexNormal(GLMmodel *meshmodel, IndexList **verticestindices, int index, float *VertexNormal);
	void Compute_SDF(GLMmodel *meshmodel, IndexList **verticestindices, float *SDF);
	void Compute_CF(GLMmodel *meshmodel, float *CF);
	void Compute_AGD(GLMmodel *meshmodel, float *AGD);
	void Compute_HKS(GLMmodel *meshmodel, float **HKS);
	void Normalized_features(float *feature, int length);
	void Exponential_weight(SortedList *SortList);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CZyView)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CZyView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CZyView)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnGlobalFrame();
	afx_msg void OnUpdateGlobalFrame(CCmdUI* pCmdUI);
	afx_msg void OnObjectMove();
	afx_msg void OnUpdateObjectMove(CCmdUI* pCmdUI);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnObjectRotation();
	afx_msg void OnUpdateObjectRotation(CCmdUI* pCmdUI);
	afx_msg void OnObjectZoom();
	afx_msg void OnUpdateObjectZoom(CCmdUI* pCmdUI);
	afx_msg void OnObjectHandles();
	afx_msg void OnUpdateObjectHandles(CCmdUI* pCmdUI);
	afx_msg void OnObjectBoundary();
	afx_msg void OnUpdateObjectBoundary(CCmdUI* pCmdUI);
	afx_msg void OnHandlesInteraction();
	afx_msg void OnUpdateHandlesInteraction(CCmdUI* pCmdUI);
	afx_msg void OnRegionGrowing();
	afx_msg void OnRelease();
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnHandlesFrame();
	afx_msg void OnUpdateHandlesFrame(CCmdUI* pCmdUI);
	afx_msg void OnDrawWireFrame();
	afx_msg void OnUpdateDrawWireFrame(CCmdUI* pCmdUI);
	afx_msg void OnKmeans();
	afx_msg void OnHierarA();
	afx_msg void OnRegionGrow();
	afx_msg void GraphSegment();
	afx_msg void GraphCut();
	afx_msg void ConstructGraph(GLMmodel *meshmodel, IndexList **neighborvertices, float *normalofvertices,
		int *numofneighboringvertices, IndexList **verticestindices);
	afx_msg void Merge(GraphAdjList G, SortedList *SortList);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnSegmentSeg();
	afx_msg void OnSegmentGraphsegment();
	afx_msg void OnSegmentGraphcut();
};

#ifndef _DEBUG  // debug version in zyView.cpp
inline CZyDoc* CZyView::GetDocument()
   { return (CZyDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ZYVIEW_H__253F8CB7_7588_47EA_9CF4_B1D4295B0AA2__INCLUDED_)
