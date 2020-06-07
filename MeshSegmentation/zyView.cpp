// zyView.cpp : implementation of the CZyView class
//

#include "stdafx.h"
#include "zy.h"

#include "zyDoc.h"
#include "zyView.h"
#include "MathFunctions.h"
#include <math.h>
#include "MainFrm.h"

//#include "Graph.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CZyView

IMPLEMENT_DYNCREATE(CZyView, CView)

BEGIN_MESSAGE_MAP(CZyView, CView)
	//{{AFX_MSG_MAP(CZyView)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_ERASEBKGND()
	ON_WM_SIZE()
	ON_COMMAND(ID_GlobalFrame, OnGlobalFrame)
	ON_UPDATE_COMMAND_UI(ID_GlobalFrame, OnUpdateGlobalFrame)
	ON_COMMAND(ID_ObjectMove, OnObjectMove)
	ON_UPDATE_COMMAND_UI(ID_ObjectMove, OnUpdateObjectMove)
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_COMMAND(ID_ObjectRotation, OnObjectRotation)
	ON_UPDATE_COMMAND_UI(ID_ObjectRotation, OnUpdateObjectRotation)
	ON_COMMAND(ID_ObjectZoom, OnObjectZoom)
	ON_UPDATE_COMMAND_UI(ID_ObjectZoom, OnUpdateObjectZoom)
	ON_COMMAND(ID_ObjectHandles, OnObjectHandles)
	ON_UPDATE_COMMAND_UI(ID_ObjectHandles, OnUpdateObjectHandles)
	ON_COMMAND(ID_ObjectBoundary, OnObjectBoundary)
	ON_UPDATE_COMMAND_UI(ID_ObjectBoundary, OnUpdateObjectBoundary)
	ON_COMMAND(ID_HandlesInteraction, OnHandlesInteraction)
	ON_UPDATE_COMMAND_UI(ID_HandlesInteraction, OnUpdateHandlesInteraction)
	ON_COMMAND(ID_RegionGrowing, OnRegionGrowing)
	ON_COMMAND(ID_Release, OnRelease)
	ON_WM_KEYDOWN()
	ON_COMMAND(ID_HandlesFrame, OnHandlesFrame)
	ON_UPDATE_COMMAND_UI(ID_HandlesFrame, OnUpdateHandlesFrame)
	ON_COMMAND(ID_DrawWireFrame, OnDrawWireFrame)
	ON_UPDATE_COMMAND_UI(ID_DrawWireFrame, OnUpdateDrawWireFrame)
	//}}AFX_MSG_MAP
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CView::OnFilePrintPreview)
	ON_COMMAND(ID_SEGMENT_SEG, &CZyView::OnSegmentSeg)
	ON_COMMAND(ID_SEGMENT_GRAPHSEGMENT, &CZyView::OnSegmentGraphsegment)
	ON_COMMAND(ID_SEGMENT_GRAPHCUT, &CZyView::OnSegmentGraphcut)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CZyView construction/destruction

CZyView::CZyView()
{
	// TODO: add construction code here
	rx = 0.0f;
	ry = 0.0f;
	rz = 0.0f;
	tx = 0.0f;
	ty = 0.0f;
	tz = -5.0f;
	viewangle = 40.0;
	flagglobalframe = false;
	flaghandlesframe = false;
	flagobjectmove = false;
	flagobjectrotation = false;
	flagobjectzoom = false;
	flagobjecthandles = false;
	flagobjectboundary = false;
	flaghandlesinteraction = false;
	flaghandlesrotation = false;
	islbpressed = false;
	prepointx = 0;
	prepointy = 0;
	curpointx = 0;
	curpointy = 0;
	prepointxrecord = 0;
	prepointyrecord = 0;
	handleslist = NULL;
	handlestail = NULL;
	boundaryverticeslist = NULL;
	boundarytail = NULL;
	ROIlist = NULL;
	ROItail = NULL;
	numhandles = 0;
	numboundaryvertices = 0;
    numROI = 0;
	isdeformed = NULL;
	rotationaxis = 0;
	totalangle = 0.0;
	flagdrawwireframe = false;
	deformationindex = NULL;

	pCluster = NULL;
	colorflag = false;

	for (int i = 0; i < 3; i++)
	{
		handlescenter[i] = 0.0;

		for (int j = 0; j < 3; j++)
		{
			handlescovariancematrix[3*i+j] = 0.0;

			if (i == j)
			{
				handlesframe[i][j] = 1.0;
			}
			else
			{
				handlesframe[i][j] = 0.0;
			}
		}
	}
}

CZyView::~CZyView()
{
	if (handleslist)
	{
		pIndexList.DeleteIndexList(handleslist);
		handleslist = NULL;
		handlestail = NULL;
	}

	if (boundaryverticeslist)
	{
		pIndexList.DeleteIndexList(boundaryverticeslist);
		boundaryverticeslist = NULL;
		boundarytail = NULL;
	}

	if (ROIlist)
	{
		pIndexList.DeleteIndexList(ROIlist);
		ROIlist = NULL;
		ROItail = NULL;
	}

	if (isdeformed)
	{
		delete []isdeformed;
		isdeformed = NULL;
	}

	if (deformationindex)
	{
		delete []deformationindex;
		deformationindex = NULL;
	}

	if (pCluster)
	{
		pCluster->DeleteClusters();
		delete pCluster;
	}
}

BOOL CZyView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	cs.style|= WS_CLIPSIBLINGS|WS_CLIPCHILDREN;

	return CView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CZyView drawing

void CZyView::OnDraw(CDC* pDC)
{
	CZyDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	// TODO: add draw code for native data here
	Draw();
}

/////////////////////////////////////////////////////////////////////////////
// CZyView printing

BOOL CZyView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CZyView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CZyView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}

/////////////////////////////////////////////////////////////////////////////
// CZyView diagnostics

#ifdef _DEBUG
void CZyView::AssertValid() const
{
	CView::AssertValid();
}

void CZyView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CZyDoc* CZyView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CZyDoc)));
	return (CZyDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CZyView message handlers

int CZyView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO: Add your specialized creation code here

	PIXELFORMATDESCRIPTOR pfd=	      // pfd Tells Windows How We Want Things To Be
	{                                 // Set pixel format 
		sizeof(PIXELFORMATDESCRIPTOR),				    // Size Of This Pixel Format Descriptor
			1,											// Version Number
			PFD_DRAW_TO_WINDOW |						// Format Must Support Window
			PFD_SUPPORT_OPENGL |						// Format Must Support OpenGL
			PFD_DOUBLEBUFFER,							// Must Support Double Buffering
			PFD_TYPE_RGBA,								// Request An RGBA Format
			24,									  	    // Select Our Color Depth
			0, 0, 0, 0, 0, 0,							// Color Bits Ignored
			0,											// No Alpha Buffer
			0,											// Shift Bit Ignored
			0,											// No Accumulation Buffer
			0, 0, 0, 0,									// Accumulation Bits Ignored
			32,											// 16Bit Z-Buffer (Depth Buffer)
			0,											// No Stencil Buffer
			0,											// No Auxiliary Buffer
			PFD_MAIN_PLANE,								// Main Drawing Layer
			0,											// Reserved
			0, 0, 0										// Layer Masks Ignored
	};

	m_hDC = GetDC()->GetSafeHdc();
	int nPixelFormat = ::ChoosePixelFormat(m_hDC, &pfd); // Choose matched pixel format
	::SetPixelFormat(m_hDC,nPixelFormat, &pfd); // Set pixel format
	m_hRC = ::wglCreateContext(m_hDC); 
	::wglMakeCurrent(m_hDC, m_hRC);

	InitGL(); // Initialize

	return 0;
}

void CZyView::InitGL()
{
	bgcolor[0] = (float)1.0;
 	bgcolor[1] = (float)1.0;
 	bgcolor[2] = (float)1.0;

	glClearColor(bgcolor[0], bgcolor[1], bgcolor[2], 1);// 	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//	glShadeModel(GL_SMOOTH);	// Set format
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	// Clear Screen And Depth Buffer
//	glEnable(GL_CULL_FACE);   // Back surface cutting
//	glCullFace(GL_BACK);

	// Set lighting material properties
	GLfloat light_ambient[] = {2.0f, 2.0f, 2.0f, 1.0f}; // 环境光
    GLfloat light_position[] = {5.0f, 10.0f, 15.0f, 0.0f}; // 光源位置
    GLfloat mat_diffuse[] = {0.3f, 0.3f, 0.3f, 1.0f}; // 漫反射
    GLfloat mat_specular[] = {0.15f, 0.15f, 0.15f, 1.0f}; // 镜面反射，控制高光
    GLfloat mat_shininess[] = {4.0f};

    glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, mat_shininess);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mat_diffuse);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
}

void CZyView::OnDestroy() 
{
	CView::OnDestroy();

	// TODO: Add your message handler code here
	::wglMakeCurrent(NULL,NULL);
	::wglDeleteContext(m_hRC);
}

/*	return true means don't render background again */
BOOL CZyView::OnEraseBkgnd(CDC* pDC) 
{
	// TODO: Add your message handler code here and/or call default

	return CView::OnEraseBkgnd(pDC);
}

void CZyView::OnSize(UINT nType, int cx, int cy) 
{
	CView::OnSize(nType, cx, cy);

	// TODO: Add your message handler code here
	if (cy == 0)										// Prevent A Divide By Zero By
	{
		cy = 1;										    // Making Height Equal One
	}

	glViewport(0,0, cx, cy);					        // Reset The Current Viewport
	glEnable(GL_DEPTH_TEST);							// Enables Depth Testing

	glMatrixMode(GL_PROJECTION);						// Select The Projection Matrix
	glLoadIdentity();									// Reset The Projection Matrix
	gluPerspective(viewangle, (GLfloat)cx/(GLfloat)cy, 0.1f, 100.0f);	// Calculate The Aspect Ratio Of The Window
	glMatrixMode(GL_MODELVIEW);							// Select The Modelview Matrix
	glLoadIdentity();
}

void CZyView::Draw()
{
	glClearColor(bgcolor[0], bgcolor[1], bgcolor[2], 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	// Clear Screen And Depth Buffer

	glLoadIdentity();									// Reset The Current Modelview Matrix
	glTranslatef(tx, ty, tz);					        // Move

	if (!flagobjecthandles && !flaghandlesinteraction)
	{
		glPushMatrix();
	}

	glRotatef(rx, 1.0f, 0.0f, 0.0f);					// Rotate The Triangle On The X axis ( NEW )
	glRotatef(ry, 0.0f, 1.0f, 0.0f);					// Rotate The Triangle On The Y axis ( NEW )
	glRotatef(rz, 0.0f, 0.0f, 1.0f);                    // Rotate The Triangle On The Z axis ( NEW )

	CZyDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);	

	// set the color of object
	glEnable(GL_COLOR_MATERIAL);
 	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);

	if (pDoc->meshmodel)
	{
		if (flagdrawwireframe)
		{
			DrawWireFrame(pDoc->meshmodel);
		}
		else
		{
			
			if (colorflag == TRUE)
			{
				DrawFlatShading(pDoc->meshmodel, pCluster->cluOfV, pCluster->colorOfClu);
			}
			else
			{
				DrawFlatShading(pDoc->meshmodel);
			}
				
		}

		DrawDeformedVertices(pDoc->meshmodel, handleslist, 0.0, 1.0, 0.0);
		DrawDeformedVertices(pDoc->meshmodel, boundaryverticeslist, 1.0, 0.0, 0.0);
		DrawDeformedVertices(pDoc->meshmodel, ROIlist, 1.0, 1.0, 0.0);
	}

	if (flagglobalframe)
	{
		DrawGlobalFrame(pDoc->meshmax, pDoc->meshmin);
	}

	if (flaghandlesframe)
	{
		DrawHandlesFrame();
	}

	glDisable(GL_COLOR_MATERIAL);

	if (islbpressed && flagobjecthandles)
	{
		DrawScreenRectangle(prepointxrecord, prepointyrecord, curpointx, curpointy);
	}

	if (!flagobjecthandles && !flaghandlesinteraction)
	{
		glPopMatrix();
	}
	
	SwapBuffers(m_hDC);
}

void CZyView::DrawDeformedVertices(GLMmodel *pmesh, IndexList *pIndexList, GLfloat red, GLfloat green, GLfloat blue)
{
	glColor3f(red, green, blue);

	while (pIndexList)
	{
		DrawCube(pmesh->vertices+3*pIndexList->index, 0.01);
//		DrawSphere(pmesh->vertices+3*pIndexList->index, 0.01);
		pIndexList = pIndexList->next;
	}
}

void CZyView::DrawSphere(float spherecenter[3], float radius)
{
	glPushMatrix();
	GLUquadricObj *sphere = gluNewQuadric();
//	gluQuadricDrawStyle(sphere, GLU_LINE);
    glTranslatef(spherecenter[0], spherecenter[1], spherecenter[2]);
	gluSphere(sphere, radius, 50, 50);
	gluDeleteQuadric(sphere);
	glPopMatrix();
}

void CZyView::DrawCube(float cubecenter[3], float dim)
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

void CZyView::DrawCube(double cubecenter[3], double dim)
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

void CZyView::DrawScreenRectangle(int prepointx, int prepointy, int curpointx, int curpointy)
{
	CClientDC dc(this);

 	dc.MoveTo(prepointx, prepointy);
 	dc.LineTo(curpointx, prepointy);
 
	dc.MoveTo(prepointx, prepointy);
	dc.LineTo(prepointx, curpointy);

	dc.MoveTo(curpointx, prepointy);
	dc.LineTo(curpointx, curpointy);

	dc.MoveTo(prepointx, curpointy);
	dc.LineTo(curpointx, curpointy);
}

void CZyView::ComputeFacetNormals(GLMmodel *pmesh)
{
	if (pmesh->facetnorms)
	{
		delete []pmesh->facetnorms;
		pmesh->facetnorms = NULL;
	}

    pmesh->numfacetnorms = pmesh->numtriangles;
    pmesh->facetnorms = new GLfloat [3*(pmesh->numfacetnorms+1)];

	int indexv1, indexv2, indexv3;
	float uvec[3], vvec[3];

	for (int i = 0; i < (int)pmesh->numtriangles; i++)
	{
		indexv1 = pmesh->triangles[i].vindices[0];
	    indexv2 = pmesh->triangles[i].vindices[1];
		indexv3 = pmesh->triangles[i].vindices[2];

		for (int j = 0; j < 3; j++)
		{
			uvec[j] = pmesh->vertices[3*indexv2+j]-pmesh->vertices[3*indexv1+j];
			vvec[j] = pmesh->vertices[3*indexv3+j]-pmesh->vertices[3*indexv1+j];
		}

		CrossProd(uvec, vvec, pmesh->facetnorms+3*(i+1));
		Normalize(pmesh->facetnorms+3*(i+1));

		pmesh->triangles[i].findex = i+1;
	}
}

