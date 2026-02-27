// scb.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "scb.h"
#include "scbDlg.h"
#include <direct.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CScbApp

BEGIN_MESSAGE_MAP(CScbApp, CWinApp)
	//{{AFX_MSG_MAP(CScbApp)
	//}}AFX_MSG_MAP
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CScbApp construction

CScbApp::CScbApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CScbApp object

CScbApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CScbApp initialization

BOOL CScbApp::InitInstance()
{
	AfxEnableControlContainer();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif

	// initialise Registry settings
	SetRegistryKey("SCB");

	if (GetProfileInt("Default File Locations", "File Drive", -1) == -1)
	{
		WriteProfileInt("Default File Locations", "File Drive", _getdrive());
	    WriteProfileInt("Default File Locations", "Ngraph Drive", _getdrive());
		char buf[_MAX_PATH];
		_getcwd(buf, _MAX_PATH - 1);
	    WriteProfileString("Default File Locations", "File Directory", buf);
	    WriteProfileString("Default File Locations", "Ngraph Directory", buf);
	}

	CScbDlg dlg;
	m_pMainWnd = &dlg;
	int nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with OK
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with Cancel
	}

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}
