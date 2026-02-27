// CribDlg.cpp : implementation file
//

#include "stdafx.h"
#include "scb.h"
#include "CribDlg.h"
#include "mono_solver.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CribDlg dialog

CribDlg::CribDlg(const Crib& c, int preserve_punctuation, CWnd* pParent /*=NULL*/)
	: CDialog(CribDlg::IDD, pParent), m_preserve_punctuation(preserve_punctuation)
{
	//{{AFX_DATA_INIT(CribDlg)
	m_key = _T("");
	m_plaintext = _T("");
	//}}AFX_DATA_INIT
	m_crib = c;
	m_ciphertext = c.get_original_ct();
    m_crib_string = c.get_crib_string();
	display();
}

void CribDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CribDlg)
	DDX_Text(pDX, IDC_CRIB_CIPHER, m_ciphertext);
	DDX_Text(pDX, IDC_CRIB_CRIB, m_crib_string);
	DDX_Text(pDX, IDC_CRIB_KEY, m_key);
	DDX_Text(pDX, IDC_CRIB_PLAIN, m_plaintext);
	//}}AFX_DATA_MAP
}

Crib CribDlg::get_crib()
{
	return m_crib;
}

BEGIN_MESSAGE_MAP(CribDlg, CDialog)
	//{{AFX_MSG_MAP(CribDlg)
	ON_BN_CLICKED(ID_CRIB_CLEAR, OnCribClear)
	ON_BN_CLICKED(IDC_CRIB_NEXT_LEFT, OnCribNextLeft)
	ON_BN_CLICKED(IDC_CRIB_NEXT_RIGHT, OnCribNextRight)
	ON_EN_UPDATE(IDC_CRIB_CRIB, OnUpdateCribCrib)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CribDlg message handlers

void CribDlg::OnCribClear() 
{
	// TODO: Add your control notification handler code here
	m_crib_string = "";
	m_crib = Crib(m_ciphertext, m_crib_string);
	display();
	UpdateData(FALSE);
}

void CribDlg::OnCribNextLeft() 
{
	// TODO: Add your control notification handler code here
	m_crib.next_left();
	display();
	UpdateData(FALSE);
}

void CribDlg::OnCribNextRight() 
{
	// TODO: Add your control notification handler code here
	m_crib.next_right();
    display();
	UpdateData(FALSE);
}

BOOL CribDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	 CFont *pFixedFont = CFont::FromHandle((HFONT)GetStockObject(ANSI_FIXED_FONT));
     if (pFixedFont)
     {
        GetDlgItem(IDC_CRIB_CIPHER)->SetFont(pFixedFont);
        GetDlgItem(IDC_CRIB_PLAIN)->SetFont(pFixedFont);
        GetDlgItem(IDC_CRIB_KEY)->SetFont(pFixedFont);
     }	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CribDlg::OnUpdateCribCrib() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function to send the EM_SETEVENTMASK message to the control
	// with the ENM_UPDATE flag ORed into the lParam mask.
	
	// TODO: Add your control notification handler code here
	CString old_crib_string = m_crib_string;
	UpdateData(TRUE);
	if (old_crib_string != m_crib_string)
	{
		int pos = m_crib.get_position();
		m_crib = Crib(m_ciphertext, m_crib_string);
		m_crib.place_at(pos);
	}

	display();
	UpdateData(FALSE);
}

void CribDlg::display()
{
	m_plaintext = m_crib.get_pt();
	if (m_preserve_punctuation)
	{
		m_plaintext = Mono_Solver::reformat(m_plaintext, m_crib.get_original_ct());
	}
	char tmp[30];
	sprintf(tmp, "\r\n%s", "abcdefghijklmnopqrstuvwxyz");
	m_key = m_crib.get_fixed_key().get_key() + CString(tmp);
}