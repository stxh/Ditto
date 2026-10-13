// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "CP_Main.h"
#include "MainFrm.h"
#include "Misc.h"
#include "CopyProperties.h"
#include "InternetUpdate.h"
#include ".\mainfrm.h"
#include <atlctrlx.h>
#include "tinyxml\tinyxml.h"
#include "Path.h"
#include "DittoCopyBuffer.h"
#include "HotKeys.h"
#include "GlobalClips.h"
#include "OptionsSheet.h"
#include "DeleteClipData.h"
#include "DatabaseUtilities.h"

static UINT indicators[] =
{
    ID_SEPARATOR,
    ID_INDICATOR_CAPS, ID_INDICATOR_NUM, ID_INDICATOR_SCRL,
};

/////////////////////////////////////////////////////////////////////////////
// CMainFrame construction/destruction

CMainFrame::CMainFrame()
{
    m_keyStateModifiers = 0;
    m_startKeyStateTime = 0;
    m_bMovedSelectionMoveKeyState = false;
    m_keyModifiersTimerCount = 0;
	m_pGlobalClips = NULL;
	m_pOptions = NULL;
	m_pDeleteClips = NULL;
	m_doubleClickGroupId = -1;
	m_doubleClickGroupStartTime = 0;
}

CMainFrame::~CMainFrame()
{
    CGetSetOptions::SetMainHWND(0);
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if(CFrameWindowImpl<CMainFrame>::OnCreate(lpCreateStruct) == -1)
    {
        return -1;
    }

    CMessageLoop* pLoop = _Module.GetMessageLoop();
    ATLASSERT(pLoop != NULL);
    pLoop->AddMessageFilter(this);
    pLoop->AddIdleHandler(this);

    m_PowerManager.Start(m_hWnd);

    CRect rcScreen = DefaultMonitorRect();
    CPoint cpCenter = rcScreen.CenterPoint();
    MoveWindow(cpCenter.x, cpCenter.x, 1, 1);

    m_startupScreenWidth = GetScreenWidth();
    m_startupScreenHeight = GetScreenHeight();

    SetWindowText(_T(""));

    Log(_T("Setting polling timer to track focus"));
    SetTimer(ACTIVE_WINDOW_TIMER, CGetSetOptions::FocusWndTimerTimeout(), 0);
    SetTimer(READ_RANDOM_DB_FILE, CGetSetOptions::ReadRandomFileInterval() * 1000, 0);
    SetWindowText(_T("Ditto"));
    
    m_trayIcon.Create(this, IDR_MENU, _T("Ditto"), CTrayNotifyIcon::LoadIcon(IDR_MAINFRAME), WM_TRAYNOTIFY, 0, 1);
    m_trayIcon.SetDefaultMenuItem(ID_FIRST_SHOWQUICKPASTE, FALSE);

    if(CGetSetOptions::GetShowStartupMessage())
    {
        CString msg = theApp.m_Language.GetString(_T("StartupMsg"), _T("Ditto is running minimized, Ditto can be opened by hot keys or by clicking the task tray icon"));
        m_trayIcon.SetBalloonDetails(msg, _T("Ditto"), CTrayNotifyIcon::BalloonStyle::Info, CGetSetOptions::GetBalloonTimeout());
    }

    theApp.m_Language.UpdateTrayIconRightClickMenu(&m_trayIcon.GetMenu());

#ifndef _DEBUG
    if(!(CGetSetOptions::GetShowIconInSysTray()))
    {
        SetTimer(HIDE_ICON_TIMER, 40000, 0);
    }
#endif

    SetTimer(REMOVE_OLD_TEMP_FILES, ONE_HOUR * 6, 0);
    SetTimer(REMOVE_OLD_ENTRIES_TIMER, ONE_MINUTE * 15, 0);
    SetTimer(CLOSE_NO_DB_WINDOW_TIMER, 10000, 0);

    IdleSeconds();
    m_ulCopyGap = CGetSetOptions::GetCopyGap();

    theApp.AfterMainCreate();
    m_thread.Start(this);

    return 0;
}

BOOL CMainFrame::OnIdle()
{
    return FALSE;
}
LRESULT CMainFrame::OnPlainTextPaste(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
    DoTextOnlyPaste();
    return 0;
}
LRESULT CMainFrame::OnTrayNotification(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
    if(WM_MOUSEFIRST <= LOWORD(lParam) && LOWORD(lParam) <= WM_MOUSELAST)
    {
        theApp.m_activeWnd.TrackActiveWnd(true);
    }

    if(lParam == 0x405)
    {
        SetTimer(DELAYED_SHOW_DITTO_TIMER, 100, NULL);
    }

    m_trayIcon.OnTrayNotification(wParam, lParam);
    return 0;
}
BOOL CMainFrame::PreCreateWindow(CREATESTRUCT &cs)
{
    if(cs.hMenu != NULL)
    {
        ::DestroyMenu(cs.hMenu); // delete menu if loaded
        cs.hMenu = NULL; // no menu for this window
    }

    if(!CFrameWindowImpl<CMainFrame>::PreCreateWindow(cs))
    {
        return FALSE;
    }

    WNDCLASS wc;
    wc.style = CS_DBLCLKS | CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = ::DefWindowProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = _Module.GetModuleInstance();
    wc.hIcon = NULL;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
    wc.lpszMenuName = NULL;
    wc.lpszClassName = _T("Ditto");

    // Create the QPaste window class
    if(!::RegisterClass(&wc))
    {
        return FALSE;
    }

    cs.lpszClass = wc.lpszClassName;

    return 0;
}
/////////////////////////////////////////////////////////////////////////////// CMainFrame message handlers




