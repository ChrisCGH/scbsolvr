#if !defined(AFX_FIXKEYDLG_H__672F09C3_1A4D_11D5_B496_64D0FC000000__INCLUDED_)
#define AFX_FIXKEYDLG_H__672F09C3_1A4D_11D5_B496_64D0FC000000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// FixKeyDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CFixKeyDlg dialog

class CFixKeyDlg : public CDialog
{
// Construction
public:
	CFixKeyDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CFixKeyDlg)
	enum { IDD = IDD_FIX_KEY_DIALOG };
	CString	m_a_ct;
	CString	m_b_ct;
	CString	m_c_ct;
	CString	m_d_ct;
	CString	m_e_ct;
	CString	m_f_ct;
	CString	m_g_ct;
	CString	m_h_ct;
	CString	m_i_ct;
	CString	m_j_ct;
	CString	m_k_ct;
	CString	m_l_ct;
	CString	m_m_ct;
	CString	m_n_ct;
	CString	m_o_ct;
	CString	m_p_ct;
	CString	m_q_ct;
	CString	m_r_ct;
	CString	m_s_ct;
	CString	m_t_ct;
	CString	m_u_ct;
	CString	m_v_ct;
	CString	m_w_ct;
	CString	m_x_ct;
	CString	m_y_ct;
	CString	m_z_ct;
	//}}AFX_DATA
public:
	void set_fixed(const Fixed_Key& fixed) { m_fixed = fixed; }
	const Fixed_Key& get_fixed() { return m_fixed; }
private:
	CString* m_ct[26];
	Fixed_Key m_fixed;
	void DoDisplay();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFixKeyDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void OnUpdateCT(int i);

	// Generated message map functions
	//{{AFX_MSG(CFixKeyDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnUpdateCT1();
	afx_msg void OnUpdateCT2();
	afx_msg void OnUpdateCT3();
	afx_msg void OnUpdateCT4();
	afx_msg void OnUpdateCT5();
	afx_msg void OnUpdateCT6();
	afx_msg void OnUpdateCT7();
	afx_msg void OnUpdateCT8();
	afx_msg void OnUpdateCT9();
	afx_msg void OnUpdateCT10();
	afx_msg void OnUpdateCT11();
	afx_msg void OnUpdateCT12();
	afx_msg void OnUpdateCT13();
	afx_msg void OnUpdateCT14();
	afx_msg void OnUpdateCT15();
	afx_msg void OnUpdateCT16();
	afx_msg void OnUpdateCT17();
	afx_msg void OnUpdateCT18();
	afx_msg void OnUpdateCT19();
	afx_msg void OnUpdateCT20();
	afx_msg void OnUpdateCT21();
	afx_msg void OnUpdateCT22();
	afx_msg void OnUpdateCT23();
	afx_msg void OnUpdateCT24();
	afx_msg void OnUpdateCT25();
	afx_msg void OnUpdateCT26();
	afx_msg void OnClear();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FIXKEYDLG_H__672F09C3_1A4D_11D5_B496_64D0FC000000__INCLUDED_)
