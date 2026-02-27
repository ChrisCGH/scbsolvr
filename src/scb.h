// scb.h : main header file for the SCB application
//

#if !defined(AFX_SCB_H__B66C2CD5_08C3_11D5_B48D_40871F000000__INCLUDED_)
#define AFX_SCB_H__B66C2CD5_08C3_11D5_B48D_40871F000000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CScbApp:
// See scb.cpp for the implementation of this class
//

class CScbApp : public CWinApp
{
public:
	CScbApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CScbApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CScbApp)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SCB_H__B66C2CD5_08C3_11D5_B48D_40871F000000__INCLUDED_)