LRESULT CMainFrame::OnFirstExit(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
    this->SendMessage(WM_CLOSE, 0, 0);
    return 0;
}
LRESULT CMainFrame::OnHotKey(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{    if(theApp.m_pDittoHotKey && wParam == theApp.m_pDittoHotKey->m_Atom ||
		theApp.m_pDittoHotKey2 && wParam == theApp.m_pDittoHotKey2->m_Atom ||
		theApp.m_pDittoHotKey3 && wParam == theApp.m_pDittoHotKey3->m_Atom)
    {
        //If they still have the shift/ctrl keys down
        if(m_keyStateModifiers != 0 && m_quickPaste.IsWindowVisibleEx())
        {
            Log(_T("On Show Ditto HotKey, key state modifiers are still down, moving selection"));

            if(m_bMovedSelectionMoveKeyState == false)
            {
                Log(_T("Setting flag m_bMovedSelectionMoveKeyState to true, will paste when modifier keys are up"));
            }

            m_quickPaste.MoveSelection(true);
            m_bMovedSelectionMoveKeyState = true;
        }
        else if(CGetSetOptions::m_HideDittoOnHotKeyIfAlreadyShown && m_quickPaste.IsWindowTopLevel() && CGetSetOptions::GetShowPersistent() == FALSE)
        {
            Log(_T("On Show Ditto HotKey, window is already visible, hiding window"));
            m_quickPaste.HideQPasteWnd();
        }
        else
        {
            Log(_T("On Show Ditto HotKey, showing window"));

			StartKeyModifierTimer();

			ShowQPasteWithActiveWindowCheck();
        }

        //KillTimer(CLOSE_WINDOW_TIMER);
        //SetTimer(CLOSE_WINDOW_TIMER, ONE_HOUR *24, 0);
    }
    else if(theApp.m_pPosOne && wParam == theApp.m_pPosOne->m_Atom)
    {
        Log(_T("Pos 1 hot key"));
        DoFirstTenPositionsPaste(0);
    }
    else if(theApp.m_pPosTwo && wParam == theApp.m_pPosTwo->m_Atom)
    {
        Log(_T("Pos 2 hot key"));
        DoFirstTenPositionsPaste(1);
    }
    else if(theApp.m_pPosThree && wParam == theApp.m_pPosThree->m_Atom)
    {
        Log(_T("Pos 3 hot key"));
        DoFirstTenPositionsPaste(2);
    }
    else if(theApp.m_pPosFour && wParam == theApp.m_pPosFour->m_Atom)
    {
        Log(_T("Pos 4 hot key"));
        DoFirstTenPositionsPaste(3);
    }
    else if(theApp.m_pPosFive && wParam == theApp.m_pPosFive->m_Atom)
    {
        Log(_T("Pos 5 hot key"));
        DoFirstTenPositionsPaste(4);
    }
    else if(theApp.m_pPosSix && wParam == theApp.m_pPosSix->m_Atom)
    {
        Log(_T("Pos 6 hot key"));
        DoFirstTenPositionsPaste(5);
    }
    else if(theApp.m_pPosSeven && wParam == theApp.m_pPosSeven->m_Atom)
    {
        Log(_T("Pos 7 hot key"));
        DoFirstTenPositionsPaste(6);
    }
    else if(theApp.m_pPosEight && wParam == theApp.m_pPosEight->m_Atom)
    {
        Log(_T("Pos 8 hot key"));
        DoFirstTenPositionsPaste(7);
    }
    else if(theApp.m_pPosNine && wParam == theApp.m_pPosNine->m_Atom)
    {
        Log(_T("Pos 9 hot key"));
        DoFirstTenPositionsPaste(8);
    }
    else if(theApp.m_pPosTen && wParam == theApp.m_pPosTen->m_Atom)
    {
        Log(_T("Pos 10 hot key"));
        DoFirstTenPositionsPaste(9);
    }
    else if(theApp.m_pCopyBuffer1 && wParam == theApp.m_pCopyBuffer1->m_Atom)
    {
        Log(_T("Copy buffer 1 hot key"));
        theApp.m_CopyBuffer.StartCopy(0);
    }
    else if(theApp.m_pPasteBuffer1 && wParam == theApp.m_pPasteBuffer1->m_Atom)
    {
        Log(_T("Paste buffer 1 hot key"));
        theApp.m_CopyBuffer.PastCopyBuffer(0);
    }
    else if(theApp.m_pCutBuffer1 && wParam == theApp.m_pCutBuffer1->m_Atom)
    {
        Log(_T("Cut buffer 1 hot key"));
        theApp.m_CopyBuffer.StartCopy(0, true);
    }
    else if(theApp.m_pCopyBuffer2 && wParam == theApp.m_pCopyBuffer2->m_Atom)
    {
        Log(_T("Copy buffer 2 hot key"));
        theApp.m_CopyBuffer.StartCopy(1);
    }
    else if(theApp.m_pPasteBuffer2 && wParam == theApp.m_pPasteBuffer2->m_Atom)
    {
        Log(_T("Paste buffer 2 hot key"));
        theApp.m_CopyBuffer.PastCopyBuffer(1);
    }
    else if(theApp.m_pCutBuffer2 && wParam == theApp.m_pCutBuffer2->m_Atom)
    {
        Log(_T("Cut buffer 2 hot key"));
        theApp.m_CopyBuffer.StartCopy(1, true);
    }
    else if(theApp.m_pCopyBuffer3 && wParam == theApp.m_pCopyBuffer3->m_Atom)
    {
        Log(_T("Copy buffer 3 hot key"));
        theApp.m_CopyBuffer.StartCopy(2);
    }
    else if(theApp.m_pPasteBuffer3 && wParam == theApp.m_pPasteBuffer3->m_Atom)
    {
        Log(_T("Paste buffer 3 hot key"));
        theApp.m_CopyBuffer.PastCopyBuffer(2);
    }
    else if(theApp.m_pCutBuffer3 && wParam == theApp.m_pCutBuffer3->m_Atom)
    {
        Log(_T("Cut buffer 3 hot key"));
        theApp.m_CopyBuffer.StartCopy(2, true);
    }
	else if (theApp.m_pCopyBuffer4 && wParam == theApp.m_pCopyBuffer4->m_Atom)
	{
		Log(_T("Copy buffer 4 hot key"));
		theApp.m_CopyBuffer.StartCopy(3);
	}
	else if (theApp.m_pPasteBuffer4 && wParam == theApp.m_pPasteBuffer4->m_Atom)
	{
		Log(_T("Paste buffer 4 hot key"));
		theApp.m_CopyBuffer.PastCopyBuffer(3);
	}
	else if (theApp.m_pCutBuffer4 && wParam == theApp.m_pCutBuffer4->m_Atom)
	{
		Log(_T("Cut buffer 4 hot key"));
		theApp.m_CopyBuffer.StartCopy(3, true);
	}
	else if (theApp.m_pCopyBuffer5 && wParam == theApp.m_pCopyBuffer5->m_Atom)
	{
		Log(_T("Copy buffer 5 hot key"));
		theApp.m_CopyBuffer.StartCopy(4);
		}
	else if (theApp.m_pPasteBuffer5 && wParam == theApp.m_pPasteBuffer5->m_Atom)
	{
		Log(_T("Paste buffer 5 hot key"));
		theApp.m_CopyBuffer.PastCopyBuffer(4);
	}
	else if (theApp.m_pCutBuffer5 && wParam == theApp.m_pCutBuffer5->m_Atom)
	{
		Log(_T("Cut buffer 5 hot key"));
		theApp.m_CopyBuffer.StartCopy(4, true);
	}
	else if(theApp.m_pTextOnlyPaste && wParam == theApp.m_pTextOnlyPaste->m_Atom)
	{
		DoTextOnlyPaste();
	}
	else if(theApp.m_pSaveClipboard && wParam == theApp.m_pSaveClipboard->m_Atom)
	{
		OnFirstSavecurrentclipboard();
	}
	else if (theApp.m_pCopyAndSaveClipboard && wParam == theApp.m_pCopyAndSaveClipboard->m_Atom)
	{
		Log(StrF(_T("START of copy and save clipboard, sending copy")));

		theApp.m_activeWnd.SendCopy(CopyReasonEnum::COPY_TO_UNKOWN);

		int delay = CGetSetOptions::GetCopyAndSveDelay();
		Log(StrF(_T("Copy and save clipboard, sending copy, delaying %dms before saving clipboard"), delay));
		Sleep(delay);

		Log(StrF(_T("Copy and save clipboard, saving clipboard")));
		OnFirstSavecurrentclipboard();

		Log(StrF(_T("END of copy and save clipboard")));
	}
	else
	{
		for(int i = 0; i < g_HotKeys.GetCount(); i++)
		{
			if(g_HotKeys[i] != NULL && 
				g_HotKeys[i]->m_Atom == wParam && 
				g_HotKeys[i]->m_clipId > 0)
			{
				if(g_HotKeys[i]->m_hkType == CHotKey::PASTE_OPEN_CLIP)
				{
					Log(StrF(_T("Pasting clip from global shortcut, clipId: %d"), g_HotKeys[i]->m_clipId));
					PasteOrShowGroup(g_HotKeys[i]->m_clipId, -1, FALSE, TRUE, false);
				}
				else if(g_HotKeys[i]->m_hkType == CHotKey::MOVE_TO_GROUP)
				{
					Log(StrF(_T("Global hot key to save clip to group Id: %d, Sending copy to save selection to this group"), g_HotKeys[i]->m_clipId));

					KillTimer(GROUP_DOUBLE_CLICK);
					m_doubleClickGroupId = -1;
					m_doubleClickGroupStartTime = 0;

					theApp.SetActiveGroupId(g_HotKeys[i]->m_clipId);
					theApp.m_activeWnd.SendCopy(CopyReasonEnum::COPY_TO_GROUP);
				}

				break;
			}
		}
	}

    return TRUE;
}

void CMainFrame::ShowQPasteWithActiveWindowCheck()
{
	//Before we show our window find the current focused window for paste into
	theApp.m_activeWnd.TrackActiveWnd(true);

	if (CGetSetOptions::GetOpenToGroupByActiveExe() &&
		theApp.m_activeWnd.ActiveWnd() != NULL)
	{
		CString exeName = GetProcessName(theApp.m_activeWnd.ActiveWnd());
		if (exeName != _T(""))
		{
			theApp.TryEnterOldGroupState();
			CString query = StrF(_T("SELECT lID FROM Main WHERE bIsGroup = 1 AND mText = '%s' COLLATE NOCASE"), exeName);
			CppSQLite3Query q = theApp.m_db.execQueryEx(query);
			if (q.eof() == false)
			{
				int groupId = q.getIntField(_T("lID"));
				//this will revert back to the old group on hide of ditto
				theApp.EnterGroupID(groupId, TRUE, TRUE);

				Log(StrF(_T("Opening Ditto to Group based on found group name, name: %s, GroupId: %d"), exeName, groupId));
			}
			else
			{
				theApp.TryEnterOldGroupState();
			}
		}
	}

	m_quickPaste.ShowQPasteWnd(this, false, true, FALSE);
}

void CMainFrame::DoTextOnlyPaste()
{
	CClipboardSaveRestore textOnlyPaste;

	Log(_T("Text Only paste, saving clipboard to be restored later"));
	textOnlyPaste.Save(TRUE);

	Log(_T("Text Only paste, Add cf_text or cf_unicodetext to clipboard"));
	textOnlyPaste.RestoreTextOnly();

	DWORD pasteDelay = CGetSetOptions::GetTextOnlyPasteDelay();

	Log(StrF(_T("Text Only paste, delaying %d ms before sending paste"), pasteDelay));

	Sleep(pasteDelay);

	Log(_T("Text Only paste, Sending paste"));
	theApp.m_activeWnd.SendPaste(false);

	Log(_T("Text Only paste, Post sending paste"));
}

void CMainFrame::DoFirstTenPositionsPaste(int nPos)
{
    try
	{
		CString csSort = _T("");
		CString strFilter = _T("");
		bool pastedFromGroup = false;

		if (theApp.m_GroupID < 0 ||
			CGetSetOptions::GetUseUISelectedGroupForLastTenCopies() == FALSE)
		{
			//do not change this this directly relates to the views in the Main table
			csSort = "Main.bIsGroup ASC, "
				"Main.stickyClipOrder DESC, "
				"Main.clipOrder DESC";

			if (CGetSetOptions::m_bShowAllClipsInMainList)
			{
				if (CGetSetOptions::GetShowGroupsInMainList())
				{
					//found to be slower on large databases
					strFilter = "((Main.bIsGroup = 1 AND Main.lParentID = -1) OR Main.bIsGroup = 0)";
				}
				else
				{
					strFilter = "(Main.bIsGroup = 0)";
				}
			}
			else
			{
				strFilter = "((Main.bIsGroup = 1 AND Main.lParentID = -1) OR (Main.bIsGroup = 0 AND Main.lParentID = -1))";
			}
		}
		else
		{
			pastedFromGroup = true;

			//do not change this this directly relates to the views in the Main table
			csSort = "Main.bIsGroup ASC, "
				"Main.stickyClipGroupOrder DESC, "
				"Main.clipGroupOrder DESC";
			
			if (theApp.m_GroupID >= 0)
			{
				strFilter.Format(_T("Main.lParentID = %d"), theApp.m_GroupID);
			}
		}

		CString query = StrF(_T("SELECT lID, bIsGroup FROM Main WHERE %s ORDER BY %s LIMIT 1 OFFSET %d"), strFilter, csSort, nPos);

		Log(StrF(_T("Doing Last Ten Paste, Index: %d Query: %s"), nPos, query));

		CppSQLite3Query q = theApp.m_db.execQueryEx(query);

        if(q.eof() == false)
        {
			PasteOrShowGroup(q.getIntField(_T("lID")), CGetSetOptions::GetMoveClipsOnGlobal10(), false, CGetSetOptions::m_bSendPasteOnFirstTenHotKeys, pastedFromGroup);
        }
    }
    CATCH_SQLITE_EXCEPTION
}

void CMainFrame::StartKeyModifierTimer()
{
	if (CGetSetOptions::m_moveSelectionOnOpenHotkey)
	{
		m_keyModifiersTimerCount = 0;
		m_bMovedSelectionMoveKeyState = false;
		m_startKeyStateTime = GetTickCount();
		m_keyStateModifiers = CAccels::GetKeyStateModifiers();
		SetTimer(KEY_STATE_MODIFIERS, 50, NULL);
	}
}

void CMainFrame::PasteOrShowGroup(int dbId, BOOL updateClipTime, BOOL activeTarget, BOOL sendPaste, bool pastedFromGroup)
{
	try
	{
		bool isGroup = false;
		CppSQLite3Query q = theApp.m_db.execQueryEx(_T("SELECT bIsGroup FROM Main WHERE lID = %d"), dbId);
		if(q.eof() == false)
		{
			if(q.getIntField(_T("bIsGroup")) > 0)
			{
				isGroup = true;
			}
		}
		
		if(isGroup)
		{
			DWORD maxDiff = (DWORD)CGetSetOptions::GetGroupDoubleClickTimeMS();
			DWORD diff = GetTickCount() - m_doubleClickGroupStartTime;		

			if(m_doubleClickGroupId == dbId &&
				diff < maxDiff)
			{
				Log(StrF(_T("Second Press of group hot key, group Id: %d, Sending copy to save selection to this group"), dbId));
				
				KillTimer(GROUP_DOUBLE_CLICK);
				m_doubleClickGroupId = -1;
				m_doubleClickGroupStartTime = 0;

				theApp.SetActiveGroupId(dbId);
				theApp.m_activeWnd.SendCopy(CopyReasonEnum::COPY_TO_GROUP);
			}
			else
			{
				m_doubleClickGroupId = dbId;
				m_doubleClickGroupStartTime = GetTickCount();

				int doubleClickTime = CGetSetOptions::GetGroupDoubleClickTimeMS();

				SetTimer(GROUP_DOUBLE_CLICK, doubleClickTime, 0);

				Log(StrF(_T("First Press of group hot key, group Id: %d, timeout: %d"), dbId, doubleClickTime));
			}
		}
		else
		{
			KillTimer(GROUP_DOUBLE_CLICK);
			m_doubleClickGroupId = -1;
			m_doubleClickGroupStartTime = 0;

			BOOL bItWas = CGetSetOptions::m_bUpdateTimeOnPaste;
			if (updateClipTime != -1)
			{				
				CGetSetOptions::m_bUpdateTimeOnPaste = updateClipTime;
			}

			CProcessPaste paste;
			paste.m_pastedFromGroup = pastedFromGroup;
			paste.GetClipIDs().Add(dbId);

			if (activeTarget != -1)
			{
				paste.m_bActivateTarget = activeTarget ? true : false;;
			}

			if (sendPaste != -1)
			{
				paste.m_bSendPaste = sendPaste ? true : false;
			}
			paste.DoPaste();
			theApp.OnPasteCompleted();

			if (updateClipTime != -1)
			{
				CGetSetOptions::m_bUpdateTimeOnPaste = bItWas;
			}
		}
	}
	CATCH_SQLITE_EXCEPTION
}

void CMainFrame::DoDittoCopyBufferPaste(int nCopyBuffer)
{
    try
    {
        CppSQLite3Query q = theApp.m_db.execQueryEx(_T("SELECT lID FROM Main WHERE CopyBuffer = %d"), nCopyBuffer);

        if(q.eof() == false)
        {
            //Don't move these to the top
            BOOL bItWas = CGetSetOptions::m_bUpdateTimeOnPaste;
            CGetSetOptions::m_bUpdateTimeOnPaste = FALSE;

            CProcessPaste paste;
            paste.GetClipIDs().Add(q.getIntField(_T("lID")));
            paste.m_bActivateTarget = false;
            paste.DoPaste();
            theApp.OnPasteCompleted();

            CGetSetOptions::m_bUpdateTimeOnPaste = bItWas;
        }
    }
    CATCH_SQLITE_EXCEPTION
}

void CMainFrame::OnTimer(UINT_PTR nIDEvent)
{
    switch(nIDEvent)
    {
        case HIDE_ICON_TIMER:
        {
            m_trayIcon.Hide();
            KillTimer(nIDEvent);
        }
        break;

        case REMOVE_OLD_ENTRIES_TIMER:
        {
            m_thread.FireDeleteEntries();
        }
        break;

        case REMOVE_OLD_TEMP_FILES:
        {
            m_thread.FireRemoveTempFiles();
        }
        break;

        case KEY_STATE_MODIFIERS:
            m_keyModifiersTimerCount++;
            if(m_keyStateModifiers != 0)
            {
                BYTE keyState = CAccels::GetKeyStateModifiers();
                if((m_keyStateModifiers & keyState) == 0)
                {
                    KillTimer(KEY_STATE_MODIFIERS);
                    long waitTime = (long)(GetTickCount() - m_startKeyStateTime);

                    if(m_bMovedSelectionMoveKeyState || m_keyModifiersTimerCount > CGetSetOptions::GetKeyStateWaitTimerCount())
                    {
                        Log(StrF(_T("Timer KEY_STATE_MODIFIERS timeout count hit(%d), count (%d), time (%d), Move Selection from Modifer (%d) sending paste"), CGetSetOptions::GetKeyStateWaitTimerCount(), m_keyModifiersTimerCount, waitTime, m_bMovedSelectionMoveKeyState));
                        m_quickPaste.OnKeyStateUp();
                    }
                    else
                    {
                        Log(StrF(_T("Timer KEY_STATE_MODIFIERS count NOT hit(%d), count (%d) time (%d)"), CGetSetOptions::GetKeyStateWaitTimerCount(), m_keyModifiersTimerCount, waitTime));
                        m_quickPaste.SetKeyModiferState(false);
                    }

                    m_keyStateModifiers = 0;
                    m_keyModifiersTimerCount = 0;
                    m_bMovedSelectionMoveKeyState = 0;
                }
            }
            else
            {
                KillTimer(KEY_STATE_MODIFIERS);
            }
            break;

        case ACTIVE_WINDOW_TIMER:
        {
            if(theApp.m_bShowingQuickPaste)
            {
                theApp.m_activeWnd.TrackActiveWnd(false);
            }
        }
        break;

        case READ_RANDOM_DB_FILE:
        {
            m_thread.FireReadDbFile();
        }
        break;

        case GROUP_DOUBLE_CLICK:
        {
            KillTimer(GROUP_DOUBLE_CLICK);

            Log(StrF(_T("Processing single click of groupId %d in timer, opening ditto to this group"), m_doubleClickGroupId));

            DWORD maxDiff = (DWORD)(CGetSetOptions::GetGroupDoubleClickTimeMS() * 1.5);
            DWORD diff = GetTickCount() - m_doubleClickGroupStartTime;

            if(diff < maxDiff)
            {
                if(m_doubleClickGroupId > -1)
                {
                    if(theApp.EnterGroupID(m_doubleClickGroupId, FALSE, TRUE))
                    {
                        theApp.m_activeWnd.TrackActiveWnd(true);
                        StartKeyModifierTimer();
                        m_quickPaste.ShowQPasteWnd(this, false, true, FALSE);
                    }
                }
            }
            else
            {
                Log(StrF(_T("Something happened and we didn't process the group timer in time, Id: %d, Diff ms: %d, maxDiff: %d"), m_doubleClickGroupId, diff, maxDiff));
            }

            m_doubleClickGroupId = -1;
            m_doubleClickGroupStartTime = 0;
        }
        break;

        case SCREEN_RESOLUTION_CHANGED:
        {
            KillTimer(SCREEN_RESOLUTION_CHANGED);
            m_quickPaste.OnScreenResolutionChange();
        }
        break;

        case DELAYED_SHOW_DITTO_TIMER:
        {
            KillTimer(DELAYED_SHOW_DITTO_TIMER);
            m_quickPaste.ShowQPasteWnd(this, false, false, FALSE);
        }
        break;

        case SET_WINDOWS_THEME_TIMER:
        {
            KillTimer(SET_WINDOWS_THEME_TIMER);
            CString theme = CGetSetOptions::GetTheme();
            if(theme != _T(""))
            {
                CGetSetOptions::m_Theme.Load(theme);

                bool visible = m_quickPaste.IsWindowVisibleEx() != FALSE;
                m_quickPaste.CloseQPasteWnd();

                if(visible)
                {
                    m_quickPaste.ShowQPasteWnd(this, true, false, TRUE);
                }
            }
        }
        break;

        case CLOSE_NO_DB_WINDOW_TIMER:
            KillTimer(CLOSE_NO_DB_WINDOW_TIMER);
            theApp.CloseNoDbWindow();
            break;
    }
}
LRESULT CMainFrame::OnShowTrayIcon(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
    if(lParam)
    {
        if(!m_trayIcon.IsHidden())
        {
            KillTimer(HIDE_ICON_TIMER);
            SetTimer(HIDE_ICON_TIMER, 40000, 0);
        }
    }

    if(wParam)
    {
        m_trayIcon.Show();
    }
    else
    {
        m_trayIcon.Hide();
    }

    return 0;
}
LRESULT CMainFrame::OnFirstShowquickpaste(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
    m_quickPaste.ShowQPasteWnd(this, false, false, FALSE);
    return 0;
}
LRESULT CMainFrame::OnFirstToggleConnectCV(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
    theApp.ToggleConnectCV();
    return 0;
}
LRESULT CMainFrame::OnClipboardCopied(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
    Log(_T("Start of function OnClipboardCopied, adding clip to thread for processing"));

    CClip* pClip = (CClip*)wParam;
    if(pClip != NULL)
    {
        m_thread.AddClipToSave(pClip);
    }

    Log(_T("End of function OnClipboardCopied"));
    return 0;
}
BOOL CMainFrame::PreTranslateMessage(MSG *pMsg)
{
	//forward the mouse wheel onto the window under the cursor
	//normally windows only sends it to the window with focus, bypass this
	if ((pMsg->message == WM_MOUSEWHEEL || pMsg->message == WM_MOUSEHWHEEL) &&
		::GetCapture() == nullptr)
	{
		POINT mouse;
		GetCursorPos(&mouse);
		HWND hwndFromPoint = ::WindowFromPoint(mouse);

		if (pMsg->hwnd != hwndFromPoint)
		{
			DWORD winProcessId = 0;
			::GetWindowThreadProcessId(hwndFromPoint, &winProcessId);
			if (winProcessId == ::GetCurrentProcessId()) //no-fail!
			{
				pMsg->hwnd = hwndFromPoint;				
			}
		}

		if (GetKeyState(VK_SHIFT) & 0x8000)
			pMsg->message = WM_MOUSEHWHEEL;
	}

    return CFrameWindowImpl<CMainFrame>::PreTranslateMessage(pMsg);
}
void CMainFrame::OnClose()
{
    CloseAllOpenDialogs();

    Log(_T("OnClose - before stop MainFrm thread"));
    m_thread.Stop();
    Log(_T("OnClose - after stop MainFrm thread"));

    theApp.BeforeMainClose();
    m_PowerManager.Close();

    DestroyWindow();
}
bool CMainFrame::CloseAllOpenDialogs()
{
    bool bRet = false;
    DWORD dwordProcessId;
    DWORD dwordChildWindowProcessId;
    GetWindowThreadProcessId(this->m_hWnd, &dwordProcessId);

    HWND hTempWnd = GetDesktopWindow();
    hTempWnd = ::GetWindow(hTempWnd, GW_CHILD);
    while((hTempWnd = ::GetWindow(hTempWnd, GW_HWNDNEXT)) != NULL)
    {
        if(hTempWnd == NULL)
        {
            break;
        }

        GetWindowThreadProcessId(hTempWnd, &dwordChildWindowProcessId);
        if(dwordChildWindowProcessId == dwordProcessId)
        {
            TCHAR szTemp[100];
            GetClassName(hTempWnd, szTemp, 100);

            // #32770 is class name for dialogs so don't process the message if it is a dialog
            if(STRCMP(szTemp, _T("#32770")) == 0)
            {
                ::SendMessage(hTempWnd, WM_CLOSE, 0, 0);
                bRet = true;
            }
        }
    }

    MSG msg;
    while(PeekMessage(&msg, NULL, NULL, NULL, PM_REMOVE))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return bRet;
}
LRESULT CMainFrame::OnLoadClipOnClipboard(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
    CClip* pClip = (CClip*)wParam;
    if(pClip == NULL)
    {
        LogSendRecieveInfo("---------ERROR OnLoadClipOnClipboard pClip == NULL");
        return 0;
    }

    if(pClip)
    {
        CProcessPaste paste;
        paste.m_bSendPaste = false;
        paste.m_bActivateTarget = false;
        paste.m_pasteOptions.m_delayRenderLockout = GetTickCount();

        LogSendRecieveInfo("---------OnLoadClipOnClipboard - Before PutFormats on clipboard");

        paste.m_pOle->PutFormatOnClipboard(&pClip->m_Formats);
        paste.m_pOle->CacheGlobalData(theApp.m_cfIgnoreClipboard, NewGlobalP("Ignore", sizeof("Ignore")));

        LogSendRecieveInfo("---------OnLoadClipOnClipboard - After PutFormats on clipboard");

        LogSendRecieveInfo(StrF(_T("---------OnLoadClipOnClipboard - Setting clip id: %d on ole clipboard"), pClip->m_id));
        paste.GetClipIDs().Add(pClip->m_id);
        paste.DoPaste();

        LogSendRecieveInfo(StrF(_T("---------OnLoadClipOnClipboard - After paste clip id: %d on ole clipboard"), pClip->m_id));
    }

    delete pClip;

    return 0;
}
LRESULT CMainFrame::OnAddToDatabaseFromSocket(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
    CClipList* pClipList = (CClipList*)wParam;
    if(pClipList == NULL)
    {
        LogSendRecieveInfo("---------OnAddToDatabaseFromSocket - ERROR pClipList == NULL");
        return 0;
    }

    DWORD flags = (DWORD)lParam;
    if(flags & REMOTE_CLIP_ADD_TO_CLIPBOARD)
    {
        CClip* pClip = pClipList->GetTail();
        if(pClip)
        {
            LogSendRecieveInfo("OnAddToDatabaseFromSocket - Adding clip from socket setting clip to be put on clipboard");
            pClip->m_param1 |= REMOTE_CLIP_ADD_TO_CLIPBOARD;
        }
    }

    if(flags & REMOTE_CLIP_MANUAL_SEND)
    {
        CClip* pClip = pClipList->GetTail();
        if(pClip)
        {
            LogSendRecieveInfo("OnAddToDatabaseFromSocket - Adding clip from socket setting clip was a manual send from other side");
            pClip->m_param1 |= REMOTE_CLIP_MANUAL_SEND;
        }
    }

    m_thread.AddRemoteClipToSave(pClipList);
    delete pClipList;

    return 0;
}
LRESULT CMainFrame::OnErrorOnSendRecieve(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
    CString csNewText = (TCHAR*)wParam;
    ShowErrorMessage(_T("Ditto - Send/Receive Error"), csNewText);
    return 0;
}

LRESULT CMainFrame::OnErrorMsg(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
    CString csNewText = (TCHAR*)wParam;
    ShowErrorMessage(_T("Ditto"), csNewText);
    return 0;
}
CString WndName(HWND hParent)
{
    TCHAR cWindowText[200];

    ::GetWindowText(hParent, cWindowText, 100);

    int nCount = 0;

    while(STRLEN(cWindowText) <= 0)
    {
        hParent = ::GetParent(hParent);
        if(hParent == NULL)
        {
            break;
        }

        ::GetWindowText(hParent, cWindowText, 100);

        nCount++;
        if(nCount > 100)
        {
            Log(_T("GetTargetName reached maximum search depth of 100"));
            break;
        }
    }

    return cWindowText;
}

LRESULT CMainFrame::OnFirstHelp(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
    ::ShellExecute(NULL, _T("open"), _T("https://github.com/sabrogden/Ditto/wiki"), NULL, NULL, SW_SHOWNORMAL);
    return 0;
}
void CMainFrame::ShowErrorMessage(CString csTitle, CString csMessage)
{
    Log(StrF(_T("ShowErrorMessage %s - %s"), csTitle, csMessage));
	m_trayIcon.SetBalloonDetails(csMessage, csTitle, CTrayNotifyIcon::BalloonStyle::Error, CGetSetOptions::GetBalloonTimeout());
}

LRESULT CMainFrame::OnFirstImport(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
    theApp.ImportClips(theApp.m_MainhWnd);
    return 0;
}
LRESULT CMainFrame::OnSetConnected(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
    if(wParam)
    {
        theApp.SetConnectCV(true);
    }
    else if(lParam)
    {
        theApp.SetConnectCV(false);
    }

    return 0;
}

LRESULT CMainFrame::OnOpenCloseWindow(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
    if(wParam)
    {
        StartKeyModifierTimer();
        ShowQPasteWithActiveWindowCheck();
    }
    else if(lParam)
    {
        m_quickPaste.HideQPasteWnd();
    }

    return 0;
}
void CMainFrame::OnDestroy()
{
    CMessageLoop* pLoop = _Module.GetMessageLoop();
    ATLASSERT(pLoop != NULL);
    pLoop->RemoveMessageFilter(this);
    pLoop->RemoveIdleHandler(this);

    PostQuitMessage(0);
}
LRESULT CMainFrame::OnFirstNewclip(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
    CClipIDs IDs;
    IDs.Add( - 1);
    theApp.EditItems(IDs, true, true);
    return 0;
}
LRESULT CMainFrame::OnFirstOption(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
    if(m_pOptions != NULL)
    {
        ::SetForegroundWindow(m_pOptions);
    }
    else
    {
        COptionsSheet* pOptions = new COptionsSheet(_T(""));
        if(pOptions != NULL)
        {
            pOptions->SetNotifyWnd(m_hWnd);
            pOptions->Create();
            pOptions->ShowWindow(SW_SHOW);
            m_pOptions = pOptions->m_hWnd;
        }
    }
    return 0;
}
LRESULT CMainFrame::OnFirstGlobalhotkeys(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
    if(m_pGlobalClips != NULL)
    {
        ::SetForegroundWindow(m_pGlobalClips);
    }
    else
    {
        GlobalClips* pGlobalClips = new GlobalClips();
        if(pGlobalClips != NULL)
        {
            CAlphaBlend tran;
            tran.SetTransparent(m_hWnd, 0, 1);
            pGlobalClips->SetNotifyWnd(m_hWnd);
            pGlobalClips->Create(IDD_GLOBAL_CLIPS, NULL);
            pGlobalClips->ShowWindow(SW_SHOW);
            m_pGlobalClips = pGlobalClips->m_hWnd;
        }
    }
    return 0;
}
LRESULT CMainFrame::OnShowOptions(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
    if(m_pOptions != NULL)
    {
        ::SetForegroundWindow(m_pOptions);
    }
    else
    {
        COptionsSheet* pOptions = new COptionsSheet(_T(""));
        if(pOptions != NULL)
        {
            pOptions->SetNotifyWnd(m_hWnd);
            pOptions->Create();
            pOptions->ShowWindow(SW_SHOW);
            m_pOptions = pOptions->m_hWnd;
        }
    }
    return 0;
}
LRESULT CMainFrame::OnOptionsClosed(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
    BOOL themeChanged = (BOOL)wParam;
    m_trayIcon.MinimiseToTray(m_hWnd);
    CAlphaBlend tran;
    tran.SetTransparent(m_hWnd, 255, 0);

    if(m_pOptions != NULL)
    {
        ::DestroyWindow(m_pOptions);
    }
    m_pOptions = NULL;

    if(themeChanged)
    {
        CGetSetOptions::m_Theme.Load(CGetSetOptions::GetTheme());
        m_quickPaste.CloseQPasteWnd();
    }
    else
    {
        if(m_quickPaste.m_pwndPaste != NULL)
        {
            m_quickPaste.m_pwndPaste->PostMessage(NM_POST_OPTIONS_WINDOW);
        }
    }

    m_trayIcon.SetMenu(NULL, IDR_MENU);
    theApp.m_Language.UpdateTrayIconRightClickMenu(&m_trayIcon.GetMenu());

    if(CGetSetOptions::GetShowIconInSysTray())
    {
        m_trayIcon.Show();
    }
    else
    {
        m_trayIcon.Hide();
    }

    return 0;
}
LRESULT CMainFrame::OnGlobalClipsClosed(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
    m_trayIcon.MinimiseToTray(m_hWnd);
    CAlphaBlend tran;
    tran.SetTransparent(m_hWnd, 255, 0);

    if(m_pGlobalClips != NULL)
    {
        ::DestroyWindow(m_pGlobalClips);
    }
    m_pGlobalClips = NULL;

    return 0;
}
void CMainFrame::RefreshShowInTaskBar()
{
	BOOL windowVisible = m_quickPaste.IsWindowVisibleEx();

	m_quickPaste.CloseQPasteWnd();

	if (windowVisible)
	{
		m_quickPaste.ShowQPasteWnd(this, true, false, true);
	}
}

LRESULT CMainFrame::OnDeleteClipDataClosed(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
    m_trayIcon.MinimiseToTray(m_hWnd);
    CAlphaBlend tran;
    tran.SetTransparent(m_hWnd, 255, 0);

    if(m_pDeleteClips != NULL)
    {
        ::DestroyWindow(m_pDeleteClips);
    }
    m_pDeleteClips = NULL;

    return 0;
}
LRESULT CMainFrame::OnFirstDeleteclipdata(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
    if(m_pDeleteClips != NULL)
    {
        ::SetForegroundWindow(m_pDeleteClips);
    }
    else
    {
        CDeleteClipData* pDeleteClips = new CDeleteClipData();
        if(pDeleteClips != NULL)
        {
            CAlphaBlend tran;
            tran.SetTransparent(m_hWnd, 0, 1);
            pDeleteClips->SetNotifyWnd(m_hWnd);
            pDeleteClips->Create(IDD_DELETE_CLIP_DATA, NULL);
            pDeleteClips->ShowWindow(SW_SHOW);
            m_pDeleteClips = pDeleteClips->m_hWnd;
        }
    }
    return 0;
}
LRESULT CMainFrame::OnSaveClipboardMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
    Log(_T("Start Saving the current clipboard to the database"));
    CClip* pClip = new CClip;
    if(pClip)
    {
        CClipTypes* pTypes = theApp.LoadTypesFromDB();
        if(pTypes)
        {
            if(pClip->LoadFromClipboard(pTypes, false, _T("")))
            {
                Log(_T("Loaded clips from the clipboard, sending message to save to the db"));
                ::PostMessage(m_hWnd, WM_CLIPBOARD_COPIED, (WPARAM)pClip, 0);
            }
            else
            {
                Log(_T("Failed to load clips from the clipboard, not saving to db"));
                delete pClip;
                pClip = NULL;
            }
        }
        else
        {
            Log(_T("Failed to load supported types from the db, not saving to the db"));
        }
    }
    Log(_T("End Saving the current clipboard to the database"));
    return 0;
}
LRESULT CMainFrame::OnFirstSavecurrentclipboard(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	Log(_T("Start Saving the current clipboard to the database"));
	CClip* pClip = new CClip;
	if(pClip)
	{
		CClipTypes* pTypes = theApp.LoadTypesFromDB();
		if(pTypes)
		{
			if(pClip->LoadFromClipboard(pTypes, false, _T("")))
			{
				Log(_T("Loaded clips from the clipboard, sending message to save to the db"));
				::PostMessage(m_hWnd, WM_CLIPBOARD_COPIED, (WPARAM)pClip, 0);
			}
			else
			{
				Log(_T("Failed to load clips from the clipboard, not saving to db"));
				delete pClip;
				pClip = NULL;
			}
		}
		else
		{
			Log(_T("Failed to load supported types from the db, not saving to the db"));
		}
	}
	Log(_T("Start Saving the current clipboard to the database"));
	return 0;
}
LRESULT CMainFrame::OnReAddTaskBarIcon(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
    if(CGetSetOptions::GetShowIconInSysTray())
    {
        m_trayIcon.SetIcon(CTrayNotifyIcon::LoadIcon(IDR_MAINFRAME));
    }
    return 0;
}

