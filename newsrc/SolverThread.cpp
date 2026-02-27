// SolverThread.cpp : implementation file
//

#include "stdafx.h"
#include "scb.h"
#include "SolverThread.h"
#include "mono_solver.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// SolverThread

IMPLEMENT_DYNCREATE(SolverThread, CWinThread)

SolverThread::SolverThread() : _solver(0)
{
}

SolverThread::~SolverThread()
{
}

BOOL SolverThread::InitInstance()
{
   // TODO:  perform and per-thread initialization here
   return TRUE;
}

int SolverThread::ExitInstance()
{
   // TODO:  perform any per-thread cleanup here
   return CWinThread::ExitInstance();
}

BEGIN_MESSAGE_MAP(SolverThread, CWinThread)
   //{{AFX_MSG_MAP(SolverThread)
   // NOTE - the ClassWizard will add and remove mapping macros here.
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// SolverThread message handlers
