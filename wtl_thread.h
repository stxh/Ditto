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

// Forward declaration
class CWinThread;

// Thread function prototype
typedef UINT (AFXAPI *AFX_THREADPROC)(LPVOID);

// WTL-compatible runtime class structure (simplified)
struct CRuntimeClass
{
    LPCSTR m_lpszClassName;
    int m_nObjectSize;
    UINT m_wSchema;
    CObject* (PASCAL* m_pfnCreateObject)();
    CRuntimeClass* m_pBaseClass;
    CRuntimeClass* m_pNextClass;
    
    static CRuntimeClass* PASCAL Load();
    CObject* CreateObject();
};

// Simplified dynamic creation macros for WTL compatibility
#define DECLARE_DYNAMIC(class_name) \
public: \
    static CRuntimeClass class##class_name; \
    virtual CRuntimeClass* GetRuntimeClass() const;

#define DECLARE_DYNCREATE(class_name) \
    DECLARE_DYNAMIC(class_name) \
    static CObject* PASCAL CreateObject();

#define IMPLEMENT_DYNAMIC(class_name, base_class) \
    CRuntimeClass class_name::class##class_name = { \
        #class_name, sizeof(class_name), 0xFFFF, \
        class_name::CreateObject, RUNTIME_CLASS(base_class), NULL }; \
    CRuntimeClass* class_name::GetRuntimeClass() const \
        { return &class_name::class##class_name; }

#define IMPLEMENT_DYNCREATE(class_name, base_class) \
    CObject* PASCAL class_name::CreateObject() \
        { return new class_name; } \
    IMPLEMENT_DYNAMIC(class_name, base_class)

#define RUNTIME_CLASS(class_name) (&class_name::class##class_name)

// Base class for dynamic creation (simplified)
class CObject
{
public:
    virtual ~CObject() {}
    virtual CRuntimeClass* GetRuntimeClass() const { return NULL; }
    static CRuntimeClass classCObject;
    static CObject* PASCAL CreateObject();
};

// WTL Thread class - replacement for MFC's CWinThread
class CWinThread : public CObject
{
public:
    DECLARE_DYNAMIC(CWinThread)

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

    // Static thread creation
    static CWinThread* BeginThread(AFX_THREADPROC pfnThreadProc, LPVOID pParam,
        int nPriority = THREAD_PRIORITY_NORMAL, UINT nStackSize = 0,
        DWORD dwCreateFlags = 0, LPSECURITY_ATTRIBUTES lpSecurityAttrs = NULL);

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