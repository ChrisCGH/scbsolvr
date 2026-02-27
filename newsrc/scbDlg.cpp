// scbDlg.cpp : implementation file
//
#include "stdafx.h"
#include "scb.h"
#include "scbDlg.h"
#include "NGraphCreatorDialog.h"
#include "FixKeyDlg.h"
#include "CribDlg.h"
#include <direct.h>
#include <process.h>
#include <sys/stat.h>


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
BOOL AFXAPI AfxFullPath(LPTSTR lpszPathOut, LPCTSTR lpszFileIn);

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
   public:
      CAboutDlg();

// Dialog Data
      //{{AFX_DATA(CAboutDlg)
      enum { IDD = IDD_ABOUTBOX };
      //}}AFX_DATA

      // ClassWizard generated virtual function overrides
      //{{AFX_VIRTUAL(CAboutDlg)
   protected:
      virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
      //}}AFX_VIRTUAL

// Implementation
   protected:
      //{{AFX_MSG(CAboutDlg)
      //}}AFX_MSG
      DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
   //{{AFX_DATA_INIT(CAboutDlg)
   //}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(CAboutDlg)
   //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
   //{{AFX_MSG_MAP(CAboutDlg)
   // No message handlers
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CScbDlg dialog
CScbDlg::CScbDlg(CWnd* pParent /*=NULL*/)
      : CDialog(CScbDlg::IDD, pParent)
{
   char temp[132];
   AfxFullPath(temp, "english.tet");
   //{{AFX_DATA_INIT(CScbDlg)
   m_ngraph_file = _T(temp);
   m_ciphertext = _T("");
   m_plaintext = _T("");
   m_keys_checked = 0;
   m_key = _T("");
   m_cipher_filename = _T("");
   solver_state = CLEARED;
   m_phi = _T("");
   m_score = _T("");
   //}}AFX_DATA_INIT
   // Note that LoadIcon does not require a subsequent DestroyIcon in Win32
   m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
   m_preserve_original_format = TRUE;
   m_remove_punctuation = FALSE;
   m_ciphertext_changed = FALSE;
   solver_thread = 0;
}

void CScbDlg::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(CScbDlg)
   DDX_Text(pDX, IDC_NGRAPH_FILE, m_ngraph_file);
   DDX_Text(pDX, IDC_CIPHER, m_ciphertext);
   DDV_MaxChars(pDX, m_ciphertext, 2000);
   DDX_Text(pDX, IDC_PLAIN, m_plaintext);
   DDX_Text(pDX, IDC_KEYS_CHECKED, m_keys_checked);
   DDX_Text(pDX, IDC_KEY, m_key);
   DDX_Text(pDX, IDC_CIPHER_FILENAME, m_cipher_filename);
   DDX_Text(pDX, IDC_PHI_VALUE, m_phi);
   DDX_Text(pDX, IDC_SCORE, m_score);
   //}}AFX_DATA_MAP
}

