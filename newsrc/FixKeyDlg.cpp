// FixKeyDlg.cpp : implementation file
//

#include "stdafx.h"
#include "scb.h"
#include "fixed.h"
#include "FixKeyDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFixKeyDlg dialog


CFixKeyDlg::CFixKeyDlg(CWnd* pParent /*=NULL*/)
      : CDialog(CFixKeyDlg::IDD, pParent), m_fixed()
{
   //{{AFX_DATA_INIT(CFixKeyDlg)
   m_a_ct = _T("");
   //}}AFX_DATA_INIT
   m_ct[0] = &m_a_ct;
   m_ct[1] = &m_b_ct;
   m_ct[2] = &m_c_ct;
   m_ct[3] = &m_d_ct;
   m_ct[4] = &m_e_ct;
   m_ct[5] = &m_f_ct;
   m_ct[6] = &m_g_ct;
   m_ct[7] = &m_h_ct;
   m_ct[8] = &m_i_ct;
   m_ct[9] = &m_j_ct;
   m_ct[10] = &m_k_ct;
   m_ct[11] = &m_l_ct;
   m_ct[12] = &m_m_ct;
   m_ct[13] = &m_n_ct;
   m_ct[14] = &m_o_ct;
   m_ct[15] = &m_p_ct;
   m_ct[16] = &m_q_ct;
   m_ct[17] = &m_r_ct;
   m_ct[18] = &m_s_ct;
   m_ct[19] = &m_t_ct;
   m_ct[20] = &m_u_ct;
   m_ct[21] = &m_v_ct;
   m_ct[22] = &m_w_ct;
   m_ct[23] = &m_x_ct;
   m_ct[24] = &m_y_ct;
   m_ct[25] = &m_z_ct;
}