void CZyView::DrawFlatShading(GLMmodel *pmesh, int *cluster, float *color)
{
	glmFacetNormals(pmesh);

	int index_v1, index_v2, index_v3, index_f;

	for (int i = 0; i < (int)pmesh->numtriangles; i++)
	{
		index_v1 = pmesh->triangles[i].vindices[0];
		index_v2 = pmesh->triangles[i].vindices[1];
		index_v3 = pmesh->triangles[i].vindices[2];
		index_f = pmesh->triangles[i].findex;

		glBegin(GL_TRIANGLES);
		glNormal3f(pmesh->facetnorms[3 * index_f],
			pmesh->facetnorms[3 * index_f + 1],
			pmesh->facetnorms[3 * index_f + 2]);

		if (cluster != NULL && color != NULL)
			glColor3f(color[3 * cluster[index_v1]],
				color[3 * cluster[index_v1] + 1],
				color[3 * cluster[index_v1] + 2]);
		glVertex3f(pmesh->vertices[3 * index_v1],
			pmesh->vertices[3 * index_v1 + 1],
			pmesh->vertices[3 * index_v1 + 2]);

		if (cluster != NULL && color != NULL)
			glColor3f(color[3 * cluster[index_v2]],
				color[3 * cluster[index_v2] + 1],
				color[3 * cluster[index_v2] + 2]);
		glVertex3f(pmesh->vertices[3 * index_v2],
			pmesh->vertices[3 * index_v2 + 1],
			pmesh->vertices[3 * index_v2 + 2]);

		if (cluster != NULL && color != NULL)
			glColor3f(color[3 * cluster[index_v3]],
				color[3 * cluster[index_v3] + 1],
				color[3 * cluster[index_v3] + 2]);
		glVertex3f(pmesh->vertices[3 * index_v3],
			pmesh->vertices[3 * index_v3 + 1],
			pmesh->vertices[3 * index_v3 + 2]);
		glEnd();
	}
}

void CZyView::DrawFlatShading(GLMmodel *pmesh)
{
	ComputeFacetNormals(pmesh);

	int indexv1, indexv2, indexv3, indexf;

	glColor3f(0.2, 0.2, 1.0);

	for(int i = 0; i < (int)pmesh->numtriangles; i++)
	{
		indexv1 = pmesh->triangles[i].vindices[0];
	    indexv2 = pmesh->triangles[i].vindices[1];
		indexv3 = pmesh->triangles[i].vindices[2];
	    indexf = pmesh->triangles[i].findex;

		glBegin(GL_TRIANGLES);
		  glNormal3f(pmesh->facetnorms[3*indexf], pmesh->facetnorms[3*indexf+1], pmesh->facetnorms[3*indexf+2]);

		  glVertex3f(pmesh->vertices[3*indexv1], pmesh->vertices[3*indexv1+1], pmesh->vertices[3*indexv1+2]);
		  glVertex3f(pmesh->vertices[3*indexv2], pmesh->vertices[3*indexv2+1], pmesh->vertices[3*indexv2+2]);
		  glVertex3f(pmesh->vertices[3*indexv3], pmesh->vertices[3*indexv3+1], pmesh->vertices[3*indexv3+2]);
		glEnd();
	}
}

void CZyView::DrawWireFrame(GLMmodel *pmesh)
{
	int indexv1, indexv2, indexv3;

	glColor3f(0.2, 0.2, 1.0);

	for(int i = 0; i < (int)pmesh->numtriangles; i++)
	{
		indexv1 = pmesh->triangles[i].vindices[0];
	    indexv2 = pmesh->triangles[i].vindices[1];
		indexv3 = pmesh->triangles[i].vindices[2];

        glBegin(GL_LINES);
		  glVertex3f(pmesh->vertices[3*indexv1], pmesh->vertices[3*indexv1+1], pmesh->vertices[3*indexv1+2]);
		  glVertex3f(pmesh->vertices[3*indexv2], pmesh->vertices[3*indexv2+1], pmesh->vertices[3*indexv2+2]);

		  glVertex3f(pmesh->vertices[3*indexv2], pmesh->vertices[3*indexv2+1], pmesh->vertices[3*indexv2+2]);
		  glVertex3f(pmesh->vertices[3*indexv3], pmesh->vertices[3*indexv3+1], pmesh->vertices[3*indexv3+2]);

		  glVertex3f(pmesh->vertices[3*indexv3], pmesh->vertices[3*indexv3+1], pmesh->vertices[3*indexv3+2]);
	      glVertex3f(pmesh->vertices[3*indexv1], pmesh->vertices[3*indexv1+1], pmesh->vertices[3*indexv1+2]);
		glEnd();
	}
}

void CZyView::OnGlobalFrame() 
{
	// TODO: Add your command handler code here
	CZyDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);	
	
	if (pDoc->meshmodel)
	{
		flagglobalframe = !flagglobalframe;
		Draw();
	}
}

void CZyView::OnUpdateGlobalFrame(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	if (flagglobalframe)
		pCmdUI->SetCheck(TRUE);
	else
		pCmdUI->SetCheck(FALSE);
}

void CZyView::DrawGlobalFrame(float meshmax[3], float meshmin[3])
{
	glLineWidth(3.0);

	glBegin(GL_LINES);
	  glColor3f(1.0, 0.0, 0.0);

 	  glVertex3f(meshmax[0]+0.2, 0.5*(meshmax[1]+meshmin[1]), 0.5*(meshmax[2]+meshmin[2]));
	  glVertex3f(meshmin[0]-0.2, 0.5*(meshmax[1]+meshmin[1]), 0.5*(meshmax[2]+meshmin[2]));

	  glVertex3f(meshmax[0]+0.2, 0.5*(meshmax[1]+meshmin[1]), 0.5*(meshmax[2]+meshmin[2]));
	  glVertex3f(meshmax[0]+0.15, 0.5*(meshmax[1]+meshmin[1])+0.05, 0.5*(meshmax[2]+meshmin[2]));

	  glVertex3f(meshmax[0]+0.2, 0.5*(meshmax[1]+meshmin[1]), 0.5*(meshmax[2]+meshmin[2]));
	  glVertex3f(meshmax[0]+0.15, 0.5*(meshmax[1]+meshmin[1])-0.05, 0.5*(meshmax[2]+meshmin[2]));

	  glVertex3f(meshmax[0]+0.2, 0.5*(meshmax[1]+meshmin[1])+0.06, 0.5*(meshmax[2]+meshmin[2]));
	  glVertex3f(meshmax[0]+0.15, 0.5*(meshmax[1]+meshmin[1])+0.11, 0.5*(meshmax[2]+meshmin[2]));

	  glVertex3f(meshmax[0]+0.2, 0.5*(meshmax[1]+meshmin[1])+0.11, 0.5*(meshmax[2]+meshmin[2]));
	  glVertex3f(meshmax[0]+0.15, 0.5*(meshmax[1]+meshmin[1])+0.06, 0.5*(meshmax[2]+meshmin[2]));

	  glColor3f(0.0, 1.0, 0.0);

	  glVertex3f(0.5*(meshmax[0]+meshmin[0]), meshmax[1]+0.2, 0.5*(meshmax[2]+meshmin[2]));
	  glVertex3f(0.5*(meshmax[0]+meshmin[0]), meshmin[1]-0.2, 0.5*(meshmax[2]+meshmin[2]));

	  glVertex3f(0.5*(meshmax[0]+meshmin[0]), meshmax[1]+0.2, 0.5*(meshmax[2]+meshmin[2]));
	  glVertex3f(0.5*(meshmax[0]+meshmin[0])+0.05, meshmax[1]+0.15, 0.5*(meshmax[2]+meshmin[2]));

	  glVertex3f(0.5*(meshmax[0]+meshmin[0]), meshmax[1]+0.2, 0.5*(meshmax[2]+meshmin[2]));
	  glVertex3f(0.5*(meshmax[0]+meshmin[0])-0.05, meshmax[1]+0.15, 0.5*(meshmax[2]+meshmin[2]));

	  glVertex3f(0.5*(meshmax[0]+meshmin[0])+0.1, meshmax[1]+0.2, 0.5*(meshmax[2]+meshmin[2]));
	  glVertex3f(0.5*(meshmax[0]+meshmin[0])+0.08, meshmax[1]+0.18, 0.5*(meshmax[2]+meshmin[2]));

	  glVertex3f(0.5*(meshmax[0]+meshmin[0])+0.06, meshmax[1]+0.2, 0.5*(meshmax[2]+meshmin[2]));
	  glVertex3f(0.5*(meshmax[0]+meshmin[0])+0.08, meshmax[1]+0.18, 0.5*(meshmax[2]+meshmin[2]));

	  glVertex3f(0.5*(meshmax[0]+meshmin[0])+0.08, meshmax[1]+0.15, 0.5*(meshmax[2]+meshmin[2]));
	  glVertex3f(0.5*(meshmax[0]+meshmin[0])+0.08, meshmax[1]+0.18, 0.5*(meshmax[2]+meshmin[2]));

	  glColor3f(0.0, 0.0, 1.0);

	  glVertex3f(0.5*(meshmax[0]+meshmin[0]), 0.5*(meshmax[1]+meshmin[1]), meshmax[2]+0.2);
	  glVertex3f(0.5*(meshmax[0]+meshmin[0]), 0.5*(meshmax[1]+meshmin[1]), meshmin[2]-0.2);

	  glVertex3f(0.5*(meshmax[0]+meshmin[0]), 0.5*(meshmax[1]+meshmin[1]), meshmax[2]+0.2);
	  glVertex3f(0.5*(meshmax[0]+meshmin[0]), 0.5*(meshmax[1]+meshmin[1])+0.05, meshmax[2]+0.15);

	  glVertex3f(0.5*(meshmax[0]+meshmin[0]), 0.5*(meshmax[1]+meshmin[1]), meshmax[2]+0.2);
	  glVertex3f(0.5*(meshmax[0]+meshmin[0]), 0.5*(meshmax[1]+meshmin[1])-0.05, meshmax[2]+0.15);

	  glVertex3f(0.5*(meshmax[0]+meshmin[0]), 0.5*(meshmax[1]+meshmin[1])+0.06, meshmax[2]+0.15);
	  glVertex3f(0.5*(meshmax[0]+meshmin[0]), 0.5*(meshmax[1]+meshmin[1])+0.06, meshmax[2]+0.2);

	  glVertex3f(0.5*(meshmax[0]+meshmin[0]), 0.5*(meshmax[1]+meshmin[1])+0.1, meshmax[2]+0.15);
	  glVertex3f(0.5*(meshmax[0]+meshmin[0]), 0.5*(meshmax[1]+meshmin[1])+0.1, meshmax[2]+0.2);

	  glVertex3f(0.5*(meshmax[0]+meshmin[0]), 0.5*(meshmax[1]+meshmin[1])+0.1, meshmax[2]+0.15);
	  glVertex3f(0.5*(meshmax[0]+meshmin[0]), 0.5*(meshmax[1]+meshmin[1])+0.06, meshmax[2]+0.2);
	glEnd();

	glLineWidth(1.0);
}





void CZyView::DrawHandlesFrame()
{
	if (numhandles)
	{
		DrawCube(handlescenter, 0.01);

		glLineWidth(3.0);

		glBegin(GL_LINES);
		  glColor3f(1.0, 0.0, 0.0);

 		  glVertex3f(handlescenter[0]+0.1*handlesframe[0][0], handlescenter[1]+0.1*handlesframe[0][1], handlescenter[2]+0.1*handlesframe[0][2]);
		  glVertex3f(handlescenter[0]-0.1*handlesframe[0][0], handlescenter[1]-0.1*handlesframe[0][1], handlescenter[2]-0.1*handlesframe[0][2]);

		  glColor3f(0.0, 1.0, 0.0);

		  glVertex3f(handlescenter[0]+0.1*handlesframe[1][0], handlescenter[1]+0.1*handlesframe[1][1], handlescenter[2]+0.1*handlesframe[1][2]);
		  glVertex3f(handlescenter[0]-0.1*handlesframe[1][0], handlescenter[1]-0.1*handlesframe[1][1], handlescenter[2]-0.1*handlesframe[1][2]);

		  glColor3f(0.0, 0.0, 1.0);

		  glVertex3f(handlescenter[0]+0.1*handlesframe[2][0], handlescenter[1]+0.1*handlesframe[2][1], handlescenter[2]+0.1*handlesframe[2][2]);
		  glVertex3f(handlescenter[0]-0.1*handlesframe[2][0], handlescenter[1]-0.1*handlesframe[2][1], handlescenter[2]-0.1*handlesframe[2][2]);
		glEnd();

		glLineWidth(1.0);
	}
}

void CZyView::OnObjectMove() 
{
	// TODO: Add your command handler code here
	CZyDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);	

	if (pDoc->meshmodel)
	{
		flagobjectmove = true;
		flagobjectrotation = false;
		flagobjectzoom = false;
		flagobjecthandles = false;
		flagobjectboundary = false;
		flaghandlesinteraction = false;
		flaghandlesrotation = false;

		Draw();
	}
}

void CZyView::OnUpdateObjectMove(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	if (flagobjectmove)
		pCmdUI->SetCheck(TRUE);
	else
		pCmdUI->SetCheck(FALSE);
}

void CZyView::GetPointFromScreen(int screenx, int screeny, GLdouble depth, GLdouble spacepoint[3])
{
	GLint realy, viewport[4];
	GLdouble mvmatrix[16];
	GLdouble projmatrix[16];	

	glGetIntegerv(GL_VIEWPORT, viewport);
	glGetDoublev(GL_MODELVIEW_MATRIX, mvmatrix);
	glGetDoublev(GL_PROJECTION_MATRIX, projmatrix);

	realy = viewport[3] - (GLint) screeny - 1;

	gluUnProject((GLdouble)screenx, (GLdouble)realy, depth, mvmatrix, projmatrix, viewport,
				 &spacepoint[0], &spacepoint[1], &spacepoint[2]);
}

void CZyView::GetPointFromSpace(float spacex, float spacey, float spacez, GLdouble screenpoint[3])
{
	GLint viewport[4];
	GLdouble mvmatrix[16];
	GLdouble projmatrix[16];

	glGetIntegerv(GL_VIEWPORT, viewport);
	glGetDoublev(GL_MODELVIEW_MATRIX, mvmatrix);
	glGetDoublev(GL_PROJECTION_MATRIX, projmatrix);

	gluProject((GLdouble)spacex, (GLdouble)spacey, (GLdouble)spacez, mvmatrix, projmatrix, viewport,
			   &screenpoint[0], &screenpoint[1], &screenpoint[2]);

	screenpoint[1] = (GLdouble)viewport[3] - screenpoint[1] -1;
}

