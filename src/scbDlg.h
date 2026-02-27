// scbDlg.h : header file
//

#if !defined(AFX_SCBDLG_H__B66C2CD7_08C3_11D5_B48D_40871F000000__INCLUDED_)
#define AFX_SCBDLG_H__B66C2CD7_08C3_11D5_B48D_40871F000000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

/////////////////////////////////////////////////////////////////////////////
// CScbDlg dialog
#include "mono_solver.h"
#include "fixed.h"
#include "crib.h"

class CScbDlg : public CDialog
{
// Construction
public:
    friend void process_result(void* me, const Mono_Solver& solver);
	CScbDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CScbDlg)
	enum { IDD = IDD_SCB_DIALOG };
	CString	m_ngraph_file;
	CString	m_ciphertext;
	CString	m_plaintext;
	int		m_keys_checked;
	CString	m_key;
	CString	m_cipher_filename;
	CString	m_phi;
	CString m_score;
	//}}AFX_DATA
	enum Solver_State { CLEARED = 0, STOPPED = 1, SOLVING = 2, INTERRUPTED = 3 };
    enum Solver_State state() const { return solver_state; }
    BOOL preserve_original_format() { return m_preserve_original_format; }
	BOOL remove_punctuation() { return m_remove_punctuation; }

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CScbDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

private:
	Mono_Solver solver;
	CWinThread* solver_thread;
	enum Solver_State solver_state;
	BOOL m_preserve_original_format;
	BOOL m_remove_punctuation;
	BOOL m_ciphertext_changed;
	Fixed_Key m_fixed;
	Crib m_crib;
	char m_working_directory[_MAX_PATH];
	void DoDisplay();
// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CScbDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnQuit();
	afx_msg void OnSolve();
	afx_msg LRESULT OnSolverCallback(WPARAM, LPARAM);
	afx_msg LRESULT OnSolverCallback1(WPARAM, LPARAM);
	afx_msg void OnStop();
	afx_msg void OnHelpAboutsecretcodebreaker();
	afx_msg void OnFileOpen();
	afx_msg void OnSetNgraphFile();
	afx_msg void OnInterrupt();
	afx_msg void OnResume();
	afx_msg void OnFileExit();
	afx_msg void OnUpdateFileOpen(CCmdUI* pCmdUI);
	afx_msg void OnUpdateSetNgraphFile(CCmdUI* pCmdUI);
	afx_msg void OnToolsCreatengraphfile();
	afx_msg void OnInitMenuPopup(CMenu*, UINT, BOOL);
	afx_msg void OnViewOutputFormatPreserve();
	afx_msg void OnUpdateViewOutputFormatPreserve(CCmdUI* pCmdUI);
	afx_msg void OnUpdateCipher();
	afx_msg void OnClear();
	afx_msg void OnFileSave();
	afx_msg void OnUpdateFileSave(CCmdUI* pCmdUI);
	afx_msg void OnToolsFixkey();
	afx_msg void OnUpdateToolsFixkey(CCmdUI* pCmdUI);
	afx_msg void OnHelpInstructions();
	afx_msg void OnViewRemovepunctuation();
	afx_msg void OnUpdateViewRemovepunctuation(CCmdUI* pCmdUI);
	afx_msg void OnHelpTheory();
	afx_msg void OnToolsCribbing();
	afx_msg void OnUpdateToolsCribbing(CCmdUI* pCmdUI);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SCBDLG_H__B66C2CD7_08C3_11D5_B48D_40871F000000__INCLUDED_)
