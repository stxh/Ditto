// WTL Thread Implementation
// This file provides WTL-compatible thread classes to replace MFC's CWinThread

#ifndef __WTL_THREAD_H__
#define __WTL_THREAD_H__

#pragma once

#include <atlbase.h>
#include <atlwin.h>
#include <atlstr.h>
#include <process.h>

namespace WTL {

// WTL Thread class - replacement for MFC's CWinThread:
// a worker thread that optionally runs a message pump.
class CWinThread
{
public:

    // Construction
    CWinThread();
    virtual ~CWinThread();

    // Attributes
    HANDLE m_hThread;           // handle to the thread
    DWORD m_nThreadID;          // thread ID
    BOOL m_bAutoDelete;         // whether to delete on thread termination

    // Operations
    virtual BOOL InitInstance();
    virtual int ExitInstance();
    virtual int Run();
    virtual BOOL PreTranslateMessage(MSG* pMsg);
    virtual BOOL PumpMessage();
    virtual BOOL IsIdleMessage(MSG* pMsg);

    // Thread creation and management
    BOOL CreateThread(DWORD dwCreateFlags = 0, UINT nStackSize = 0,
        LPSECURITY_ATTRIBUTES lpSecurityAttrs = NULL);
    BOOL PostThreadMessage(UINT message, WPARAM wParam = 0, LPARAM lParam = 0);
    DWORD SuspendThread();
    DWORD ResumeThread();
    BOOL SetThreadPriority(int nPriority);
    int GetThreadPriority();
    DWORD Resume();

protected:
    // Internal thread function
    static UINT __stdcall ThreadProc(LPVOID pParam);
    
    // Member variables
    BOOL m_bRunning;
    BOOL m_bDone;
    CRITICAL_SECTION m_cs;

    // Internal operations
    void CommonConstruct();
    void CommonDestruct();
};

// Simple thread class for basic threading needs
class CSimpleThread : public CWinThread
{
public:
    CSimpleThread();
    virtual ~CSimpleThread();

    // Override this to implement thread logic
    virtual void ThreadProc() = 0;

protected:
    virtual int Run();
};

} // namespace WTL

// Global using declarations for easier migration
using WTL::CWinThread;
using WTL::CSimpleThread;

#endif // __WTL_THREAD_H__