void CZyView::ObjectMove(GLMmodel *pmesh, int prepointx, int prepointy, int curpointx, int curpointy, float displacement[3])
{
	GLdouble meshvertexprojection[3];

	GetPointFromSpace(pmesh->vertices[3*1+0], pmesh->vertices[3*1+1],
 			          pmesh->vertices[3*1+2], meshvertexprojection);

 	GLdouble prepoint[3], curpoint[3];

	GetPointFromScreen(prepointx, prepointy, meshvertexprojection[2], prepoint);
	GetPointFromScreen(curpointx, curpointy, meshvertexprojection[2], curpoint);

	for (int i = 0; i < 3; i++)
 	{
 		displacement[i] = curpoint[i] - prepoint[i];
 	}
}

void CZyView::OnMouseMove(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	if (islbpressed)
	{
	    curpointx = point.x;
		curpointy = point.y;

		CZyDoc* pDoc = GetDocument();
		ASSERT_VALID(pDoc);

		if (pDoc->meshmodel)
		{
			if (flagobjectmove)
			{
				float displacement[3];
				ObjectMove(pDoc->meshmodel, prepointx, prepointy, curpointx, curpointy, displacement);

				tx += displacement[0];
				ty += displacement[1];
				tz += displacement[2];
			}
			else if (flagobjectrotation)
			{
				rx += ObjectRotation(prepointy, curpointy);
				ry += ObjectRotation(prepointx, curpointx);
			}
			else if (flagobjectzoom)
			{
				viewangle += ObjectZoom(prepointx, prepointy, curpointx, curpointy);

				RECT rect;
				GetClientRect(&rect);
				OnSize(SIZE_RESTORED, rect.right, rect.bottom);
			}
			else if (flaghandlesinteraction)
			{
				if (flaghandlesrotation)
				{
					HandlesRotation(pDoc->meshmodel, prepointx, prepointy, curpointx, curpointy);
				}
				else
				{
					HandlesInteraction(pDoc->meshmodel, prepointx, prepointy, curpointx, curpointy);
				}
			}
		}

		prepointx = curpointx;
        prepointy = curpointy;

		Draw();
	}

	CView::OnMouseMove(nFlags, point);
}

void CZyView::HandlesInteraction(GLMmodel *pmesh, int prepointx, int prepointy, int curpointx, int curpointy)
{
	if (!handleslist)
	{
		return;
	}

	GLdouble handleprojection[3];

	GetPointFromSpace(pmesh->vertices[3*handleslist->index], pmesh->vertices[3*handleslist->index+1],
					  pmesh->vertices[3*handleslist->index+2], handleprojection);

	GLdouble prepoint[3], curpoint[3], displacement[3];

	GetPointFromScreen(prepointx, prepointy, handleprojection[2], prepoint);
	GetPointFromScreen(curpointx, curpointy, handleprojection[2], curpoint);

	for (int j = 0; j < 3; j++)
	{
		displacement[j] = curpoint[j] - prepoint[j];
	}

	IndexList *p1 = handleslist;

	while (p1)
	{
		for (int j = 0; j < 3; j++)
		{
			pmesh->vertices[3*p1->index+j] += displacement[j];
		}

		p1 = p1->next;
	}

	for (int j = 0; j < 3; j++)
	{
		handlescenter[j] += displacement[j];
	}
}

void CZyView::HandlesRotation(GLMmodel *pmesh, int prepointx, int prepointy, int curpointx, int curpointy)
{
	if (!handleslist)
	{
		return;
	}

	float angle;
	angle = (prepointy-curpointy)*PI/180;

	totalangle += angle;

	float cosangle = cos(angle), sinangle = sin(angle);	

	double localvector[3], deformedlocalvector[3];
	double localcoordinate[3], deformedlocalcoordinate[3];

	IndexList *p1 = handleslist;

	while (p1)
	{
		for (int j = 0; j < 3; j++)
		{
			localvector[j] = pmesh->vertices[3*p1->index+j]-handlescenter[j];
		}

		for (int j = 0; j < 3; j++)
		{
			localcoordinate[j] = Dot(localvector, handlesframe[j]);
		}

		if (rotationaxis == axisz)
		{
			deformedlocalcoordinate[0] = cosangle*localcoordinate[0]-sinangle*localcoordinate[1];
			deformedlocalcoordinate[1] = sinangle*localcoordinate[0]+cosangle*localcoordinate[1];
			deformedlocalcoordinate[2] = localcoordinate[2];
		}
		else if (rotationaxis == axisx)
		{
			deformedlocalcoordinate[0] = localcoordinate[0];
			deformedlocalcoordinate[1] = cosangle*localcoordinate[1]-sinangle*localcoordinate[2];
			deformedlocalcoordinate[2] = sinangle*localcoordinate[1]+cosangle*localcoordinate[2];
		}
		else
		{
			deformedlocalcoordinate[0] = cosangle*localcoordinate[0]-sinangle*localcoordinate[2];
			deformedlocalcoordinate[1] = localcoordinate[1];
			deformedlocalcoordinate[2] = sinangle*localcoordinate[0]+cosangle*localcoordinate[2];
		}

		for (int j = 0; j < 3; j++)
		{
			deformedlocalvector[j] = 0.0;
			
			for (int k = 0; k < 3; k++)
			{
				deformedlocalvector[j] += handlesframe[k][j] * deformedlocalcoordinate[k];
			}

			pmesh->vertices[3*p1->index+j] = deformedlocalvector[j]+handlescenter[j];
		}

		p1 = p1->next;
	}
}

void CZyView::HandlesFrameRotation()
{
	if (!handleslist)
	{
		return;
	}

	float cosangle = cos(totalangle), sinangle = sin(totalangle);
	double localcoordinate[3], deformedlocalcoordinate[3], deformedlocalvector[3][3];

	for (int i = 0; i < 3; i++)
	{
		if (i == 0)
		{
			localcoordinate[0] = 1.0;
			localcoordinate[1] = 0.0;
			localcoordinate[2] = 0.0;
		}
		else if (i == 1)
		{
			localcoordinate[0] = 0.0;
			localcoordinate[1] = 1.0;
			localcoordinate[2] = 0.0;
		}
		else
		{
			localcoordinate[0] = 0.0;
			localcoordinate[1] = 0.0;
			localcoordinate[2] = 1.0;
		}

		if (rotationaxis == axisz)
		{
			deformedlocalcoordinate[0] = cosangle*localcoordinate[0]-sinangle*localcoordinate[1];
			deformedlocalcoordinate[1] = sinangle*localcoordinate[0]+cosangle*localcoordinate[1];
			deformedlocalcoordinate[2] = localcoordinate[2];
		}
		else if (rotationaxis == axisx)
		{
			deformedlocalcoordinate[0] = localcoordinate[0];
			deformedlocalcoordinate[1] = cosangle*localcoordinate[1]-sinangle*localcoordinate[2];
			deformedlocalcoordinate[2] = sinangle*localcoordinate[1]+cosangle*localcoordinate[2];
		}
		else
		{
			deformedlocalcoordinate[0] = cosangle*localcoordinate[0]-sinangle*localcoordinate[2];
			deformedlocalcoordinate[1] = localcoordinate[1];
			deformedlocalcoordinate[2] = sinangle*localcoordinate[0]+cosangle*localcoordinate[2];
		}

		for (int j = 0; j < 3; j++)
		{
			deformedlocalvector[i][j] = 0.0;

			for (int k = 0; k < 3; k++)
			{
				deformedlocalvector[i][j] += handlesframe[k][j] * deformedlocalcoordinate[k];
			}
		}
	}

	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			handlesframe[i][j] = deformedlocalvector[i][j];
		}
	}

	totalangle = 0.0;
}

void CZyView::OnLButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	CZyDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	if (pDoc->meshmodel)
	{
		prepointx = point.x;
		prepointy = point.y;

		prepointxrecord = point.x;
		prepointyrecord = point.y;

		curpointx = point.x;
		curpointy = point.y;

		islbpressed = true;

		Draw();
	}

	CView::OnLButtonDown(nFlags, point);
}

void CZyView::OnLButtonUp(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	CZyDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	if (pDoc->meshmodel)
	{
		if (flagobjecthandles)
		{
			int pointmaxx, pointmaxy, pointminx, pointminy;

 			pointmaxx = max(prepointxrecord, curpointx);
 			pointmaxy = max(prepointyrecord, curpointy);
 			pointminx = min(prepointxrecord, curpointx);
 			pointminy = min(prepointyrecord, curpointy);

			ObjectHandles(pDoc->meshmodel, pointmaxx, pointminx, pointmaxy, pointminy, flagobjectboundary);
		}
		else if (flaghandlesinteraction) //  && precomputationflag
		{
			if (flaghandlesrotation)
			{
				HandlesFrameRotation();
			}
		}

		islbpressed = false;

		Draw();
	}

	CView::OnLButtonUp(nFlags, point);
}

void CZyView::ObjectHandles(GLMmodel *pmesh, int rectmaxx, int rectminx, int rectmaxy, int rectminy, bool flag)
{
	GLdouble meshprojection[3];
	IndexList *p1;

	for (int i = 1; i <= (int)pmesh->numvertices; i++)
	{
		GetPointFromSpace(pmesh->vertices[3*i], pmesh->vertices[3*i+1],
		                  pmesh->vertices[3*i+2], meshprojection);

		if (meshprojection[0] >= rectminx && meshprojection[0] <= rectmaxx &&
 			meshprojection[1] >= rectminy && meshprojection[1] <= rectmaxy &&
			!pIndexList.IsInList(i, handleslist) && !pIndexList.IsInList(i, boundaryverticeslist))
		{
//			printf("\nselectedindex = %d", i);

			if (flag)
			{
				if (!boundaryverticeslist)
				{
					boundaryverticeslist = boundarytail = new IndexList;
					boundarytail->index = i;
					boundarytail->next = NULL;
				}
				else
				{
					p1 = new IndexList;
					p1->index = i;
					p1->next = NULL;

					boundarytail->next = p1;
					boundarytail = p1;
				}

				numboundaryvertices++;
			}
			else
			{
				if (!handleslist)
				{
					handleslist = handlestail = new IndexList;
					handlestail->index = i;
					handlestail->next = NULL;
				}
				else
				{
					p1 = new IndexList;
					p1->index = i;
					p1->next = NULL;

					handlestail->next = p1;
					handlestail = p1;
				}

				for (int j = 0; j < 3; j++)
				{
					handlescenter[j] *= numhandles;
					handlescenter[j] += pmesh->vertices[3*i+j];
					handlescenter[j] /= (numhandles+1);
				}

				numhandles++;

				for (int j = 0; j < 3; j++)
				{
					for (int k = 0; k < 3; k++)
					{
						handlescovariancematrix[3*j+k] += pmesh->vertices[3*i+j]
								                         *pmesh->vertices[3*i+k];
					}
				}
			}
		}
	}

	if (!flag && numhandles > 0)
	{
		if (numhandles == 1)
 		{
			for (int i = 0; i < 3; i++)
			{
				for (int j = 0; j < 3; j++)
				{
					if (i == j)
					{
						handlesframe[i][j] = 1.0;
					}
					else
					{
						handlesframe[i][j] = 0.0;
					}
				}
			}
 		}
		else
		{
			PrincipalComponentAnalysis(handlescovariancematrix, handlesframe);
		}
	}
}

float CZyView::ObjectRotation(int prepointxy, int curpointxy)
{
	return 0.3*(curpointxy-prepointxy);
}

void CZyView::OnObjectRotation() 
{
	// TODO: Add your command handler code here
	CZyDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);	

	if (pDoc->meshmodel)
	{
		flagobjectmove = false;
		flagobjectrotation = true;
		flagobjectzoom = false;
		flagobjecthandles = false;
		flagobjectboundary = false;
		flaghandlesinteraction = false;
		flaghandlesrotation = false;
		
		Draw();
	}
}

void CZyView::OnUpdateObjectRotation(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	if (flagobjectrotation)
		pCmdUI->SetCheck(TRUE);
	else
		pCmdUI->SetCheck(FALSE);
}

float CZyView::ObjectZoom(int prepointx, int prepointy, int curpointx, int curpointy)
{
	float viewangle;
	viewangle = (curpointy - prepointy - curpointx + prepointx)/40.0f;

	return viewangle;
}

void CZyView::OnObjectZoom() 
{
	// TODO: Add your command handler code here
	CZyDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);	

	if (pDoc->meshmodel)
	{
		flagobjectmove = false;
		flagobjectrotation = false;
		flagobjectzoom = true;
		flagobjecthandles = false;
		flagobjectboundary = false;
		flaghandlesinteraction = false;
		flaghandlesrotation = false;

		Draw();
	}
}

void CZyView::OnUpdateObjectZoom(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	if (flagobjectzoom)
		pCmdUI->SetCheck(TRUE);
	else
		pCmdUI->SetCheck(FALSE);
}

void CZyView::OnObjectHandles() 
{
	// TODO: Add your command handler code here
	CZyDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);	

	if (pDoc->meshmodel)
	{
		flagobjectmove = false;
		flagobjectrotation = false;
		flagobjectzoom = false;
		flagobjecthandles = true;
		flaghandlesinteraction = false;
		flaghandlesrotation = false;

		Draw();
	}
}

void CZyView::OnUpdateObjectHandles(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	if (flagobjecthandles)
		pCmdUI->SetCheck(TRUE);
	else
		pCmdUI->SetCheck(FALSE);
}

void CZyView::OnObjectBoundary() 
{
	// TODO: Add your command handler code here
	CZyDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);	

	if (pDoc->meshmodel)
	{
		flagobjectboundary = !flagobjectboundary;

		flagobjectmove = false;
		flagobjectrotation = false;
		flagobjectzoom = false;
		flaghandlesinteraction = false;
		flaghandlesrotation = false;

		Draw();
	}
}

void CZyView::OnUpdateObjectBoundary(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	if (flagobjectboundary)
		pCmdUI->SetCheck(TRUE);
	else
		pCmdUI->SetCheck(FALSE);
}

void CZyView::OnHandlesInteraction() 
{
	// TODO: Add your command handler code here
	CZyDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);	

	if (pDoc->meshmodel)
	{
		flagobjectmove = false;
		flagobjectrotation = false;
		flagobjectzoom = false;
		flagobjecthandles = false;
		flagobjectboundary = false;
		flaghandlesinteraction = true;

		Draw();
	}
}

void CZyView::OnUpdateHandlesInteraction(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	if (flaghandlesinteraction)
		pCmdUI->SetCheck(TRUE);
	else
		pCmdUI->SetCheck(FALSE);
}

