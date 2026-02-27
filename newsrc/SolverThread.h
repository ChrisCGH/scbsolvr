#if !defined(AFX_SOLVERTHREAD_H__3F969C43_0A2A_11D5_B48F_B86480000000__INCLUDED_)
#define AFX_SOLVERTHREAD_H__3F969C43_0A2A_11D5_B48F_B86480000000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// SolverThread.h : header file
//



/////////////////////////////////////////////////////////////////////////////
// SolverThread thread

class SolverThread : public CWinThread
{
      DECLARE_DYNCREATE(SolverThread)
   protected:
      SolverThread();           // protected constructor used by dynamic creation

// Attributes
   public:
      SetSolver(Mono_Solver& solver)
      {
         _solver = &solver;
      }
// Operations
   public:

// Overrides
      // ClassWizard generated virtual function overrides
      //{{AFX_VIRTUAL(SolverThread)
   public:
      virtual BOOL InitInstance();
      virtual int ExitInstance();
      //}}AFX_VIRTUAL
   private:
      Mono_Solver* _solver;
// Implementation
   protected:
      virtual ~SolverThread();

      // Generated message map functions
      //{{AFX_MSG(SolverThread)
      // NOTE - the ClassWizard will add and remove member functions here.
      //}}AFX_MSG

      DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SOLVERTHREAD_H__3F969C43_0A2A_11D5_B48F_B86480000000__INCLUDED_)