LRESULT CMainFrame::OnReOpenDatabase(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
    Log(StrF(_T("OnReOpenDatabase, Start closing and reopening database Delay: %d"), CGetSetOptions::GetWindowsResumeDelayReOpenDbMS()));

    try
    {
        Sleep(CGetSetOptions::GetWindowsResumeDelayReOpenDbMS());
        m_quickPaste.CloseQPasteWnd();
        theApp.m_db.close();
        OpenDatabase(CGetSetOptions::GetDBPath());
    }
    CATCH_SQLITE_EXCEPTION

    Log(StrF(_T("OnReOpenDatabase, End closing and reopening database Delay: %d"), CGetSetOptions::GetWindowsResumeDelayReOpenDbMS()));

    return 0;
}

LRESULT CMainFrame::OnShowMsgWindow(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
    CString* pMsg = (CString*)wParam;
    int clipId = (int)lParam;

    m_trayIcon.SetBalloonDetails(pMsg->GetBuffer(), _T("Ditto"), CTrayNotifyIcon::BalloonStyle::Info, CGetSetOptions::GetBalloonTimeout());

    delete pMsg;
    return 0;
}

LRESULT CMainFrame::OnShowDittoGroup(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
    int groupId = (int)wParam;
    CppSQLite3Query q = theApp.m_db.execQueryEx(_T("SELECT bIsGroup FROM Main WHERE lID = %d"), groupId);
    if(q.eof() == false)
    {
        if(q.getIntField(_T("bIsGroup")) > 0)
        {
            PasteOrShowGroup(groupId, FALSE, FALSE, FALSE, false);
        }
    }

    return 0;
}