void CZyView::OnRegionGrowing() 
{
	// TODO: Add your command handler code here
	CZyDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	if (pDoc->meshmodel)
	{
		isdeformed = new bool [pDoc->meshmodel->numvertices];
		for (int i = 0; i < (int)pDoc->meshmodel->numvertices; i++)
		{
			isdeformed[i] = false;
		}

		IndexList *p1 = handleslist;
		while (p1)
		{
			isdeformed[p1->index-1] = true;
			p1 = p1->next;
		}

		p1 = boundaryverticeslist;
		while (p1)
		{
			isdeformed[p1->index-1] = true;
			p1 = p1->next;
		}

		p1 = handleslist;
		while (p1)
		{
			RegionGrowing(pDoc->verticesvindices[p1->index]);

			p1 = p1->next;
		}

//		printf("numhandles = %d, numboundaryvertices = %d, numROI = %d\n", numhandles, numboundaryvertices, numROI);

		deformationindex = new int [pDoc->meshmodel->numvertices];
		for (int i = 0; i < (int)pDoc->meshmodel->numvertices; i++)
		{
			deformationindex[i] = -1;
		}

		int count = 0;

		p1 = handleslist;
		while (p1)
		{
			deformationindex[p1->index-1] = count;

			p1 = p1->next;
			count++;
		}

		p1 = boundaryverticeslist;
		while (p1)
		{
			deformationindex[p1->index-1] = count;

			p1 = p1->next;
			count++;
		}

		p1 = ROIlist;
		while (p1)
		{
			deformationindex[p1->index-1] = count;

			p1 = p1->next;
			count++;
		}

		Draw();
	}
}

void CZyView::RegionGrowing(IndexList *pindex)
{
	CZyDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	IndexList *p2;

	while (pindex)
	{
		if (!isdeformed[pindex->index-1])
		{
			if (!ROIlist)
			{
				ROIlist = ROItail = new IndexList;	
			}
			else
			{
				p2 = new IndexList;

				ROItail->next = p2;
				ROItail = p2;
			}			

			ROItail->index = pindex->index;
			ROItail->next = NULL;

			isdeformed[pindex->index-1] = true;
			numROI++;

			RegionGrowing(pDoc->verticesvindices[pindex->index]);
		}

		pindex = pindex->next;
	}
}

void CZyView::OnRelease() 
{
	// TODO: Add your command handler code here
	if (handleslist)
	{
		pIndexList.DeleteIndexList(handleslist);
		handleslist = NULL;
		handlestail = NULL;
		numhandles = 0;

		flaghandlesframe = false;
	}

	if (boundaryverticeslist)
	{
		pIndexList.DeleteIndexList(boundaryverticeslist);
		boundaryverticeslist = NULL;
		boundarytail = NULL;
		numboundaryvertices = 0;
	}

	if (ROIlist)
	{
		pIndexList.DeleteIndexList(ROIlist);
		ROIlist = NULL;
		ROItail = NULL;
		numROI = 0;
	}

	if (isdeformed)
	{
		delete []isdeformed;
		isdeformed = NULL;
	}

	if (deformationindex)
	{
		delete []deformationindex;
		deformationindex = NULL;
	}

	Draw();
}

void CZyView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	// TODO: Add your message handler code here and/or call default
	if (flaghandlesinteraction)
	{
	    switch (nChar)
		{
			case 'X':
			case 'x':
				rotationaxis = axisx;
				break;

			case 'Y':
			case 'y':
				rotationaxis = axisy;
				break;

			case 'Z':
			case 'z':
				rotationaxis = axisz;
				break;

			case 'R':
			case 'r':
				flaghandlesrotation = !flaghandlesrotation;
				rotationaxis = axisz;
				break;
		}

		Draw();
	}

	CView::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CZyView::OnHandlesFrame() 
{
	// TODO: Add your command handler code here
	if (numhandles)
	{
		flaghandlesframe = !flaghandlesframe;
		Draw();
	}
}

void CZyView::OnUpdateHandlesFrame(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	if(flaghandlesframe)
		pCmdUI->SetCheck(TRUE);
	else
		pCmdUI->SetCheck(FALSE);
}

void CZyView::OnDrawWireFrame() 
{
	// TODO: Add your command handler code here
	CZyDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	if (pDoc->meshmodel)
	{
		flagdrawwireframe = !flagdrawwireframe;
		Draw();
	}
}

void CZyView::OnUpdateDrawWireFrame(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	if(flagdrawwireframe)
		pCmdUI->SetCheck(TRUE);
	else
		pCmdUI->SetCheck(FALSE);
}

void CZyView::OnHierarA()
{
	//DLG_SIZE_VAR dlg;
	//int maxsize = 50;
	//double maxvar = 0.1;
	//double maxangle = 1.04;
	//double curvature = 10;

	int maxsize = 20;
	double maxvar = 0.1;
	double maxangle = 1.04;
	double curvature = 10;

	/*
	if (dlg.DoModal() == IDOK)
	{
		maxsize = dlg.m_maxsize;
		maxvar = dlg.m_maxvar;
		maxangle = dlg.m_maxangle;
		curvature = dlg.m_curvature;
	}
	else
		return;
	*/
	if (maxsize <= 0)
	{
		return;
	}
	if (maxvar <= 0)
	{
		return;
	}

	CZyDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	if (pDoc->meshmodel == NULL)
		return;
	/*
	if (pCluster)
	{
		pCluster->DeleteClusters();
	}*/

	pCluster->maxsize = maxsize;
	pCluster->maxvar = maxvar;
	pCluster->maxangle = maxangle;
	pCluster->maxcurvature = curvature;
	pDoc->ReconstructVertexNormals();
	pCluster->AdvancedHierar(pDoc->meshmodel, pDoc->reconstructednormals,
		pDoc->verticesvindices, pDoc->numofneighboringvertices,
		pDoc->verticestindices);
	colorflag = true;
	Draw();
}

void CZyView::OnRegionGrow()
{
	//DLG_SIZE_VAR dlg;
	//int maxsize = 50;//origin
	//double maxvar = 0.1;
	//double maxangle = 1.04;
	//float weight = 0.8;
	//float curvature = 10;

	int maxsize = 20;
	double maxvar = 0.4;
	double maxangle = 1.04;
	float weight = 0.8;
	float curvature = 10;

	/*
	if (dlg.DoModal() == IDOK)
	{
		maxsize = dlg.m_maxsize;
		maxvar = dlg.m_maxvar;
		maxangle = dlg.m_maxangle;
		weight = dlg.m_weight;
		curvature = dlg.m_curvature;
	}
	else
		return;
	*/
	if (maxsize <= 0)
	{
		return;
	}
	if (maxvar <= 0)
	{
		return;
	}

	CZyDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	if (pDoc->meshmodel == NULL)
		return;
	/*
	if (pCluster)
	{
		pCluster->DeleteClusters();
	}
	else
	{
		pCluster = new Cluster;
	}*/

	pCluster->maxsize = maxsize;
	pCluster->maxvar = maxvar;
	pCluster->maxangle = maxangle;
	pCluster->weight = weight;
	pCluster->maxcurvature = curvature;
	pDoc->ReconstructVertexNormals();
	pCluster->Regiongrow(pDoc->meshmodel, pDoc->verticesvindices,
		pDoc->reconstructednormals, pDoc->numofneighboringvertices,
		pDoc->verticestindices);
	colorflag = true;
	Draw();
}

#include <string>
char filename[20] = "1994.txt";
float **HKS;
float **HKS_cluster;
float max_HKS = 0.0;

void CZyView::Weight(_cluster *cluster, int index, float *weight)
{
	_cluster *tmpcluster = pCluster->pcluster;

	//printf("Finding index position in pCluster(index:%d)\n", index);
	int i;
	for (i = 0; i < index; ++i)
	{
		tmpcluster = tmpcluster->nextcluster;
	}
	//printf("pCluster->pcluster->index:%d\n", pCluster->pcluster->index);
	
	//location_diff
	float diffLocation, diff_x, diff_y, diff_z;
	diff_x = cluster->center[0] - tmpcluster->center[0];
	diff_y = cluster->center[1] - tmpcluster->center[1];
	diff_z = cluster->center[2] - tmpcluster->center[2];
	diffLocation = sqrt(diff_x * diff_x + diff_y * diff_y + diff_z * diff_z);

	//normal_diff
	float diffNormal, diff_nx, diff_ny, diff_nz;
	diff_nx = cluster->normal[0] - tmpcluster->normal[0];
	diff_ny = cluster->normal[1] - tmpcluster->normal[1];
	diff_nz = cluster->normal[2] - tmpcluster->normal[2];
	diffNormal = sqrt(diff_nx * diff_nx + diff_ny * diff_ny + diff_nz * diff_nz);

	//SDF_diff
	float diffSDF;
	diffSDF = cluster->SDF - tmpcluster->SDF;
	diffSDF = fabs(diffSDF);

	//AGD_diff
	float diffAGD;
	diffAGD = cluster->AGD - tmpcluster->AGD;
	diffAGD = fabs(diffAGD);

	//CF_diff
	float diffCF;
	diffCF = cluster->CF - tmpcluster->CF;
	diffCF = fabs(diffCF);

	//HKS_diff
	float diffHKS = 0.0;
	float diffHKS_tensor[102];
	for (int i = 1; i < 102; ++i)
	{
		diffHKS_tensor[i] = HKS_cluster[cluster->index][i] - HKS_cluster[tmpcluster->index][i];
	}

	for (int i = 1; i < 102; ++i)
	{
		diffHKS = diffHKS + diffHKS_tensor[i] * diffHKS_tensor[i];
	}
	diffHKS = sqrt(diffHKS);
	//printf("cluster->normal[0]:%f, cluster->normal[1]:%f, cluster->normal[2]:%f\n", cluster->normal[0], cluster->normal[1], cluster->normal[2]);
	//weight = 0.0 * diffLocation + 1.0 * diffNormal;
	//weight = exp(weight);
	float a0, a1, a2, a3, a4, a5;
	a0 = 0.0; a1 = 0.0; a2 = 5.0000; a3 = 0.000; a4 = 0.0000; a5 = 0.00;
	weight[0] = a0 * diffLocation + a1 * diffNormal + a2 * diffSDF + a3 * diffAGD + a4 * diffCF + a5 * diffHKS;
	weight[1] = diffNormal;
	weight[2] = diffSDF;
	weight[3] = diffAGD;
	weight[4] = diffCF;
	weight[5] = diffHKS;
	//return weight;
}

#define FEATURE 0
void CZyView::InsertSort(SortedList *SortList, float *weight, int index1, int index2)
{
	//printf("Inserting\n");
	SortedList *tmpSort = (SortedList*)malloc(sizeof(SortedList));
	tmpSort->weight[0] = weight[0];
	tmpSort->weight[1] = weight[1];
	tmpSort->weight[2] = weight[2];
	tmpSort->weight[3] = weight[3];
	tmpSort->weight[4] = weight[4];
	tmpSort->weight[5] = weight[5];
	//printf("weight:%f\n", weight);
	tmpSort->index1 = index1;
	tmpSort->index2 = index2;
	if (SortList->Next == NULL)
	{
		SortList->Next = tmpSort;
		tmpSort->Next = NULL;
	}
	else
	{
		SortedList *pos, *posNext;
		pos = SortList;
		posNext = SortList->Next;
		while ((posNext != NULL) && (posNext->weight[FEATURE] <= weight[FEATURE]))
		{
			//take away same weight
			//if (posNext->weight == weight)
			if((pos->Next->weight[FEATURE]-weight[FEATURE]) <= 0.0001)
			{
				if (((posNext->index1 == tmpSort->index1) && (posNext->index2 == tmpSort->index2))
					|| ((posNext->index1 == tmpSort->index2) && (posNext->index2 == tmpSort->index1)))
				{
					goto DonotInsert;
				}
			}
			pos = pos->Next;
			posNext = posNext->Next;
		}

		SortedList *tmp = pos->Next;
		pos->Next = tmpSort;
		tmpSort->Next = tmp;
		numofweight++;

		//printf("sortlist->weight:%f\n", tmpSort->weight);
	}
	DonotInsert:
	;
}
float *SDF;
float *AGD;
float *CF;
void CZyView::ConstructGraph(GLMmodel *meshmodel, IndexList **neighborvertices, float *normalofvertices,
	int *numofneighboringvertices, IndexList **verticestindices)
{
	/*clock_t start, stop;
	double time;
	start = clock();*/

	//caculate SDF for each vertex
	//Cumpute_SDF()
	//float *SDF = (float*)malloc(sizeof(float)*(meshmodel->numvertices+1));//begin: 1,2,3,...
	SDF = (float*)malloc(sizeof(float)*(meshmodel->numvertices + 1));//begin: 1,2,3,...
	Compute_SDF(meshmodel, verticestindices, SDF);
	printf("SDF[0]:%f\n", SDF[0]);
	printf("SDF[1]:%f\n", SDF[1]);
	//printf("SDF[15910]:%f\n", SDF[15910]);
	Normalized_features(SDF, meshmodel->numvertices + 1);
	//printf("SDF[15910]:%f\n", SDF[15910]);

	//Compute_AGD
	//float *AGD = (float*)malloc(sizeof(float)*(meshmodel->numvertices + 1));//begin: 1,2,3,...
	AGD = (float*)malloc(sizeof(float)*(meshmodel->numvertices + 1));//begin: 1,2,3,...
	Compute_AGD(meshmodel, AGD);
	printf("AGD[0]:%f\n", AGD[0]);
	printf("AGD[1]:%f\n", AGD[1]);
	//printf("AGD[15910]:%f\n", AGD[15910]);
	Normalized_features(AGD, meshmodel->numvertices + 1);
	//printf("AGD[15910]:%f\n", AGD[15910]);

	//Compute_CF
	//float *CF = (float*)malloc(sizeof(float)*(meshmodel->numvertices + 1));//begin: 1,2,3,...
	CF = (float*)malloc(sizeof(float)*(meshmodel->numvertices + 1));//begin: 1,2,3,...
	Compute_CF(meshmodel, CF);
	printf("CF[0]:%f\n", CF[0]);
	printf("CF[1]:%f\n", CF[1]);
	//printf("CF[15910]:%f\n", CF[15910]);
	Normalized_features(CF, meshmodel->numvertices + 1);
	//printf("CF[15910]:%f\n", CF[15910]);

	//Compute_HKS
	HKS = (float**)malloc(sizeof(float*) * (meshmodel->numvertices + 1));//为二维数组分配15911行 
	for (int i = 0; i < meshmodel->numvertices + 1; ++i)//为每列分配101个大小空间 
	{
		HKS[i] = (float*)malloc(sizeof(float) * 102);
	}
	Compute_HKS(meshmodel, HKS);
	
	HKS_cluster = (float**)malloc(sizeof(float*)*(pCluster->numCluster + 1));
	for (int i = 0; i < pCluster->numCluster + 1; ++i)//为每列分配101个大小空间 
	{
		HKS_cluster[i] = (float*)malloc(sizeof(float) * 102);
	}

	//caculate features for cluster(normal, SDF, AGD, CF)
	_cluster *clusterForFeatures = pCluster->pcluster->nextcluster;
	int iabc = 0;
	
	while (clusterForFeatures != NULL)
	{
		IndexList *pverticesForFeatures = clusterForFeatures->pvertices;
		//printf("beign i:%d\n", iabc++);
		float normal[3] = { 0.0, 0.0, 0.0 };
		float SDF_cluster = 0.0;
		float AGD_cluster = 0.0;
		float CF_cluster = 0.0;
		//memset(HKS_cluster, 0, sizeof(HKS_cluster[clusterForFeatures->index]));//set 0
		for (int i = 1; i < 102; ++i)
		{
			HKS_cluster[clusterForFeatures->index][i] = 0;
		}

		while (pverticesForFeatures != NULL)
		{
			//caculate vertex normal
			int index = pverticesForFeatures->index;
			int numNeighbor = 0;
			
			float neighborFaceNormal[3] = {0.0, 0.0, 0.0};
			Compute_VertexNormal(meshmodel, verticestindices, index, neighborFaceNormal);
			
			glmNormalize(neighborFaceNormal);

			normal[0] = normal[0] + neighborFaceNormal[0];
			normal[1] = normal[1] + neighborFaceNormal[1];
			normal[2] = normal[2] + neighborFaceNormal[2];

			SDF_cluster = SDF_cluster + SDF[index];
			AGD_cluster = AGD_cluster + AGD[index];
			CF_cluster = CF_cluster + CF[index];
	
			for (int i = 1; i < 102; ++i)
			{
				HKS_cluster[clusterForFeatures->index][i] = HKS_cluster[clusterForFeatures->index][i] + HKS[index][i];
			}

			pverticesForFeatures = pverticesForFeatures->next;
		}

		//clusterForNormal->normal[0] = normal[0] / float(clusterForNormal->order);
		//clusterForNormal->normal[1] = normal[1] / float(clusterForNormal->order);
		//clusterForNormal->normal[2] = normal[2] / float(clusterForNormal->order);
		glmNormalize(normal);

		clusterForFeatures->normal[0] = normal[0];
		clusterForFeatures->normal[1] = normal[1];
		clusterForFeatures->normal[2] = normal[2];

		clusterForFeatures->SDF = SDF_cluster / float(clusterForFeatures->order);
		clusterForFeatures->AGD = AGD_cluster / float(clusterForFeatures->order);
		clusterForFeatures->CF = CF_cluster / float(clusterForFeatures->order);
		for (int i = 1; i < 102; ++i)
		{
			HKS_cluster[clusterForFeatures->index][i] = HKS_cluster[clusterForFeatures->index][i]/ float(clusterForFeatures->order);
		}
		//printf("%d\n", clusterForNormal->index);

		clusterForFeatures = clusterForFeatures->nextcluster;
	}//caculate feature
	//printf("0\n");
	
	_cluster *tmpcluster = pCluster->pcluster->nextcluster;// start from 1，not 0
	Cluster *copyCluster = pCluster;
	
	clock_t start, stop;
	double time;
	start = clock();
	//SortedList *SortList;
	printf("copyCluster->numCluster:%d\n", copyCluster->numCluster);
	SortList = (SortedList *)malloc(sizeof(SortedList));//
	SortList->Next = NULL;

	for (int i = 1; i < copyCluster->numCluster+1; ++i)	//numcluster loop
	{
		int numEachCluster = tmpcluster->order;//each cluster's number

		IndexList *copypvertices = tmpcluster->pvertices;
		for (int j = 0; j < numEachCluster; ++j)	//numEachCluster's number
		{
			//printf("j:%d\n", j);

			int index = copypvertices->index;//index in current Cluster

			IndexList  *q1;
			q1 = neighborvertices[index];
	
			while(q1 != NULL)//numofneighboringvertices loop
			{
				int IndexInOriginModel = q1->index;//face index

				if (copyCluster->cluOfV[IndexInOriginModel] != i) //add Cluster to adjacent list
				{
					EdgeNode *curNode;
					curNode = (EdgeNode*)malloc(sizeof(EdgeNode));
					curNode->index = copyCluster->cluOfV[IndexInOriginModel];
					curNode->weight = (float*)malloc(sizeof(float) * 6);
					//curNode->weight = Weight(tmpcluster, curNode->index);
					Weight(tmpcluster, curNode->index, curNode->weight);
					//printf("weighted\n");
					InsertSort(SortList, curNode->weight, i, copyCluster->cluOfV[IndexInOriginModel]);
					//printf("sorted\n");
				}
				//printf("Next neighbor vertex\n");
				q1 = q1->next;
			}

			copypvertices = copypvertices->next;
			//printf("Go to next vertex in current Cluster\n");
		}

		tmpcluster = tmpcluster->nextcluster;
		//printf("Go to next Cluster\n");
	}

	//printf("SortList->weight:%f", SortList->weight);
	//int count = 0;
	//while (SortList != NULL)
	////while(count<500)
	//{
	//	//printf("SortList not empty\n");
	//	printf("weight:%f, index1:%d, index2:%d\n", SortList->weight, SortList->index1, SortList->index2);
	//	count++;
	//	SortList = SortList->Next;
	//}
	//printf("count:%d\n", count);
	//printf("pCluster:%d\n", pCluster->pcluster->nextcluster->pvertices->index);
	stop = clock();
	time = ((double)(stop - start)) / CLK_TCK;
	printf("\nConstructed Time: %lf\n", time);
	printf("Num of weight:%d\n", numofweight);
	printf("first weight:%f\n", SortList->Next->weight);
}