void CFixKeyDlg::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(CFixKeyDlg)
   DDX_Text(pDX, IDC_EDIT1, m_a_ct);
   DDV_MaxChars(pDX, m_a_ct, 1);
   DDX_Text(pDX, IDC_EDIT2, m_b_ct);
   DDV_MaxChars(pDX, m_b_ct, 1);
   DDX_Text(pDX, IDC_EDIT3, m_c_ct);
   DDV_MaxChars(pDX, m_c_ct, 1);
   DDX_Text(pDX, IDC_EDIT4, m_d_ct);
   DDV_MaxChars(pDX, m_d_ct, 1);
   DDX_Text(pDX, IDC_EDIT5, m_e_ct);
   DDV_MaxChars(pDX, m_e_ct, 1);
   DDX_Text(pDX, IDC_EDIT6, m_f_ct);
   DDV_MaxChars(pDX, m_f_ct, 1);
   DDX_Text(pDX, IDC_EDIT7, m_g_ct);
   DDV_MaxChars(pDX, m_g_ct, 1);
   DDX_Text(pDX, IDC_EDIT8, m_h_ct);
   DDV_MaxChars(pDX, m_h_ct, 1);
   DDX_Text(pDX, IDC_EDIT9, m_i_ct);
   DDV_MaxChars(pDX, m_i_ct, 1);
   DDX_Text(pDX, IDC_EDIT10, m_j_ct);
   DDV_MaxChars(pDX, m_j_ct, 1);
   DDX_Text(pDX, IDC_EDIT11, m_k_ct);
   DDV_MaxChars(pDX, m_k_ct, 1);
   DDX_Text(pDX, IDC_EDIT12, m_l_ct);
   DDV_MaxChars(pDX, m_l_ct, 1);
   DDX_Text(pDX, IDC_EDIT13, m_m_ct);
   DDV_MaxChars(pDX, m_m_ct, 1);
   DDX_Text(pDX, IDC_EDIT14, m_n_ct);
   DDV_MaxChars(pDX, m_n_ct, 1);
   DDX_Text(pDX, IDC_EDIT15, m_o_ct);
   DDV_MaxChars(pDX, m_o_ct, 1);
   DDX_Text(pDX, IDC_EDIT16, m_p_ct);
   DDV_MaxChars(pDX, m_p_ct, 1);
   DDX_Text(pDX, IDC_EDIT17, m_q_ct);
   DDV_MaxChars(pDX, m_q_ct, 1);
   DDX_Text(pDX, IDC_EDIT18, m_r_ct);
   DDV_MaxChars(pDX, m_r_ct, 1);
   DDX_Text(pDX, IDC_EDIT19, m_s_ct);
   DDV_MaxChars(pDX, m_s_ct, 1);
   DDX_Text(pDX, IDC_EDIT20, m_t_ct);
   DDV_MaxChars(pDX, m_t_ct, 1);
   DDX_Text(pDX, IDC_EDIT21, m_u_ct);
   DDV_MaxChars(pDX, m_u_ct, 1);
   DDX_Text(pDX, IDC_EDIT22, m_v_ct);
   DDV_MaxChars(pDX, m_v_ct, 1);
   DDX_Text(pDX, IDC_EDIT23, m_w_ct);
   DDV_MaxChars(pDX, m_w_ct, 1);
   DDX_Text(pDX, IDC_EDIT24, m_x_ct);
   DDV_MaxChars(pDX, m_x_ct, 1);
   DDX_Text(pDX, IDC_EDIT25, m_y_ct);
   DDV_MaxChars(pDX, m_y_ct, 1);
   DDX_Text(pDX, IDC_EDIT26, m_z_ct);
   DDV_MaxChars(pDX, m_z_ct, 1);

   //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CFixKeyDlg, CDialog)
   //{{AFX_MSG_MAP(CFixKeyDlg)
   ON_EN_UPDATE(IDC_EDIT1, OnUpdateCT1)
   ON_EN_UPDATE(IDC_EDIT2, OnUpdateCT2)
   ON_EN_UPDATE(IDC_EDIT3, OnUpdateCT3)
   ON_EN_UPDATE(IDC_EDIT4, OnUpdateCT4)
   ON_EN_UPDATE(IDC_EDIT5, OnUpdateCT5)
   ON_EN_UPDATE(IDC_EDIT6, OnUpdateCT6)
   ON_EN_UPDATE(IDC_EDIT7, OnUpdateCT7)
   ON_EN_UPDATE(IDC_EDIT8, OnUpdateCT8)
   ON_EN_UPDATE(IDC_EDIT9, OnUpdateCT9)
   ON_EN_UPDATE(IDC_EDIT10, OnUpdateCT10)
   ON_EN_UPDATE(IDC_EDIT11, OnUpdateCT11)
   ON_EN_UPDATE(IDC_EDIT12, OnUpdateCT12)
   ON_EN_UPDATE(IDC_EDIT13, OnUpdateCT13)
   ON_EN_UPDATE(IDC_EDIT14, OnUpdateCT14)
   ON_EN_UPDATE(IDC_EDIT15, OnUpdateCT15)
   ON_EN_UPDATE(IDC_EDIT16, OnUpdateCT16)
   ON_EN_UPDATE(IDC_EDIT17, OnUpdateCT17)
   ON_EN_UPDATE(IDC_EDIT18, OnUpdateCT18)
   ON_EN_UPDATE(IDC_EDIT19, OnUpdateCT19)
   ON_EN_UPDATE(IDC_EDIT20, OnUpdateCT20)
   ON_EN_UPDATE(IDC_EDIT21, OnUpdateCT21)
   ON_EN_UPDATE(IDC_EDIT22, OnUpdateCT22)
   ON_EN_UPDATE(IDC_EDIT23, OnUpdateCT23)
   ON_EN_UPDATE(IDC_EDIT24, OnUpdateCT24)
   ON_EN_UPDATE(IDC_EDIT25, OnUpdateCT25)
   ON_EN_UPDATE(IDC_EDIT26, OnUpdateCT26)
   ON_BN_CLICKED(ID_CLEAR, OnClear)
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFixKeyDlg message handlers

