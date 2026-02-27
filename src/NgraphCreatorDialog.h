#if !defined(AFX_NGRAPHCREATORDIALOG_H__56D3A3D3_100E_11D5_B492_8A2B15000000__INCLUDED_)
#define AFX_NGRAPHCREATORDIALOG_H__56D3A3D3_100E_11D5_B492_8A2B15000000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// NgraphCreatorDialog.h : header file
//
#include "ngraph.h"

/////////////////////////////////////////////////////////////////////////////
// CNgraphCreatorDialog dialog

class CNgraphCreatorDialog : public CDialog
{
// Construction
public:
	CNgraphCreatorDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CNgraphCreatorDialog)
	enum { IDD = IDD_NGRAPH_FILE_DIALOG };
	CProgressCtrl	m_ngraph_progress;
	CString	m_sample_file;
	CString	m_ngraph_file;
	int		m_n;
	CString	m_progress_text;
	int     m_spaces;
	//}}AFX_DATA
	long int m_sample_size;
	Ngraph_File_Creator* m_creator;
	CWinThread* m_creator_thread;
	long int m_progress;
	long int m_previous_progress;


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CNgraphCreatorDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CNgraphCreatorDialog)
	afx_msg void OnBrowseNgraphFile();
	afx_msg void OnBrowseSampleFile();
	virtual void OnOK();
	afx_msg void OnCreateNgraphFile();
	afx_msg LRESULT OnCreatorCallback(WPARAM, LPARAM);
	afx_msg void OnDone();
	afx_msg void OnSpaces();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_NGRAPHCREATORDIALOG_H__56D3A3D3_100E_11D5_B492_8A2B15000000__INCLUDED_)