void CZyView::Exponential_weight(SortedList *SortList)
{
	printf("==================\n");
	SortedList *numSortList, *copySortList;
	numSortList = SortList->Next;
	copySortList = SortList->Next;//ignore head node(NULL)

	float average_weight = 0;
	float max_weight = 0;

	int numEdge = 0;
	while (numSortList != NULL) //head pointer
	{
		numEdge++;
		average_weight = average_weight + numSortList->weight[FEATURE];
		if (numSortList->weight[FEATURE] > max_weight)
		{
			max_weight = numSortList->weight[FEATURE];
		}
		numSortList = numSortList->Next;
	}

	average_weight = average_weight / numEdge;

	while (copySortList != NULL) //head pointer
	{
		float tmp_weight = (copySortList->weight[FEATURE] - average_weight) * (copySortList->weight[FEATURE] - average_weight);
		tmp_weight = tmp_weight / max_weight;
		copySortList->weight[FEATURE] = exp(tmp_weight);

		copySortList = copySortList->Next;
	}
}

void CZyView::Merge(GraphAdjList G, SortedList *SortList)
{
	SortedList *numSortList, *copySortList;
	numSortList = SortList;
	copySortList = SortList->Next;//ignore head node(NULL)

	int numEdge = 0;
	while (numSortList->Next != NULL) //head pointer
	{
		numEdge++;
		numSortList = numSortList->Next;
	}
	printf("numEdge:%d\n", numEdge);//num of edges
	printf("copySortList->weight:%f\n, index1:%d, index2:%d\n", copySortList->weight, copySortList->index1, copySortList->index2);
	printf("copySortList->Next->weight:%f, index1:%d, index2:%d\n", copySortList->Next->weight, copySortList->Next->index1, copySortList->Next->index2);
	printf("copySortList->Next->Next->weight:%f, index1:%d, index2:%d\n", copySortList->Next->Next->weight, copySortList->Next->Next->index1, copySortList->Next->Next->index2);
	
	mycluster *overSeg_cluster = (mycluster*)malloc(sizeof(mycluster) * (pCluster->numCluster + 1));//copy Cluster to mycluster

	_cluster *tmpCluster = pCluster->pcluster->nextcluster;

	printf("tmpCluster:%d\n", tmpCluster->pvertices->next->index);
	//tmpCluster = pCluster->pcluster->nextcluster;//first cluster is NULL
	printf("tmpCluster:%d\n", tmpCluster->index);
	printf("tmpCluster:%d\n", tmpCluster->pvertices->index);
	
	printf("pCluster:%d\n", pCluster->pcluster->nextcluster->index);
	// copy cluster 
	overSeg_cluster[0].index = 0;
	overSeg_cluster[0].order = 0;

	overSeg_cluster[0].numMergedCluster = 0;

	//overSeg_cluster[0].center[0] = tmpCluster->center[0];
	//overSeg_cluster[0].normal[0] = tmpCluster->normal[0];
	//overSeg_cluster[0].center[1] = tmpCluster->center[1];
	//overSeg_cluster[0].normal[1] = tmpCluster->normal[1];
	//overSeg_cluster[0].center[2] = tmpCluster->center[2];
	//overSeg_cluster[0].normal[2] = tmpCluster->normal[2];

	overSeg_cluster[0].Internal_DiffLocation = -1;
	overSeg_cluster[0].Internal_DiffNormal = -1;
	overSeg_cluster[0].Internal_DiffSDF = -1;
	overSeg_cluster[0].Internal_DiffAGD = -1;
	overSeg_cluster[0].Internal_DiffCF = -1;

	overSeg_cluster[0].belongsTo = 0;
	///copy pCluster's imformation
	for (int i = 1; i < pCluster->numCluster+1; ++i)
	{
		overSeg_cluster[i].index = tmpCluster->index;
		//printf("tmpCluster index:%d\n", tmpCluster->index);
		overSeg_cluster[i].order = tmpCluster->order;

		overSeg_cluster[i].numMergedCluster = 1;
		//printf("overSeg_cluster[%d].numMergedCluster:%d\n", i, overSeg_cluster[i].numMergedCluster);

		//overSeg_cluster[i].center[0] = tmpCluster->center[0];
		//overSeg_cluster[i].normal[0] = tmpCluster->normal[0];
		//overSeg_cluster[i].center[1] = tmpCluster->center[1];
		//overSeg_cluster[i].normal[1] = tmpCluster->normal[1];
		//overSeg_cluster[i].center[2] = tmpCluster->center[2];
		//overSeg_cluster[i].normal[2] = tmpCluster->normal[2];

		overSeg_cluster[i].Internal_DiffLocation = 0;
		overSeg_cluster[i].Internal_DiffNormal = 0;
		overSeg_cluster[i].Internal_DiffSDF = 0;
		overSeg_cluster[i].Internal_DiffAGD = 0;
		overSeg_cluster[i].Internal_DiffCF = 0;
	
		overSeg_cluster[i].belongsTo = tmpCluster->index;
		//printf("belongsto:%d\n", overSeg_cluster[i].belongsTo);
		tmpCluster = tmpCluster->nextcluster;
		//printf("tmpvertex:%d\n", tmpCluster->pvertices->index);
		//printf("next cluster\n");
	}
	printf("Copy cluster\n");
	printf("pCluster:%d\n", pCluster->pcluster->nextcluster->index);
	//printf("index:%d\n", overSeg_cluster[5].index);
	printf("weight:%f, index1:%d, index2:%d\n", copySortList->weight[FEATURE], copySortList->index1, copySortList->index2);
	//printf("pverteces:%d\n", overSeg_cluster[5].pvertices->index);

	clock_t start, stop;
	double time;
	start = clock();

	/// merge
	int merge = 0;
	int notmerge = 0;
	int neednot = 0;
	//printf("21:%d\n", overSeg_cluster[21].numMergedCluster);
	for (int i = 0; i < numEdge; ++i)
	{
		int index1 = copySortList->index1;
		int index2 = copySortList->index2;

		//judge current vertex belongs to which patch(cluster)
		int patch1 = overSeg_cluster[index1].belongsTo;
		while (patch1 != index1)
		{
			index1 = patch1;
			patch1 = overSeg_cluster[patch1].belongsTo;
		}
		int patch2 = overSeg_cluster[index2].belongsTo;
		while (patch2 != index2)
		{
			index2 = patch2;
			patch2 = overSeg_cluster[patch2].belongsTo;
		}
		//printf("patch1:%d, patch2:%d\n", patch1, patch2);
		
		if (patch1 == patch2)//belong to same cluster, needn't merge
		{
			int a = MIN(patch1, patch2);
			//overSeg_cluster[a].Internal_DiffLocation = MAX(copySortList->weight[0], overSeg_cluster[a].Internal_DiffLocation);
			//printf("patch1==patch2\n");
			neednot++;
		}
		else//merge
		{
			//printf("i:%d, copySortList->weight:%f, overSeg_cluster[patch1].Internal_Diff:%f, overSeg_cluster[patch2].Internal_Diff:%f\n",
				//i, copySortList->weight, overSeg_cluster[patch1].Internal_Diff, overSeg_cluster[patch2].Internal_Diff);
			
			float k = 4.000;
			float c1 = k / overSeg_cluster[patch1].numMergedCluster;
			float c2 = k / overSeg_cluster[patch2].numMergedCluster;
			//printf("c1:%f, c2:%f\n", c1, c2);
			//printf("overSeg_cluster[%d].numMergedCluster:%d, overSeg_cluster[%d].numMergedCluster:%d\n", patch1, overSeg_cluster[patch1].numMergedCluster, patch2, overSeg_cluster[patch2].numMergedCluster);
			//printf("21:%d, 40:%d, patch1:%d, patch2:%d\n", overSeg_cluster[21].numMergedCluster, overSeg_cluster[40].numMergedCluster,
				//overSeg_cluster[patch1].numMergedCluster, overSeg_cluster[patch2].numMergedCluster);


			//if weight <Internal_Diff, it need merge
			if (copySortList->weight[FEATURE] < MIN(overSeg_cluster[patch1].Internal_DiffLocation + c1, overSeg_cluster[patch2].Internal_DiffLocation + c2))
			{
				//printf("i:%d, copySortList->weight:%f, overSeg_cluster[patch1].Internal_Diff:%f, overSeg_cluster[patch2].Internal_Diff:%f\n",
					//i, copySortList->weight, overSeg_cluster[patch1].Internal_Diff, overSeg_cluster[patch2].Internal_Diff);
				//printf("Merging!\n");
				merge++;
				int a = MIN(patch1, patch2);
				int b = MAX(patch1, patch2);
				//give b's information to a(cluster index smaller)
				//patch1
				overSeg_cluster[a].numMergedCluster = overSeg_cluster[a].numMergedCluster + overSeg_cluster[b].numMergedCluster;
				overSeg_cluster[a].Internal_DiffLocation = MAX(overSeg_cluster[b].Internal_DiffLocation, 
						MAX(overSeg_cluster[a].Internal_DiffLocation, copySortList->weight[FEATURE]));
				//overSeg_cluster[a].Internal_DiffLocation = MAX(overSeg_cluster[a].Internal_DiffLocation, copySortList->weight[0]);
				overSeg_cluster[b].belongsTo = a;
			}
			else
			{
				//printf("Do not merge!\n");
				notmerge++;
			}
		}
		copySortList = copySortList->Next;
	}
	printf("Merge:%d, Not Merge:%d, need not merge:%d\n", merge, notmerge, neednot);
	
	int notscaned = 0;
	for (int i = 1; i < pCluster->numCluster + 1; ++i)
	{
		if (overSeg_cluster[i].Internal_DiffLocation == 0)
		{
			notscaned++;
		}
	}
	printf("How many cluster not merged:%d\n", notscaned);

	int m = 0;
	for (int i = 1; i < 100; ++i)
	{

		//printf("overSeg_cluster[%d].numMergedCluster:%d\n", i, overSeg_cluster[i].numMergedCluster);
		//printf("overSeg_cluster[%d].belongsto:%d\n", i, overSeg_cluster[i].belongsTo);
		//printf("cluofV:%d\n", pCluster->cluOfV[i]);
		//printf("%d\n", overSeg_cluster[i + 1].numMergedCluster);
	}

	stop = clock();
	time = ((double)(stop - start)) / CLK_TCK;
	printf("\nMerge Time: %lf\n", time);

	///generate Segmented Cluster
	Cluster *SegmentedCluster = (Cluster*)malloc(sizeof(Cluster));
	///Cluster *SegmentedCluster;
	SegmentedCluster->cluOfV = pCluster->cluOfV;
	
	SegmentedCluster->numCluster = 1;
	SegmentedCluster->pcluster = (_cluster*)malloc(sizeof(_cluster));
	SegmentedCluster->pcluster = pCluster->pcluster->nextcluster;
	SegmentedCluster->pcluster->index = 1;
	SegmentedCluster->pcluster->pvertices = NULL;
	int numSegCluster = 0;

	///change pClustes->cluOfV
	pCluster->pcluster = pCluster->pcluster->nextcluster;
	int change = 0;
	while (pCluster->pcluster != NULL)
	{
		while (pCluster->pcluster->pvertices != NULL)
		{
			int index = pCluster->pcluster->pvertices->index;
			int BelongsTo = overSeg_cluster[pCluster->pcluster->index].belongsTo;
			int IndexCluster = overSeg_cluster[pCluster->pcluster->index].index;
			while (BelongsTo != IndexCluster)
			{
				IndexCluster = BelongsTo;
				BelongsTo = overSeg_cluster[BelongsTo].belongsTo;
			}
			pCluster->cluOfV[index] = BelongsTo;
			change++;
			pCluster->pcluster->pvertices = pCluster->pcluster->pvertices->next;
		}
		pCluster->pcluster = pCluster->pcluster->nextcluster;
	}
	//printf("SegmentedCluster->numCluster:%d\n", SegmentedCluster->numCluster);

	for (int i = 0; i < 40; ++i)
	{
		//printf("cluofV:%d\n", pCluster->cluOfV[i]);
		//printf("%d\n", overSeg_cluster[i + 1].numMergedCluster);
	}
	printf("change:%d\n", change);
}

