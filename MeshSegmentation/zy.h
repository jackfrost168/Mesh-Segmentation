// zy.h : main header file for the ZY application
//

#if !defined(AFX_ZY_H__3BFDD786_DA12_493F_9A84_E8A2E4BF94E4__INCLUDED_)
#define AFX_ZY_H__3BFDD786_DA12_493F_9A84_E8A2E4BF94E4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CZyApp:
// See zy.cpp for the implementation of this class
//

class CZyApp : public CWinApp
{
public:
	CZyApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CZyApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation
	//{{AFX_MSG(CZyApp)
	afx_msg void OnAppAbout();
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ZY_H__3BFDD786_DA12_493F_9A84_E8A2E4BF94E4__INCLUDED_)