LRESULT CMainFrame::OnFirstFixupstickycliporder(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
    ReOrderStickyClips(-1, theApp.m_db);
    return 0;
}

LRESULT CMainFrame::OnResolutionChange(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
    if(m_startupScreenWidth != GetScreenWidth() ||
        m_startupScreenHeight != GetScreenHeight())
    {
        m_startupScreenWidth = GetScreenWidth();
        m_startupScreenHeight = GetScreenHeight();

        SetTimer(SCREEN_RESOLUTION_CHANGED, 1000, NULL);
    }

    return 0;
}
LRESULT CMainFrame::OnWinIniChange(UINT nFlags, LPCTSTR lpszSection)
{
    if(lpszSection != NULL && wcscmp(lpszSection, L"ImmersiveColorSet") == 0)
    {
        Log(StrF(_T("OnWinIniChange %s, setting timer to 1000ms to change theme"), lpszSection));
        KillTimer(SET_WINDOWS_THEME_TIMER);
        SetTimer(SET_WINDOWS_THEME_TIMER, 1000, NULL);
    }

    return 0;
}

LRESULT CMainFrame::OnFirstShowstartupmessage(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
    BOOL existing = CGetSetOptions::GetShowStartupMessage();
    CGetSetOptions::SetShowStartupMessage(!existing);
    return 0;
}