void CZyView::myDraw()
{
	glClearColor(bgcolor[0], bgcolor[1], bgcolor[2], 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	// Clear Screen And Depth Buffer

	glLoadIdentity();									// Reset The Current Modelview Matrix
	glTranslatef(tx, ty, tz);					        // Move

	if (!flagobjecthandles && !flaghandlesinteraction)
	{
		glPushMatrix();
	}

	glRotatef(rx, 1.0f, 0.0f, 0.0f);					// Rotate The Triangle On The X axis ( NEW )
	glRotatef(ry, 0.0f, 1.0f, 0.0f);					// Rotate The Triangle On The Y axis ( NEW )
	glRotatef(rz, 0.0f, 0.0f, 1.0f);                    // Rotate The Triangle On The Z axis ( NEW )

	CZyDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);	

	// set the color of object
	glEnable(GL_COLOR_MATERIAL);
 	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);

	if (pDoc->meshmodel)
	{
		if (flagdrawwireframe)
		{
			DrawWireFrame(pDoc->meshmodel);
		}
		else
		{
			
			if (colorflag == TRUE)
			{
				DrawFlatShading(pDoc->meshmodel, pCluster->cluOfV, pCluster->colorOfClu);
			}
			else
			{
				DrawFlatShading(pDoc->meshmodel);
			}
				
		}

		DrawDeformedVertices(pDoc->meshmodel, handleslist, 0.0, 1.0, 0.0);
		DrawDeformedVertices(pDoc->meshmodel, boundaryverticeslist, 1.0, 0.0, 0.0);
		DrawDeformedVertices(pDoc->meshmodel, ROIlist, 1.0, 1.0, 0.0);
	}

	if (flagglobalframe)
	{
		DrawGlobalFrame(pDoc->meshmax, pDoc->meshmin);
	}

	if (flaghandlesframe)
	{
		DrawHandlesFrame();
	}

	glDisable(GL_COLOR_MATERIAL);

	if (islbpressed && flagobjecthandles)
	{
		DrawScreenRectangle(prepointxrecord, prepointyrecord, curpointx, curpointy);
	}

	if (!flagobjecthandles && !flaghandlesinteraction)
	{
		glPopMatrix();
	}
	
	SwapBuffers(m_hDC);
}

void CZyView::mytestDraw(int *a)
{
	glClearColor(bgcolor[0], bgcolor[1], bgcolor[2], 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	// Clear Screen And Depth Buffer

	glLoadIdentity();									// Reset The Current Modelview Matrix
	glTranslatef(tx, ty, tz);					        // Move

	if (!flagobjecthandles && !flaghandlesinteraction)
	{
		glPushMatrix();
	}

	glRotatef(rx, 1.0f, 0.0f, 0.0f);					// Rotate The Triangle On The X axis ( NEW )
	glRotatef(ry, 0.0f, 1.0f, 0.0f);					// Rotate The Triangle On The Y axis ( NEW )
	glRotatef(rz, 0.0f, 0.0f, 1.0f);                    // Rotate The Triangle On The Z axis ( NEW )

	CZyDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	// set the color of object
	glEnable(GL_COLOR_MATERIAL);
	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);

	if (pDoc->meshmodel)
	{
		if (flagdrawwireframe)
		{
			DrawWireFrame(pDoc->meshmodel);
		}
		else
		{

			if (colorflag == TRUE)
			{
				DrawFlatShading(pDoc->meshmodel, a, pCluster->colorOfClu);
			}
			else
			{
				DrawFlatShading(pDoc->meshmodel);
			}

		}

		DrawDeformedVertices(pDoc->meshmodel, handleslist, 0.0, 1.0, 0.0);
		DrawDeformedVertices(pDoc->meshmodel, boundaryverticeslist, 1.0, 0.0, 0.0);
		DrawDeformedVertices(pDoc->meshmodel, ROIlist, 1.0, 1.0, 0.0);
	}

	if (flagglobalframe)
	{
		DrawGlobalFrame(pDoc->meshmax, pDoc->meshmin);
	}

	if (flaghandlesframe)
	{
		DrawHandlesFrame();
	}

	glDisable(GL_COLOR_MATERIAL);

	if (islbpressed && flagobjecthandles)
	{
		DrawScreenRectangle(prepointxrecord, prepointyrecord, curpointx, curpointy);
	}

	if (!flagobjecthandles && !flaghandlesinteraction)
	{
		glPopMatrix();
	}

	SwapBuffers(m_hDC);
}

//int numSegedCluster;
//int *copy_cluOfV;
//_cluster *segmentedCluster;
//nodegraph *Segedgraph;
void CZyView::GraphSegment()
{
	CZyDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	if (pDoc->meshmodel == NULL)
		return;

	/*
	for (int i = 0; i < pDoc->meshmodel->numvertices + 1; ++i)
	{
		printf("i:%d, cluOfv[%d]:%d\n", i, i, pCluster->cluOfV[i]);
	}
	*/

	printf("ConstructGraph\n");
	printf("pCluster->order:%d\n", pCluster->numCluster);

	ConstructGraph(pDoc->meshmodel, pDoc->verticesvindices,
		pDoc->reconstructednormals, pDoc->numofneighboringvertices,
		pDoc->verticestindices);
	printf("Constructed!!\n");

	//Exponential_weight(SortList);

	Merge(G, SortList);

	printf("Merged!\n");
	colorflag = true;
	//myDraw();
	printf("drawed!\n");
	
	//int numSegedCluster = 1;
	//_cluster *segmentedCluster = (_cluster*)malloc(sizeof(_cluster));
	////segmentedCluster = (_cluster*)malloc(sizeof(_cluster)); //?
	//segmentedCluster->index = 0;
	//segmentedCluster->order = 0;
	//segmentedCluster->nextcluster = NULL;
	//segmentedCluster->pvertices = NULL;
	/////generate segmented clusters////////////////////////////////////////////////
	//int *copy_cluOfV = pCluster->cluOfV;
	////copy_cluOfV = (int*)malloc(sizeof(int)*(pDoc->meshmodel->numvertices + 1));

	////copy_cluOfV = pCluster->cluOfV;//?

	//for (int i = 1; i < pDoc->meshmodel->numvertices + 1; ++i)
	//{
	//	_cluster *tmp_seged = segmentedCluster;
	//	if (segmentedCluster->index == 0)
	//	{
	//		segmentedCluster->index = copy_cluOfV[i];
	//	}

	//	while (tmp_seged->index != copy_cluOfV[i] && tmp_seged->nextcluster != NULL)
	//	{
	//		tmp_seged = tmp_seged->nextcluster;
	//	}

	//	IndexList* curIndex = (IndexList*)malloc(sizeof(IndexList));
	//	curIndex->index = i;
	//	curIndex->next = NULL;

	//	if (tmp_seged->index == copy_cluOfV[i])
	//	{
	//		tmp_seged->order++;
	//		curIndex->next = tmp_seged->pvertices;
	//		tmp_seged->pvertices = curIndex;
	//	}
	//	else
	//	{
	//		numSegedCluster++;
	//		_cluster *new_segmentedCluster = (_cluster*)malloc(sizeof(_cluster));
	//		new_segmentedCluster->index = copy_cluOfV[i];
	//		new_segmentedCluster->pvertices = curIndex;
	//		new_segmentedCluster->nextcluster = tmp_seged->nextcluster;
	//		new_segmentedCluster->order = 1;
	//		tmp_seged->nextcluster = new_segmentedCluster;
	//	}

	//	//printf("%d\n", copy_cluOfV[i]);
	//}
	//////////////////////////////////////////////////////////////////////////////////////////////
	//printf("numSegedCluster:%d\n", numSegedCluster);

	//nodegraph *Segedgraph = (nodegraph*)malloc(sizeof(nodegraph)*(numSegedCluster));

	//GenerateSegedGraph(Segedgraph, segmentedCluster, copy_cluOfV, pDoc);
	//MergeSmall(Segedgraph, segmentedCluster, numSegedCluster, copy_cluOfV);

	//_cluster *ShowSegmentedCluster = segmentedCluster;
	//while (ShowSegmentedCluster != NULL)
	//{
	//	printf("index:%d, order:%d\n", ShowSegmentedCluster->index, ShowSegmentedCluster->order);
	//	ShowSegmentedCluster = ShowSegmentedCluster->nextcluster;
	//}

	myDraw();
	//printf("%d/n", pCluster->neighborclusters[4][5]);
	/*
	for (int i = 0; i < pCluster->numCluster; ++i)
	{
		printf("order:%d\nindex:%d,%f,%f,%f\n", pCluster->pcluster->order, pCluster->pcluster->index, pCluster->pcluster->center[0], pCluster->pcluster->center[1], pCluster->pcluster->center[2]);
		pCluster->pcluster = pCluster->pcluster->nextcluster;
	}*/
	//pCluster
}

void CZyView::OnSegmentSeg()
{
	printf("seg\n");

	// TODO: 在此添加命令处理程序代码
	
	//OnRegionGrow();//K = 5.00, formal = 1.0, location = 0.0
	OnHierarA();
}

IndexList *CZyView::FindSeeds(_cluster *cluster, CZyDoc* pDoc)
{
	int index_1 = cluster->index;
	int num_1 = cluster->order;

	IndexList *tmpvertices = cluster->pvertices;
	SegedCluster *cluster_1 = (SegedCluster *)malloc(sizeof(SegedCluster) * num_1);

	int *ClusterOfV = (int*)malloc(sizeof(int)*(pDoc->meshmodel->numvertices + 1));
	for (int i = 0; i < pDoc->meshmodel->numvertices + 1; ++i)
	{
		ClusterOfV[i] = pCluster->cluOfV[i];
	}
	///construct origin index with new index，which the node belongs to
	for (int i = 0; i < num_1; ++i)
	{
		cluster_1[i].index = i;
		cluster_1[i].origin_index = tmpvertices->index;
		tmpvertices = tmpvertices->next;
		cluster_1[i].belongsto = index_1;
		cluster_1[i].is_seed = 1;
	}

	for (int iter = 0; iter < 7; ++iter)
	{
		List *indexofChange = (List*)malloc(sizeof(List)); //head node
		indexofChange->nextnode = NULL;
		for (int i = 0; i < num_1; ++i)
		{
			int index = cluster_1[i].origin_index;
			if (ClusterOfV[index] == 0)
			{
				continue;
			}

			IndexList *q1;
			q1 = pDoc->verticesvindices[index];
			while (q1 != NULL)
			{
				if (ClusterOfV[q1->index] != ClusterOfV[index])
				{
					List *newnode = (List*)malloc(sizeof(List));
					newnode->index = index;
					newnode->nextnode = indexofChange->nextnode;
					indexofChange->nextnode = newnode;
					//ClusterOfV[index] = 0;
					break;
				}
				q1 = q1->next;
			}
		}

		//indexofChange = indexofChange->nextnode;

		List *tmp = indexofChange->nextnode;
		while (tmp != NULL)
		{
			ClusterOfV[tmp->index] = 0;//bound set 0
			tmp = tmp->nextnode;//release memory
		}

		while (indexofChange != NULL)
		{
			List *pre = indexofChange;
			indexofChange = indexofChange->nextnode;
			pre->nextnode = NULL;
			free(pre);
		}
	}

	IndexList *Seeds = (IndexList*)malloc(sizeof(IndexList));
	Seeds->next = NULL;
	int number = 0;
	for (int i = 0; i < num_1; ++i)
	{
		if (ClusterOfV[cluster_1[i].origin_index] != 0)
		{
			IndexList *tmpnode = (IndexList*)malloc(sizeof(IndexList));
			tmpnode->index = cluster_1[i].origin_index;
			tmpnode->next = Seeds->next;
			Seeds->next = tmpnode;
			number++;
		}
	}

	free(cluster_1);
	free(ClusterOfV);

	return Seeds;
}

