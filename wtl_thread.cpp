// WTL Thread Implementation
// This file provides WTL-compatible thread classes to replace MFC's CWinThread

#include "stdafx.h"
#include "wtl_thread.h"
#include "Misc.h"

namespace WTL {

CWinThread::CWinThread() : m_hThread(NULL), m_nThreadID(0), 
    m_bAutoDelete(TRUE), m_bRunning(FALSE), m_bDone(FALSE)
{
    CommonConstruct();
}

CWinThread::~CWinThread()
{
    CommonDestruct();
}

void CWinThread::CommonConstruct()
{
    InitializeCriticalSection(&m_cs);
    m_hThread = NULL;
    m_nThreadID = 0;
    m_bAutoDelete = TRUE;
    m_bRunning = FALSE;
    m_bDone = FALSE;
}

void CWinThread::CommonDestruct()
{
    if (m_hThread != NULL)
    {
        CloseHandle(m_hThread);
        m_hThread = NULL;
    }
    DeleteCriticalSection(&m_cs);
}

BOOL CWinThread::InitInstance()
{
    return TRUE;
}

int CWinThread::ExitInstance()
{
    return 0;
}

int CWinThread::Run()
{
    // Default message pump implementation
    MSG msg;
    while (::GetMessage(&msg, NULL, 0, 0))
    {
        if (!PreTranslateMessage(&msg))
        {
            ::TranslateMessage(&msg);
            ::DispatchMessage(&msg);
        }
    }
    return (int)msg.wParam;
}

BOOL CWinThread::PreTranslateMessage(MSG* pMsg)
{
    // Default implementation - can be overridden
    return FALSE;
}

BOOL CWinThread::PumpMessage()
{
    MSG msg;
    if (::GetMessage(&msg, NULL, 0, 0))
    {
        if (!PreTranslateMessage(&msg))
        {
            ::TranslateMessage(&msg);
            ::DispatchMessage(&msg);
        }
        return TRUE;
    }
    return FALSE;
}

BOOL CWinThread::IsIdleMessage(MSG* pMsg)
{
    // Default implementation - can be overridden
    return FALSE;
}

BOOL CWinThread::CreateThread(DWORD dwCreateFlags, UINT nStackSize,
    LPSECURITY_ATTRIBUTES lpSecurityAttrs)
{
    m_hThread = (HANDLE)_beginthreadex(lpSecurityAttrs, nStackSize, 
        ThreadProc, this, dwCreateFlags, (UINT*)&m_nThreadID);
    
    if (m_hThread == NULL)
        return FALSE;
    
    m_bRunning = TRUE;
    return TRUE;
}

BOOL CWinThread::PostThreadMessage(UINT message, WPARAM wParam, LPARAM lParam)
{
    if (m_hThread == NULL || m_nThreadID == 0)
        return FALSE;
    
    return ::PostThreadMessage(m_nThreadID, message, wParam, lParam);
}

DWORD CWinThread::SuspendThread()
{
    if (m_hThread == NULL)
        return (DWORD)-1;
    
    return ::SuspendThread(m_hThread);
}

DWORD CWinThread::ResumeThread()
{
    if (m_hThread == NULL)
        return (DWORD)-1;
    
    return ::ResumeThread(m_hThread);
}

BOOL CWinThread::SetThreadPriority(int nPriority)
{
    if (m_hThread == NULL)
        return FALSE;
    
    return ::SetThreadPriority(m_hThread, nPriority);
}

int CWinThread::GetThreadPriority()
{
    if (m_hThread == NULL)
        return THREAD_PRIORITY_ERROR_RETURN;
    
    return ::GetThreadPriority(m_hThread);
}

DWORD CWinThread::Resume()
{
    return ResumeThread();
}

UINT __stdcall CWinThread::ThreadProc(LPVOID pParam)
{
    CWinThread* pThread = (CWinThread*)pParam;
    if (pThread == NULL)
        return 0;
    
    int nResult = 0;
    
    try
    {
        // Call InitInstance
        if (pThread->InitInstance())
        {
            // Call Run
            nResult = pThread->Run();
        }
        
        // Call ExitInstance
        pThread->ExitInstance();
    }
    catch (...)
    {
        Log(_T("Exception in CWinThread::ThreadProc"));
    }
    
    // Mark thread as done
    EnterCriticalSection(&pThread->m_cs);
    pThread->m_bRunning = FALSE;
    pThread->m_bDone = TRUE;
    LeaveCriticalSection(&pThread->m_cs);
    
    // Auto-delete if requested
    if (pThread->m_bAutoDelete)
    {
        delete pThread;
    }
    
    return nResult;
}

// CSimpleThread implementation
CSimpleThread::CSimpleThread()
{
}

CSimpleThread::~CSimpleThread()
{
}

int CSimpleThread::Run()
{
    ThreadProc();
    return 0;
}

} // namespace WTL