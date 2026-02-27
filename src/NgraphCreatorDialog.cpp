// NgraphCreatorDialog.cpp : implementation file
//

#include "stdafx.h"
#include "scb.h"
#include "NgraphCreatorDialog.h"
#include "ngraph.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CNgraphCreatorDialog dialog


CNgraphCreatorDialog::CNgraphCreatorDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CNgraphCreatorDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CNgraphCreatorDialog)
	m_sample_file = _T("");
	m_ngraph_file = _T("");
	m_n = 0;
	m_progress_text = _T("");
	m_spaces = 0;
	//}}AFX_DATA_INIT
}


void CNgraphCreatorDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CNgraphCreatorDialog)
	DDX_Control(pDX, IDC_NGRAPH_PROGRESS, m_ngraph_progress);
	DDX_Text(pDX, IDC_SAMPLE_FILE, m_sample_file);
	DDX_Text(pDX, IDC_NGRAPH_FILE, m_ngraph_file);
	DDX_Text(pDX, IDC_NGRAPH_LEN, m_n);
	DDV_MinMaxInt(pDX, m_n, 3, 8);
	DDX_Text(pDX, IDC_PROGRESS_TEXT, m_progress_text);
	//}}AFX_DATA_MAP
}

#define M_CREATORCALLBACK 9997
BEGIN_MESSAGE_MAP(CNgraphCreatorDialog, CDialog)
	//{{AFX_MSG_MAP(CNgraphCreatorDialog)
	ON_BN_CLICKED(IDC_BROWSE_NGRAPH_FILE, OnBrowseNgraphFile)
	ON_BN_CLICKED(IDC_BROWSE_SAMPLE_FILE, OnBrowseSampleFile)
	ON_BN_CLICKED(ID_CREATE_NGRAPH_FILE, OnCreateNgraphFile)
	ON_MESSAGE(M_CREATORCALLBACK, OnCreatorCallback)
	ON_BN_CLICKED(IDDONE, OnDone)
	ON_BN_CLICKED(IDC_SPACES, OnSpaces)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CNgraphCreatorDialog message handlers
LRESULT CNgraphCreatorDialog::OnCreatorCallback(WPARAM, LPARAM) 
{
	// TODO: Add your control notification handler code here
    if (m_creator->is_done())
	{
       GetDlgItem(IDDONE)->EnableWindow(TRUE);
	   GetDlgItem(ID_CREATE_NGRAPH_FILE)->EnableWindow(TRUE);
	   m_progress_text = "Reading file, 100% done";
	   UpdateData(FALSE);
	}
	else
	{
	   double pct = ((double)m_progress * 100.0) / (double)m_sample_size;
	   m_ngraph_progress.SetPos((int)pct);
	   char buf[132];
	   sprintf(buf, "Reading file, %4.2f%s done", pct, "%");
	   m_progress_text = buf;
	   UpdateData(FALSE);
	}
  return 0;
}

void CNgraphCreatorDialog::OnBrowseNgraphFile() 
{
	// TODO: Add your control notification handler code here
	CFileDialog dlg(TRUE);
	if (dlg.DoModal() == IDOK)
	{
		m_ngraph_file = dlg.GetPathName();
		UpdateData(FALSE);
	}
}

void CNgraphCreatorDialog::OnBrowseSampleFile() 
{
	// TODO: Add your control notification handler code here
	CFileDialog dlg(TRUE);
	if (dlg.DoModal() == IDOK)
	{
		m_sample_file = dlg.GetPathName();
		UpdateData(FALSE);
	}	
}

void CNgraphCreatorDialog::OnOK() 
{
	// TODO: Add extra validation here
	
	CDialog::OnOK();
}

static unsigned int create_ngraph_file(void* param)
{
	Ngraph_File_Creator* ngraph_file_creator = (Ngraph_File_Creator*)param;
	ngraph_file_creator->go();
	return 0;
}

void ProcessProgress(void* me, const Ngraph_File_Creator& creator)
{
	CNgraphCreatorDialog* ptr = (CNgraphCreatorDialog*)me;
	ptr->m_progress = creator.get_progress();
    ptr->SendMessage(M_CREATORCALLBACK, FALSE, 0);
}

void CNgraphCreatorDialog::OnCreateNgraphFile() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	if (m_sample_file != "" && m_ngraph_file != "" && m_n > 0)
	{
		CFile f(m_sample_file, CFile::modeRead);
	   CFileStatus stat;
       f.GetStatus(stat);
	   m_sample_size = stat.m_size;
       m_creator = new Ngraph_File_Creator(m_sample_file, m_ngraph_file, m_n, m_spaces);
	   m_creator->set_user_callback(this, ProcessProgress);
	   m_ngraph_progress.SetPos(0);
	   m_ngraph_progress.SetRange(0, 100);
	   m_ngraph_progress.SetStep(1);
	   m_previous_progress = 0L;
	   m_progress = 0L;
	   GetDlgItem(IDDONE)->EnableWindow(FALSE);
	   GetDlgItem(ID_CREATE_NGRAPH_FILE)->EnableWindow(FALSE);
	   m_creator_thread = AfxBeginThread(create_ngraph_file, m_creator);

	}
	else
	{
		AfxMessageBox("Please enter file names and ngraph length");
	}
}

void CNgraphCreatorDialog::OnDone() 
{
	// TODO: Add your control notification handler code here
	CDialog::OnOK();
}

void CNgraphCreatorDialog::OnSpaces() 
{
	// TODO: Add your control notification handler code here
	if (m_spaces)
	{
		m_spaces = 0;
	}
	else
	{
		m_spaces = 1;
	}
}
