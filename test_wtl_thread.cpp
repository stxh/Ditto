// Test file to verify WTL thread compilation
#include "stdafx.h"
#include "wtl_thread.h"

// Test class to verify WTL thread functionality
class CTestThread : public WTL::CWinThread
{

public:
    CTestThread() {}
    virtual ~CTestThread() {}

    virtual BOOL InitInstance()
    {
        // Test initialization
        return TRUE;
    }

    virtual int Run()
    {
        // Test message pump
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

    virtual int ExitInstance()
    {
        // Test cleanup
        return 0;
    }
};


// Simple test function
bool TestWTLThread()
{
    // Test thread creation
    CTestThread* pThread = new CTestThread();
    if (pThread == NULL)
        return false;

    pThread->m_bAutoDelete = FALSE; // We'll delete it manually for testing

    // Test thread creation
    if (!pThread->CreateThread())
    {
        delete pThread;
        return false;
    }

    // Test thread message posting
    if (!pThread->PostThreadMessage(WM_USER, 0, 0))
    {
        delete pThread;
        return false;
    }

    // Test thread priority
    if (!pThread->SetThreadPriority(THREAD_PRIORITY_NORMAL))
    {
        delete pThread;
        return false;
    }

    // Test thread cleanup
    pThread->PostThreadMessage(WM_QUIT, 0, 0);
    
    // Wait for thread to finish
    ::WaitForSingleObject(pThread->m_hThread, 5000);
    
    delete pThread;
    return true;
}