void CZyView::GenerateSegedGraph(nodegraph *Segedgraph, _cluster *segmentedCluster, int *copy_cluOfV, CZyDoc* pDoc)
{
	printf("GenerateSegedGraph!\n");
	_cluster *tmpSegmentedCluster = segmentedCluster;
	int i = 0;
	while (tmpSegmentedCluster != NULL)
	{
		printf("index:%d, order:%d\n", tmpSegmentedCluster->index, tmpSegmentedCluster->order);
		Segedgraph[i].index = tmpSegmentedCluster->index;
		Segedgraph[i].order = tmpSegmentedCluster->order;
		Segedgraph[i].marked = FALSE;
		Segedgraph[i].Next = NULL;

		int numEachCluster = tmpSegmentedCluster->order;//each cluster's number
		IndexList *copypvertices = tmpSegmentedCluster->pvertices;
		for (int j = 0; j < numEachCluster; ++j)	//numEachCluster's number
		{
			int index = copypvertices->index;//index in current Cluster

			IndexList  *q1;
			q1 = pDoc->verticesvindices[index];

			while (q1 != NULL)//numofneighboringvertices loop
			{
				int IndexInOriginModel = q1->index;//face index

				if (copy_cluOfV[IndexInOriginModel] != tmpSegmentedCluster->index) //add Cluster to adjacent list
				{
					bool needadd = TRUE;
					nodegraph *tmpSegedgraph = Segedgraph[i].Next;
					while (tmpSegedgraph != NULL)
					{
						if (tmpSegedgraph->index == copy_cluOfV[IndexInOriginModel])
						{
							needadd = FALSE;
							break;
						}
						tmpSegedgraph = tmpSegedgraph->Next;
					}
					if (needadd == TRUE)
					{
						nodegraph *neighborSeged = (nodegraph*)malloc(sizeof(nodegraph));
						neighborSeged->index = copy_cluOfV[IndexInOriginModel];
						neighborSeged->marked = FALSE;
						neighborSeged->Next = Segedgraph[i].Next;
						Segedgraph[i].Next = neighborSeged;
					}
				}
				q1 = q1->next;
			}
			copypvertices = copypvertices->next;
		}
		tmpSegmentedCluster = tmpSegmentedCluster->nextcluster;
		i++;
	}
}

void CZyView::MergeSmall(nodegraph *Segedgraph, _cluster *segmentedCluster, int numSegedCluster, int *copy_cluOfV)
{
	printf("MergeSmall!\n");
	_cluster *SegedCluster = segmentedCluster->nextcluster;
	_cluster *PreSegedCluster = segmentedCluster;
	while (SegedCluster != NULL)
	{
		//find <100
		if (SegedCluster->order < 400)
		{
			//find nth cluster in graph
			int max_neighbor_order = 0;
			int max_neighbor_index = 0;
			int TheNthCluster = 0;
			for (int i = 0; i < numSegedCluster; ++i)
			{	
				//
				if (Segedgraph[i].index == SegedCluster->index)
				{
					TheNthCluster = i;
				}
			}
			//find neighbor index which has the most vertices
			nodegraph *seged = Segedgraph[TheNthCluster].Next;
			while (seged != NULL)
			{
				_cluster *findSegedCluster = segmentedCluster;
				while (findSegedCluster!=NULL)
				{
					if (findSegedCluster->index == seged->index) 
					{
						if (findSegedCluster->order > max_neighbor_order)
						{
							max_neighbor_order = findSegedCluster->order; 
							max_neighbor_index = findSegedCluster->index;
							//break;
						}
					}
					findSegedCluster = findSegedCluster->nextcluster;
				}
				seged = seged->Next;
			}

			_cluster *max_neighbor_cluster = segmentedCluster;
			//find max neighbor index
			while (max_neighbor_cluster != NULL)
			{
				if (max_neighbor_cluster->index == max_neighbor_index)
				{
					break;
				}
				max_neighbor_cluster = max_neighbor_cluster->nextcluster;
			}

			//max_neighbor_cluster->pvertices = SegedCluster->pvertices
			//merge small(add to max_neighbor_cluster and take away small)
			//find max neighbor cluster pvertices' last one
			IndexList *max_neighbor_vertices = max_neighbor_cluster->pvertices;
			while (max_neighbor_vertices->next != NULL)
			{
				max_neighbor_vertices = max_neighbor_vertices->next;
			}
			max_neighbor_vertices->next = SegedCluster->pvertices;
			//change cluOfV
			IndexList *small_vertices = SegedCluster->pvertices;
			while (small_vertices != NULL)
			{
				copy_cluOfV[small_vertices->index] = max_neighbor_index;
				small_vertices = small_vertices->next;
			}
			//delete small cluster
			max_neighbor_cluster->order = max_neighbor_cluster->order + SegedCluster->order;
			SegedCluster->index = 0;
			Segedgraph[TheNthCluster].index = 0;
		}

		//PreSegedCluster = PreSegedCluster->nextcluster;
		SegedCluster = SegedCluster->nextcluster;
		//SegedCluster = PreSegedCluster->nextcluster;
	}
}

void CZyView::GenerateNodeGraph(nodegraph *graph, _cluster *cluster1, _cluster *cluster2, int *copy_cluOfV, CZyDoc* pDoc)
{
	IndexList *pvertices1 = cluster1->pvertices;
	IndexList *pvertices2 = cluster2->pvertices;
	int numberofnode = 0;
	while (pvertices1 != NULL || pvertices2 != NULL)
	{
		IndexList *q1;
		if (pvertices1 != NULL)
		{
			graph[numberofnode].index = pvertices1->index;
			graph[numberofnode].Next = NULL;

			q1 = pDoc->verticesvindices[pvertices1->index];
		}
		else
		{
			graph[numberofnode].index = pvertices2->index;
			graph[numberofnode].Next = NULL;

			q1 = pDoc->verticesvindices[pvertices2->index];
		}

		while (q1 != NULL)
		{
			if (copy_cluOfV[q1->index] == cluster1->index ||
				copy_cluOfV[q1->index] == cluster2->index)
			{
				nodegraph *neighbornode = (nodegraph*)malloc(sizeof(nodegraph));
				neighbornode->index = q1->index;
				neighbornode->Next = graph[numberofnode].Next;
				graph[numberofnode].Next = neighbornode;
			}
			q1 = q1->next;
		}

		if (pvertices1 != NULL)
		{
			pvertices1 = pvertices1->next;
		}
		else
		{
			pvertices2 = pvertices2->next;
		}
		numberofnode++;
		//printf("numberofnode:%d\n", numberofnode);
	}
	printf("numberofnode:%d\n", numberofnode);
}

void CZyView::Normalized_features(float *feature, int length)//length already added 1
{
	float maxvalue = feature[1];
	float minvalue = feature[1];
	for (int i = 1; i < length; ++i)//begin 1,2,3,...
	{
		if (feature[i] > maxvalue)
			maxvalue = feature[i];
		if (feature[i] < minvalue)
			minvalue = feature[i];
	}
	printf("maxvalue:%f\n", maxvalue);
	printf("minvalue:%f\n", minvalue);
	for (int i = 1; i < length + 1; ++i)//begin 1,2,3,...
	{
		feature[i] = (feature[i] - minvalue) / (maxvalue - minvalue);
	}
}

void CZyView::Compute_SDF(GLMmodel *meshmodel, IndexList **verticestindices, float *SDF)
{
	float *face_SDF = (float*)malloc(sizeof(float)*meshmodel->numtriangles);
	printf("meshmodel->numtriangles:%d\n", meshmodel->numtriangles);

	char SDF_file_path[50] = "features/SDF_";
	strcat(SDF_file_path, filename);
	printf("SDF_file_path:%s\n", SDF_file_path);

	FILE *fpRead = fopen(SDF_file_path, "r");
	if (fpRead == NULL)
	{
		printf("read failed!\n");
	}
	for (int i = 0; i < meshmodel->numtriangles; i++)
	{
		fscanf(fpRead, "%f ", &face_SDF[i]);
	}
	//printf("SDF[0]:%f\n", face_SDF[0]);
	//printf("SDF[2]:%f\n", face_SDF[2]);
	//printf("SDF_face[31815]:%f\n", face_SDF[31815]);
	for (int i = 1; i < meshmodel->numvertices + 1; ++i)
	{
		IndexList  *q1;
		q1 = verticestindices[i];
		int numfaces = 0;
		float tmpSDF = 0;
		while (q1 != NULL)
		{
			//if (i == 1)
			//{
			//	printf("face_SDF[q1->index]:%d, %f\n", q1->index, face_SDF[q1->index]);
			//}
			tmpSDF = tmpSDF + face_SDF[q1->index];
			numfaces++;
			q1 = q1->next;
		}
		SDF[i] = tmpSDF / numfaces;
	}
	//printf("SDF[i]:%f\n", SDF[1]);
	//printf("SDF[31816]:%f\n", SDF[31816]);
}

void CZyView::Compute_AGD(GLMmodel *meshmodel, float *AGD)
{
	char AGD_file_path[50] = "features/AGD_";
	strcat(AGD_file_path, filename);
	printf("AGD_file_path:%s\n", AGD_file_path);

	FILE *fpRead = fopen(AGD_file_path, "r");
	if (fpRead == NULL)
	{
		printf("read failed!\n");
	}
	for (int i = 1; i < meshmodel->numvertices + 1; i++)
	{
		fscanf(fpRead, "%f ", &AGD[i]);
	}
}

void CZyView::Compute_CF(GLMmodel *meshmodel, float *CF)
{
	char CF_file_path[50] = "features/CF_";
	strcat(CF_file_path, filename);
	printf("CF_file_path:%s\n", CF_file_path);

	FILE *fpRead = fopen(CF_file_path, "r");
	if (fpRead == NULL)
	{
		printf("read failed!\n");
	}
	for (int i = 1; i < meshmodel->numvertices + 1; i++)
	{
		fscanf(fpRead, "%f ", &CF[i]);
	}
	//printf("CF[0]:%f\n", CF[0]);
	//printf("CF[2]:%f\n", CF[2]);
}

void CZyView::Compute_HKS(GLMmodel *meshmodel, float **HKS)
{
	char HKS_file_path[50] = "features/HKS_";
	strcat(HKS_file_path, filename);
	printf("HKS_file_path:%s\n", HKS_file_path);

	FILE *fpRead = fopen(HKS_file_path, "r");
	if (fpRead == NULL)
	{
		printf("read failed!\n");
	}

	for (int i = 1; i < meshmodel->numvertices + 1; i++)
	{
		for (int j = 1; j < 102; ++j)
		{
			fscanf(fpRead, "%f ", &HKS[i][j]);
			if (HKS[i][j] > max_HKS)
			{
				max_HKS = HKS[i][j];
			}
		}
		//printf("%d:%f\n", i, HKS[i][100]);
	}

	for (int i = 1; i < meshmodel->numvertices + 1; i++)
	{
		for (int j = 1; j < 102; ++j)
		{
			HKS[i][j] = HKS[i][j] / max_HKS;
		}
	}
}

void CZyView::Compute_VertexNormal(GLMmodel *meshmodel, IndexList **verticestindices, int index, float *VertexNormal)
{
	IndexList  *q1;
	q1 = verticestindices[index];
	while (q1 != NULL)
	{
		//get vertex neighbor face formal
		float normal[3] = { 0, 0, 0 };
		for (int j = 0; j < 3; ++j)
		{
			normal[j] = meshmodel->facetnorms[3 * (q1->index + 1) + j];//need +1
		}

		VertexNormal[0] = VertexNormal[0] + normal[0];
		VertexNormal[1] = VertexNormal[1] + normal[1];
		VertexNormal[2] = VertexNormal[2] + normal[2];

		q1 = q1->next;
	}
	glmNormalize(VertexNormal);
}

#include <stdio.h>
#include "graph_cut.h"
void CZyView::graphcut(nodegraph *graph, IndexList *SeedsS, IndexList *SeedsT, _cluster *cluster1, _cluster *cluster2, 
	GLMmodel *meshmodel, IndexList **verticestindices, int *FunctionOri2NewIndex, int *copy_cluOfV, int *ori_cluOfV)
{
	typedef Graph<float, float, float> GraphType;
	int EstimatedOfNodes = cluster1->order + cluster2->order;
	GraphType *g = new GraphType(/*estimated # of nodes*/ cluster1->order+cluster2->order, /*estimated # of edges*/ EstimatedOfNodes * 3);

	///add node
	g->add_node(EstimatedOfNodes);

	///add seeds edge
	//printf("Seeds:%d,Seeds->next:%d", SeedsT->index, SeedsT->next->index);
	IndexList *TmpS = SeedsS->next;
	while (TmpS != NULL)
	{
		g->add_tweights(FunctionOri2NewIndex[TmpS->index],   /* capacities */  10, 0.001);
		TmpS = TmpS->next;
	}
	IndexList *TmpT = SeedsT->next;
	while (TmpT != NULL)
	{
		g->add_tweights(FunctionOri2NewIndex[TmpT->index],   /* capacities */  0.001, 10);
		TmpT = TmpT->next;
	}
	
	///add edge
	for (int i = 0; i < EstimatedOfNodes; ++i)
	{
		int index = graph[i].index;
		float VertexNormal[3] = { 0.0, 0.0, 0.0 };
		Compute_VertexNormal(meshmodel, verticestindices, index, VertexNormal);
		nodegraph *tmp = graph[i].Next;
		while (tmp != NULL)
		{
			float VertexneighborNormal[3] = { 0.0, 0.0, 0.0 };
			Compute_VertexNormal(meshmodel, verticestindices, tmp->index, VertexneighborNormal);

			float d0 = VertexNormal[0] - VertexneighborNormal[0];
			float d1 = VertexNormal[1] - VertexneighborNormal[1];
			float d2 = VertexNormal[2] - VertexneighborNormal[2];

			//float weight = sqrt(d0 * d0 + d1 * d1 + d2 * d2);
			float weight = fabs(SDF[index] - SDF[tmp->index]);
			//float weight = fabs(AGD[index] - AGD[tmp->index]);//bad
			//float weight = fabs(CF[index] - CF[tmp->index]);
			//float weight = fabs(SDF[index] - SDF[tmp->index]) + 1.0*sqrt(d0 * d0 + d1 * d1 + d2 * d2);
			weight = 1.0 / weight;
			//weight = exp(-weight);
			//printf("i:%d, normal:%f\n",i, weight);
			//printf("FunctionOri2NewIndex[%d]:%d, graph[%d].index:%d\n", tmp->index, FunctionOri2NewIndex[tmp->index], i, graph[i].index);
			if (FunctionOri2NewIndex[tmp->index] >= EstimatedOfNodes)
			{
				printf("EstimatedOfNodes:%d\n", FunctionOri2NewIndex[tmp->index]);
			}
			g->add_edge(i, FunctionOri2NewIndex[tmp->index],    /* capacities */  weight, 0);

			tmp = tmp->Next;
		}
	}

	printf("overflow\n");
	float flow = g->maxflow();
	printf("Flow = %f\n", flow);

	for (int i = 0; i < EstimatedOfNodes; ++i)
	{
		if (g->what_segment(i) == GraphType::SOURCE)
		{
			//printf("node%d is in the SOURCE set\n", i);
			if (copy_cluOfV[graph[i].index] != cluster1->index && copy_cluOfV[graph[i].index] != cluster2->index)
			{
				continue;
			}
			copy_cluOfV[graph[i].index] = cluster1->index;
		}
		else
		{
			//printf("node%d is in the SINK set\n", i);
			if (copy_cluOfV[graph[i].index] != cluster1->index && copy_cluOfV[graph[i].index] != cluster2->index)
			{
				continue;
			}
			copy_cluOfV[graph[i].index] = cluster2->index;
		}
	}
	printf("belowFlow\n");
	delete g;////delete g;
	printf("a");
}