#define M_SOLVERCALLBACK 9999
#define M_SOLVERCALLBACK1 9998
BEGIN_MESSAGE_MAP(CScbDlg, CDialog)
   //{{AFX_MSG_MAP(CScbDlg)
   ON_WM_SYSCOMMAND()
   ON_WM_PAINT()
   ON_WM_QUERYDRAGICON()
   ON_BN_CLICKED(IDQUIT, OnQuit)
   ON_BN_CLICKED(ID_SOLVE, OnSolve)
   ON_MESSAGE(M_SOLVERCALLBACK, OnSolverCallback)
   ON_MESSAGE(M_SOLVERCALLBACK1, OnSolverCallback1)
   ON_BN_CLICKED(ID_STOP, OnStop)
   ON_COMMAND(ID_HELP_ABOUTSECRETCODEBREAKER, OnHelpAboutsecretcodebreaker)
   ON_COMMAND(IDM_FILE_OPEN, OnFileOpen)
   ON_COMMAND(IDM_SET_NGRAPH_FILE, OnSetNgraphFile)
   ON_BN_CLICKED(ID_INTERRUPT, OnInterrupt)
   ON_BN_CLICKED(ID_RESUME, OnResume)
   ON_COMMAND(ID_FILE_EXIT, OnFileExit)
   ON_UPDATE_COMMAND_UI(IDM_FILE_OPEN, OnUpdateFileOpen)
   ON_UPDATE_COMMAND_UI(IDM_SET_NGRAPH_FILE, OnUpdateSetNgraphFile)
   ON_COMMAND(ID_TOOLS_CREATENGRAPHFILE, OnToolsCreatengraphfile)
   ON_WM_INITMENUPOPUP()
   ON_COMMAND(ID_VIEW_OUTPUT_FORMAT_PRESERVE, OnViewOutputFormatPreserve)
   ON_UPDATE_COMMAND_UI(ID_VIEW_OUTPUT_FORMAT_PRESERVE, OnUpdateViewOutputFormatPreserve)
   ON_EN_UPDATE(IDC_CIPHER, OnUpdateCipher)
   ON_BN_CLICKED(ID_CLEAR, OnClear)
   ON_COMMAND(ID_FILE_SAVE, OnFileSave)
   ON_UPDATE_COMMAND_UI(ID_FILE_SAVE, OnUpdateFileSave)
   ON_COMMAND(ID_TOOLS_FIXKEY, OnToolsFixkey)
   ON_UPDATE_COMMAND_UI(ID_TOOLS_FIXKEY, OnUpdateToolsFixkey)
   ON_COMMAND(ID_HELP_INSTRUCTIONS, OnHelpInstructions)
   ON_COMMAND(ID_VIEW_REMOVEPUNCTUATION, OnViewRemovepunctuation)
   ON_UPDATE_COMMAND_UI(ID_VIEW_REMOVEPUNCTUATION, OnUpdateViewRemovepunctuation)
   ON_COMMAND(ID_HELP_THEORY, OnHelpTheory)
   ON_COMMAND(ID_TOOLS_CRIBBING, OnToolsCribbing)
   ON_UPDATE_COMMAND_UI(ID_TOOLS_CRIBBING, OnUpdateToolsCribbing)
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CScbDlg message handlers
static unsigned int solve(void* param)
{
   Mono_Solver* solver = (Mono_Solver*)param;
   solver->solve();
   return 0;
}

BOOL CScbDlg::OnInitDialog()
{
   CDialog::OnInitDialog();

   // Add "About..." menu item to system menu.

   // IDM_ABOUTBOX must be in the system command range.
   ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
   ASSERT(IDM_ABOUTBOX < 0xF000);

   CMenu* pSysMenu = GetSystemMenu(FALSE);
   if (pSysMenu != NULL)
   {
      CString strAboutMenu;
      strAboutMenu.LoadString(IDS_ABOUTBOX);
      if (!strAboutMenu.IsEmpty())
      {
         pSysMenu->AppendMenu(MF_SEPARATOR);
         pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
      }
   }

   CFont *pFixedFont = CFont::FromHandle((HFONT)GetStockObject(ANSI_FIXED_FONT));
   if (pFixedFont)
   {
      GetDlgItem(IDC_CIPHER)->SetFont(pFixedFont);
      GetDlgItem(IDC_PLAIN)->SetFont(pFixedFont);
      GetDlgItem(IDC_KEY)->SetFont(pFixedFont);
   }
   // Set the icon for this dialog.  The framework does this automatically
   //  when the application's main window is not a dialog
   SetIcon(m_hIcon, TRUE);			// Set big icon
   SetIcon(m_hIcon, FALSE);		// Set small icon

   DoDisplay();

   // TODO: Add extra initialization here
   // get current working directory

   _getcwd(m_working_directory, _MAX_PATH - 1);

   return TRUE;  // return TRUE  unless you set the focus to a control
}