LRESULT CMainFrame::OnFirstBackupdatabase(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	BackupDbPrompt(m_hWnd);
	return 0;
}
LRESULT CMainFrame::OnFirstRestoredatabase(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	RestoreDbPrompt(m_hWnd);
	return 0;
}
LRESULT CMainFrame::OnBackupDb(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
    OnFirstBackupdatabase(0, 0, NULL, bHandled);
    return 0;
}

LRESULT CMainFrame::OnRestoreDb(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
    OnFirstRestoredatabase(0, 0, NULL, bHandled);
    return 0;
}

LRESULT CMainFrame::OnFirstDeleteallnonusedclips(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
    int nRet = MessageBox(theApp.m_Language.GetString(_T("Delete_All_Non_Used_Clips"), _T("Delete all clips that are not groups, in groups, marked as never auto delete, has a shortcut key or marked as sticky.\r\n\r\nThis cannot be undone.")), _T("Ditto"), MB_YESNO | MB_TOPMOST);
    if(nRet == IDNO)
    {
        return 0;
    }

    DeleteNonUsedClips(false);
    theApp.RefreshView();
    return 0;
}

LRESULT CMainFrame::OnPasteClip(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
    PasteOrShowGroup((int)wParam, TRUE, FALSE, TRUE, false);
    return 0;
}

LRESULT CMainFrame::OnEditClip(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
    CClipIDs IDs;
    IDs.Add((int)wParam);

    bool textOnly = false;
    if(GetKeyState(VK_SHIFT) & 0x8000)
    {
        textOnly = true;
    }

    theApp.EditItems(IDs, true, textOnly);
    return 0;
}
void CMainFrame::OnSetFocus(HWND hWndOldFocus)
{
}