//int *copy_cluOfV;
//_cluster *segmentedCluster;
void CZyView::GraphCut()
{
	printf("Graph cut\n");

	CZyDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	if (pDoc->meshmodel == NULL)
		return;

	printf("%d\n", pDoc->meshmodel->numvertices + 1);
	printf("%d\n", pCluster->cluOfV[0]);
	printf("%d\n", pCluster->cluOfV[pDoc->meshmodel->numvertices]);
	int numSegedCluster = 1;
	_cluster *segmentedCluster = (_cluster*)malloc(sizeof(_cluster));
	//segmentedCluster = (_cluster*)malloc(sizeof(_cluster)); //?
	segmentedCluster->index = 0;
	segmentedCluster->order = 0;
	segmentedCluster->nextcluster = NULL;
	segmentedCluster->pvertices = NULL;
	///generate segmented clusters////////////////////////////////////////////////
	int *copy_cluOfV = pCluster->cluOfV;
	//copy_cluOfV = (int*)malloc(sizeof(int)*(pDoc->meshmodel->numvertices + 1));
	
	//copy_cluOfV = pCluster->cluOfV;//?

	for (int i = 1; i < pDoc->meshmodel->numvertices + 1; ++i)
	{
		_cluster *tmp_seged = segmentedCluster;
		if (segmentedCluster->index == 0)
		{
			segmentedCluster->index = copy_cluOfV[i];
		}

		while (tmp_seged->index != copy_cluOfV[i] && tmp_seged->nextcluster != NULL)
		{
			tmp_seged = tmp_seged->nextcluster;
		}

		IndexList* curIndex = (IndexList*)malloc(sizeof(IndexList));
		curIndex->index = i;
		curIndex->next = NULL;

		if(tmp_seged->index == copy_cluOfV[i])
		{
			tmp_seged->order++;
			curIndex->next = tmp_seged->pvertices;
			tmp_seged->pvertices = curIndex;
		}
		else
		{
			numSegedCluster++;
			_cluster *new_segmentedCluster = (_cluster*)malloc(sizeof(_cluster));
			new_segmentedCluster->index = copy_cluOfV[i];
			new_segmentedCluster->pvertices = curIndex;
			new_segmentedCluster->nextcluster = tmp_seged->nextcluster;
			new_segmentedCluster->order = 1;
			tmp_seged->nextcluster = new_segmentedCluster;
		}

		//printf("%d\n", copy_cluOfV[i]);
	}
	//////////////////////////////////////////////////////////////////////////////////////////
	printf("numSegedCluster:%d\n", numSegedCluster);

	nodegraph *Segedgraph = (nodegraph*)malloc(sizeof(nodegraph)*(numSegedCluster));
	GenerateSegedGraph(Segedgraph, segmentedCluster, copy_cluOfV, pDoc);
	MergeSmall(Segedgraph, segmentedCluster, numSegedCluster, copy_cluOfV);

	///test right or not
	//int *mycluOfV = (int*)malloc(sizeof(int)*pDoc->meshmodel->numvertices);
	//for (int i = 0; i < pDoc->meshmodel->numvertices+1; ++i)
	//{
	//	mycluOfV[i] = 3;
	//}
	//IndexList *tmp = segmentedCluster->pvertices;
	//while (tmp != NULL)
	//{
	//	printf("huluhulu:%d\n", tmp->index);
	//	//mycluOfV[tmp->index] = 1;
	//	tmp = tmp->next;
	//}

	int *ori_cluOfV = (int*)malloc(sizeof(int)*(pDoc->meshmodel->numvertices + 1));
	for (int i = 0; i < (pDoc->meshmodel->numvertices + 1); ++i)
	{
		//copy_cluOfV[i] = pCluster->cluOfV[i];
		ori_cluOfV[i] = pCluster->cluOfV[i];
	}

	_cluster *ShowSegmentedCluster = segmentedCluster;
	while (ShowSegmentedCluster != NULL)
	{
		printf("index:%d, order:%d\n", ShowSegmentedCluster->index, ShowSegmentedCluster->order);
		ShowSegmentedCluster = ShowSegmentedCluster->nextcluster;
	}

	


	/*for (int i = 0; i < numSegedCluster; ++i)
	{
		printf("Segedgraph[%d].index:%d\n", i, Segedgraph[i].index);
	}*/
	for (int NthCluster = 0; NthCluster < numSegedCluster; ++NthCluster)
	{
		if (Segedgraph[NthCluster].index == 0)
		{
			continue;
		}
		int index1 = Segedgraph[NthCluster].index;
		printf("neighborcluster->index1:%d\n", index1);
		nodegraph *neighborcluster = Segedgraph[NthCluster].Next;
		
		while (neighborcluster != NULL)
		{
			int index2;
			index2 = neighborcluster->index;
			printf("neighborcluster->index2:%d\n", index2);
			bool empty = TRUE;
			_cluster *tmp = segmentedCluster;
			while (tmp != NULL)
			{
				if (index2 == tmp->index)
				{
					empty = FALSE;
					break;
				}
				tmp = tmp->nextcluster;
			}
			if (empty == FALSE)
			{
				_cluster *cluster1 = segmentedCluster;
				while (cluster1 != NULL)
				{
					if (cluster1->index == index1)
					{
						break;
					}
					cluster1 = cluster1->nextcluster;
				}

				_cluster *cluster2 = segmentedCluster;
				while (cluster2 != NULL)
				{
					if (cluster2->index == index2)
					{
						break;
					}

					cluster2 = cluster2->nextcluster;
				}

				IndexList *SeedsS;
				SeedsS = FindSeeds(cluster1, pDoc);
				//test find seeds are right
				IndexList *tmpS = SeedsS;
				int s = 0;
				while (tmpS->next != NULL)
				{
					tmpS = tmpS->next;
					s++;
				}
				printf("SeedsS:%d\n", s);

				IndexList *SeedsT;
				SeedsT = FindSeeds(cluster2, pDoc);
				//test find seeds are right
				IndexList *tmpT = SeedsT;
				int t = 0;
				while (tmpT->next != NULL)
				{
					tmpT = tmpT->next;
					t++;
				}
				printf("SeedsT:%d\n", t);

				nodegraph *graph = (nodegraph*)malloc(sizeof(nodegraph)*(cluster1->order + cluster2->order));
				GenerateNodeGraph(graph, cluster1, cluster2, ori_cluOfV, pDoc);

				//build a relationship between origin index and new index(0,1,2,......)
				int *FunctionOri2NewIndex = (int*)malloc(sizeof(int)*(pDoc->meshmodel->numvertices + 1));
				for (int i = 0; i < pDoc->meshmodel->numvertices + 1; ++i)
				{
					FunctionOri2NewIndex[i] = -1;
				}
				IndexList *pvertices1 = cluster1->pvertices;
				IndexList *pvertices2 = cluster2->pvertices;
				int newindex = 0;
				while (pvertices1 != NULL)
				{
					FunctionOri2NewIndex[pvertices1->index] = newindex;
					newindex++;
					pvertices1 = pvertices1->next;
				}
				while (pvertices2 != NULL)
				{
					FunctionOri2NewIndex[pvertices2->index] = newindex;
					newindex++;
					pvertices2 = pvertices2->next;
				}
				printf("index1:%d, index2:%d\n", index1, index2);
				printf("newindex:%d\n", newindex);

				graphcut(graph, SeedsS, SeedsT, cluster1, cluster2, pDoc->meshmodel, pDoc->verticestindices, FunctionOri2NewIndex, copy_cluOfV, ori_cluOfV);
				
				free(graph);
				free(FunctionOri2NewIndex);
			}
			printf("=====================================\n");
			neighborcluster = neighborcluster->Next;
		}
	}
	printf("End Graph Cut\n");
	//=================================================================
	//_cluster *cluster1 = segmentedCluster;
	//_cluster *cluster2 = segmentedCluster->nextcluster;

	////find seed
	//IndexList *SeedsS;
	//SeedsS = FindSeeds(cluster1, pDoc);
	////test find seeds are right
	//IndexList *tmpS = SeedsS;
	//int s = 0;
	//while (tmpS->next != NULL)
	//{
	//	tmpS = tmpS->next;
	//	s++;
	//}
	//printf("SeedsS:%d\n", s);

	//IndexList *SeedsT;
	//SeedsT = FindSeeds(cluster2, pDoc);
	////test find seeds are right
	//IndexList *tmpT = SeedsT;
	//int t = 0;
	//while (tmpT->next != NULL)
	//{
	//	tmpT = tmpT->next;
	//	t++;
	//}
	//printf("SeedsT:%d\n", t);
	//
	//	
	////construct the normal node's graph

	//nodegraph *graph = (nodegraph*)malloc(sizeof(nodegraph)*(cluster1->order + cluster2->order));
	//GenerateNodeGraph(graph, cluster1, cluster2, ori_cluOfV, pDoc);

	////build a relationship between origin index and new index(0,1,2,......)
	//int *FunctionOri2NewIndex = (int*)malloc(sizeof(int)*(pDoc->meshmodel->numvertices + 1));
	//for (int i = 0; i < pDoc->meshmodel->numvertices + 1; ++i)
	//{
	//	FunctionOri2NewIndex[i] = -1;
	//}
	//IndexList *pvertices1 = cluster1->pvertices;
	//IndexList *pvertices2 = cluster2->pvertices;
	//int newindex = 0;
	//printf("the first one:%d\n", pvertices1->index);
	//while (pvertices1 != NULL)
	//{
	//	FunctionOri2NewIndex[pvertices1->index] = newindex;
	//	newindex++;
	//	pvertices1 = pvertices1->next;
	//}
	//while (pvertices2 != NULL)
	//{
	//	FunctionOri2NewIndex[pvertices2->index] = newindex;
	//	newindex++;
	//	pvertices2 = pvertices2->next;
	//}
	//printf("newindex:%d\n", newindex);
	//printf("test;%d\n", FunctionOri2NewIndex[6638]);
	//graphcut(graph, SeedsS, SeedsT, cluster1, cluster2, pDoc->meshmodel, pDoc->verticestindices, FunctionOri2NewIndex, copy_cluOfV, ori_cluOfV);
	//free(graph);
	//graph = NULL;
	//free(FunctionOri2NewIndex);
	//FunctionOri2NewIndex = NULL;

	////~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	//IndexList *SS = SeedsS;
	//while (SeedsS != NULL)
	//{
	//	SeedsS = SeedsS->next;
	//	SS->next = NULL;
	//	free(SS);
	//	SS = SeedsS;
	//}

	//IndexList *ST = SeedsT;
	//while (SeedsT != NULL)
	//{
	//	SeedsT = SeedsT->next;
	//	ST->next = NULL;
	//	free(ST);
	//	ST = SeedsT;
	//}

	//for (int m = 0; m < (cluster1->order + cluster2->order); ++m)
	//{
	//	nodegraph *tmp = graph[m].Next;
	//	nodegraph *tmpdel = graph[m].Next;
	//	graph[m].Next = NULL;
	//	while (tmp != NULL)
	//	{
	//		tmp = tmp->Next;
	//		tmpdel->Next = NULL;
	//		free(tmpdel);
	//		tmpdel = tmp;
	//	}
	//}
	//free(graph);
	//graph = NULL;
	////mytestDraw(copy_cluOfV);

	////for (int i = 1; i < pDoc->meshmodel->numvertices + 1; ++i)
	////{
	////	if (copy_cluOfV[i] != ori_cluOfV[i])
	////	{

	////	}
	////}

	////=====================================================================================


	//_cluster *cluster12 = segmentedCluster;
	//_cluster *cluster22 = segmentedCluster->nextcluster->nextcluster->nextcluster;

	/////find seed
	//IndexList *SeedsS2;
	//SeedsS2 = FindSeeds(cluster12, pDoc);
	/////test find seeds are right
	//IndexList *tmpS2 = SeedsS2;
	//int s2 = 0;
	//while (tmpS2->next != NULL)
	//{
	//	tmpS2 = tmpS2->next;
	//	s2++;
	//}
	//printf("SeedsS2:%d\n", s2);

	//IndexList *SeedsT2;
	//SeedsT2 = FindSeeds(cluster22, pDoc);
	/////test find seeds are right
	//IndexList *tmpT2 = SeedsT2;
	//int t2 = 0;
	//while (tmpT2->next != NULL)
	//{
	//	tmpT2 = tmpT2->next;
	//	t2++;
	//}
	//printf("SeedsT2:%d\n", t2);

	/////construct the normal node's graph

	////_cluster *cluster3 = segmentedCluster->nextcluster->nextcluster->nextcluster;
	//nodegraph *graph2 = (nodegraph*)malloc(sizeof(nodegraph)*(cluster12->order + cluster22->order));
	//GenerateNodeGraph(graph2, cluster12, cluster22, ori_cluOfV, pDoc);

	//for (int abc = 0; abc < (cluster12->order + cluster22->order); ++abc)
	//{
	//	nodegraph *testgraph = graph2[abc].Next;
	//	while (testgraph != NULL)
	//	{
	//		if (testgraph->index == 6638)
	//		{
	//			printf("there it is 6638, graph[%d].index:%d\n", abc, graph2[abc].index);
	//		}

	//		testgraph = testgraph->Next;
	//	}
	//}

	/////build a relationship between origin index and new index(0,1,2,......)
	//int *FunctionOri2NewIndex2 = (int*)malloc(sizeof(int)*(pDoc->meshmodel->numvertices + 1));
	//for (int i = 0; i < pDoc->meshmodel->numvertices + 1; ++i)
	//{
	//	FunctionOri2NewIndex2[i] = -2;
	//}
	//IndexList *pvertices12 = cluster12->pvertices;
	//IndexList *pvertices22 = cluster22->pvertices;
	//int newindex2 = 0;
	//while (pvertices12 != NULL)
	//{
	//	FunctionOri2NewIndex2[pvertices12->index] = newindex2;
	//	newindex2++;
	//	pvertices12 = pvertices12->next;
	//}
	//while (pvertices22 != NULL)
	//{
	//	FunctionOri2NewIndex2[pvertices22->index] = newindex2;
	//	newindex2++;
	//	pvertices22 = pvertices22->next;
	//}
	//printf("newindex2:%d\n", newindex2);

	//printf("test;%d\n", FunctionOri2NewIndex2[6638]);

	//graphcut(graph2, SeedsS2, SeedsT2, cluster12, cluster22, pDoc->meshmodel, pDoc->verticestindices, FunctionOri2NewIndex2, copy_cluOfV, ori_cluOfV);
	//printf("1\n");
	//free(graph2);
	//free(FunctionOri2NewIndex2);

	//printf("2\n");
	//mytestDraw(copy_cluOfV);
}

void CZyView::OnSegmentGraphsegment()
{
	// TODO: 在此添加命令处理程序代码
	printf("GraphSegment\n");
	
	GraphSegment();
}

void CZyView::OnSegmentGraphcut()
{
	// TODO: 在此添加命令处理程序代码
	printf("hutoubao\n");
	GraphCut();

}