void CScbDlg::OnInitMenuPopup(CMenu* pMenu, UINT, BOOL bSysMenu)
{
   if (bSysMenu)
      return;     // don't support system menu
   ASSERT(pMenu != NULL);
   // check the enabled state of various menu items


   CCmdUI state;
   state.m_pMenu = pMenu;
   ASSERT(state.m_pOther == NULL);
   ASSERT(state.m_pParentMenu == NULL);

   // determine if menu is popup in top-level menu and set m_pOther to
   //  it if so (m_pParentMenu == NULL indicates that it is secondary popup)
   HMENU hParentMenu;
   if (AfxGetThreadState()->m_hTrackingMenu == pMenu->m_hMenu)
      state.m_pParentMenu = pMenu;    // parent == child for tracking popup
   else if ((hParentMenu = ::GetMenu(m_hWnd)) != NULL)
   {
      CWnd* pParent = GetTopLevelParent();
      // child windows don't have menus -- need to go to the top!
      if (pParent != NULL &&
            (hParentMenu = ::GetMenu(pParent->m_hWnd)) != NULL)
      {
         int nIndexMax = ::GetMenuItemCount(hParentMenu);
         for (int nIndex = 0; nIndex < nIndexMax; nIndex++)
         {
            if (::GetSubMenu(hParentMenu, nIndex) == pMenu->m_hMenu)
            {
               // when popup is found, m_pParentMenu is containing menu
               state.m_pParentMenu = CMenu::FromHandle(hParentMenu);
               break;
            }
         }
      }
   }

   state.m_nIndexMax = pMenu->GetMenuItemCount();
   for (state.m_nIndex = 0; state.m_nIndex < state.m_nIndexMax;
         state.m_nIndex++)
   {
      state.m_nID = pMenu->GetMenuItemID(state.m_nIndex);
      if (state.m_nID == 0)
         continue; // menu separator or invalid cmd - ignore it

      ASSERT(state.m_pOther == NULL);
      ASSERT(state.m_pMenu != NULL);
      if (state.m_nID == (UINT)-1)
      {
         // possibly a popup menu, route to first item of that popup
         state.m_pSubMenu = pMenu->GetSubMenu(state.m_nIndex);
         if (state.m_pSubMenu == NULL ||
               (state.m_nID = state.m_pSubMenu->GetMenuItemID(0)) == 0 ||
               state.m_nID == (UINT)-1)
         {
            continue;       // first item of popup can't be routed to
         }
         state.DoUpdate(this, FALSE);    // popups are never auto disabled
      }
      else
      {
         // normal menu item
         // Auto enable/disable if frame window has 'm_bAutoMenuEnable'
         //    set and command is _not_ a system command.
         state.m_pSubMenu = NULL;
         BOOL m_bAutoMenuEnable = TRUE;
         state.DoUpdate(this, m_bAutoMenuEnable && state.m_nID < 0xF000);
      }

      // adjust for menu deletions and additions
      UINT nCount = pMenu->GetMenuItemCount();
      if (nCount < state.m_nIndexMax)
      {
         state.m_nIndex -= (state.m_nIndexMax - nCount);
         while (state.m_nIndex < nCount &&
                pMenu->GetMenuItemID(state.m_nIndex) == state.m_nID)
         {
            state.m_nIndex++;
         }
      }
      state.m_nIndexMax = nCount;
   }

}

void CScbDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
   if ((nID & 0xFFF0) == IDM_ABOUTBOX)
   {
      CAboutDlg dlgAbout;
      dlgAbout.DoModal();
   }
   else
   {
      CDialog::OnSysCommand(nID, lParam);
   }
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CScbDlg::OnPaint()
{
   if (IsIconic())
   {
      CPaintDC dc(this); // device context for painting

      SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

      // Center icon in client rectangle
      int cxIcon = GetSystemMetrics(SM_CXICON);
      int cyIcon = GetSystemMetrics(SM_CYICON);
      CRect rect;
      GetClientRect(&rect);
      int x = (rect.Width() - cxIcon + 1) / 2;
      int y = (rect.Height() - cyIcon + 1) / 2;

      // Draw the icon
      dc.DrawIcon(x, y, m_hIcon);
   }
   else
   {
      CDialog::OnPaint();
   }
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CScbDlg::OnQueryDragIcon()
{
   return (HCURSOR) m_hIcon;
}

void CScbDlg::OnQuit()
{
   // TODO: Add your control notification handler code here
   if (solver_thread)
   {
      solver_thread->SuspendThread();
      solver.set_terminate();
      solver_thread->ResumeThread();
   }
   CDialog::OnOK();
}

// callback function called by Mono_Solver when
// there's something to update
static char* remove_spaces(const char* str)
{
   static char buf[2000];
   const char* c = str;
   char* d = buf;
   while (c && *c)
   {
      if (*c != 'z' + 1)
      {
         *d++ = *c;
      }
      c++;
   }
   *d = '\0';
   return buf;
}

void CScbDlg::process_result(const Mono_Solver& solver)
{
   if (solver.get_terminate()) return;
   CString result = solver.best_result(0);
   if (solver.spaces_scored())
   {
      result = remove_spaces(result);
   }
   if (preserve_original_format())
   {
      result = Mono_Solver::reformat(result, m_ciphertext);
   }
   int keys = solver.keys_checked();
   if (result != m_plaintext)
   {
      m_plaintext = result;
      m_keys_checked = solver.keys_checked();
      m_key = solver.best_key();
      char tmp[100];
      sprintf(tmp, "%d", solver.score());
      m_score = tmp;
      SendMessage(M_SOLVERCALLBACK, FALSE, 0);
   }
   else if (keys - m_keys_checked > 100000)
   {
      m_keys_checked = solver.keys_checked();
      SendMessage(M_SOLVERCALLBACK1, FALSE, 0);
   }
}

void CScbDlg::OnSolve()
{
   // TODO: Add your control notification handler code here
   struct _stat statbuf;
   if (_stat(m_ngraph_file, &statbuf) < 0)
   {
      char msg[132];
      sprintf(msg, "NGraph file %s does not exist", (const char*)m_ngraph_file);
      AfxMessageBox(msg);
      return;
   }
   m_plaintext = "";
   m_key = "";
   m_keys_checked = 0;
   UpdateData(FALSE);
   solver_state = SOLVING;
   DoDisplay();

   GetDlgItem(ID_SOLVE)->EnableWindow(FALSE);
   {
      CWaitCursor wait;
      solver.set_scoring_file(m_ngraph_file);
   }

   const char* ct = static_cast<const char*>(m_ciphertext);
   solver.set_cipher_text(ct);
   if (m_fixed.number_fixed() > 0)
   {
      if (m_fixed.check() < 0)
      {
         char msg[132];
         sprintf(msg, "m_fixed is bad : %d", m_fixed.check());
         AfxMessageBox(msg);
         solver.set_fixed(0);
      }
      else
      {
         solver.set_fixed(&m_fixed);
      }
   }
   else solver.set_fixed(0);
   solver.set_verbose();
   solver.set_solver_callback(this);
   solver_thread = AfxBeginThread(solve, &solver);
}

void CScbDlg::OnStop()
{
   // TODO: Add your control notification handler code here

   if (solver_thread)
   {
      solver_thread->SuspendThread();
      m_plaintext = solver.best_result(0);
      if (solver.spaces_scored())
      {
         m_plaintext = remove_spaces(m_plaintext);
      }
      if (preserve_original_format())
      {
         m_plaintext = Mono_Solver::reformat(m_plaintext, m_ciphertext);
      }
   }

   solver_state = STOPPED;
   DoDisplay();
   UpdateData(FALSE);
   if (solver_thread)
   {
      solver.set_terminate();
      solver_thread->ResumeThread();
   }
}
static void adjust_cipher_text(char* ciphertext)
{
   return;
   int* spaces = new int [ strlen(ciphertext) + 1 ];
   char* c = ciphertext;
   // Find spaces
   int sc = 0;
   int index = 0;
   while (c && *c)
   {
      if (*c == ' ')
      {
         spaces[sc] = index;
         sc++;
      }
      index++;
      c++;
   }
   if (sc > 0)
   {
      index = 0;
      while (index == 0 || index == strlen(ciphertext) - 1)
      {
         int si = rand() % sc;
         index = spaces[si];
      }
      int inc = rand() % 2;
      if (inc == 0) inc = -1;
      char tmp = ciphertext[index + inc];
      ciphertext[index + inc] = ' ';
      ciphertext[index] = tmp;
   }
}

static int Callback1_count = 0;

LRESULT CScbDlg::OnSolverCallback(WPARAM, LPARAM)
{
   // TODO: Add your control notification handler code here
   char tmp[30];
   sprintf(tmp, "\r\n%s", "abcdefghijklmnopqrstuvwxyz");
   m_key = m_key + tmp;

   UpdateData(FALSE);
   Callback1_count=0;
   return 0;
}
LRESULT CScbDlg::OnSolverCallback1(WPARAM, LPARAM)
{
   // TODO: Add your control notification handler code here
   Callback1_count++;
   if (Callback1_count > 5)
   {
      static char* tmp1 = 0;
      if (!tmp1) tmp1 = new char [ strlen(static_cast<const char*>(m_ciphertext)) + 1];
      strcpy(tmp1, static_cast<const char*>(m_ciphertext));
      adjust_cipher_text(tmp1);
      m_ciphertext = tmp1;
      const char* ct = static_cast<const char*>(m_ciphertext);
      solver.set_cipher_text(ct);
      Callback1_count = 0;
   }
   UpdateData(FALSE);
   return 0;
}

void CScbDlg::OnHelpAboutsecretcodebreaker()
{
   // TODO: Add your command handler code here
   CAboutDlg aboutDialog;
   aboutDialog.DoModal();
}


static char* remove_punctuation_from_string(const char* str)
{
   static char* buf = 0;
   static int buf_len = 0;
   if (buf_len < strlen(str) + 1)
   {
      delete [] buf; // harmless if buf == 0
      buf = new char [ strlen(str) + 1 ];
      buf_len = strlen(str) + 1;
   }
   const char* c = str;
   char* d = buf;
   while (c && *c)
   {
      if (isalpha(*c))
      {
         *d++ = *c;
      }
      c++;
   }
   *d = '\0';
   return buf;
}

void CScbDlg::OnFileOpen()
{
   // TODO: Add your command handler code here
   int file_drive = AfxGetApp()->GetProfileInt("Default File Locations", "File Drive", -1);
   CString file_directory = AfxGetApp()->GetProfileString("Default File Locations", "File Directory");
   _chdrive(file_drive);
   _chdir(file_directory);
   CFileDialog fileOpenDialog(TRUE);
   if (fileOpenDialog.DoModal() == IDOK)
   {
      CString filename = fileOpenDialog.GetPathName();
      std::fstream file(filename, std::ios::in);
      char buf[133];
      CString ciphertext = "";
      while (file.getline(buf, 132))
      {
         ciphertext += buf;
      }
      m_ciphertext = ciphertext;
      if (m_remove_punctuation)
      {
         m_ciphertext = remove_punctuation_from_string(ciphertext);
      }
      m_cipher_filename = filename;
      sprintf(buf, "%6.4f", Mono_Solver::phi(ciphertext));
      m_phi = buf;
      m_plaintext = _T("");
      m_key = _T("");
      m_keys_checked = 0;
      m_ciphertext_changed = FALSE;
      UpdateData(FALSE);
      solver_state = STOPPED;
      AfxGetApp()->WriteProfileInt("Default File Locations", "File Drive", _getdrive());
      char dirbuf[_MAX_PATH];
      _getcwd(dirbuf, _MAX_PATH - 1);
      AfxGetApp()->WriteProfileString("Default File Locations", "File Directory", dirbuf);
   }
   DoDisplay();
}

void CScbDlg::OnSetNgraphFile()
{
   // TODO: Add your command handler code here
   int ngraph_drive = AfxGetApp()->GetProfileInt("Default File Locations", "Ngraph Drive", -1);
   CString ngraph_directory = AfxGetApp()->GetProfileString("Default File Locations", "Ngraph Directory");
   _chdrive(ngraph_drive);
   _chdir(ngraph_directory);
   CFileDialog fileOpenDialog(TRUE);
   if (fileOpenDialog.DoModal() == IDOK)
   {
      m_ngraph_file = fileOpenDialog.GetPathName();
      UpdateData(FALSE);
      AfxGetApp()->WriteProfileInt("Default File Locations", "Ngraph Drive", _getdrive());
      char dirbuf[_MAX_PATH];
      _getcwd(dirbuf, _MAX_PATH - 1);
      AfxGetApp()->WriteProfileString("Default File Locations", "NGraph Directory", dirbuf);
   }
   DoDisplay();
}

void CScbDlg::OnInterrupt()
{
   // TODO: Add your control notification handler code here
   solver_thread->SuspendThread();
   m_plaintext = solver.best_result(0);
   if (solver.spaces_scored())
   {
      m_plaintext = remove_spaces(m_plaintext);
   }
   if (preserve_original_format())
   {
      m_plaintext = Mono_Solver::reformat(m_plaintext, m_ciphertext);
   }
   UpdateData(FALSE);
   solver_state = INTERRUPTED;
   DoDisplay();
}

void CScbDlg::OnResume()
{
   // TODO: Add your control notification handler code here
   solver_thread->ResumeThread();
   solver_state = SOLVING;
   DoDisplay();
}

void CScbDlg::OnUpdateSetNgraphFile(CCmdUI* pCmdUI)
{
   if (solver_state == STOPPED || solver_state == CLEARED)
   {
      pCmdUI->Enable(TRUE);
   }
   else
   {
      pCmdUI->Enable(FALSE);
   }
}

void CScbDlg::OnUpdateFileOpen(CCmdUI* pCmdUI)
{
   // TODO: Add your command update UI handler code here
   if (solver_state == STOPPED || solver_state == CLEARED)
   {
      pCmdUI->Enable(TRUE);
   }
   else
   {
      pCmdUI->Enable(FALSE);
   }
}

void CScbDlg::OnFileExit()
{
   // TODO: Add your command handler code here
   if (solver_thread)
   {
      solver_thread->SuspendThread();
      solver.set_terminate();
      solver_thread->ResumeThread();
   }
   CDialog::OnOK();
}

void CScbDlg::OnToolsCreatengraphfile()
{
   // TODO: Add your command handler code here
   CNgraphCreatorDialog dlg;
   if (dlg.DoModal() == IDOK)
   {
   }
}

void CScbDlg::OnViewOutputFormatPreserve()
{
   // TODO: Add your command handler code here
   if (m_preserve_original_format)
   {
      m_preserve_original_format = FALSE;
   }
   else
   {
      m_preserve_original_format = TRUE;
   }
}

void CScbDlg::OnUpdateViewOutputFormatPreserve(CCmdUI* pCmdUI)
{
   // TODO: Add your command update UI handler code here
   if (solver_state == STOPPED || solver_state == CLEARED)
   {
      pCmdUI->Enable(TRUE);
   }
   else
   {
      pCmdUI->Enable(FALSE);
   }
   if (m_preserve_original_format)
   {
      pCmdUI->SetCheck(1);
   }
   else
   {
      pCmdUI->SetCheck(0);
   }
}

void CScbDlg::OnUpdateCipher()
{
   // TODO: If this is a RICHEDIT control, the control will not
   // send this notification unless you override the CDialog::OnInitDialog()
   // function to send the EM_SETEVENTMASK message to the control
   // with the ENM_UPDATE flag ORed into the lParam mask.

   // TODO: Add your control notification handler code here
   //if (solver_state != STOPPED && solver_state != CLEARED) return;
   CString old_ciphertext = m_ciphertext;
   UpdateData(TRUE);
   if (old_ciphertext != m_ciphertext)
   {
      if (!m_ciphertext_changed)
      {
         m_ciphertext_changed = TRUE;
         m_cipher_filename = m_cipher_filename + _T("(changed)");
      }
      char buf[10];
      sprintf(buf, "%6.4f", Mono_Solver::phi(m_ciphertext));
      m_phi = buf;
      UpdateData(FALSE);
      if (solver.is_cipher_text_different(m_ciphertext))
      {
         OnStop();
      }
   }
   DoDisplay();
}

void CScbDlg::OnClear()
{
   // TODO: Add your control notification handler code here
   OnStop();
   m_ciphertext = _T("");
   m_ciphertext_changed = FALSE;
   m_cipher_filename = _T("");
   m_phi = _T("");
   m_key = _T("");
   m_keys_checked = 0;
   m_preserve_original_format = TRUE;
   m_remove_punctuation = FALSE;
   m_plaintext = _T("");
   m_fixed.clear();
   m_crib.clear();
   UpdateData(FALSE);
   solver_state = CLEARED;
   DoDisplay();
}

void CScbDlg::DoDisplay()
{
   switch (solver_state)
   {
   case CLEARED:
      GetDlgItem(ID_SOLVE)->EnableWindow(FALSE);
      GetDlgItem(ID_STOP)->EnableWindow(FALSE);
      GetDlgItem(ID_RESUME)->EnableWindow(FALSE);
      GetDlgItem(ID_INTERRUPT)->EnableWindow(FALSE);
      GetDlgItem(ID_CLEAR)->EnableWindow(FALSE);
      GetDlgItem(IDC_CIPHER)->EnableWindow(TRUE);
      break;
   case STOPPED:
      GetDlgItem(ID_SOLVE)->EnableWindow(TRUE);
      GetDlgItem(ID_STOP)->EnableWindow(FALSE);
      GetDlgItem(ID_RESUME)->EnableWindow(FALSE);
      GetDlgItem(ID_INTERRUPT)->EnableWindow(FALSE);
      GetDlgItem(ID_CLEAR)->EnableWindow(TRUE);
      GetDlgItem(IDC_CIPHER)->EnableWindow(TRUE);
      break;
   case SOLVING:
      GetDlgItem(ID_SOLVE)->EnableWindow(FALSE);
      GetDlgItem(ID_STOP)->EnableWindow(TRUE);
      GetDlgItem(ID_RESUME)->EnableWindow(FALSE);
      GetDlgItem(ID_INTERRUPT)->EnableWindow(TRUE);
      GetDlgItem(ID_CLEAR)->EnableWindow(TRUE);
      GetDlgItem(IDC_CIPHER)->EnableWindow(FALSE);
      break;
   case INTERRUPTED:
      GetDlgItem(ID_SOLVE)->EnableWindow(FALSE);
      GetDlgItem(ID_STOP)->EnableWindow(TRUE);
      GetDlgItem(ID_RESUME)->EnableWindow(TRUE);
      GetDlgItem(ID_INTERRUPT)->EnableWindow(FALSE);
      GetDlgItem(ID_CLEAR)->EnableWindow(TRUE);
      GetDlgItem(IDC_CIPHER)->EnableWindow(TRUE);
      break;
   }
}

void CScbDlg::OnFileSave()
{
   // TODO: Add your command handler code here
   int file_drive = AfxGetApp()->GetProfileInt("Default File Locations", "File Drive", -1);
   CString file_directory = AfxGetApp()->GetProfileString("Default File Locations", "File Directory");
   _chdrive(file_drive);
   _chdir(file_directory);
   CFileDialog fileOpenDialog(FALSE);
   if (fileOpenDialog.DoModal() == IDOK)
   {
      CString filename = fileOpenDialog.GetPathName();
      std::fstream file(filename, std::ios::out);
      file << "Secret Code Breaker Monoalphabetic Substitution Solver" << std::endl;
      file << "Copyright (C) 2001 Chris Card  All rights reserved." << std::endl;
      file << "------------------------------------------------------" << std::endl;
      file << "Ciphertext:" << std::endl;
      file << m_ciphertext << std::endl;
      file << "Plaintext:" << std::endl;
      file << m_plaintext << std::endl;
      file << "Key:" << std::endl;
      file << m_key << std::endl;
      file.close();

      AfxGetApp()->WriteProfileInt("Default File Locations", "File Drive", _getdrive());
      char dirbuf[_MAX_PATH];
      _getcwd(dirbuf, _MAX_PATH - 1);
      AfxGetApp()->WriteProfileString("Default File Locations", "File Directory", dirbuf);
   }
   DoDisplay();
}

void CScbDlg::OnUpdateFileSave(CCmdUI* pCmdUI)
{
   // TODO: Add your command update UI handler code here
   if ((solver_state == STOPPED && m_plaintext != "") || solver_state == INTERRUPTED)
   {
      pCmdUI->Enable(TRUE);
   }
   else
   {
      pCmdUI->Enable(FALSE);
   }

}

void CScbDlg::OnToolsFixkey()
{
   // TODO: Add your command handler code here
   CFixKeyDlg dlg;

   dlg.set_fixed(m_fixed);
   if (dlg.DoModal() == IDOK)
   {
      m_fixed = dlg.get_fixed();
   }
}

void CScbDlg::OnUpdateToolsFixkey(CCmdUI* pCmdUI)
{
   // TODO: Add your command update UI handler code here
   if (solver_state != SOLVING && solver_state != INTERRUPTED)
   {
      pCmdUI->Enable(TRUE);
   }
   else
   {
      pCmdUI->Enable(FALSE);
   }

}

void CScbDlg::OnHelpInstructions()
{
   // TODO: Add your command handler code here
   // add code to view instructions file
   char instructions_file[MAX_PATH];
   sprintf(instructions_file, "%s\\instruct.txt", m_working_directory);
   struct _stat statbuf;
   if (_stat(instructions_file, &statbuf) < 0)
   {
      char msg[132];
      sprintf(msg, "instructions file %s does not exist", (const char*)instructions_file);
      AfxMessageBox(msg);
      return;
   }
   char notepad_cmd[MAX_PATH];
   GetWindowsDirectory(notepad_cmd, MAX_PATH - 1);
   strcat(notepad_cmd, "\\notepad.exe");
   _spawnlp(_P_NOWAIT, notepad_cmd, "notepad", instructions_file, NULL);
}

void CScbDlg::OnViewRemovepunctuation()
{
   // TODO: Add your command handler code here
   if (m_remove_punctuation)
   {
      m_remove_punctuation = FALSE;
   }
   else
   {
      m_remove_punctuation = TRUE;
      if (m_ciphertext != "")
      {
         m_ciphertext = remove_punctuation_from_string(m_ciphertext);
         UpdateData(FALSE);
      }
   }
}

void CScbDlg::OnUpdateViewRemovepunctuation(CCmdUI* pCmdUI)
{
   // TODO: Add your command update UI handler code here
   if (solver_state == STOPPED || solver_state == CLEARED)
   {
      pCmdUI->Enable(TRUE);
   }
   else
   {
      pCmdUI->Enable(FALSE);
   }
   if (m_remove_punctuation)
   {
      pCmdUI->SetCheck(1);
   }
   else
   {
      pCmdUI->SetCheck(0);
   }
}

void CScbDlg::OnHelpTheory()
{
   // TODO: Add your command handler code here
   char theory_file[MAX_PATH];
   sprintf(theory_file, "%s\\theory.txt", m_working_directory);
   struct _stat statbuf;
   if (_stat(theory_file, &statbuf) < 0)
   {
      char msg[132];
      sprintf(msg, "theory file %s does not exist", (const char*)theory_file);
      AfxMessageBox(msg);
      return;
   }
   char notepad_cmd[MAX_PATH];
   GetWindowsDirectory(notepad_cmd, MAX_PATH - 1);
   strcat(notepad_cmd, "\\notepad.exe");
   _spawnlp(_P_NOWAIT, notepad_cmd, "notepad", theory_file, NULL);

}

void CScbDlg::OnToolsCribbing()
{
   // TODO: Add your command handler code here
   const char* ct = static_cast<const char*>(m_ciphertext);
   if (m_crib.ct_has_changed(ct))
   {
      m_crib = Crib(ct, "");
   }
   CribDlg dlg(m_crib, m_preserve_original_format);

   if (dlg.DoModal() == IDOK)
   {
      m_crib = dlg.get_crib();
      m_fixed = m_crib.get_fixed_key();
   }
}

void CScbDlg::OnUpdateToolsCribbing(CCmdUI* pCmdUI)
{
   // TODO: Add your command update UI handler code here
   if (solver_state == STOPPED)
   {
      pCmdUI->Enable(TRUE);
   }
   else
   {
      pCmdUI->Enable(FALSE);
   }
}
