#if !defined(AFX_CRIBDLG_H__F6832BD3_54F3_11D5_B4AC_467C5A000000__INCLUDED_)
#define AFX_CRIBDLG_H__F6832BD3_54F3_11D5_B4AC_467C5A000000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// CribDlg.h : header file
//
#include "crib.h"
/////////////////////////////////////////////////////////////////////////////
// CribDlg dialog

class CribDlg : public CDialog
{
// Construction
public:
//	CribDlg(CWnd* pParent = NULL);   // standard constructor
	CribDlg(const Crib& c, int preserve_punctuation, 
		    CWnd* pParent = NULL);   // standard constructor
	Crib get_crib();

// Dialog Data
	//{{AFX_DATA(CribDlg)
	enum { IDD = IDD_CRIB_DIALOG };
	CString	m_ciphertext;
	CString	m_crib_string;
	CString	m_key;
	CString	m_plaintext;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CribDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
	private:
		Crib m_crib;
		int m_preserve_punctuation;
		void display();
protected:

	// Generated message map functions
	//{{AFX_MSG(CribDlg)
	afx_msg void OnCribClear();
	afx_msg void OnCribNextLeft();
	afx_msg void OnCribNextRight();
	virtual BOOL OnInitDialog();
	afx_msg void OnUpdateCribCrib();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CRIBDLG_H__F6832BD3_54F3_11D5_B4AC_467C5A000000__INCLUDED_)