BOOL CFixKeyDlg::OnInitDialog()
{
   CDialog::OnInitDialog();

   // TODO: Add extra initialization here
   CFont *pFixedFont = CFont::FromHandle((HFONT)GetStockObject(ANSI_FIXED_FONT));
   if (pFixedFont)
   {
      for (int i = 0; i < 26; i++)
      {
         GetDlgItem(IDC_STATIC1 + i)->SetFont(pFixedFont);
      }
   }
   DoDisplay();
   return TRUE;  // return TRUE unless you set the focus to a control
   // EXCEPTION: OCX Property Pages should return FALSE
}

void CFixKeyDlg::OnUpdateCT1()
{
   OnUpdateCT(0);
}
void CFixKeyDlg::OnUpdateCT2()
{
   OnUpdateCT(1);
}
void CFixKeyDlg::OnUpdateCT3()
{
   OnUpdateCT(2);
}
void CFixKeyDlg::OnUpdateCT4()
{
   OnUpdateCT(3);
}
void CFixKeyDlg::OnUpdateCT5()
{
   OnUpdateCT(4);
}
void CFixKeyDlg::OnUpdateCT6()
{
   OnUpdateCT(5);
}
void CFixKeyDlg::OnUpdateCT7()
{
   OnUpdateCT(6);
}
void CFixKeyDlg::OnUpdateCT8()
{
   OnUpdateCT(7);
}
void CFixKeyDlg::OnUpdateCT9()
{
   OnUpdateCT(8);
}
void CFixKeyDlg::OnUpdateCT10()
{
   OnUpdateCT(9);
}
void CFixKeyDlg::OnUpdateCT11()
{
   OnUpdateCT(10);
}
void CFixKeyDlg::OnUpdateCT12()
{
   OnUpdateCT(11);
}
void CFixKeyDlg::OnUpdateCT13()
{
   OnUpdateCT(12);
}
void CFixKeyDlg::OnUpdateCT14()
{
   OnUpdateCT(13);
}
void CFixKeyDlg::OnUpdateCT15()
{
   OnUpdateCT(14);
}
void CFixKeyDlg::OnUpdateCT16()
{
   OnUpdateCT(15);
}
void CFixKeyDlg::OnUpdateCT17()
{
   OnUpdateCT(16);
}
void CFixKeyDlg::OnUpdateCT18()
{
   OnUpdateCT(17);
}
void CFixKeyDlg::OnUpdateCT19()
{
   OnUpdateCT(18);
}
void CFixKeyDlg::OnUpdateCT20()
{
   OnUpdateCT(19);
}
void CFixKeyDlg::OnUpdateCT21()
{
   OnUpdateCT(20);
}
void CFixKeyDlg::OnUpdateCT22()
{
   OnUpdateCT(21);
}
void CFixKeyDlg::OnUpdateCT23()
{
   OnUpdateCT(22);
}
void CFixKeyDlg::OnUpdateCT24()
{
   OnUpdateCT(23);
}
void CFixKeyDlg::OnUpdateCT25()
{
   OnUpdateCT(24);
}
void CFixKeyDlg::OnUpdateCT26()
{
   OnUpdateCT(25);
}
void CFixKeyDlg::OnUpdateCT(int i)
{
   UpdateData(TRUE);
   char pt = i + 'a';
   char ct = m_ct[i]->GetAt(0);
   if (ct == 0) ct = ' ';
   if ((ct < 'A' || ct > 'Z') && ct != ' ')
   {
      ct = m_fixed.get_ct(pt);
      *m_ct[i] = CString(ct);
      UpdateData(FALSE);
   }
   else
   {
      if (ct != ' ')
      {
         if (m_fixed.is_set(pt))
         {
            m_fixed.clear(m_fixed.get_ct(pt));
         }
         m_fixed.set(pt, ct);
      }
      else
      {
         m_fixed.clear(m_fixed.get_ct(pt));
      }
      DoDisplay();
   }
}

void CFixKeyDlg::DoDisplay()
{
   for (int i = 0; i < 26; i++)
   {
      char ct = m_fixed.get_ct(i + 'a');
      if (ct != ' ')
      {
         *m_ct[i] = CString(ct);
      }
      else
      {
         *m_ct[i] = CString("");
      }
   }
   UpdateData(FALSE);
}

void CFixKeyDlg::OnClear()
{
   m_fixed.clear();
   DoDisplay();
}