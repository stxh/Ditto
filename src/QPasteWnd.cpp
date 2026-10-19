// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "stdafx.h"
#include "..\Shared\Tokenizer.h"
#include ".\qpastewnd.h"
#include "ActionEnums.h"
#include "CF_TextAggregator.h"
#include "CF_UnicodeTextAggregator.h"
#include "client.h"
#include "ClipCompare.h"
#include "CopyProperties.h"
#include "CP_Main.h"
#include "DimWnd.h"
#include "FormatSQL.h"
#include "FriendPromptDlg.h"
#include "GroupName.h"
#include ".\qpastewnd.h"
#include "MoveToGroupDlg.h"
#include <atlctrlx.h>
#include "FormatSQL.h"
#include "MainTableFunctions.h"
#include "Misc.h"
#include "MoveToGroupDlg.h"
#include "Path.h"
#include "ProcessPaste.h"
#include "QPasteWnd.h"
#include "SendMail.h"
#include "CF_UnicodeTextAggregator.h"
#include "CF_TextAggregator.h"
#include "htmlformataggregator.h"
#include "..\Shared\Tokenizer.h"
#include <signal.h>
#include "CreateQRCodeImage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif 

#define QPASTE_TITLE			"DittoWTL"



#define QPASTE_WIDTH			200
#define QPASTE_HEIGHT			200

#define TIMER_FILL_CACHE		1
#define TIMER_DO_SEARCH			2
#define TIMER_PASTE_FROM_MODIFER	3
#define TIMER_ERROR_MSG			4
#define TIMER_DRAG_HIDE_WINDOW	6
#define TIMER_DO_ACTION	7

#define THREAD_DO_QUERY				0
#define THREAD_EXIT_THREAD			1
#define THREAD_FILL_ACCELERATORS	2
#define THREAD_DESTROY_ACCELERATORS	3
#define THREAD_LOAD_ITEMS			4
#define THREAD_LOAD_EXTRA_DATA		5


/////////////////////////////////////////////////////////////////////////////
// CQPasteWnd

CQPasteWnd::CQPasteWnd()
{
	m_Title = QPASTE_TITLE;
	m_bHideWnd = true;
	m_strSQLSearch = "";
	m_strSearch = "";
	m_bHandleSearchTextChange = true;
	m_bModifersMoveActive = false;
	m_showScrollBars = false;
	m_leftSelectedCompareId = 0;
	m_extraDataCounter = 0;
	m_noSearchResults = false;
	m_bShowStarredClips = false;
	m_lastDbWrite = 0;
	m_pendingRefresh = false;
	m_lastNonActiveMouseMove = 0;
}

CQPasteWnd::~CQPasteWnd()
{
}



/////////////////////////////////////////////////////////////////////////////
// CQPasteWnd message handlers

HBRUSH CQPasteWnd::CtlColor(HDC hDC)
{
	::SetBkMode(hDC, TRANSPARENT);
	::SetBkColor(hDC, RGB(255, 0, 0));

	return (HBRUSH)GetStockObject(NULL_BRUSH);
}

BOOL CQPasteWnd::Create(const CRect& rcStart, CWindow *pParentWnd)
{
	return CWndEx::Create(rcStart, pParentWnd);
}

int CQPasteWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWndEx::OnCreate(lpCreateStruct) == -1)
	{
		return -1;
	}

	HICON b = (HICON)LoadImage(theApp.GetResourceInstance(), MAKEINTRESOURCE(IDR_MAINFRAME), IMAGE_ICON, 64, 64, LR_SHARED);
	SetIcon(b, TRUE);

	//BOOL b = this->Register(this);

	SetWindowText(_T(QPASTE_TITLE));

	CRect rcZero(0, 0, 0, 0);

	m_search.Create(m_hWnd, CRect(0, 0, 0, 0), NULL, WS_TABSTOP | WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL, 0, ID_EDIT_SEARCH);
	m_search.SetDpiInfo(&m_DittoWindow.m_dpi);
	m_search.SetPromptText(theApp.m_Language.GetString(_T("Search"), _T("Search")));
	::SHAutoComplete(m_search.m_hWnd, SHACF_AUTOSUGGEST_FORCE_OFF);
	SetSearchImages();
	m_search.LoadPastSearches(CGetSetOptions::GetPastSearchXml());

	CRect rcEditArea(m_DittoWindow.m_dpi.Scale(4), m_DittoWindow.m_dpi.Scale(2), m_DittoWindow.m_dpi.Scale(20), m_DittoWindow.m_dpi.Scale(2));
	//m_search.SetBorder(rcEditArea);

	CRect rcCloseArea(m_DittoWindow.m_dpi.Scale(85), m_DittoWindow.m_dpi.Scale(3), m_DittoWindow.m_dpi.Scale(99), m_DittoWindow.m_dpi.Scale(15));
	//m_search.SetButtonArea(rcCloseArea);

	// Create the header control
	if (!m_lstHeader.Create(m_hWnd, rcZero, NULL, WS_TABSTOP | WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | LVS_NOCOLUMNHEADER | LVS_REPORT | LVS_SHOWSELALWAYS | LVS_OWNERDATA | LVS_OWNERDRAWFIXED, 0, ID_LIST_HEADER))
	{
		ASSERT(FALSE);
		return -1;
	}
	m_lstHeader.SetDpiInfo(&m_DittoWindow.m_dpi);
	m_lstHeader.ShowWindow(SW_SHOW);

	// Create modern scrollbar overlay (vertical)
	m_modernScrollBar.Create(m_hWnd, &m_lstHeader, ScrollBarOrientation::Vertical);
	m_modernScrollBar.SetDPI(&m_DittoWindow.m_dpi);
	m_modernScrollBar.SetColors(
		CGetSetOptions::m_Theme.ScrollBarTrack(),
		CGetSetOptions::m_Theme.ScrollBarThumb(),
		CGetSetOptions::m_Theme.ScrollBarThumbHover());

	// Create modern scrollbar overlay (horizontal)
	m_modernScrollBarHorz.Create(m_hWnd, &m_lstHeader, ScrollBarOrientation::Horizontal);
	m_modernScrollBarHorz.SetDPI(&m_DittoWindow.m_dpi);
	m_modernScrollBarHorz.SetColors(
		CGetSetOptions::m_Theme.ScrollBarTrack(),
		CGetSetOptions::m_Theme.ScrollBarThumb(),
		CGetSetOptions::m_Theme.ScrollBarThumbHover());
	CRect rcTree(0, 0, 100, 100);
	m_GroupTree.Create(m_hWnd, rcTree, NULL, WS_CHILD | WS_VISIBLE | TVS_HASLINES | TVS_LINESATROOT | TVS_HASBUTTONS);
	m_GroupTree.ModifyStyle(WS_CAPTION | WS_TABSTOP, 0);

	m_GroupTree.SetNotificationWndEx(m_hWnd);
	m_GroupTree.ShowWindow(SW_HIDE);
	m_GroupTree.m_showRightClickMenu = true;

	m_ShowGroupsFolderBottom.Create(m_hWnd, rcZero, NULL, WS_CHILD | WS_VISIBLE | BS_OWNERDRAW | WS_TABSTOP, 0, ID_SHOW_GROUPS_BOTTOM);
	//m_ShowGroupsFolderBottom.LoadBitmaps(IDB_CLOSED_FOLDER, IDB_CLOSED_FOLDER_PRESSED, IDB_CLOSED_FOLDER_FOCUSED);
	m_ShowGroupsFolderBottom.LoadStdImageDPI(m_DittoWindow.m_dpi.GetDPI(), open_folder_24, open_folder_30, open_folder_36, open_folder_42, open_folder_48, _T("PNG"), open_folder_54, open_folder_60, open_folder_66, open_folder_72, open_folder_78, open_folder_84);
	m_ShowGroupsFolderBottom.ShowWindow(SW_SHOW);
	m_ShowGroupsFolderBottom.SetToolTipText(theApp.m_Language.GetString(_T("GroupsTooltip"), _T("Groups")));
	m_ShowGroupsFolderBottom.ModifyStyle(WS_TABSTOP, 0);

	m_BackButton.Create(m_hWnd, rcZero, NULL, WS_CHILD | WS_VISIBLE | BS_OWNERDRAW | WS_TABSTOP, 0, ID_BACK_BUTTON);
	m_BackButton.LoadStdImageDPI(m_DittoWindow.m_dpi.GetDPI(), return_16, return_20, return_24, return_28, return_32, _T("PNG"));
	m_BackButton.ModifyStyle(WS_TABSTOP, 0);
	m_BackButton.ShowWindow(SW_SHOW);

	m_systemMenu.Create(m_hWnd, rcZero, NULL, WS_CHILD | WS_VISIBLE | BS_OWNERDRAW | WS_TABSTOP, 0, ID_SYSTEM_BUTTON);
	m_systemMenu.LoadStdImageDPI(m_DittoWindow.m_dpi.GetDPI(), system_menu_2_24, system_menu_2_30, system_menu_2_36, system_menu_2_42, system_menu_2_48, _T("PNG"), system_menu_54, system_menu_60, system_menu_66, system_menu_72, system_menu_78, system_menu_84);
	m_systemMenu.ModifyStyle(WS_TABSTOP, 0);
	m_systemMenu.ShowWindow(SW_SHOW);

	m_stGroup.Create(m_hWnd, rcZero, _T(""), WS_CHILD | WS_VISIBLE, 0, ID_GROUP_TEXT);

	//Set the z-order
	m_lstHeader.SetWindowPos(m_hWnd, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
	m_search.SetWindowPos(m_lstHeader.m_hWnd, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
	m_ShowGroupsFolderBottom.SetWindowPos(m_search.m_hWnd, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);

	//LVS_EX_FLATSB
	m_lstHeader.SetExtendedListViewStyle(LVS_EX_FULLROWSELECT | LVS_EX_HEADERDRAGDROP);

	// Create the columns
	if (m_lstHeader.InsertColumn(0, _T(""), LVCFMT_LEFT, 2500, 0) != 0)
	{
		ASSERT(FALSE);
		return -1;
	}

	m_Alpha.SetWindowHandle(m_hWnd);

	CString onTopMsg = theApp.m_Language.GetString(_T("TurnOfAlwaysOntop"), _T("Always on Top Enabled"));
	CString shortcutText = m_actions.GetCmdKeyText(ActionEnums::TOGGLESHOWPERSISTANT);
	if (shortcutText != _T("") &&
		shortcutText.Find("\t" + shortcutText) < 0)
	{
		onTopMsg += "\t(";
		onTopMsg += shortcutText;
		onTopMsg += ")";
	}

	m_alwaysOnToWarningStatic.Create(m_hWnd, rcZero, onTopMsg, WS_CHILD | SS_CENTERIMAGE | SS_NOTIFY, 0, ON_TOP_WARNING);
	m_alwaysOnToWarningStatic.SetBkColor(COLORREF(RGB(255, 255, 0)));
	m_alwaysOnToWarningStatic.SetTextColor(COLORREF(RGB(0, 0, 255)));
	m_alwaysOnToWarningStatic.SetToggleCursor(true);
	m_alwaysOnToWarningStatic.SetFont(m_groupFont);

	m_noSearchResultsStatic.Create(m_hWnd, rcZero, onTopMsg, WS_CHILD, 0, ID_NO_SEARCH_RESULTS);

	m_popupMsg.m_hWndPosRelativeTo = m_hWnd;

	UpdateFont();

	m_thread.Start(this);
	m_extraDataThread.Start(this);

	LoadShortcuts();

	InvalidateNc();

	return 0;
}

void CQPasteWnd::LoadShortcuts()
{
	m_modifierKeyActions.RemoveAll();

	m_modifierKeyActions.m_checkModifierKeys = false;
	m_modifierKeyActions.AddAccel(ActionEnums::MODIFIER_ACTVE_SELECTIONUP, VK_UP);
	m_modifierKeyActions.AddAccel(ActionEnums::MODIFIER_ACTVE_SELECTIONDOWN, VK_DOWN);
	m_modifierKeyActions.AddAccel(ActionEnums::MODIFIER_ACTVE_MOVEFIRST, VK_HOME);
	m_modifierKeyActions.AddAccel(ActionEnums::MODIFIER_ACTVE_MOVELAST, VK_END);

	m_actions.RemoveAll();
	m_toolTipActions.RemoveAll();

	m_actions.AddAccel(ActionEnums::NEXTTABCONTROL, VK_TAB);
	m_actions.AddAccel(ActionEnums::PREVTABCONTROL, ACCEL_MAKEKEY(VK_TAB, HOTKEYF_CONTROL));
	m_actions.AddAccel(ActionEnums::BACKGRROUP, VK_BACK);
	m_actions.AddAccel(ActionEnums::DELETE_SELECTED, VK_DELETE);

	m_actions.AddAccel(ActionEnums::HOMELIST, VK_HOME);
	m_actions.AddAccel(ActionEnums::SHOWMENU, VK_APPS);
	m_actions.AddAccel(ActionEnums::SYSTEM_MENU, ACCEL_MAKEKEY(VK_APPS, HOTKEYF_CONTROL));

	m_search.SetLastSearchAccel(CAccel(0, ActionEnums::APPLY_LAST_SEARCH, 0));

	for (DWORD i = ActionEnums::FIRST_ACTION + 1; i < ActionEnums::LAST_ACTION; i++)
	{
		ActionEnums::ActionEnumValues action = (ActionEnums::ActionEnumValues)i;

		if (ActionEnums::UserConfigurable(action))
		{
			for (int i = 0; i < 10; i++)
			{
				int a = CGetSetOptions::GetActionShortCutA(action, i);
				if (a > 0)
				{
					int b = CGetSetOptions::GetActionShortCutB(action, i);
					m_actions.AddAccel(action, a, b);

					//always add a shift variation to show description F3 so it will search backwards in the text search
					if (action == ActionEnums::SHOWDESCRIPTION)
					{
						int shift = HOTKEYF_SHIFT;
						if ((HIBYTE(a) & HOTKEYF_SHIFT))
						{
							shift = 0;
						}

						m_actions.AddAccel(action, ACCEL_MAKEKEY(LOBYTE(a), shift), b);
					}
					else if (action == ActionEnums::APPLY_LAST_SEARCH)
					{
						m_search.SetLastSearchAccel(CAccel(a, action, b));
					}

					if (ActionEnums::ToolTipAction(action))
					{
						m_toolTipActions.AddAccel(action, a, b);

						if (action == ActionEnums::SHOWDESCRIPTION)
						{
							int shift = HOTKEYF_SHIFT;
							if ((HIBYTE(a) & HOTKEYF_SHIFT))
							{
								shift = 0;
							}

							m_toolTipActions.AddAccel(action, ACCEL_MAKEKEY(LOBYTE(a), shift), b);
						}
					}
				}
			}
		}
	}

	for (auto& element : CGetSetOptions::m_pasteScripts.m_list)
	{
		for (int i = 0; i < 10; i++)
		{
			int a = CGetSetOptions::GetActionShortCutA(ActionEnums::PASTE_SCRIPT, i, element.m_guid);
			if (a > 0)
			{
				int b = CGetSetOptions::GetActionShortCutB(ActionEnums::PASTE_SCRIPT, i, element.m_guid);
				m_actions.AddAccel(ActionEnums::PASTE_SCRIPT, a, b, element.m_guid);
			}
		}
	}

	m_actions.AddAccel(ActionEnums::TOGGLEFILELOGGING, ACCEL_MAKEKEY('D', HOTKEYF_CONTROL), ACCEL_MAKEKEY('F', HOTKEYF_CONTROL));
	m_actions.AddAccel(ActionEnums::TOGGLEOUTPUTDEBUGSTRING, ACCEL_MAKEKEY('D', HOTKEYF_CONTROL), ACCEL_MAKEKEY('O', HOTKEYF_CONTROL));

	m_lstHeader.SetTooltipActions(&m_toolTipActions);
}

void CQPasteWnd::SetSearchImages()
{
	//int iSourceImageDPIToUse = 96; // We will assume 96 by default.

	//if (m_DittoWindow.m_dpi.GetDPI() > 144) 
	//	iSourceImageDPIToUse = 192;
	//else if (m_DittoWindow.m_dpi.GetDPI() > 120) 
	//	iSourceImageDPIToUse = 144;
	//else if (m_DittoWindow.m_dpi.GetDPI() > 96) 
	//	iSourceImageDPIToUse = 120;

	//// Now select the right resource to load.
	//switch(iSourceImageDPIToUse)
	//{
	//case 120: 
	//	m_search.SetBitmaps(IDB_BITMAP_SEARCH_NORMAL_125, IDB_BITMAP_SEARCH_CLOSE_125);
	//	break;
	//case 144: 
	//	m_search.SetBitmaps(IDB_BITMAP_SEARCH_NORMAL_150, IDB_BITMAP_SEARCH_CLOSE_150);
	//	break;
	//case 192: 
	//	m_search.SetBitmaps(IDB_BITMAP_SEARCH_NORMAL_200, IDB_BITMAP_SEARCH_CLOSE_200);
	//	break;						
	//default: // default to 96 DPI
	//	m_search.SetBitmaps(IDB_BITMAP_SEARCH_NORMAL, IDB_BITMAP_SEARCH_CLOSE);
	//	break;
	//}
}

LRESULT CQPasteWnd::OnSize(UINT nType, CSize size)
{
	__super::OnSize(nType, size);

	if (!::IsWindow(m_lstHeader.m_hWnd))
	{
		return 0;
	}

	m_popupMsg.Hide();

	MoveControls();
	return 0;
}

void CQPasteWnd::MoveControls()
{
	CRect crRect;
	GetClientRect(crRect);
	int cx = crRect.Width();
	int cy = crRect.Height();

	//Hide the two pixels of space at the top, not sure where this is coming from
	int topOfListBox = 0;

	if (theApp.m_GroupID > 0 && m_bShowStarredClips == false)
	{
		m_stGroup.ShowWindow(SW_SHOW);
		m_BackButton.ShowWindow(SW_SHOW);

		m_BackButton.MoveWindow(m_DittoWindow.m_dpi.Scale(2), m_DittoWindow.m_dpi.Scale(2), m_DittoWindow.m_dpi.Scale(16), m_DittoWindow.m_dpi.Scale(16));
		m_stGroup.MoveWindow(m_DittoWindow.m_dpi.Scale(24), m_DittoWindow.m_dpi.Scale(2), cx - m_DittoWindow.m_dpi.Scale(20), m_DittoWindow.m_dpi.Scale(16));

		topOfListBox = m_DittoWindow.m_dpi.Scale(20);
	}
	else
	{
		m_BackButton.ShowWindow(SW_HIDE);
		m_stGroup.ShowWindow(SW_HIDE);
	}

	int searchRowStart = 33;

	/*if(CGetSetOptions::m_bShowPersistent)
	{
		searchRowStart = 41;
	}*/

	int nWidth = cx;
	int listBoxBottomOffset = m_DittoWindow.m_dpi.Scale(searchRowStart);

	int extraSize = 0;

	// Hide native scrollbar if using modern scrollbar OR if scrollbar is set to not always show
	bool hideNativeScrollbar = CGetSetOptions::m_useModernScrollBar || 
		(m_showScrollBars == false && CGetSetOptions::m_showScrollBar == false);

	if (hideNativeScrollbar)
	{
		extraSize = m_DittoWindow.m_dpi.Scale(::GetSystemMetrics(SM_CXVSCROLL));

		CRgn rgnRect;
		CRect r;
		m_lstHeader.GetWindowRect(&r);

		rgnRect.CreateRectRgn(0, 0, cx, (cy - listBoxBottomOffset - topOfListBox) );

		m_lstHeader.SetWindowRgn(rgnRect, TRUE);
	}
	else
	{
		// Clear region to show native scrollbar
		m_lstHeader.SetWindowRgn(NULL, TRUE);
	}

	if (m_noSearchResults &&
		(m_strSearch != _T("") || m_bShowStarredClips))
	{
		m_lstHeader.ShowWindow(SW_HIDE);
		m_noSearchResultsStatic.ShowWindow(SW_SHOW);
		m_modernScrollBar.ShowWindow(SW_HIDE);
		m_modernScrollBarHorz.ShowWindow(SW_HIDE);

		auto border = m_DittoWindow.m_dpi.Scale(10);
		m_noSearchResultsStatic.MoveWindow(border, topOfListBox + border, cx - border, cy - listBoxBottomOffset - topOfListBox + 1 - border);
	}
	else
	{
		m_lstHeader.ShowWindow(SW_SHOW);
		m_noSearchResultsStatic.ShowWindow(SW_HIDE);

		m_lstHeader.MoveWindow(0, topOfListBox, cx + extraSize, cy - listBoxBottomOffset - topOfListBox + extraSize + 1);
		
		// Update modern scrollbar position and visibility (only if enabled)
		if (CGetSetOptions::m_useModernScrollBar)
		{
			if (CGetSetOptions::m_showScrollBar)
			{
				m_modernScrollBar.UpdateScrollBar();
				m_modernScrollBar.Show(false);
				m_modernScrollBarHorz.UpdateScrollBar();
				m_modernScrollBarHorz.Show(false);
			}
			else
			{
				m_modernScrollBar.UpdateScrollBar();
				m_modernScrollBar.Hide(false);
				m_modernScrollBarHorz.UpdateScrollBar();
				m_modernScrollBarHorz.Hide(false);
			}
		}
		else
		{
			m_modernScrollBar.Hide(false);
			m_modernScrollBarHorz.Hide(false);
		}
	}
	m_search.MoveWindow(m_DittoWindow.m_dpi.Scale(34), cy - m_DittoWindow.m_dpi.Scale(searchRowStart - 5), cx - m_DittoWindow.m_dpi.Scale(70), m_DittoWindow.m_dpi.Scale(25));

	m_systemMenu.MoveWindow(cx - m_DittoWindow.m_dpi.Scale(30), cy - m_DittoWindow.m_dpi.Scale(28), m_DittoWindow.m_dpi.Scale(24), m_DittoWindow.m_dpi.Scale(24));

	m_ShowGroupsFolderBottom.MoveWindow(m_DittoWindow.m_dpi.Scale(4), cy - m_DittoWindow.m_dpi.Scale(28), m_DittoWindow.m_dpi.Scale(24), m_DittoWindow.m_dpi.Scale(24));

	/*if (CGetSetOptions::m_bShowPersistent &&
		CGetSetOptions::m_bShowAlwaysOnTopWarning)
	{
		m_alwaysOnToWarningStatic.ShowWindow(SW_SHOW);
		m_alwaysOnToWarningStatic.MoveWindow(m_DittoWindow.m_dpi.Scale(2), cy - m_DittoWindow.m_dpi.Scale(18), cx - m_DittoWindow.m_dpi.Scale(4), m_DittoWindow.m_dpi.Scale(17));
	}
	else*/
	{
		m_alwaysOnToWarningStatic.ShowWindow(SW_HIDE);
	}
}

void CQPasteWnd::OnSetFocus(CWindow wndOld)
{
	if (::IsWindow(m_lstHeader.m_hWnd))
	{
		m_lstHeader.SetFocus();
	}
}

void CQPasteWnd::OnKillFocus(CWindow wndLostFocus)
{
}

void CQPasteWnd::OnActivate(UINT nState, BOOL bMinimized, CWindow wndOther)
{

	if (m_bHideWnd == false || (m_lstHeader.GetToolTipHWnd() != NULL && m_lstHeader.GetToolTipHWnd() == wndOther.m_hWnd))
	{
		return;
	}

	Log(StrF(_T("CQPasteWnd::OnActivate, nState: %d, Other: %d, Minimized: %d"), nState, wndOther.m_hWnd, bMinimized));

	if (nState == WA_INACTIVE)
	{
		SaveWindowSize();

		m_bModifersMoveActive = false;

		if (!CGetSetOptions::m_bShowPersistent && !CGetSetOptions::m_bDoNotHideOnDeactivate)
		{
			HideQPasteWindow(false);
		}
		else if (CGetSetOptions::GetAutoHide())
		{
			MinMaxWindow(FORCE_MIN);
		}

		//re register the global hot keys for the last ten
		if (theApp.m_bAppExiting == false)
		{
			g_HotKeys.RegisterAll();
		}

		m_lstHeader.HidePopup(true);
	}
	else if (nState == WA_ACTIVE || nState == WA_CLICKACTIVE)
	{
		if (bMinimized == FALSE)
		{
			if (theApp.m_bShowingQuickPaste == false)
			{
				BOOL fillList = FALSE;
				if (m_listItems.size() == 0)
				{
					fillList = TRUE;
				}
				else if (theApp.m_databaseOnNetworkShare)
				{
					__int64 lastWrite = GetLastWriteTime(CGetSetOptions::GetDBPath());
					if (lastWrite > m_lastDbWrite)
					{
						m_lastDbWrite = lastWrite;
						fillList = TRUE;
					}
				}

				ShowQPasteWindow(fillList);
			}

			//Unregister the global hot keys for the last ten copies
			g_HotKeys.UnregisterAll(false, true);
		}
	}
}

BOOL CQPasteWnd::HideQPasteWindow(bool releaseFocus, BOOL clearSearchData)
{
	if (clearSearchData = -1)
	{
		if ((CGetSetOptions::m_maintainSearchView || CGetSetOptions::m_refreshViewAfterPasting == false) &&
			m_strSearch != _T(""))
		{
			Log(_T("Currently searching for something and setting to maintain search view is enabled, not refreshing"));
			clearSearchData = FALSE;
		}
		else
		{
			clearSearchData = TRUE;
		}
	}

	Log(_T("Start of HideQPasteWindow"));
	DWORD startTick = GetTickCount();

	if (!theApp.m_bShowingQuickPaste)
	{
		Log(_T("End of HideQPasteWindow, !theApp.m_bShowingQuickPaste"));
	}

	{
		ATL::CCritSecLock csLock(m_CritSection);

		m_bStopQuery = true;
	}

	theApp.m_bShowingQuickPaste = false;

	//needs to be before we hide our window - inorder to set focus to another window we need to be the foreground window
	//http://msdn.microsoft.com/en-us/library/windows/desktop/ms632668%28v=vs.85%29.aspx
	if (releaseFocus)
	{
		theApp.m_activeWnd.ReleaseFocus();
	}

	KillTimer(TIMER_FILL_CACHE);

	m_lstHeader.HidePopup(true);

	m_popupMsg.Hide();

	//Save the size
	SaveWindowSize();

	if (CGetSetOptions::GetShowInTaskBar() && !CGetSetOptions::GetHideTaskbarIconOnClose())
	{
		ShowWindow(SW_MINIMIZE);
	}
	else
	{
		ShowWindow(SW_HIDE);
	}

	if (clearSearchData == TRUE)
	{
		//Reset the selection in the search combo
		m_bHandleSearchTextChange = false;
		m_search.SetWindowText(_T(""));
		m_bHandleSearchTextChange = true;
		m_bShowStarredClips = false;

		if (m_strSQLSearch.IsEmpty() == FALSE || m_pendingRefresh)
		{
			{
				ATL::CCritSecLock csLock(m_CritSection);

				m_bStopQuery = true;
			}

			//Wait for the thread to stop fill the cache so we can clear it
			WaitForSingleObject(m_thread.m_SearchingEvent, 5000);

			{
				ATL::CCritSecLock csLock(m_CritSection);

				m_listItems.clear();
				m_lstHeader.SetItemCountEx(0);
			}
		}
	}

	if (theApp.m_GroupID > 0 &&
		CGetSetOptions::GetRevertToTopLevelGroup())
	{
		theApp.EnterGroupID(-1);
	}
	else
	{
		theApp.TryEnterOldGroupState();
	}

	m_pendingRefresh = false;

	DWORD endTick = GetTickCount();
	if ((endTick - startTick) > 150)
		Log(StrF(_T("Paste Timing HideQPasteWindow: %d"), endTick - startTick));

	Log(StrF(_T("End of HideQPasteWindow, ItemCount: %d"), m_listItems.size()));

	return TRUE;
}

void CQPasteWnd::SaveWindowSize()
{
	if (this->IsIconic() == FALSE)
	{
		CRect rect;
		GetWindowRectEx(&rect);
		CSize s = rect.Size();
		CGetSetOptions::SetQuickPasteSize(CSize(m_DittoWindow.m_dpi.UnScale(s.cx), m_DittoWindow.m_dpi.UnScale(s.cy)));
		CGetSetOptions::SetQuickPastePoint(rect.TopLeft());
	}
}

BOOL CQPasteWnd::ShowQPasteWindow(BOOL bFillList)
{
	theApp.m_bShowingQuickPaste = true;

	Log(StrF(_T("Start - ShowQPasteWindow - Fill List: %d, array count: %d"), bFillList, m_listItems.size()));

	//Ensure we have the latest theme file, this checks the last write time so it doesn't read the file each time
	CGetSetOptions::m_Theme.Load(CGetSetOptions::GetTheme(), false, true);

	SetCaptionColorActive(CGetSetOptions::m_bShowPersistent, theApp.GetConnectCV());
	SetCaptionOn(CGetSetOptions::GetCaptionPos(), true, CGetSetOptions::m_Theme.GetCaptionSize(), CGetSetOptions::m_Theme.GetCaptionFontSize());

	UpdateStatus();

	m_bHideWnd = true;

	SetCurrentTransparency();

	m_lstHeader.SetNumberOfLinesPerRow(CGetSetOptions::GetLinesPerRow(), false);
	m_lstHeader.SetShowTextForFirstTenHotKeys(CGetSetOptions::GetShowTextForFirstTenHotKeys());
	m_lstHeader.SetShowIfClipWasPasted(CGetSetOptions::GetShowIfClipWasPasted());

	if (bFillList)
	{
		FillList();
	}
	else
	{
		MoveControls();
	}

	// always on top... for persistent showing (CGetSetOptions::m_bShowPersistent)
	// SHOWWINDOW was also integrated into this function rather than calling it separately
	if (CGetSetOptions::GetShowPersistent())
	{
		::SetWindowPos(m_hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_SHOWWINDOW);
	}

	//SetKeyModiferState(true);

	Log(StrF(_T("END - ShowQPasteWindow - Fill List: %d, array count: %d"), bFillList, m_listItems.size()));

	return TRUE;
}

bool CQPasteWnd::Add(const CString& csHeader, const CString& csText, int nID)
{
	int nNewIndex;

	if ((nNewIndex = m_lstHeader.InsertItem(m_lstHeader.GetItemCount(), csHeader)) == -1)
	{
		return false;
	}

	m_lstHeader.SetItemData(nNewIndex, nID);

	return true;
}

BOOL CQPasteWnd::OpenID(int id, CSpecialPasteOptions pasteOptions)
{
	Log(StrF(_T("Start OpenId, Id: %d, Only CF_TEXT: %s"), id, pasteOptions.ToString()));

	if (pasteOptions.m_pPasteFormats == NULL)
	{
		if (theApp.EnterGroupID(id, FALSE, FALSE))
		{
			Log(_T("Entered group"));
			return TRUE;
		}
	}

	// else, it is a clip, so paste it
	CProcessPaste paste;

	paste.m_bSendPaste = CGetSetOptions::m_bSendPasteMessageAfterSelection == TRUE ? true : false;
	paste.m_pasteOptions = pasteOptions;
	paste.m_pastedFromGroup = (theApp.m_GroupID > 0);

	paste.GetClipIDs().Add(id);

	if (paste.DoPaste())
	{
		theApp.OnPasteCompleted();

		if (CGetSetOptions::m_bSendPasteMessageAfterSelection == FALSE)
		{
			theApp.m_activeWnd.ActivateTarget();
		}

		if (CGetSetOptions::m_bShowPersistent && CGetSetOptions::GetAutoHide())
		{
			MinMaxWindow(FORCE_MIN);
		}
	}
	else
	{
		CString errorMessage;
		errorMessage.Format(_T("Paste Error - %s"), paste.m_lastErrorMessage);
		m_popupMsg.Show(errorMessage, CPoint(0, 0), true);
		SetTimer(TIMER_ERROR_MSG, CGetSetOptions::GetErrorMsgPopupTimeout(), NULL);
	}

	Log(StrF(_T("End OpenId, Id: %d, Only CF_TEXT: %s"), id, pasteOptions.ToString()));

	return TRUE;
}

BOOL CQPasteWnd::OpenSelection(CSpecialPasteOptions pasteOptions)
{
	Log(_T("Start Open Selection"));
	ARRAY IDs;
	m_lstHeader.GetSelectionItemData(IDs);

	INT_PTR count = IDs.GetSize();

	if (count <= 0)
	{
		return FALSE;
	}

	if (count == 1)
	{
		return OpenID(IDs[0], pasteOptions);
	}

	CProcessPaste paste;

	paste.m_bSendPaste = CGetSetOptions::m_bSendPasteMessageAfterSelection == TRUE ? true : false;
	paste.m_pasteOptions = pasteOptions;
	paste.m_pastedFromGroup = (theApp.m_GroupID > 0);


	paste.GetClipIDs().Copy(IDs);
	if (paste.DoPaste())
	{
		theApp.OnPasteCompleted();

		if (CGetSetOptions::m_bSendPasteMessageAfterSelection == FALSE)
		{
			theApp.m_activeWnd.ActivateTarget();
		}

		if (CGetSetOptions::m_bShowPersistent && CGetSetOptions::GetAutoHide())
		{
			MinMaxWindow(FORCE_MIN);
		}
	}
	else
	{
		CString errorMessage;
		errorMessage.Format(_T("Paste Error - %s"), paste.m_lastErrorMessage);
		m_popupMsg.Show(errorMessage, CPoint(0, 0), true);
		SetTimer(TIMER_ERROR_MSG, CGetSetOptions::GetErrorMsgPopupTimeout(), NULL);
	}

	Log(_T("End Open Selection"));
	return TRUE;
}

BOOL CQPasteWnd::OpenIndex(int item, bool plainTextOnly)
{
	if (item >= m_lstHeader.GetItemCount())
	{
		return FALSE;
	}

	CSpecialPasteOptions pasteOptions;
	pasteOptions.m_pasteAsPlainText = plainTextOnly;
	return OpenID(m_lstHeader.GetItemData(item), pasteOptions);
}

BOOL CQPasteWnd::NewGroup(bool bGroupSelection, int parentId)
{
	CGroupName Name;
	CString csName("");

	if (CGetSetOptions::m_bPrompForNewGroupName)
	{
		m_bHideWnd = false;

		CDimWnd dimmer(m_hWnd);

		INT_PTR nRet = Name.DoModal();

		m_bHideWnd = true;

		if (nRet == IDOK)
		{
			csName = Name.m_csName;
		}
		else
		{
			return false;
		}
	}

	int id = NewGroupID(parentId, csName);

	if (id <= 0)
	{
		return FALSE;
	}

	if (!bGroupSelection)
	{
		theApp.m_FocusID = id; // focus on the new group
		FillList();
		return TRUE;
	}

	CClipIDs IDs;
	m_lstHeader.GetSelectionItemData(IDs);
	IDs.MoveTo(id);
	theApp.EnterGroupID(id);
	return TRUE;
}

LRESULT CQPasteWnd::OnListSelect_DB_ID(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	CSpecialPasteOptions pasteOptions;
	OpenID((int)wParam, pasteOptions);
	return TRUE;
}

LRESULT CQPasteWnd::OnListMoveSelectionToGroup(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	int groupId = (int)wParam;
	if (groupId >= -1)
	{
		CClipIDs IDs;
		m_lstHeader.GetSelectionItemData(IDs);

		IDs.MoveTo(groupId);
	}
	return TRUE;
}

LRESULT CQPasteWnd::OnCopyClip(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	DoCopySelection();
	return TRUE;
}

LRESULT CQPasteWnd::OnSearchEnterKeyPressed(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	CString csText;
	m_search.GetWindowText(csText);

	MSG msg;
	msg.lParam = 0;
	msg.wParam = VK_RETURN;
	msg.message = WM_KEYDOWN;
	if (CheckActions(&msg) == false)
	{
	}
	return TRUE;
}

LRESULT CQPasteWnd::OnListEnd(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	return 0;
}

LRESULT CQPasteWnd::OnReloadClipInUI(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	BOOL foundClip = FALSE;
	int clipId = (int)wParam;
	int updateFlags = (int)lParam;

	if (CGetSetOptions::m_maintainSearchView &&
		m_strSearch != _T("") &&
		updateFlags & UPDATE_AFTER_PASTE_SELECT_CLIP)
	{
		Log(_T("Currently searching for something and setting to maintain search view is enabled, not refreshing clip order"));
		return FALSE;
	}

	DWORD startTick = GetTickCount();

	theApp.m_FocusID = -1;

	CppSQLite3Query q = theApp.m_db.execQueryEx(_T("SELECT clipOrder, clipGroupOrder, lastPasteDate, mText FROM Main WHERE lID = %d"), clipId);
	if (q.eof() == false)
	{
		double order = q.getFloatField(_T("clipOrder"));
		double orderGroup = q.getFloatField(_T("clipGroupOrder"));
		auto lastPasted = q.getInt64Field(_T("lastPasteDate"));
		CString description = q.getStringField(_T("mText"));

		std::vector<CMainTable>::iterator iter = m_listItems.begin();
		while (iter != m_listItems.end())
		{
			if (iter->m_lID == clipId)
			{
				if (updateFlags & UPDATE_AFTER_PASTE_SELECT_CLIP)
				{
					iter->m_datePasted = lastPasted;

					if (iter->m_clipOrder != order || iter->m_clipGroupOrder != orderGroup)
					{
						iter->m_clipOrder = order;
						iter->m_clipGroupOrder = orderGroup;

						if (theApp.m_GroupID > 0)
						{
							std::sort(m_listItems.begin(), m_listItems.end(), CMainTable::GroupSortDesc);
						}
						else
						{
							std::sort(m_listItems.begin(), m_listItems.end(), CMainTable::SortDesc);
						}
					}

					foundClip = TRUE;

					m_lstHeader.RefreshVisibleRows();
					m_lstHeader.RedrawWindow();
					SelectFocusID();
				}
				else if (updateFlags & UPDATE_CLIP_DESCRIPTION)
				{
					iter->m_Desc = description;

					foundClip = TRUE;

					RemoveFromImageRtfCache(-1, clipId);

					m_lstHeader.RefreshVisibleRows();
					m_lstHeader.RedrawWindow();
				}

				break;
			}

			iter++;
		}
	}

	DWORD endTick = GetTickCount();
	if ((endTick - startTick) > 150)
		Log(StrF(_T("Paste Timing OnReloadClipInUI: %d, ClipId: %d"), endTick - startTick, clipId));

	return foundClip;
}

LRESULT CQPasteWnd::OnRefreshView(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	MSG msg;
	// remove all additional refresh view messages from the queue
	while (::PeekMessage(&msg, m_hWnd, WM_REFRESH_VIEW, WM_REFRESH_VIEW, PM_REMOVE)) {}

	if (theApp.m_bShowingQuickPaste)
	{
		CopyReasonEnum::CopyReason copyReason = (CopyReasonEnum::CopyReason)wParam;
		if (copyReason == CopyReasonEnum::COPY_FROM_TOOLTIP)
		{
			m_pendingRefresh = true;
			return FALSE;
		}
	}

	Log(_T("OnRefreshView - Start"));
	CString action;

	theApp.m_FocusID = -1;

	m_bHandleSearchTextChange = false;

	m_search.SetWindowText(_T(""));
	m_bHandleSearchTextChange = true;

	if (theApp.m_bShowingQuickPaste)
	{
		FillList(_T(""));
		action = _T("Filled List");
	}
	else
	{
		//Wait for the thread to stop fill the cache so we can clear it
		WaitForSingleObject(m_thread.m_SearchingEvent, 5000);

		{
			ATL::CCritSecLock csLock(m_CritSection);
			m_listItems.clear();
		}

		m_lstHeader.SetItemCountEx(0);
		UpdateStatus();

		action = _T("Cleared Items");
	}

	Log(StrF(_T("OnRefreshView - End - Count: %d, Action: %s"), m_listItems.size(), action));

	return TRUE;
}

void CQPasteWnd::RefreshNc()
{
	if (!theApp.m_bShowingQuickPaste)
	{
		return;
	}

	InvalidateNc();
}

void CQPasteWnd::UpdateStatus(bool bRepaintImmediately)
{
	CString title = m_Title;

	if (CGetSetOptions::m_bShowPersistent)
	{
		title = (StrF(_T("%s %s"), _T(QPASTE_TITLE), theApp.m_Language.GetString("top_window", "[Always on top]")));
	}

	if (theApp.IsClipboardViewerConnected() == FALSE)
	{
		title += _T(" ");
		title += theApp.m_Language.GetString("disconnected", "[Disconnected]");
	}

	if (m_bShowStarredClips)
	{
		title += _T(" ");
		title += theApp.m_Language.GetString("starred_clips", "[Starred clips]");
	}

	CString cs;
	cs.Format(_T(" - %d/%d"), m_lstHeader.GetSelectedCount(), m_lstHeader.GetItemCount());
	title += cs;

	if (theApp.m_Status != "")
	{
		title += " [ ";
		title += theApp.m_Status;
		title += " ] - ";
	}
	else
	{
		title += " - ";
	}

	if (::IsWindow(theApp.m_activeWnd.ActiveWnd()))
	{
		title += theApp.m_activeWnd.ActiveWndName();
	}
	else
	{
		title += theApp.m_Language.GetString("No_Target", "No target");
	}

	SetToolTipText(title);

	CString windowTitle = _T(QPASTE_TITLE);

	if (CGetSetOptions::m_bShowPersistent)
	{
		windowTitle += StrF(_T(" %s"), theApp.m_Language.GetString("top_window", "[Always on top]"));
	}

	if (theApp.IsClipboardViewerConnected() == FALSE)
	{
		windowTitle += StrF(_T(" %s"), theApp.m_Language.GetString("disconnected", "[Disconnected]"));
	}

	if (m_bShowStarredClips)
	{
		windowTitle += StrF(_T(" %s"), theApp.m_Language.GetString("starred_clips", "[Starred clips]"));
	}

	SetCustomWindowTitle(windowTitle);
}

BOOL CQPasteWnd::FillList(CString csSQLSearch)
{
	KillTimer(TIMER_DO_SEARCH);

	m_lstHeader.HidePopup(true);

	Log(StrF(_T("Start Fill List - %s"), csSQLSearch));

	m_lstHeader.SetSearchText(csSQLSearch);

	{
		ATL::CCritSecLock csLock(m_CritSection);
		m_bStopQuery = true;
	}

	CString strFilter;
	CString strParentFilter;
	CString strStarredFilter = _T("Main.bIsGroup = 0 AND Main.lDontAutoDelete > 0");
	CString csSort;

	// History Groupiter->m_stickyClipGroupOrder = clip.m_stickyClipGroupOrder;
	if (m_bShowStarredClips)
	{
		csSort = "Main.stickyClipOrder DESC, "
			"Main.bIsGroup ASC, "
			"Main.clipOrder DESC";

		strFilter = strStarredFilter;
	}
	else if (theApp.m_GroupID < 0)
	{
		//do not change this this directly relates to the views in the Main table
		csSort = "Main.stickyClipOrder DESC, "
			"Main.bIsGroup ASC, "
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
		// it's some other group
	{
		//do not change this this directly relates to the views in the Main table
		csSort = "Main.stickyClipGroupOrder DESC, "
			"Main.bIsGroup ASC, "
			"Main.clipGroupOrder DESC";

		//Main.stickyClipGroupOrder DESC, Main.clipGroupOrder DESC";//

		if (theApp.m_GroupID >= 0)
		{
			strFilter.Format(_T("Main.lParentID = %d"), theApp.m_GroupID);
			strParentFilter = strFilter;
		}

		m_stGroup.SetWindowText(theApp.m_GroupText);
	}

	CRect crRect;
	GetClientRect(crRect);

	CString csSQL;
	CString dataJoin;
	CString IsDistinct = _T("");

	CString sqlSearch = "";

	if (csSQLSearch == "")
	{
		m_strSQLSearch = m_bShowStarredClips ? strFilter : _T("");
		m_strSearch = "";
	}
	else
	{
		CFormatSQL descriptionFormat;
		CString descriptionSql;
		CFormatSQL quickPasteFormat;
		CString quickPasteSql;
		CFormatSQL fullTextFormat;
		CString fullTextSql;

		//If other are off then always search the description
		if (CGetSetOptions::GetSearchDescription() ||
			(CGetSetOptions::GetSearchFullText() == FALSE && CGetSetOptions::GetSearchQuickPaste() == FALSE))
		{
			descriptionFormat.SetVariable("Main.mText");

			descriptionFormat.Parse(csSQLSearch);
			descriptionSql = descriptionFormat.GetSQLString();
		}

		if (csSQLSearch.Left(3) == _T("/q ") ||
			csSQLSearch.Left(3) == _T("\\q ") ||
			CGetSetOptions::GetSearchQuickPaste())
		{
			quickPasteFormat.SetVariable("Main.QuickPasteText");

			if (csSQLSearch.Left(3) == _T("/q ") ||
				csSQLSearch.Left(3) == _T("\\q "))
			{
				csSQLSearch = csSQLSearch.Mid(3);
			}

			quickPasteFormat.Parse(csSQLSearch);
			quickPasteSql = quickPasteFormat.GetSQLString();
		}

		if (csSQLSearch.Left(3) == _T("/f ") ||
			csSQLSearch.Left(3) == _T("\\f ") ||
			CGetSetOptions::GetSearchFullText())
		{
			dataJoin = _T("INNER JOIN Data on Data.lParentID = Main.lID");

			if (csSQLSearch.Left(3) == _T("/f ") ||
				csSQLSearch.Left(3) == _T("\\f "))
			{
				csSQLSearch = csSQLSearch.Mid(3);
			}

			fullTextFormat.SetVariable("Data.ooData");
			fullTextFormat.Parse(csSQLSearch);
			fullTextSql = fullTextFormat.GetSQLString();

			fullTextSql.Insert(1, _T("Data.strClipBoardFormat = 'CF_UNICODETEXT' AND "));

			//If we are also search for other text make sure we only get one entry, including the data rows will cause multiple rows to be returned
			if (descriptionSql != _T(""))
			{
				IsDistinct = _T("DISTINCT");
			}

			if (quickPasteSql != _T(""))
			{
				IsDistinct = _T("DISTINCT");
			}
		}

		strFilter = _T("(");

		if (descriptionSql != _T(""))
		{
			strFilter += descriptionSql;
		}

		if (quickPasteSql != _T(""))
		{
			if (descriptionSql != _T(""))
			{
				strFilter += _T(" OR ");
			}

			strFilter += quickPasteSql;
		}

		if (fullTextSql != _T(""))
		{
			if (descriptionSql != _T("") ||
				quickPasteSql != _T(""))
			{
				strFilter += _T(" OR ");
			}

			strFilter += fullTextSql;
		}

		strFilter += _T(")");

		if (strParentFilter.IsEmpty() == FALSE)
		{
			strFilter += " AND ";
			strFilter += strParentFilter;
		}

		if (m_bShowStarredClips)
		{
			strFilter += " AND (";
			strFilter += strStarredFilter;
			strFilter += ")";
		}

		m_strSQLSearch = strFilter;
		m_strSearch = csSQLSearch;
	}

	CString sql;
	CString countSql;

	//Format the count and select sql queries for the thread
	countSql.Format(_T("SELECT COUNT(%s Main.lID) FROM Main %s where %s"), IsDistinct, dataJoin, strFilter);

	sql.Format(_T("SELECT %s Main.lID, Main.mText, Main.lParentID, Main.lDontAutoDelete, ")
		_T("Main.lShortCut, Main.bIsGroup, Main.QuickPasteText, Main.clipOrder, Main.clipGroupOrder, ")
		_T("Main.stickyClipOrder, Main.stickyClipGroupOrder, Main.lDate, Main.lastPasteDate FROM Main %s ")
		_T("where %s order by %s"), IsDistinct, dataJoin, strFilter, csSort);


	{
		ATL::CCritSecLock csLock(m_CritSection);
		m_listItems.clear();
	}

	m_noSearchResults = false;
	m_lstHeader.SetItemCount(0);
	m_lstHeader.RefreshVisibleRows();

	CPoint loadItem(-1, m_lstHeader.GetCountPerPage() + 2);
	m_loadItems.push_back(loadItem);

	m_thread.SetSearchSql(sql, countSql);
	m_thread.FireLoadItems(true);

	MoveControls();

	countSql.Replace(_T("%"), _T("%%"));
	sql.Replace(_T("%"), _T("%%"));
	Log(StrF(_T("Start Fill List - Count SQL: %s, Query SQL: %s"), countSql, sql));

	return TRUE;
}

void CQPasteWnd::ShowRightClickMenu()
{
	POINT pp;
	CMenuHandle cmPopUp;
	CMenuHandle cmSubMenu = NULL;

	GetCursorPos(&pp);
	if (cmPopUp.LoadMenu(IDR_QUICK_PASTE) != 0)
	{
		cmSubMenu = cmPopUp.GetSubMenu(0);
		if (cmSubMenu == NULL)
		{
			return;
		}

		int nItem = m_lstHeader.GetCaret();

		CRect rc;
		m_lstHeader.GetItemRect(nItem, rc, LVIR_BOUNDS);
		ClientToScreen(rc);

		if (rc.PtInRect(pp) == FALSE)
		{
			pp.x = rc.left;
			pp.y = rc.bottom;
		}

		theApp.m_Addins.AddPrePasteAddinsToMenu(&cmSubMenu);

		SetFriendChecks(&cmSubMenu);

		CString specialPaste("Special Paste");
		int nPos = -1;
		CMenuHandle sendToMenu = CMultiLanguage::GetMenuPos(cmSubMenu.m_hMenu, specialPaste, nPos, TRUE);
		if (sendToMenu != NULL)
		{
			CGetSetOptions::m_pasteScripts.AddToMenu(&sendToMenu, &m_actions);
		}

		AddShowStarredClipsMenuItem(&cmSubMenu);

		theApp.m_Language.UpdateRightClickMenu(cmSubMenu.m_hMenu);

		if (m_bShowStarredClips)
		{
			cmSubMenu.CheckMenuItem(ID_MENU_SHOWSTARREDCLIPS, MF_CHECKED);
		}

		cmSubMenu.TrackPopupMenu(TPM_LEFTALIGN | TPM_TOPALIGN | TPM_RIGHTBUTTON, pp.x, pp.y, m_hWnd, NULL);
	}
}

void CQPasteWnd::AddShowStarredClipsMenuItem(CMenuHandle* pMenu)
{
	if (pMenu == NULL ||
		pMenu->GetMenuState(ID_MENU_SHOWSTARREDCLIPS, MF_BYCOMMAND) != 0xFFFFFFFF)
	{
		return;
	}

	CString csText = theApp.m_Language.GetString(_T("ShowStarredClips"), _T("Show Starred Clips"));
	CString shortcutText = m_actions.GetCmdKeyText(ActionEnums::SHOW_STARRED_CLIPS);
	if (shortcutText != _T("") &&
		csText.Find(_T("\t")) < 0)
	{
		csText += _T("\t");
		csText += shortcutText;
	}

	CString csFilterOn(_T("Filter On Selected Clip"));
	int nPos = -1;
	CMenuHandle pParentMenu = CMultiLanguage::GetMenuPos(pMenu->m_hMenu, csFilterOn, nPos);
	if (pParentMenu.m_hMenu != NULL &&
		nPos >= 0)
	{
		::InsertMenu(pParentMenu.m_hMenu, nPos + 1, MF_BYPOSITION | MF_STRING, ID_MENU_SHOWSTARREDCLIPS, csText);
		return;
	}

	CString csSearchQuickPaste(_T("Search Quick Paste"));
	nPos = -1;
	pParentMenu = CMultiLanguage::GetMenuPos(pMenu->m_hMenu, csSearchQuickPaste, nPos);
	if (pParentMenu.m_hMenu != NULL &&
		nPos >= 0)
	{
		::InsertMenu(pParentMenu.m_hMenu, nPos + 1, MF_BYPOSITION | MF_STRING, ID_MENU_SHOWSTARREDCLIPS, csText);
	}
}
void CQPasteWnd::SetFriendChecks(CMenuHandle *pMenu)
{
	SetSendToMenu(pMenu, ID_MENU_SENTTO_FRIENDONE, 0);
	SetSendToMenu(pMenu, ID_MENU_SENTTO_FRIEND_TWO, 1);
	SetSendToMenu(pMenu, ID_MENU_SENTTO_FRIEND_THREE, 2);
	SetSendToMenu(pMenu, ID_MENU_SENTTO_FRIEND_FORE, 3);
	SetSendToMenu(pMenu, ID_MENU_SENTTO_FRIEND_FIVE, 4);
	SetSendToMenu(pMenu, ID_MENU_SENTTO_FRIEND_SIX, 5);
	SetSendToMenu(pMenu, ID_MENU_SENTTO_FRIEND_SEVEN, 6);
	SetSendToMenu(pMenu, ID_MENU_SENTTO_FRIEND_EIGHT, 7);
	SetSendToMenu(pMenu, ID_MENU_SENTTO_FRIEND_NINE, 8);
	SetSendToMenu(pMenu, ID_MENU_SENTTO_FRIEND_TEN, 9);
	SetSendToMenu(pMenu, ID_MENU_SENTTO_FRIEND_ELEVEN, 10);
	SetSendToMenu(pMenu, ID_MENU_SENTTO_FRIEND_TWELVE, 11);
	SetSendToMenu(pMenu, ID_MENU_SENTTO_FRIEND_THIRTEEN, 12);
	SetSendToMenu(pMenu, ID_MENU_SENTTO_FRIEND_FORETEEN, 13);
	SetSendToMenu(pMenu, ID_MENU_SENTTO_FRIEND_FIFTEEN, 14);

	if (CGetSetOptions::GetAllowFriends() == false)
	{
		CString csText("Send To");
		int nPos = -1;
		CMultiLanguage::GetMenuPos(*pMenu, csText, nPos);
		if (nPos >= 0)
		{
			pMenu->DeleteMenu(nPos, MF_BYPOSITION);
		}
	}
	else
	{
		CString csText("Send To");
		int nPos = -1;
		CMenuHandle sendToMenu = CMultiLanguage::GetMenuPos(*pMenu, csText, nPos, TRUE);

		if (sendToMenu != NULL)
		{
			m_customFriendsHelper.Load();
			m_customFriendsHelper.AddToMenu(&sendToMenu);
		}
	}
}

void CQPasteWnd::SetMenuChecks(CMenuHandle *pMenu)
{
	//Set the transparency Check
	if (!CGetSetOptions::GetEnableTransparency())
	{
		pMenu->CheckMenuItem(ID_MENU_TRANSPARENCY_NONE, MF_CHECKED);
	}
	else
	{
		int nTrans = CGetSetOptions::GetTransparencyPercent();
		int nCheckID = -1;
		switch (nTrans)
		{
		case 5:
			nCheckID = ID_MENU_TRANSPARENCY_5;
			break;
		case 10:
			nCheckID = ID_MENU_TRANSPARENCY_10;
			break;
		case 15:
			nCheckID = ID_MENU_TRANSPARENCY_15;
			break;
		case 20:
			nCheckID = ID_MENU_TRANSPARENCY_20;
			break;
		case 25:
			nCheckID = ID_MENU_TRANSPARENCY_25;
			break;
		case 30:
			nCheckID = ID_MENU_TRANSPARENCY_30;
			break;
		case 35:
			nCheckID = ID_TRANSPARENCY_35;
			break;
		case 40:
			nCheckID = ID_MENU_TRANSPARENCY_40;
			break;
		}
		if (nCheckID >= 0)
		{
			pMenu->CheckMenuItem(nCheckID, MF_CHECKED);
		}
	}

	//Set the lines per row check
	int nLinesPerRow = CGetSetOptions::GetLinesPerRow();
	int nCheckID = -1;
	switch (nLinesPerRow)
	{
	case 1:
		nCheckID = ID_MENU_LINESPERROW_1;
		break;
	case 2:
		nCheckID = ID_MENU_LINESPERROW_2;
		break;
	case 3:
		nCheckID = ID_MENU_LINESPERROW_3;
		break;
	case 4:
		nCheckID = ID_MENU_LINESPERROW_4;
		break;
	case 5:
		nCheckID = ID_MENU_LINESPERROW_5;
		break;
	}
	if (nCheckID >= 0)
	{
		pMenu->CheckMenuItem(nCheckID, MF_CHECKED);
	}

	//Set the position check
	int nPosition = CGetSetOptions::GetQuickPastePosition();
	nCheckID = -1;
	switch (nPosition)
	{
	case POS_AT_CARET:
		nCheckID = ID_MENU_POSITIONING_ATCARET;
		break;
	case POS_AT_CURSOR:
		nCheckID = ID_MENU_POSITIONING_ATCURSOR;
		break;
	case POS_AT_PREVIOUS:
		nCheckID = ID_MENU_POSITIONING_ATPREVIOUSPOSITION;
		break;
	}
	if (nCheckID >= 0)
	{
		pMenu->CheckMenuItem(nCheckID, MF_CHECKED);
	}

	theApp.UpdateMenuConnectCV(pMenu, ID_MENU_TOGGLECONNECTCV);

	if (CGetSetOptions::GetShowTextForFirstTenHotKeys())
	{
		pMenu->CheckMenuItem(ID_MENU_FIRSTTENHOTKEYS_SHOWHOTKEYTEXT, MF_CHECKED);
	}

	if (CGetSetOptions::GetUseCtrlNumForFirstTenHotKeys())
	{
		pMenu->CheckMenuItem(ID_MENU_FIRSTTENHOTKEYS_USECTRLNUM, MF_CHECKED);
	}

	if (CGetSetOptions::m_bShowPersistent)
	{
		pMenu->CheckMenuItem(ID_MENU_ALLWAYSONTOP, MF_CHECKED);
	}

	if (CGetSetOptions::GetAutoHide())
	{
		pMenu->CheckMenuItem(ID_MENU_AUTOHIDE, MF_CHECKED);
	}

	switch (CGetSetOptions::GetCaptionPos())
	{
	case 1:
		pMenu->CheckMenuItem(ID_VIEWCAPTIONBARON_RIGHT, MF_CHECKED);
		break;
	case 2:
		pMenu->CheckMenuItem(ID_VIEWCAPTIONBARON_BOTTOM, MF_CHECKED);
		break;
	case 3:
		pMenu->CheckMenuItem(ID_VIEWCAPTIONBARON_LEFT, MF_CHECKED);
		break;
	case 4:
		pMenu->CheckMenuItem(ID_VIEWCAPTIONBARON_TOP, MF_CHECKED);
		break;
	}

	if (CGetSetOptions::GetAllwaysShowDescription())
	{
		pMenu->CheckMenuItem(ID_MENU_QUICKOPTIONS_ALLWAYSSHOWDESCRIPTION, MF_CHECKED);
	}

	switch (CGetSetOptions::GetDoubleClickingOnCaptionDoes())
	{
	case TOGGLES_ALLWAYS_ON_TOP:
		pMenu->CheckMenuItem(ID_MENU_QUICKOPTIONS_DOUBLECLICKINGONCAPTION_TOGGLESALWAYSONTOP, MF_CHECKED);
		break;
	case TOGGLES_ALLWAYS_SHOW_DESCRIPTION:
		pMenu->CheckMenuItem(ID_MENU_QUICKOPTIONS_DOUBLECLICKINGONCAPTION_TOGGLESALWAYSSHOWDESCRIPTION, MF_CHECKED);
		break;
	case ROLLES_UP_WINDOW:
		pMenu->CheckMenuItem(ID_MENU_QUICKOPTIONS_DOUBLECLICKINGONCAPTION_ROLLUPWINDOW, MF_CHECKED);
		break;
	}

	if (CGetSetOptions::m_bPrompForNewGroupName)
	{
		pMenu->CheckMenuItem(ID_MENU_QUICKOPTIONS_PROMPTFORNEWGROUPNAMES, MF_CHECKED);
	}

	if (CGetSetOptions::m_bDrawThumbnail)
	{
		pMenu->CheckMenuItem(ID_MENU_QUICKOPTIONS_SHOWTHUMBNAILS, MF_CHECKED);
	}

	if (CGetSetOptions::m_bDrawRTF)
	{
		pMenu->CheckMenuItem(ID_MENU_QUICKOPTIONS_DRAWRTFTEXT, MF_CHECKED);
	}

	if (CGetSetOptions::m_bSendPasteMessageAfterSelection)
	{
		pMenu->CheckMenuItem(ID_MENU_QUICKOPTIONS_PASTECLIPAFTERSELECTION, MF_CHECKED);
	}

	if (CGetSetOptions::m_bFindAsYouType)
	{
		pMenu->CheckMenuItem(ID_MENU_QUICKOPTIONS_FINDASYOUTYPE, MF_CHECKED);
	}

	if (CGetSetOptions::m_bEnsureEntireWindowCanBeSeen)
	{
		pMenu->CheckMenuItem(ID_MENU_QUICKOPTIONS_ENSUREENTIREWINDOWISVISIBLE, MF_CHECKED);
	}

	if (CGetSetOptions::m_bShowAllClipsInMainList)
	{
		pMenu->CheckMenuItem(ID_MENU_QUICKOPTIONS_SHOWCLIPSTHATAREINGROUPSINMAINLIST, MF_CHECKED);
	}

	if (CGetSetOptions::GetPromptWhenDeletingClips())
	{
		pMenu->CheckMenuItem(ID_QUICKOPTIONS_PROMPTTODELETECLIP, MF_CHECKED);
	}

	if (CGetSetOptions::GetPasteAsAdmin())
	{
		pMenu->CheckMenuItem(ID_QUICKOPTIONS_ELEVATEPREVILEGESTOPASTEINTOELEVATEDAPPS, MF_CHECKED);
	}

	if (CGetSetOptions::GetShowInTaskBar())
	{
		pMenu->CheckMenuItem(ID_QUICKOPTIONS_SHOWINTASKBAR, MF_CHECKED);
	}

	if (CGetSetOptions::GetShowTextForFirstTenHotKeys())
	{
		pMenu->CheckMenuItem(ID_QUICKOPTIONS_SHOWTEXTFORFIRSTTENCOPYHOTKEYS, MF_CHECKED);
	}

	if (CGetSetOptions::GetShowIfClipWasPasted())
	{
		pMenu->CheckMenuItem(ID_QUICKOPTIONS_SHOWINDICATORACLIPHASBEENPASTED, MF_CHECKED);
	}

	if (CGetSetOptions::GetSearchDescription())
		pMenu->CheckMenuItem(ID_MENU_SEARCHDESCRIPTION, MF_CHECKED);

	if (CGetSetOptions::GetSearchFullText())
		pMenu->CheckMenuItem(ID_MENU_SEARCHFULLTEXT, MF_CHECKED);

	if (CGetSetOptions::GetSearchQuickPaste())
		pMenu->CheckMenuItem(ID_MENU_SEARCHQUICKPASTE, MF_CHECKED);

	if (m_bShowStarredClips)
		pMenu->CheckMenuItem(ID_MENU_SHOWSTARREDCLIPS, MF_CHECKED);

	if (CGetSetOptions::GetSimpleTextSearch())
		pMenu->CheckMenuItem(ID_MENU_CONTAINSTEXTSEARCHONLY, MF_CHECKED);

	if (CGetSetOptions::GetRegExTextSearch())
		pMenu->CheckMenuItem(ID_MENU_REGULAREXPRESSIONSEARCH, MF_CHECKED);

	if (CGetSetOptions::GetSimpleTextSearch() == FALSE &&
		CGetSetOptions::GetRegExTextSearch() == FALSE)
	{
		pMenu->CheckMenuItem(ID_MENU_WILDCARDSEARCH, MF_CHECKED);
	}
}

void CQPasteWnd::SetSendToMenu(CMenuHandle *pMenu, int nMenuID, int nArrayPos)
{
	if (CGetSetOptions::m_SendClients[nArrayPos].csIP.GetLength() > 0)
	{
		CString cs;
		if (CGetSetOptions::m_SendClients[nArrayPos].csDescription != _T(""))
		{
			cs.Format(_T("(%s) - %s"), CGetSetOptions::m_SendClients[nArrayPos].csIP, CGetSetOptions::m_SendClients[nArrayPos].csDescription);
		}
		else
		{
			cs.Format(_T("%s"), CGetSetOptions::m_SendClients[nArrayPos].csIP);
		}
		pMenu->ModifyMenu(nMenuID, MF_BYCOMMAND, nMenuID, cs);
	}
	else
	{
		pMenu->DeleteMenu(nMenuID, MF_BYCOMMAND);
	}
}

LRESULT CQPasteWnd::OnSearch(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	CString csText;
	m_search.GetWindowText(csText);

	FillList(csText);

	m_lstHeader.SetFocus();

	MoveControls();

	m_search.SetSel(-1, 0);

	return TRUE;
}


///////////////////////////////////////////////////////////////////////
//Menu Stuff
///////////////////////////////////////////////////////////////////////
LRESULT CQPasteWnd::OnMenuLinesperrow1(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	SetLinesPerRow(1, false, true);
	return 0;
}

LRESULT CQPasteWnd::OnMenuLinesperrow2(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	SetLinesPerRow(2, false, true);
	return 0;
}

LRESULT CQPasteWnd::OnMenuLinesperrow3(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	SetLinesPerRow(3, false, true);
	return 0;
}

LRESULT CQPasteWnd::OnMenuLinesperrow4(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	SetLinesPerRow(4, false, true);
	return 0;
}

LRESULT CQPasteWnd::OnMenuLinesperrow5(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	SetLinesPerRow(5, false, true);
	return 0;
}

void CQPasteWnd::SetLinesPerRow(int lines, bool force, bool resetListCount)
{
	ARRAY Indexs;
	int listCount = 0;

	if (resetListCount)
	{
		//save and restore state, list box seems to pain funny (gap at header) if items are not reset
		m_lstHeader.GetSelectionIndexes(Indexs);
		if (Indexs.GetCount() <= 0)
		{
			Indexs.Add(0);
		}
		listCount = m_lstHeader.GetItemCount();
		m_lstHeader.SetItemCountEx(0);
	}

	CGetSetOptions::SetLinesPerRow(lines);
	m_lstHeader.SetNumberOfLinesPerRow(lines, force);

	ATL::CCritSecLock csLock(m_CritSection);

	m_cf_dibCache.clear();
	m_cf_NO_dibCache.clear();
	m_cf_rtfCache.clear();
	m_cf_NO_rtfCache.clear();

	if (resetListCount)
	{
		m_lstHeader.SetItemCountEx(listCount);
		m_lstHeader.SetListPos(Indexs[0]);
		m_lstHeader.RefreshVisibleRows();
	}
}

LRESULT CQPasteWnd::OnMenuDelete(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	DeleteSelectedRows();
	return 0;
}

LRESULT CQPasteWnd::OnMenuPositioningAtcaret(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	CGetSetOptions::SetQuickPastePosition(POS_AT_CARET);
	return 0;
}

LRESULT CQPasteWnd::OnMenuPositioningAtcursor(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	CGetSetOptions::SetQuickPastePosition(POS_AT_CURSOR);
	return 0;
}

LRESULT CQPasteWnd::OnMenuPositioningAtpreviousposition(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	CGetSetOptions::SetQuickPastePosition(POS_AT_PREVIOUS);
	return 0;
}

LRESULT CQPasteWnd::OnMenuOptions(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	theApp.m_pMainFrame->SendMessage(WM_SHOW_OPTIONS, 0, 0);
	return 0;
}

LRESULT CQPasteWnd::OnMenuExitprogram(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	::SendMessage(theApp.m_MainhWnd, WM_CLOSE, 0, 0);
	return 0;
}

LRESULT CQPasteWnd::OnMenuToggleConnectCV(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	this->DoAction(ActionEnums::TOGGLE_CLIPBOARD_CONNECTION);
	return 0;
}

void CQPasteWnd::OnUpdate32775(CmdUI *pCmdUI)
{
	if (!pCmdUI->m_pMenu)
	{
		return;
	}

	UpdateMenuShortCut(pCmdUI, ActionEnums::TOGGLE_CLIPBOARD_CONNECTION);
}

LRESULT CQPasteWnd::OnMenuProperties(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	this->DoAction(ActionEnums::CLIP_PROPERTIES);
	return 0;
}

void CQPasteWnd::UpdateFont()
{
	LOGFONT lf;
	CGetSetOptions::GetFont(lf);
	lf.lfHeight = m_DittoWindow.m_dpi.Scale(lf.lfHeight);
	m_lstHeader.SetLogFont(lf);

	if (!m_SearchFont.IsNull())
		m_SearchFont.DeleteObject();
	m_SearchFont.CreateFont(-m_DittoWindow.m_dpi.Scale(15), 0, 0, 0, 400, 0, 0, 0, DEFAULT_CHARSET, 3, 2, 1, 34, _T("Segoe UI"));
	m_search.SetFont(m_SearchFont);
	m_search.SetPromptFont(m_SearchFont);

	m_GroupTree.SetFont(m_SearchFont);

	if (!m_groupFont.IsNull())
		m_groupFont.DeleteObject();
	m_groupFont.CreateFont(-m_DittoWindow.m_dpi.Scale(12), 0, 0, 0, 400, 0, 1, 0, DEFAULT_CHARSET, 3, 2, 1, 34, _T("Segoe UI"));
	m_stGroup.SetFont(m_groupFont);
	m_stGroup.SetBkColor(CGetSetOptions::m_Theme.MainWindowBG());
	m_stGroup.SetTextColor(CGetSetOptions::m_Theme.ListBoxEvenRowsText());

	m_noSearchResultsStatic.SetBkColor(CGetSetOptions::m_Theme.MainWindowBG());
	m_noSearchResultsStatic.SetTextColor(CGetSetOptions::m_Theme.ListBoxEvenRowsText());
	m_noSearchResultsStatic.SetFont(m_SearchFont);

	m_lstHeader.CreateSmallFont();
}

LRESULT CQPasteWnd::OnMenuFirsttenhotkeysUsectrlnum(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	CGetSetOptions::SetUseCtrlNumForFirstTenHotKeys(!CGetSetOptions::GetUseCtrlNumForFirstTenHotKeys());
	m_lstHeader.RefreshVisibleRows();
	return 0;
}

LRESULT CQPasteWnd::OnMenuFirsttenhotkeysShowhotkeytext(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	CGetSetOptions::SetShowTextForFirstTenHotKeys(!CGetSetOptions::GetShowTextForFirstTenHotKeys());
	m_lstHeader.SetShowTextForFirstTenHotKeys(CGetSetOptions::GetShowTextForFirstTenHotKeys());

	m_lstHeader.RefreshVisibleRows();
	return 0;
}

LRESULT CQPasteWnd::OnViewcaptionbaronRight(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	SetCaptionOn(CAPTION_RIGHT, false, CGetSetOptions::m_Theme.GetCaptionSize(), CGetSetOptions::m_Theme.GetCaptionFontSize());
	CGetSetOptions::SetCaptionPos(CAPTION_RIGHT);
	return 0;
}

LRESULT CQPasteWnd::OnViewcaptionbaronBottom(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	SetCaptionOn(CAPTION_BOTTOM, false, CGetSetOptions::m_Theme.GetCaptionSize(), CGetSetOptions::m_Theme.GetCaptionFontSize());
	CGetSetOptions::SetCaptionPos(CAPTION_BOTTOM);
	return 0;
}

LRESULT CQPasteWnd::OnViewcaptionbaronLeft(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	SetCaptionOn(CAPTION_LEFT, false, CGetSetOptions::m_Theme.GetCaptionSize(), CGetSetOptions::m_Theme.GetCaptionFontSize());
	CGetSetOptions::SetCaptionPos(CAPTION_LEFT);
	return 0;
}

LRESULT CQPasteWnd::OnViewcaptionbaronTop(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	SetCaptionOn(CAPTION_TOP, false, CGetSetOptions::m_Theme.GetCaptionSize(), CGetSetOptions::m_Theme.GetCaptionFontSize());
	CGetSetOptions::SetCaptionPos(CAPTION_TOP);
	return 0;
}

LRESULT CQPasteWnd::OnMenuAutohide(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	bool bAutoHide = !CGetSetOptions::GetAutoHide();
	CGetSetOptions::SetAutoHide(bAutoHide);
	return 0;
}

LRESULT CQPasteWnd::OnMenuViewfulldescription(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	this->DoAction(ActionEnums::SHOWDESCRIPTION);
	return 0;
}

LRESULT CQPasteWnd::OnMenuAllwaysontop(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	this->DoAction(ActionEnums::TOGGLESHOWPERSISTANT);
	return 0;
}

LRESULT CQPasteWnd::OnMenuNewGroup(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	this->DoAction(ActionEnums::NEWGROUP);
	return 0;
}

LRESULT CQPasteWnd::OnMenuNewGroupSelection(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	this->DoAction(ActionEnums::NEWGROUPSELECTION);
	return 0;
}

LRESULT CQPasteWnd::OnMenuQuickoptionsAllwaysshowdescription(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	CGetSetOptions::SetAllwaysShowDescription(!CGetSetOptions::m_bAllwaysShowDescription);

	return 0;
}

LRESULT CQPasteWnd::OnMenuQuickoptionsDoubleclickingoncaptionTogglesalwaysontop(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	CGetSetOptions::SetDoubleClickingOnCaptionDoes(TOGGLES_ALLWAYS_ON_TOP);

	return 0;
}

LRESULT CQPasteWnd::OnMenuQuickoptionsDoubleclickingoncaptionRollupwindow(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	CGetSetOptions::SetDoubleClickingOnCaptionDoes(ROLLES_UP_WINDOW);

	return 0;
}

LRESULT CQPasteWnd::OnMenuQuickoptionsDoubleclickingoncaptionTogglesshowdescription(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	CGetSetOptions::SetDoubleClickingOnCaptionDoes(TOGGLES_ALLWAYS_SHOW_DESCRIPTION);
	return 0;
}

LRESULT CQPasteWnd::OnMenuQuickoptionsPromptfornewgroupnames(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	CGetSetOptions::SetPrompForNewGroupName(!CGetSetOptions::m_bPrompForNewGroupName);
	return 0;
}

LRESULT CQPasteWnd::OnMenuViewgroups(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	this->DoAction(ActionEnums::SHOWGROUPS);
	return 0;
}

LRESULT CQPasteWnd::OnMenuQuickpropertiesSettoneverautodelete(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	CWaitCursor wait;
	ARRAY IDs;
	ARRAY Indexs;
	m_lstHeader.GetSelectionItemData(IDs);
	m_lstHeader.GetSelectionIndexes(Indexs);

	INT_PTR count = IDs.GetSize();

	for (int i = 0; i < count; i++)
	{
		try
		{
			theApp.m_db.execDMLEx(_T("UPDATE Main SET lDontAutoDelete = %d where lID = %d;"), (int)CTime::GetCurrentTime().GetTime(), IDs[i]);
		}
		CATCH_SQLITE_EXCEPTION
	}

	{
		ATL::CCritSecLock csLock(m_CritSection);

		count = Indexs.GetSize();
		for (int row = 0; row < count; row++)
		{
			if (Indexs[row] < (int)m_listItems.size())
			{
				m_listItems[Indexs[row]].m_bDontAutoDelete = true;
			}
		}
	}

	m_lstHeader.RefreshVisibleRows();
	return 0;
}

LRESULT CQPasteWnd::OnMenuQuickpropertiesAutodelete(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	CWaitCursor wait;
	ARRAY IDs;
	ARRAY Indexs;
	m_lstHeader.GetSelectionItemData(IDs);
	m_lstHeader.GetSelectionIndexes(Indexs);

	INT_PTR count = IDs.GetSize();

	for (int i = 0; i < count; i++)
	{
		try
		{
			theApp.m_db.execDMLEx(_T("UPDATE Main SET lDontAutoDelete = 0 where lID = %d;"), IDs[i]);
		}
		CATCH_SQLITE_EXCEPTION
	}

	{
		ATL::CCritSecLock csLock(m_CritSection);
		count = Indexs.GetSize();
		for (int row = 0; row < count; row++)
		{
			if (Indexs[row] < (int)m_listItems.size())
			{
				m_listItems[Indexs[row]].m_bDontAutoDelete = false;
			}
		}
	}

	m_lstHeader.RefreshVisibleRows();
	return 0;
}

LRESULT CQPasteWnd::OnMenuQuickpropertiesRemovehotkey(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	CWaitCursor wait;
	ARRAY IDs;
	ARRAY Indexs;
	m_lstHeader.GetSelectionItemData(IDs);
	m_lstHeader.GetSelectionIndexes(Indexs);

	INT_PTR count = IDs.GetSize();

	for (int i = 0; i < count; i++)
	{
		try
		{
			theApp.m_db.execDMLEx(_T("UPDATE Main SET lShortCut = 0, globalShortCut = 0 where lID = %d;"), IDs[i]);
			g_HotKeys.Remove(IDs[i], CHotKey::PASTE_OPEN_CLIP);
			theApp.m_db.execDMLEx(_T("UPDATE Main SET MoveToGroupShortCut = 0, GlobalMoveToGroupShortCut = 0 where lID = %d;"), IDs[i]);
			g_HotKeys.Remove(IDs[i], CHotKey::MOVE_TO_GROUP);
		}
		CATCH_SQLITE_EXCEPTION
	}

	{
		ATL::CCritSecLock csLock(m_CritSection);

		count = Indexs.GetSize();
		for (int row = 0; row < count; row++)
		{
			if (Indexs[row] < (int)m_listItems.size())
			{
				m_listItems[Indexs[row]].m_bHasShortCut = false;
			}
		}
	}

	m_lstHeader.RefreshVisibleRows();
	return 0;
}

LRESULT CQPasteWnd::OnQuickpropertiesRemovequickpaste(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	CWaitCursor wait;
	ARRAY IDs;
	ARRAY Indexs;
	m_lstHeader.GetSelectionItemData(IDs);
	m_lstHeader.GetSelectionIndexes(Indexs);

	INT_PTR count = IDs.GetSize();

	for (int i = 0; i < count; i++)
	{
		try
		{
			theApp.m_db.execDMLEx(_T("UPDATE Main SET QuickPasteText = '' where lID = %d;"), IDs[i]);
		}
		CATCH_SQLITE_EXCEPTION
	}

	{
		ATL::CCritSecLock csLock(m_CritSection);

		count = Indexs.GetSize();
		for (int row = 0; row < count; row++)
		{
			if (Indexs[row] < (int)m_listItems.size())
			{
				m_listItems[Indexs[row]].m_QuickPaste.Empty();
			}
		}
	}

	m_lstHeader.RefreshVisibleRows();
	return 0;
}

LRESULT CQPasteWnd::OnMenuSenttoFriendFifteen(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	SendToFriendbyPos(14, _T(""));
	return 0;
}

LRESULT CQPasteWnd::OnMenuSenttoFriendForeteen(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	SendToFriendbyPos(13, _T(""));
	return 0;
}

LRESULT CQPasteWnd::OnMenuSenttoFriendThirteen(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	SendToFriendbyPos(12, _T(""));
	return 0;
}

LRESULT CQPasteWnd::OnMenuSenttoFriendTwelve(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	SendToFriendbyPos(11, _T(""));
	return 0;
}

LRESULT CQPasteWnd::OnMenuSenttoFriendEleven(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	SendToFriendbyPos(10, _T(""));
	return 0;
}

LRESULT CQPasteWnd::OnMenuSenttoFriendTen(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	SendToFriendbyPos(9, _T(""));
	return 0;
}

LRESULT CQPasteWnd::OnMenuSenttoFriendNine(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	SendToFriendbyPos(8, _T(""));
	return 0;
}

LRESULT CQPasteWnd::OnMenuSenttoFriendEight(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	SendToFriendbyPos(7, _T(""));
	return 0;
}

LRESULT CQPasteWnd::OnMenuSenttoFriendSeven(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	SendToFriendbyPos(6, _T(""));
	return 0;
}

LRESULT CQPasteWnd::OnMenuSenttoFriendSix(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	SendToFriendbyPos(5, _T(""));
	return 0;
}

LRESULT CQPasteWnd::OnMenuSenttoFriendFive(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	SendToFriendbyPos(4, _T(""));
	return 0;
}

LRESULT CQPasteWnd::OnMenuSenttoFriendFore(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	SendToFriendbyPos(3, _T(""));
	return 0;
}

LRESULT CQPasteWnd::OnMenuSenttoFriendThree(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	SendToFriendbyPos(2, _T(""));
	return 0;
}

LRESULT CQPasteWnd::OnMenuSenttoFriendTwo(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	SendToFriendbyPos(1, _T(""));
	return 0;
}

LRESULT CQPasteWnd::OnMenuSenttoFriendone(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	SendToFriendbyPos(0, _T(""));
	return 0;
}

LRESULT CQPasteWnd::OnMenuSenttoPromptforip(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	// TODO: Add your command handler code here

	return 0;
}

LRESULT CQPasteWnd::OnMenuGroupsMovetogroup(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	this->DoAction(ActionEnums::MOVE_CLIP_TO_GROUP);
	return 0;
}

LRESULT CQPasteWnd::OnMenuPasteplaintextonly(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	this->DoAction(ActionEnums::PASTE_SELECTED_PLAIN_TEXT);
	return 0;
}

LRESULT CQPasteWnd::OnPromptToDeleteClip(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	CGetSetOptions::SetPromptWhenDeletingClips(!CGetSetOptions::GetPromptWhenDeletingClips());
	return 0;
}

LRESULT CQPasteWnd::OnMakeTopStickyClip(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	this->DoAction(ActionEnums::MAKE_TOP_STICKY);
	return 0;
}

LRESULT CQPasteWnd::OnMakeLastStickyClip(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	this->DoAction(ActionEnums::MAKE_LAST_STICKY);
	return 0;
}

LRESULT CQPasteWnd::OnRemoveSticky(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	this->DoAction(ActionEnums::REMOVE_STICKY);
	return 0;
}

void CQPasteWnd::OnUpdateStickyclipsMaketopstickyclip(CmdUI *pCmdUI)
{
	if (!pCmdUI->m_pMenu)
	{
		return;
	}

	UpdateMenuShortCut(pCmdUI, ActionEnums::MAKE_TOP_STICKY);
}

void CQPasteWnd::OnUpdateStickyclipsMakelaststickyclip(CmdUI *pCmdUI)
{
	if (!pCmdUI->m_pMenu)
	{
		return;
	}

	UpdateMenuShortCut(pCmdUI, ActionEnums::MAKE_LAST_STICKY);
}

void CQPasteWnd::OnUpdateStickyclipsRemovestickysetting(CmdUI *pCmdUI)
{
	if (!pCmdUI->m_pMenu)
	{
		return;
	}

	UpdateMenuShortCut(pCmdUI, ActionEnums::REMOVE_STICKY);
}

LRESULT CQPasteWnd::OnElevateAppToPasteIntoElevatedApp(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	this->DoAction(ActionEnums::ELEVATE_PRIVlEGES);
	return 0;
}

LRESULT CQPasteWnd::OnMenuExport(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	CClipIDs IDs;
	INT_PTR lCount = m_lstHeader.GetSelectedCount();
	if (lCount <= 0)
	{
		return 0;
	}

	m_lstHeader.GetSelectionItemData(IDs);
	lCount = IDs.GetSize();
	if (lCount <= 0)
	{
		return 0;
	}

	OPENFILENAME ofn;
	TCHAR szFile[400];
	TCHAR szDir[400];

	memset(&szFile, 0, sizeof(szFile));
	memset(szDir, 0, sizeof(szDir));
	memset(&ofn, 0, sizeof(ofn));

	CString csInitialDir = CGetSetOptions::GetLastImportDir();
	STRCPY(szDir, csInitialDir);

	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = m_hWnd;
	ofn.lpstrFile = szFile;
	ofn.nMaxFile = sizeof(szFile);
	ofn.lpstrFilter = _T("Exported Ditto Clips (.dto)\0*.dto\0\0");
	ofn.nFilterIndex = 1;
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = szDir;
	ofn.lpstrDefExt = _T("dto");
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_OVERWRITEPROMPT | OFN_NOCHANGEDIR;

	m_bHideWnd = false;

	if (GetSaveFileName(&ofn))
	{
		using namespace nsPath;
		CPath path(ofn.lpstrFile);
		CString csPath = path.GetPath();
		CGetSetOptions::SetLastExportDir(csPath);

		CString csFile(ofn.lpstrFile);
		IDs.Export(csFile);
	}

	m_bHideWnd = true;
	return 0;
}

LRESULT CQPasteWnd::OnMenuImport(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	m_bHideWnd = false;
	theApp.ImportClips(m_hWnd);
	m_bHideWnd = true;
	return 0;
}

LRESULT CQPasteWnd::OnMenuHelp(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	::ShellExecute(NULL, _T("open"), _T("https://github.com/sabrogden/Ditto/wiki"), NULL, NULL, SW_SHOWNORMAL);
	return 0;
}

LRESULT CQPasteWnd::OnMenuQuickoptionsFont(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	m_bHideWnd = false;

	CDimWnd dimmer(m_hWnd);

	CFont fnt = m_lstHeader.GetFont();
	LOGFONT lf;
	fnt.GetLogFont(&lf);

	lf.lfHeight = m_DittoWindow.m_dpi.UnScale(lf.lfHeight);

	CFontDialog dlg(&lf);
	if (dlg.DoModal() == IDOK)
	{
		CGetSetOptions::SetFont(*dlg.m_cf.lpLogFont);
		(*dlg.m_cf.lpLogFont).lfHeight = m_DittoWindow.m_dpi.Scale((*dlg.m_cf.lpLogFont).lfHeight);
		m_lstHeader.SetLogFont(*dlg.m_cf.lpLogFont);
		this->SetLinesPerRow(CGetSetOptions::GetLinesPerRow(), true, true);
	}

	m_bHideWnd = true;
	return 0;
}

LRESULT CQPasteWnd::OnMenuQuickoptionsShowthumbnails(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	CGetSetOptions::SetDrawThumbnail(!CGetSetOptions::m_bDrawThumbnail);
	m_lstHeader.RefreshVisibleRows();
	return 0;
}

LRESULT CQPasteWnd::OnMenuQuickoptionsDrawrtftext(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	CGetSetOptions::SetDrawRTF(!CGetSetOptions::m_bDrawRTF);
	m_lstHeader.RefreshVisibleRows();
	return 0;
}

LRESULT CQPasteWnd::OnMenuQuickoptionsPasteclipafterselection(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	CGetSetOptions::SetSendPasteAfterSelection(!CGetSetOptions::m_bSendPasteMessageAfterSelection);
	return 0;
}

LRESULT CQPasteWnd::OnMenuQuickoptionsFindasyoutype(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	CGetSetOptions::SetFindAsYouType(!CGetSetOptions::m_bFindAsYouType);
	return 0;
}

LRESULT CQPasteWnd::OnMenuQuickoptionsEnsureentirewindowisvisible(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	CGetSetOptions::SetEnsureEntireWindowCanBeSeen(!CGetSetOptions::m_bEnsureEntireWindowCanBeSeen);
	return 0;
}

LRESULT CQPasteWnd::OnMenuQuickoptionsShowclipsthatareingroupsinmainlist(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	CGetSetOptions::SetShowAllClipsInMainList(!CGetSetOptions::m_bShowAllClipsInMainList);

	CString csText;
	m_search.GetWindowText(csText);
	FillList(csText);
	return 0;
}

LRESULT CQPasteWnd::OnMenuEdititem(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	this->DoAction(ActionEnums::EDITCLIP);
	return 0;
}

LRESULT CQPasteWnd::OnMenuNewclip(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	this->DoAction(ActionEnums::NEWCLIP);
	return 0;
}


///////////////////////////////////////////////////////////////////////
//END END Menu Stuff
///////////////////////////////////////////////////////////////////////


BOOL CQPasteWnd::SendToFriendbyPos(int nPos, CString override_IP_Host)
{
	if (CGetSetOptions::GetAllowFriends() == false)
	{
		return FALSE;
	}

	CWaitCursor wait;

	m_bHideWnd = false;

	CClipIDs IDs;
	INT_PTR count = m_lstHeader.GetSelectedCount();
	if (count <= 0)
	{
		return FALSE;
	}

	m_lstHeader.GetSelectionItemData(IDs);
	count = IDs.GetSize();
	if (count <= 0)
	{
		return FALSE;
	}

	CSendToFriendInfo Info;

	BOOL bRet = FALSE;

	try
	{
		Info.m_manualSend = true;
		Info.m_csIP = override_IP_Host;
		if (Info.m_csIP == _T(""))
		{
			Info.m_csIP = CGetSetOptions::m_SendClients[nPos].csIP;
		}

		if (Info.m_csIP != _T(""))
		{
			CPopup Popup(0, 0, m_hWnd);
			Popup.Show(StrF(_T("Sending clip to %s"), Info.m_csIP));

			Info.m_pPopup = &Popup;

			Info.m_pClipList = new CClipList;
			for (int i = 0; i < count; i++)
			{
				CppSQLite3Query q = theApp.m_db.execQueryEx(_T("SELECT mText FROM Main WHERE lID = %d"), IDs[i]);
				if (q.eof() == false)
				{
					CClip* pClip = new CClip;
					if (pClip)
					{
						pClip->LoadFormats(IDs[i]);
						pClip->m_Desc = q.getStringField(0);
						pClip->m_id = IDs[i];
						Info.m_pClipList->AddTail(pClip);
					}
				}
			}

			if (SendToFriend(Info) == FALSE)
			{
				MessageBox(StrF(_T("Error Sending data to %s\n\n%s"), Info.m_csIP, Info.m_csErrorText), _T("DittoWTL"), MB_OK | MB_TOPMOST);
			}
			else
			{
				bRet = TRUE;
			}
		}
	}
	CATCH_SQLITE_EXCEPTION

		m_bHideWnd = true;

	return bRet;
}

LRESULT CQPasteWnd::OnDelete(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	DeleteSelectedRows();
	return TRUE;
}

void CQPasteWnd::DeleteSelectedRows()
{
	if (CGetSetOptions::GetPromptWhenDeletingClips())
	{
		bool bStartValue = m_bHideWnd;
		m_bHideWnd = false;

		int nRet = MessageBox(theApp.m_Language.GetString("Delete_Clip", "Delete Selected Clips?"), _T("DittoWTL"), MB_OKCANCEL | MB_TOPMOST);

		m_bHideWnd = bStartValue;

		if (nRet != IDOK)
		{
			return;
		}
	}

	CClipIDs IDs;
	ARRAY Indexs;

	if (m_lstHeader.GetSelectedCount() == 0)
	{
		return;
	}


	m_lstHeader.GetSelectionItemData(IDs);
	m_lstHeader.GetSelectionIndexes(Indexs);

	DeleteClips(IDs, Indexs);
}

bool CQPasteWnd::DeleteClips(CClipIDs& IDs, ARRAY& Indexs)
{
	int nFirstSel = m_lstHeader.GetNextItem(-1, LVNI_SELECTED);

	IDs.DeleteIDs(true, theApp.m_db);

	Indexs.SortDescending();
	INT_PTR count = Indexs.GetSize();

	int erasedCount = 0;

	{
		ATL::CCritSecLock csLock(m_CritSection);

		for (int i = 0; i < count; i++)
		{
			if (Indexs[i] < (int)m_listItems.size())
			{
				RemoveFromImageRtfCache(Indexs[i]);
				g_HotKeys.Remove(m_lstHeader.GetItemData(Indexs[i]), CHotKey::PASTE_OPEN_CLIP);

				m_listItems.erase(m_listItems.begin() + Indexs[i]);
				erasedCount++;
			}
		}
	}

	CClip::m_LastAddedCRC = 0;

	m_extraDataThread.FireLoadAccelerators();

	m_lstHeader.SetItemCountEx(m_lstHeader.GetItemCount() - erasedCount);

	// if there are no items after the one we deleted, then select the last one.
	if (nFirstSel >= m_lstHeader.GetItemCount())
	{
		nFirstSel = m_lstHeader.GetItemCount() - 1;
	}

	m_lstHeader.SetListPos(nFirstSel);
	UpdateStatus();

	if (m_lstHeader.IsToolTipWindowVisible())
	{
		m_lstHeader.ShowFullDescription(false, true);
	}

	return true;
}

void CQPasteWnd::RemoveFromImageRtfCache(int row, int id)
{
	ATL::CCritSecLock csLock(m_CritSection);

	if (id < 0)
	{
		id = m_lstHeader.GetItemData(row);
	}

	CF_DibTypeMap::iterator iterDib = m_cf_dibCache.find(id);
	if (iterDib != m_cf_dibCache.end())
	{
		m_cf_dibCache.erase(iterDib);
	}

	CF_NoDibTypeMap::iterator iterNoDib = m_cf_NO_dibCache.find(id);
	if (iterNoDib != m_cf_NO_dibCache.end())
	{
		m_cf_NO_dibCache.erase(iterNoDib);
	}

	CF_DibTypeMap::iterator iterRtf = m_cf_rtfCache.find(id);
	if (iterRtf != m_cf_rtfCache.end())
	{
		m_cf_rtfCache.erase(iterRtf);
	}

	CF_NoDibTypeMap::iterator iterNoRtf = m_cf_NO_rtfCache.find(id);
	if (iterNoRtf != m_cf_NO_rtfCache.end())
	{
		m_cf_NO_rtfCache.erase(iterNoRtf);
	}
}

CString CQPasteWnd::LoadDescription(int nItem)
{
	if (nItem < 0 || nItem >= m_lstHeader.GetItemCount())
	{
		return "";
	}

	CString cs;
	try
	{
		int id = m_lstHeader.GetItemData(nItem);

		CppSQLite3Query q = theApp.m_db.execQueryEx(_T("SELECT mText FROM Main WHERE lID = %d"), id);
		if (q.eof() == false)
		{
			cs = q.getStringField(0);
		}
	}
	CATCH_SQLITE_EXCEPTION

		return cs;
}

void CQPasteWnd::MoveSelection(bool down, bool requireModifersActive)
{
	if (m_bModifersMoveActive || requireModifersActive == false)
	{
		ARRAY arr;
		m_lstHeader.GetSelectionIndexes(arr);
		if (arr.GetCount() > 0)
		{
			int index = arr[0];

			if (down)
			{
				if (index < m_lstHeader.GetItemCount() - 1)
				{
					m_lstHeader.SetListPos(index + 1);
				}
			}
			else
			{
				if (index > 0)
				{
					m_lstHeader.SetListPos(index - 1);
				}
			}
		}
		else
		{
			if (m_lstHeader.GetItemCount() > 0)
			{
				m_lstHeader.SetListPos(0);
			}
		}
	}
}

void CQPasteWnd::OnKeyStateUp()
{
	if (CGetSetOptions::m_moveSelectionOnOpenHotkey)
	{
		if (m_bModifersMoveActive)
		{
			Log(_T("OnKeyStateUp"));
			SetTimer(TIMER_PASTE_FROM_MODIFER, CGetSetOptions::GetKeyStatePasteDelay(), NULL);
		}
		else
		{
			Log(_T("OnKeyStateUp - Modifers not active"));
		}
	}
}

void CQPasteWnd::SetKeyModiferState(bool bActive)
{
	if (CGetSetOptions::m_moveSelectionOnOpenHotkey)
	{
		Log(StrF(_T("SetKeyModiferState %d"), bActive));
		m_bModifersMoveActive = bActive;
	}
}

BOOL CQPasteWnd::PreTranslateMessage(MSG* pMsg)
{
	if (!IsMsgForThisWindowTree(pMsg))
	{
		return FALSE;
	}

	// MFC ran the focused control's PreTranslateMessage before walking up to this window.
	HWND hFocus = ::GetFocus();
	if (hFocus != NULL)
	{
		if (hFocus == m_lstHeader.m_hWnd && m_lstHeader.PreTranslateMessage(pMsg))
		{
			return TRUE;
		}
		if (hFocus == m_search.m_hWnd && m_search.PreTranslateMessage(pMsg))
		{
			return TRUE;
		}
		if (hFocus == m_GroupTree.m_hWnd && m_GroupTree.PreTranslateMessage(pMsg))
		{
			return TRUE;
		}
	}

	switch (pMsg->message)
	{
	case WM_MBUTTONUP:
	{
		MSG msg;
		msg.lParam = 0;
		msg.wParam = VK_MOUSE_MIDDLE_CLICK;
		msg.message = WM_KEYDOWN;
		if (CheckActions(&msg) == false)
		{
		}
	}
	break;
	case WM_NCMOUSEMOVE:
	case WM_MOUSEMOVE:
	{
		if (CGetSetOptions::m_bShowPersistent)
		{
			bool hasFocus = ::GetForegroundWindow() == m_hWnd;
			if (hasFocus == false)
			{
				DWORD tick = GetTickCount();
				if ((tick - m_lastNonActiveMouseMove) > 1000)
				{
					theApp.m_activeWnd.TrackActiveWnd(true);
					m_lastNonActiveMouseMove = GetTickCount();
				}
			}
			else
			{
				m_lastNonActiveMouseMove = 0;
			}
		}
	}
	break;
	default:
		if (CheckActions(pMsg))
		{
			return TRUE;
		}
		else if (pMsg->message == WM_CHAR)
		{
			if (pMsg->hwnd == m_lstHeader.m_hWnd)
			{
				bool bStartNewSearch = (::GetFocus() != m_search.m_hWnd);
				TCHAR ch = (TCHAR)pMsg->wParam;
				m_search.SetFocus();
				CString csSearch;
				if (bStartNewSearch == false)
				{
					m_search.GetWindowText(csSearch);
				}
				csSearch += ch;
				m_search.SetWindowText(csSearch);
				int nLen = csSearch.GetLength();
				m_search.SetSel(nLen, nLen);

				BOOL bHandled = FALSE;
				OnSearchEditChange(0, ID_EDIT_SEARCH, m_search.m_hWnd, bHandled);

				return TRUE;
			}
		}
		break;
	}
	return CWndEx::PreTranslateMessage(pMsg);
}

bool CQPasteWnd::CheckActions(MSG* pMsg)
{
	bool ret = false;
	CAccel a;

	if (m_bModifersMoveActive)
	{
		if (m_modifierKeyActions.OnMsg(pMsg, a))
		{
			ret = DoAction(a);
		}
	}

	if (ret == false)
	{
		if (m_actions.OnMsg(pMsg, a))
		{
			KillTimer(TIMER_DO_ACTION);
			ret = DoAction(a);
		}
		else if (a.Cmd > 0)
		{
			m_timerAction = a;
			SetTimer(TIMER_DO_ACTION, CGetSetOptions::m_doubleKeyStrokeTimeout, NULL);

			ret = true;
		}
	}

	return ret;
}

bool CQPasteWnd::DoAction(DWORD cmd)
{
	CAccel a(0, cmd);
	return DoAction(a);
}

bool CQPasteWnd::DoAction(CAccel a)
{
	bool ret = false;

	switch (a.Cmd)
	{
	case ActionEnums::SHOWDESCRIPTION:
		ret = DoActionShowDescription();
		break;
	case ActionEnums::NEXTDESCRIPTION:
		ret = DoActionNextDescription();
		break;
	case ActionEnums::PREVDESCRIPTION:
		ret = DoActionPrevDescription();
		break;
	case ActionEnums::SHOWMENU:
		ret = DoActionShowMenu();
		break;
	case ActionEnums::SYSTEM_MENU:
		ret = DoActionShowSystemMenu();
		break;
	case ActionEnums::NEWGROUP:
		ret = DoActionNewGroup();
		break;
	case ActionEnums::NEWGROUPSELECTION:
		ret = DoActionNewGroupSelection();
		break;
	case ActionEnums::TOGGLEFILELOGGING:
		ret = DoActionToggleFileLogging();
		break;
	case ActionEnums::TOGGLEOUTPUTDEBUGSTRING:
		ret = DoActionToggleOutputDebugString();
		break;
	case ActionEnums::CLOSEWINDOW:
		ret = DoActionCloseWindow();
		break;
	case ActionEnums::FORCE_CLOSE_WINDOW:
		ret = DoActionForceCloseWindow();
		break;
	case ActionEnums::NEXTTABCONTROL:
		ret = DoActionNextTabControl();
		break;
	case ActionEnums::PREVTABCONTROL:
		ret = DoActionPrevTabControl();
		break;
	case ActionEnums::SHOWGROUPS:
		ret = DoActionShowGroups();
		break;
	case ActionEnums::NEWCLIP:
		ret = DoActionNewClip();
		break;
	case ActionEnums::EDITCLIP:
		ret = DoActionEditClip();
		break;
	case ActionEnums::MODIFIER_ACTVE_SELECTIONUP:
		ret = DoModifierActiveActionSelectionUp();
		break;
	case ActionEnums::MODIFIER_ACTVE_SELECTIONDOWN:
		ret = DoModifierActiveActionSelectionDown();
		break;
	case ActionEnums::MODIFIER_ACTVE_MOVEFIRST:
		ret = DoModifierActiveActionMoveFirst();
		break;
	case ActionEnums::MODIFIER_ACTVE_MOVELAST:
		ret = DoModifierActiveActionMoveLast();
		break;
	case ActionEnums::CANCELFILTER:
		ret = DoActionCancelFilter();
		break;
	case ActionEnums::HOMELIST:
		ret = DoActionHomeList();
		break;
	case ActionEnums::BACKGRROUP:
		ret = DoActionBackGroup();
		break;
	case ActionEnums::TOGGLESHOWPERSISTANT:
		ret = DoActionToggleShowPersistant();
		break;
	case ActionEnums::PASTE_SELECTED:
		ret = DoActionPasteSelected();
		break;
	case ActionEnums::DELETE_SELECTED:
		ret = DoActionDeleteSelected();
		break;
	case ActionEnums::CLIP_PROPERTIES:
		ret = DoActionClipProperties();
		break;
	case ActionEnums::PASTE_SELECTED_PLAIN_TEXT:
		ret = DoActionPasteSelectedPlainText();
		break;
	case ActionEnums::MOVE_CLIP_TO_GROUP:
		ret = DoActionMoveClipToGroup();
		break;
	case ActionEnums::ELEVATE_PRIVlEGES:
		ret = DoActionElevatePrivleges();
		break;
	case ActionEnums::SHOW_IN_TASKBAR:
		ret = DoShowInTaskBar();
		break;
	case ActionEnums::COMPARE_SELECTED_CLIPS:
		ret = DoClipCompare();
		break;
	case ActionEnums::SELECT_LEFT_SIDE_COMPARE:
		ret = DoSelectLeftSideCompare();
		break;
	case ActionEnums::SELECT_RIGHT_SITE_AND_DO_COMPARE:
		ret = DoSelectRightSideAndDoCompare();
		break;
	case ActionEnums::EXPORT_TO_TEXT_FILE:
		ret = DoExportToTextFile();
		break;
	case ActionEnums::EXPORT_TO_QR_CODE:
		ret = DoExportToQRCode();
		break;
	case ActionEnums::EXPORT_TO_GOOGLE_TRANSLATE:
		ret = DoExportToGoogleTranslate();
		break;
	case ActionEnums::EXPORT_TO_BITMAP_FILE:
		ret = DoExportToBitMapFile();
		break;
	case ActionEnums::SAVE_CURRENT_CLIPBOARD:
		ret = DoSaveCurrentClipboard();
		break;
	case ActionEnums::MOVE_CLIP_DOWN:
		ret = DoMoveClipDown();
		break;
	case ActionEnums::MOVE_CLIP_UP:
		ret = DoMoveClipUp();
		break;
	case ActionEnums::MOVE_CLIP_TOP:
		ret = DoMoveClipTOP();
		break;
	case ActionEnums::MOVE_CLIP_LAST:
		ret = DoMoveClipLast();
		break;
	case ActionEnums::FILTER_ON_SELECTED_CLIP:
		ret = DoFilterOnSelectedClip();
		break;
	case ActionEnums::PASTE_UPPER_CASE:
		ret = DoPasteUpperCase();
		break;
	case ActionEnums::PASTE_LOWER_CASE:
		ret = DoPasteLowerCase();
		break;
	case ActionEnums::PASTE_CAPITALiZE:
		ret = DoPasteCapitalize();
		break;
	case ActionEnums::PASTE_SENTENCE_CASE:
		ret = DoPasteSentenceCase();
		break;
	case ActionEnums::INVERT_CASE:
		ret = DoInvertCase();
		break;
	case ActionEnums::PASTE_REMOVE_LINE_FEEDS:
		ret = DoPasteRemoveLineFeeds();
		break;
	case ActionEnums::PASTE_ADD_ONE_LINE_FEED:
		ret = DoPastePlusAddLineFeed();
		break;
	case ActionEnums::PASTE_ADD_TWO_LINE_FEEDS:
		ret = DoPasteAddTwoLineFeeds();
		break;
	case ActionEnums::PASTE_TYPOGLYCEMIA:
		ret = DoPasteTypoglycemia();
		break;
	case ActionEnums::PASTE_ADD_CURRENT_TIME:
		ret = DoPasteAddCurrentTime();
		break;
	case ActionEnums::SEND_TO_FRIEND_1:
		ret = SendToFriendbyPos(0, _T(""));
		break;
	case ActionEnums::SEND_TO_FRIEND_2:
		ret = SendToFriendbyPos(1, _T(""));
		break;
	case ActionEnums::SEND_TO_FRIEND_3:
		ret = SendToFriendbyPos(2, _T(""));
		break;
	case ActionEnums::SEND_TO_FRIEND_4:
		ret = SendToFriendbyPos(3, _T(""));
		break;
	case ActionEnums::SEND_TO_FRIEND_5:
		ret = SendToFriendbyPos(4, _T(""));
		break;
	case ActionEnums::SEND_TO_FRIEND_6:
		ret = SendToFriendbyPos(5, _T(""));
		break;
	case ActionEnums::SEND_TO_FRIEND_7:
		ret = SendToFriendbyPos(6, _T(""));
		break;
	case ActionEnums::SEND_TO_FRIEND_8:
		ret = SendToFriendbyPos(7, _T(""));
		break;
	case ActionEnums::SEND_TO_FRIEND_9:
		ret = SendToFriendbyPos(8, _T(""));
		break;
	case ActionEnums::SEND_TO_FRIEND_10:
		ret = SendToFriendbyPos(9, _T(""));
		break;
	case ActionEnums::SEND_TO_FRIEND_11:
		ret = SendToFriendbyPos(10, _T(""));
		break;
	case ActionEnums::SEND_TO_FRIEND_12:
		ret = SendToFriendbyPos(11, _T(""));
		break;
	case ActionEnums::SEND_TO_FRIEND_13:
		ret = SendToFriendbyPos(12, _T(""));
		break;
	case ActionEnums::SEND_TO_FRIEND_14:
		ret = SendToFriendbyPos(13, _T(""));
		break;
	case ActionEnums::SEND_TO_FRIEND_15:
		ret = SendToFriendbyPos(14, _T(""));
		break;
	case ActionEnums::PASTE_POSITION_1:
		ret = OpenIndex(0);
		break;
	case ActionEnums::PASTE_POSITION_2:
		ret = OpenIndex(1);
		break;
	case ActionEnums::PASTE_POSITION_3:
		ret = OpenIndex(2);
		break;
	case ActionEnums::PASTE_POSITION_4:
		ret = OpenIndex(3);
		break;
	case ActionEnums::PASTE_POSITION_5:
		ret = OpenIndex(4);
		break;
	case ActionEnums::PASTE_POSITION_6:
		ret = OpenIndex(5);
		break;
	case ActionEnums::PASTE_POSITION_7:
		ret = OpenIndex(6);
		break;
	case ActionEnums::PASTE_POSITION_8:
		ret = OpenIndex(7);
		break;
	case ActionEnums::PASTE_POSITION_9:
		ret = OpenIndex(8);
		break;
	case ActionEnums::PASTE_POSITION_10:
		ret = OpenIndex(9);
		break;
	case ActionEnums::PASTE_POSITION_1_PLAIN_TEXT:
		ret = OpenIndex(0, true);
		break;
	case ActionEnums::PASTE_POSITION_2_PLAIN_TEXT:
		ret = OpenIndex(1, true);
		break;
	case ActionEnums::PASTE_POSITION_3_PLAIN_TEXT:
		ret = OpenIndex(2, true);
		break;
	case ActionEnums::PASTE_POSITION_4_PLAIN_TEXT:
		ret = OpenIndex(3, true);
		break;
	case ActionEnums::PASTE_POSITION_5_PLAIN_TEXT:
		ret = OpenIndex(4, true);
		break;
	case ActionEnums::PASTE_POSITION_6_PLAIN_TEXT:
		ret = OpenIndex(5, true);
		break;
	case ActionEnums::PASTE_POSITION_7_PLAIN_TEXT:
		ret = OpenIndex(6, true);
		break;
	case ActionEnums::PASTE_POSITION_8_PLAIN_TEXT:
		ret = OpenIndex(7, true);
		break;
	case ActionEnums::PASTE_POSITION_9_PLAIN_TEXT:
		ret = OpenIndex(8, true);
		break;
	case ActionEnums::PASTE_POSITION_10_PLAIN_TEXT:
		ret = OpenIndex(9, true);
		break;
	case ActionEnums::CONFIG_SHOW_FIRST_TEN_TEXT:
		ret = OnShowFirstTenText();
		break;
	case ActionEnums::CONFIG_SHOW_CLIP_WAS_PASTED:
		ret = OnShowClipWasPasted();
		break;
	case ActionEnums::TOGGLE_LAST_GROUP_TOGGLE:
		ret = OnToggleLastGroupToggle();
		break;
	case ActionEnums::MAKE_TOP_STICKY:
		ret = OnMakeTopSticky(false);
		break;
	case ActionEnums::MAKE_LAST_STICKY:
		ret = OnMakeLastSticky();
		break;
	case ActionEnums::REMOVE_STICKY:
		ret = OnRemoveStickySetting();
		break;
	case ActionEnums::GLOBAl_HOTKEYS:
		ret = OnGlobalHotkyes();
		break;
	case ActionEnums::DELETE_CLIP_DATA:
		ret = OnDeleteClipData();
		break;
	case ActionEnums::IMPORT_CLIP:
		ret = OnImportClip();
		break;
	case ActionEnums::REPLACE_TOP_STICKY_CLIP:
		ret = DoActionReplaceTopStickyClip();
		break;
	case ActionEnums::PROMPT_SEND_TO_FRIEND:
		ret = DoActionPromptSendToFriend();
		break;
	case ActionEnums::SAVE_CF_HDROP_FIlE_DATA:
		ret = DoActionSaveCF_HDROP_FileData();
		break;
	case ActionEnums::TOGGLE_CLIPBOARD_CONNECTION:
		ret = DoActionToggleClipboardConnection();
		break;
	case ActionEnums::MOVE_SELECTION_UP:
		ret = DoActionMoveSelectionUp();
		break;
	case ActionEnums::MOVE_SELECTION_DOWN:
		ret = DoActionMoveSelectionDown();
		break;
	case ActionEnums::TOGGLE_DESCRIPTION_WORD_WRAP:
		ret = DoActionToggleDescriptionWordWrap();
		break;
	case ActionEnums::APPLY_LAST_SEARCH:
		ret = DoActionApplyLastSearch();
		break;
	case ActionEnums::TOGGLE_SEARCH_METHOD:
		ret = DoActionToggleSearchMethod();
		break;
	case ActionEnums::PASTE_SCRIPT:
		ret = DoActionPasteScript(a.RefData);
		break;
	case ActionEnums::PASTE_DONT_MOVE_CLIP:
		ret = DoActionPasteDontMoveClip();
		break;
	case ActionEnums::PASTE_TRIM_WHITE_SPACE:
		ret = DoActionPasteTrimWhiteSpace();
		break;
	case ActionEnums::PASTE_POSIXIFY_PATHS:
		ret = DoActionPastePosixifyPaths();
		break;
	case ActionEnums::TRANSPARENCY_NONE:
		SetTransparency(0);
		break;
	case ActionEnums::TRANSPARENCY_5:
		SetTransparency(5);
		break;
	case ActionEnums::TRANSPARENCY_10:
		SetTransparency(10);
		break;
	case ActionEnums::TRANSPARENCY_15:
		SetTransparency(15);
		break;
	case ActionEnums::TRANSPARENCY_20:
		SetTransparency(20);
		break;
	case ActionEnums::TRANSPARENCY_25:
		SetTransparency(25);
		break;
	case ActionEnums::TRANSPARENCY_30:
		SetTransparency(30);
		break;
	case ActionEnums::TRANSPARENCY_35:
		SetTransparency(35);
		break;
	case ActionEnums::TRANSPARENCY_40:
		SetTransparency(40);
		break;
	case ActionEnums::TRANSPARENCY_TOGGLE:
		DoActionToggleTransparency();
		break;
	case ActionEnums::TRANSPARENCY_INCREASE:
		DoActionIncreaseTransparency();
		break;
	case ActionEnums::TRANSPARENCY_DECREASE:
		DoActionDecreaseTransparency();
		break;
	case ActionEnums::EMAILTO_BODY:
		DoActionEmailTo();
		break;
	case ActionEnums::GMAIL:
		DoActionGmail();
		break;
	case ActionEnums::EMAILTO_ATTACH_EXPORT:
		DoActionEmailToAttachExport();
		break;
	case ActionEnums::EMAILTO_ATTACH_CONTENT:
		DoActionEmailToAttachContent();
		break;
	case ActionEnums::SLUGIFY:
		DoActionSlugify();
		break;
	case ActionEnums::COPY_SELECTION:
		ret = DoCopySelection();
		break;
	case ActionEnums::REFRESH_LIST:
		ret = DoRefreshList();
		break;
	case ActionEnums::DELETE_ALL_NON_USED_CLIPS:
		ret = DoDeleteAllNonUsedClips();
		break;
	case ActionEnums::SET_DRAG_FILE_NAME:
		ret = DoSetDragFileName();
		break;
	case ActionEnums::PASTE_CAMEL_CASE:
		ret = DoPasteCamelCase();
		break;
	case ActionEnums::PASTE_MULTI_IMAGE_HORIZONTAL:
		ret = DoPasteImagesHorz();
		break;
	case ActionEnums::PASTE_MULTI_IMAGE_VERTICAL:
		ret = DoPasteImagesVert();
		break;
	case ActionEnums::ASCII_TEXT_ONLY:
		ret = DoPasteAsciiOnly();
		break;
	case ActionEnums::EXPORT_TO_WEB_SEARCH:
		ret = DoExportToWebSearch();
		break;
	case ActionEnums::GENERATE_GUID:
		ret = DoActionGenerateGuid();
		break;
	case ActionEnums::PASTE_AS_IMAGE:
		ret = DoPasteAsImage();
		break;
	case ActionEnums::SHOW_STARRED_CLIPS:
		{ BOOL bHandled = TRUE; OnMenuShowStarredClips(0, ID_MENU_SHOWSTARREDCLIPS, NULL, bHandled); }
		ret = true;
		break;
	}

	return ret;
}

bool CQPasteWnd::DoSetDragFileName()
{
	m_bHideWnd = false;

	CGroupName Name;

	CDimWnd dimmer(m_hWnd);

	INT_PTR nRet = Name.DoModal();

	if (nRet == IDOK)
	{
		CString csName = Name.m_csName;
		CGetSetOptions::SetTempDragFileName(csName);
	}

	m_bHideWnd = true;

	return true;
}

bool CQPasteWnd::DoActionPasteDontMoveClip()
{
	CSpecialPasteOptions pasteOptions;
	pasteOptions.m_updateClipOrder = false;
	OpenSelection(pasteOptions);

	return true;
}

bool CQPasteWnd::DoActionPasteTrimWhiteSpace()
{
	CSpecialPasteOptions pasteOptions;
	pasteOptions.m_trimWhiteSpace = true;
	OpenSelection(pasteOptions);

	return true;
}

bool CQPasteWnd::DoActionPastePosixifyPaths()
{
	CSpecialPasteOptions pasteOptions;
	pasteOptions.m_PosixifyPaths = true;
	OpenSelection(pasteOptions);

	return true;
}

bool CQPasteWnd::DoActionPasteScript(CString scriptGuid)
{
	CSpecialPasteOptions pasteOptions;
	pasteOptions.m_pasteScriptGuid = scriptGuid;
	OpenSelection(pasteOptions);

	return true;
}

bool CQPasteWnd::DoActionToggleSearchMethod()
{
	if (CGetSetOptions::GetRegExTextSearch())
	{
		//if regex go back to wildcard
		CGetSetOptions::SetSimpleTextSearch(FALSE);
		CGetSetOptions::SetRegExTextSearch(FALSE);
	}
	else if (CGetSetOptions::GetSimpleTextSearch())
	{
		//if contains search go to regex
		CGetSetOptions::SetSimpleTextSearch(FALSE);
		CGetSetOptions::SetRegExTextSearch(TRUE);
	}
	else
	{
		//if wildcard to to contains
		CGetSetOptions::SetSimpleTextSearch(TRUE);
		CGetSetOptions::SetRegExTextSearch(FALSE);
	}
	return true;
}

bool CQPasteWnd::DoActionApplyLastSearch()
{
	return m_search.ApplyLastSearch();
}

bool CQPasteWnd::DoActionToggleDescriptionWordWrap()
{
	if (m_lstHeader.IsToolTipWindowVisible() == FALSE)
		return false;

	bool ret = m_lstHeader.ToggleToolTipWordWrap();
	return (ret == true);
}

bool CQPasteWnd::DoActionShowDescription()
{
	bool ret = false;

	m_actions.m_handleRepeatKeys = true;

	CString csText;
	m_search.GetWindowText(csText);
	if (csText != _T(""))
	{
		m_search.AddToSearchHistory();
	}

	if (m_lstHeader.IsToolTipWindowVisible() == false)
	{
		ret = m_lstHeader.ShowFullDescription(false, false);
	}
	else
	{
		if (csText != _T(""))
		{
			m_lstHeader.DoToolTipSearch();
			ret = true;
		}
		else
		{
			m_lstHeader.HideToolTip();
			ret = true;
		}
	}

	return (ret == true);
}

bool CQPasteWnd::DoActionNextDescription()
{
	if (m_lstHeader.IsToolTipWindowVisible() == FALSE)
		return false;

	if (CGetSetOptions::m_bAllwaysShowDescription)
		return false;

	m_actions.m_handleRepeatKeys = true;

	ARRAY Indexes;
	m_lstHeader.GetSelectionIndexes(Indexes);

	long caret = m_lstHeader.GetCaret();

	if (Indexes.GetCount() > 1)
	{
		for (int i = 0; i < Indexes.GetCount(); i++)
		{
			int index = Indexes[i];
			if (index == caret)
			{
				if (i < Indexes.GetCount() - 1)
				{
					caret = Indexes[i + 1];
					break;
				}
				else
				{
					caret = Indexes[0];
				}
			}
		}

		m_lstHeader.SetCaret(caret);
	}
	else
	{
		caret++;
		m_lstHeader.SetListPos(caret);
	}

	m_lstHeader.ShowFullDescription(false, true);

	return true;
}

bool CQPasteWnd::DoActionPrevDescription()
{
	if (m_lstHeader.IsToolTipWindowVisible() == FALSE)
		return false;

	if (CGetSetOptions::m_bAllwaysShowDescription)
		return false;

	m_actions.m_handleRepeatKeys = true;

	ARRAY Indexes;
	m_lstHeader.GetSelectionIndexes(Indexes);

	long caret = m_lstHeader.GetCaret();

	if (Indexes.GetCount() > 1)
	{
		for (int i = ((int)Indexes.GetCount()) - 1; i >= 0; i--)
		{
			int index = Indexes[i];
			if (index == caret)
			{
				if (i > 0)
				{
					caret = Indexes[i - 1];
					break;
				}
				else
				{
					caret = Indexes[((int)Indexes.GetCount()) - 1];
				}
			}
		}

		m_lstHeader.SetCaret(caret);
	}
	else
	{
		caret--;
		m_lstHeader.SetListPos(caret);
	}

	m_lstHeader.ShowFullDescription(false, true);

	return true;
}

bool CQPasteWnd::DoActionShowMenu()
{
	ShowRightClickMenu();
	return true;
}

bool CQPasteWnd::DoActionShowSystemMenu()
{
	BOOL bHandled = FALSE;
	OnSystemButton(0, ID_SYSTEM_BUTTON, m_systemMenu.m_hWnd, bHandled);
	return true;
}

bool CQPasteWnd::DoActionNewGroup()
{
	NewGroup(false, theApp.GetValidGroupID());

	return true;
}

bool CQPasteWnd::DoActionNewGroupSelection()
{
	NewGroup(true, theApp.GetValidGroupID());

	return true;
}

bool CQPasteWnd::DoActionToggleFileLogging()
{
	if (CGetSetOptions::m_bEnableDebugLogging)
	{
		Log(_T("turning file logging OFF"));
	}

	CGetSetOptions::m_bEnableDebugLogging = !CGetSetOptions::m_bEnableDebugLogging;

	if (CGetSetOptions::m_bEnableDebugLogging)
	{
		Log(_T("turning file logging ON"));
	}

	return true;
}

bool CQPasteWnd::DoActionToggleOutputDebugString()
{
	if (CGetSetOptions::m_bEnableDebugLogging)
	{
		Log(_T("turning DebugString logging OFF"));
	}

	CGetSetOptions::m_outputDebugStringLogging = !CGetSetOptions::m_outputDebugStringLogging;

	if (CGetSetOptions::m_bEnableDebugLogging)
	{
		Log(_T("turning DebugString logging ON"));
	}

	return true;
}

bool CQPasteWnd::DoActionForceCloseWindow()
{
	Log(_T("Force closing window from hot keys"));
	HideQPasteWindow(true);

	return true;
}

bool CQPasteWnd::DoActionCloseWindow()
{
	bool ret = false;
	if (m_bModifersMoveActive)
	{
		Log(_T("Escape key hit setting modifers to NOT active"));
		m_bModifersMoveActive = false;
		ret = true;
	}
	else
	{
		Log(_T("close 1"));

		if (m_lstHeader.IsToolTipShowPersistant() == false &&
			m_lstHeader.IsToolTipWindowVisible())
		{
			m_lstHeader.HidePopup(true);
			Log(_T("close 2"));
			ret = true;
		}
		else if (m_strSQLSearch.IsEmpty() == FALSE)
		{
			BOOL bDummy = FALSE;
			OnCancelFilter(NM_CANCEL_SEARCH, 0, 0, bDummy);
			Log(_T("close 3"));
			ret = true;
		}
		else
		{
			if (CGetSetOptions::GetShowPersistent() && this->GetMinimized() == false)
			{
				MinMaxWindow(FORCE_MIN);
				theApp.m_activeWnd.ReleaseFocus();

				Log(_T("close 4"));

				ret = true;
			}
			else
			{
				if (m_GroupTree.IsWindowVisible() == FALSE)
				{
					HideQPasteWindow(true);
					ret = true;
					Log(_T("close 5"));
				}
			}
		}
	}

	Log(_T("close 6"));

	return ret;
}

bool CQPasteWnd::DoActionNextTabControl()
{
	BOOL bPrev = FALSE;

	CWindow wFocus = ::GetFocus();
	if (wFocus != NULL)
	{
		CWindow wNext = GetNextDlgTabItem(wFocus.m_hWnd, bPrev);
		if (wNext != NULL)
		{
			wNext.SetFocus();
		}
	}
	return true;
}

bool CQPasteWnd::DoActionPrevTabControl()
{
	BOOL bPrev = TRUE;

	CWindow wFocus = ::GetFocus();
	if (wFocus != NULL)
	{
		CWindow wNext = GetNextDlgTabItem(wFocus.m_hWnd, bPrev);
		if (wNext != NULL)
		{
			wNext.SetFocus();
		}
	}
	return true;
}

bool CQPasteWnd::DoActionShowGroups()
{
	BOOL bHandled = FALSE;
	OnShowGroupsTop(0, ID_SHOW_GROUPS_TOP, NULL, bHandled);

	return true;
}

bool CQPasteWnd::DoActionNewClip()
{
	CClipIDs IDs;
	IDs.Add(-1);
	theApp.EditItems(IDs, true, true);

	return true;
}

bool CQPasteWnd::DoActionEditClip()
{
	if (m_lstHeader.GetSelectedCount() == 0)
	{
		return false;
	}

	CClipIDs IDs;
	m_lstHeader.GetSelectionItemData(IDs);

	bool textOnly = false;
	if (GetKeyState(VK_SHIFT) & 0x8000)
	{
		textOnly = true;
	}

	theApp.EditItems(IDs, true, textOnly);

	return true;
}

bool CQPasteWnd::DoActionMoveSelectionUp()
{
	MoveSelection(false, false);
	m_actions.m_handleRepeatKeys = true;
	return true;
}

bool CQPasteWnd::DoActionMoveSelectionDown()
{
	MoveSelection(true, false);
	m_actions.m_handleRepeatKeys = true;
	return true;
}

bool CQPasteWnd::DoModifierActiveActionSelectionUp()
{
	if (m_bModifersMoveActive)
	{
		MoveSelection(false, true);
		m_modifierKeyActions.m_handleRepeatKeys = true;
		return true;
	}

	return false;
}

bool CQPasteWnd::DoModifierActiveActionSelectionDown()
{
	if (m_bModifersMoveActive)
	{
		MoveSelection(true, true);
		m_modifierKeyActions.m_handleRepeatKeys = true;
		return true;
	}

	return false;
}

bool CQPasteWnd::DoModifierActiveActionMoveFirst()
{
	if (m_bModifersMoveActive)
	{
		m_lstHeader.SetListPos(0);
		return true;
	}

	return false;
}

bool CQPasteWnd::DoModifierActiveActionMoveLast()
{
	if (m_bModifersMoveActive)
	{
		if (m_lstHeader.GetItemCount() > 0)
		{
			m_lstHeader.SetListPos(m_lstHeader.GetItemCount() - 1);
		}
		return true;
	}

	return false;
}

bool CQPasteWnd::DoActionCancelFilter()
{
	m_bShowStarredClips = false;

	FillList();

	m_bHandleSearchTextChange = false;
	m_search.SetWindowText(_T(""));
	m_bHandleSearchTextChange = true;

	MoveControls();

	m_lstHeader.SetFocus();

	return true;
}

bool CQPasteWnd::DoActionHomeList()
{
	if (::GetFocus() == m_lstHeader.m_hWnd)
	{
		theApp.EnterGroupID(-1); // History
		return true;
	}

	return false;
}

bool CQPasteWnd::DoActionBackGroup()
{
	if (::GetFocus() == m_lstHeader.m_hWnd)
	{
		theApp.EnterGroupID(theApp.m_GroupParentID);
		return true;
	}

	return false;
}

bool CQPasteWnd::DoActionToggleShowPersistant()
{
	if (m_lstHeader.IsToolTipWindowVisible())
	{
		m_lstHeader.ToggleToolTipShowPersistant();
	}
	else
	{
		theApp.ShowPersistent(!CGetSetOptions::m_bShowPersistent);
		if (CGetSetOptions::m_bShowPersistent)
		{
			::SetWindowPos(m_hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_SHOWWINDOW);
		}

		MoveControls();

		UpdateStatus();
	}
	return true;
}

bool CQPasteWnd::DoActionPasteSelected()
{
	CSpecialPasteOptions pasteOptions;
	OpenSelection(pasteOptions);
	return true;
}

bool CQPasteWnd::DoActionDeleteSelected()
{
	if (::GetFocus() == m_lstHeader.m_hWnd)
	{
		m_actions.m_handleRepeatKeys = true;
		DeleteSelectedRows();
		return true;
	}

	return false;
}

bool CQPasteWnd::DoActionClipProperties()
{
	bool ret = false;

	if (::GetFocus() == m_lstHeader.m_hWnd)
	{
		m_bHideWnd = false;

		ARRAY IDs, Indexes;
		m_lstHeader.GetSelectionItemData(IDs);
		m_lstHeader.GetSelectionIndexes(Indexes);

		INT_PTR size = IDs.GetSize();
		if (size < 1)
		{
			return ret;
		}

		int id = IDs[0];
		int row = Indexes[0];

		if (id < 0)
		{
			return ret;
		}

		m_lstHeader.RemoveAllSelection();
		m_lstHeader.SetSelection(row);

		ret = ShowProperties(id, row);

		m_lstHeader.SetListPos(row);
	}

	return false;
}

bool CQPasteWnd::ShowProperties(int id, int row)
{
	if (id < 0)
	{
		return false;
	}

	m_bHideWnd = false;

	CDimWnd dimmer(m_hWnd);

	CCopyProperties props(id, m_hWnd);
	INT_PTR doModalRet = props.DoModal();

	if (doModalRet == IDOK)
	{
		{
			ATL::CCritSecLock csLock(m_CritSection);

			if (row < 0)
			{
				bool selectedItem = false;
				int index = 0;
				std::vector<CMainTable>::iterator iter = m_listItems.begin();
				while (iter != m_listItems.end())
				{
					if (iter->m_lID == id)
					{
						row = index;
						break;
					}
					iter++;
					index++;
				}
			}

			if (row >= 0 &&
				row < (int)m_listItems.size())
			{
				CppSQLite3Query q = theApp.m_db.execQueryEx(_T("SELECT * FROM Main WHERE lID = %d"), id);
				if (!q.eof())
				{
					FillMainTable(m_listItems[row], q);
				}

				RemoveFromImageRtfCache(row);
			}
		}

		m_extraDataThread.FireLoadAccelerators();

		m_lstHeader.RefreshVisibleRows();

		if (props.m_lGroupChangedTo >= 0)
		{
			CSpecialPasteOptions pasteOptions;
			OpenID(props.m_lGroupChangedTo, pasteOptions);
		}

		m_lstHeader.SetFocus();
	}

	m_bHideWnd = true;
	return true;
}

bool CQPasteWnd::DoActionPasteSelectedPlainText()
{
	CSpecialPasteOptions pasteOptions;
	pasteOptions.m_pasteAsPlainText = true;
	OpenSelection(pasteOptions);
	return true;
}

bool CQPasteWnd::DoActionMoveClipToGroup()
{
	if (::GetFocus() == m_lstHeader.m_hWnd)
	{
		m_bHideWnd = false;

		CDimWnd dimmer(m_hWnd);

		CMoveToGroupDlg dlg;

		INT_PTR nRet = dlg.DoModal();
		if (nRet == IDOK)
		{
			int nGroup = dlg.GetSelectedGroup();

			Log(StrF(_T("Move to Group, GroupId: %d"), nGroup));

			if (nGroup >= -1)
			{
				CClipIDs IDs;
				m_lstHeader.GetSelectionItemData(IDs);

				IDs.MoveTo(nGroup);
			}
			FillList();
		}

		m_bHideWnd = true;
		return true;
	}

	return false;
}

bool CQPasteWnd::DoActionElevatePrivleges()
{
	CGetSetOptions::SetPasteAsAdmin(!CGetSetOptions::GetPasteAsAdmin());

	return true;
}

bool CQPasteWnd::DoShowInTaskBar()
{
	CGetSetOptions::SetShowInTaskBar(!CGetSetOptions::GetShowInTaskBar());

	theApp.RefreshShowInTaskBar();

	return true;
}

bool CQPasteWnd::DoClipCompare()
{
	ARRAY IDs;
	m_lstHeader.GetSelectionItemData(IDs);

	if (IDs.GetCount() > 1)
	{
		if (!CGetSetOptions::m_bShowPersistent)
		{
			HideQPasteWindow(false, false);
		}
		else if (CGetSetOptions::GetAutoHide())
		{
			MinMaxWindow(FORCE_MIN);
		}

		CClipCompare compare;
		compare.Compare(IDs[0], IDs[1]);

		return true;
	}
	else
	{
		Log(StrF(_T("DoClipCompare, at least 2 clips need to be selected, count: %d"), IDs.GetCount()));
	}

	return false;
}

bool CQPasteWnd::DoSelectLeftSideCompare()
{
	ARRAY IDs;
	m_lstHeader.GetSelectionItemData(IDs);

	if (IDs.GetCount() > 0)
	{
		m_leftSelectedCompareId = IDs[0];

		return true;
	}
	else
	{
		Log(StrF(_T("DoSelectLeftSideCompare, no selected clip, not assigning left side")));
	}

	return false;
}

bool CQPasteWnd::DoSelectRightSideAndDoCompare()
{
	if (m_leftSelectedCompareId > 0)
	{
		ARRAY IDs;
		m_lstHeader.GetSelectionItemData(IDs);

		if (IDs.GetCount() > 0)
		{
			int rightId = IDs[0];

			if (!CGetSetOptions::m_bShowPersistent)
			{
				HideQPasteWindow(false, false);
			}
			else if (CGetSetOptions::GetAutoHide())
			{
				MinMaxWindow(FORCE_MIN);
			}

			CClipCompare compare;
			compare.Compare(m_leftSelectedCompareId, rightId);

			return true;
		}
		else
		{
			Log(StrF(_T("DoSelectRightSideAndDoCompare, no selected clips")));
		}
	}
	else
	{
		Log(StrF(_T("DoSelectRightSideAndDoCompare, no left side selected, select left side first")));
	}

	return false;
}

bool CQPasteWnd::DoExportToTextFile()
{
	bool ret = false;

	CClipIDs IDs;
	INT_PTR lCount = m_lstHeader.GetSelectedCount();
	if (lCount <= 0)
	{
		return ret;
	}

	m_lstHeader.GetSelectionItemData(IDs);
	lCount = IDs.GetSize();
	if (lCount <= 0)
	{
		return ret;
	}

	OPENFILENAME ofn;
	TCHAR szFile[400];
	TCHAR szDir[400];

	memset(&szFile, 0, sizeof(szFile));
	memset(szDir, 0, sizeof(szDir));
	memset(&ofn, 0, sizeof(ofn));

	CString csInitialDir = CGetSetOptions::GetLastImportDir();
	STRCPY(szDir, csInitialDir);

	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = m_hWnd;
	ofn.lpstrFile = szFile;
	ofn.nMaxFile = sizeof(szFile);
	ofn.lpstrFilter = _T("Exported Ditto Clips (.txt)\0*.txt\0\0");
	ofn.nFilterIndex = 1;
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = szDir;
	ofn.lpstrDefExt = _T("txt");
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_OVERWRITEPROMPT | OFN_NOCHANGEDIR;

	m_bHideWnd = false;

	if (GetSaveFileName(&ofn))
	{
		using namespace nsPath;
		CString startingFilePath = ofn.lpstrFile;
		CPath path(ofn.lpstrFile);
		CString csPath = path.GetPath();
		CString csExt = path.GetExtension();
		path.RemoveExtension();
		CString csFileName = path.GetName();

		CGetSetOptions::SetLastExportDir(csPath);
		int lastFileCheckId = 1;

		for (int i = 0; i < IDs.GetCount(); i++)
		{
			int id = IDs[i];

			CClip clip;
			if (clip.LoadFormats(id, true))
			{
				CString savePath = startingFilePath;
				if (IDs.GetCount() > 1 ||
					FileExists(startingFilePath))
				{
					savePath = _T("");

					for (int y = lastFileCheckId; y < 1000000; y++)
					{
						CString testFilePath;
						testFilePath.Format(_T("%s%s_%d.%s"), csPath, csFileName, y, csExt);
						if (FileExists(testFilePath) == FALSE)
						{
							savePath = testFilePath;
							lastFileCheckId = y + 1;
							break;
						}
					}
				}

				if (savePath != _T(""))
				{
					clip.WriteTextToFile(savePath, true, true, false);

					ret = true;
				}
				else
				{
					Log(StrF(_T("Failed to find a valid file name for starting path: %s"), startingFilePath));
				}
			}
		}
	}

	m_bHideWnd = true;

	return ret;
}

bool CQPasteWnd::DoExportToQRCode()
{
	bool ret = false;

	ARRAY IDs;
	m_lstHeader.GetSelectionItemData(IDs);

	if (IDs.GetCount() > 0)
	{
		int id = IDs[0];
		CClip clip;
		if (clip.LoadMainTable(id))
		{
			if (clip.LoadFormats(id, true))
			{
				CString clipText = clip.GetUnicodeTextFormat();

				CString clipTextUrlEncoded = InternetEncode(clipText);
				CString qrCodeUrl = CGetSetOptions::GetQRCodeUrl();
				CString url = StrF(_T("%s%s"), qrCodeUrl, clipTextUrlEncoded);

				Log(StrF(_T("Opening qr code url: %s"), url));

				if (!CGetSetOptions::m_bShowPersistent)
				{
					HideQPasteWindow(false, false);
				}
				else if (CGetSetOptions::GetAutoHide())
				{
					MinMaxWindow(FORCE_MIN);
				}

				::ShellExecute(NULL, _T("open"), url, NULL, NULL, SW_SHOWNORMAL);
			}
		}
	}

	return ret;
}

bool CQPasteWnd::DoExportToGoogleTranslate()
{
	bool ret = false;

	ARRAY IDs;
	m_lstHeader.GetSelectionItemData(IDs);

	if (IDs.GetCount() > 0)
	{
		int id = IDs[0];
		CClip clip;
		if (clip.LoadMainTable(id))
		{
			if (clip.LoadFormats(id, true))
			{
				CString clipText = clip.GetUnicodeTextFormat();
				if (clipText == _T(""))
				{
					CStringA aText = clip.GetCFTextTextFormat();
					if (aText != _T(""))
					{
						clipText = CTextConvert::AnsiToUnicode(aText);
					}
				}

				if (clipText != _T(""))
				{
					CString clipTextUrlEncoded = InternetEncode(clipText);

					CString url;
					url.Format(CGetSetOptions::GetTranslateUrl(), clipTextUrlEncoded);

					if (!CGetSetOptions::m_bShowPersistent)
					{
						HideQPasteWindow(false, false);
					}
					else if (CGetSetOptions::GetAutoHide())
					{
						MinMaxWindow(FORCE_MIN);
					}

					::ShellExecute(NULL, _T("open"), url, NULL, NULL, SW_SHOWNORMAL);

					ret = true;
				}
			}
		}
	}

	return true;
}

bool CQPasteWnd::DoExportToWebSearch()
{
	bool ret = false;

	ARRAY IDs;
	m_lstHeader.GetSelectionItemData(IDs);

	if (IDs.GetCount() > 0)
	{
		int id = IDs[0];
		CClip clip;
		if (clip.LoadMainTable(id))
		{
			if (clip.LoadFormats(id, true))
			{
				CString clipText = clip.GetUnicodeTextFormat();
				if (clipText == _T(""))
				{
					CStringA aText = clip.GetCFTextTextFormat();
					if (aText != _T(""))
					{
						clipText = CTextConvert::AnsiToUnicode(aText);
					}
				}

				if (clipText != _T(""))
				{
					CString clipTextUrlEncoded = InternetEncode(clipText);

					CString url;

					url.Format(CGetSetOptions::GetWebSearchUrl(), clipTextUrlEncoded);

					if (!CGetSetOptions::m_bShowPersistent)
					{
						HideQPasteWindow(false, false);
					}
					else if (CGetSetOptions::GetAutoHide())
					{
						MinMaxWindow(FORCE_MIN);
					}

					::ShellExecute(NULL, _T("open"), url, NULL, NULL, SW_SHOWNORMAL);

					ret = true;
				}
			}
		}
	}

	return true;
}

bool CQPasteWnd::DoActionGenerateGuid()
{
	if (::GetFocus() == m_lstHeader.m_hWnd)
	{
		CSpecialPasteOptions pasteOptions;
		pasteOptions.m_pasteGuid = true;
		OpenSelection(pasteOptions);
		return true;
	}

	return false;
}

bool CQPasteWnd::DoPasteAsImage()
{
	if (::GetFocus() == m_lstHeader.m_hWnd)
	{
		CSpecialPasteOptions pasteOptions;
		pasteOptions.m_pasteAsImage = true;
		OpenSelection(pasteOptions);
		return true;
	}

	return false;
}

bool CQPasteWnd::DoSaveCurrentClipboard()
{
	theApp.m_pMainFrame->PostMessage(WM_SAVE_CLIPBOARD, 0, 0);

	return true;
}

bool CQPasteWnd::DoMoveClipDown()
{
	ARRAY IDs;
	m_lstHeader.GetSelectionItemData(IDs);

	m_actions.m_handleRepeatKeys = true;

	if (IDs.GetCount() > 0)
	{
		bool sort = false;
		for (int i = ((int)IDs.GetCount()) - 1; i >= 0; i--)
		{
			int id = IDs[i];
			CClip clip;
			if (clip.LoadMainTable(id))
			{
				clip.MoveDown(theApp.m_GroupID);
				clip.ModifyMainTable();

				sort = SyncClipDataToArrayData(clip);
			}
		}

		if (sort)
		{
			if (theApp.m_GroupID > 0)
			{
				std::sort(m_listItems.begin(), m_listItems.end(), CMainTable::GroupSortDesc);
			}
			else
			{
				std::sort(m_listItems.begin(), m_listItems.end(), CMainTable::SortDesc);
			}

			SelectIds(IDs);

			m_lstHeader.RefreshVisibleRows();
			m_lstHeader.RedrawWindow();
		}
	}

	return true;
}

bool CQPasteWnd::DoMoveClipUp()
{
	ARRAY IDs;
	m_lstHeader.GetSelectionItemData(IDs);

	m_actions.m_handleRepeatKeys = true;

	if (IDs.GetCount() > 0)
	{
		bool sort = false;
		for (int i = 0; i < IDs.GetCount(); i++)
		{
			int id = IDs[i];
			CClip clip;
			if (clip.LoadMainTable(id))
			{
				clip.MoveUp(theApp.m_GroupID);
				clip.ModifyMainTable();

				sort = SyncClipDataToArrayData(clip);
			}
		}

		if (sort)
		{
			if (theApp.m_GroupID > 0)
			{
				std::sort(m_listItems.begin(), m_listItems.end(), CMainTable::GroupSortDesc);
			}
			else
			{
				std::sort(m_listItems.begin(), m_listItems.end(), CMainTable::SortDesc);
			}

			SelectIds(IDs);

			m_lstHeader.RefreshVisibleRows();
			m_lstHeader.RedrawWindow();
		}
	}

	return true;
}

bool CQPasteWnd::DoMoveClipLast()
{
	ARRAY IDs;
	m_lstHeader.GetSelectionItemData(IDs);

	if (IDs.GetCount() > 0)
	{
		bool sort = false;
		for (int i = 0; i < IDs.GetCount(); i++)
		{
			int id = IDs[i];
			CClip clip;
			if (clip.LoadMainTable(id))
			{
				if (theApp.m_GroupID > 0)
				{
					clip.MakeLastGroupOrder();
				}
				else
				{
					clip.MakeLastOrder();
				}
				clip.ModifyMainTable();

				//have we loaded all clips, if so then sort and select
				if (m_listItems.size() == m_lstHeader.GetItemCount())
				{
					sort = SyncClipDataToArrayData(clip);
				}
				else
				{
					//haven't loaded all clips so this will be out of what we have loaded
					//remove this from the list, will be shown as they scroll down the list
					std::vector<CMainTable>::iterator iter = m_listItems.begin();
					while (iter != m_listItems.end())
					{
						if (iter->m_lID == clip.ID())
						{
							m_listItems.erase(iter);
							break;
						}
						iter++;
					}
				}
			}
		}

		if (sort)
		{
			if (theApp.m_GroupID > 0)
			{
				std::sort(m_listItems.begin(), m_listItems.end(), CMainTable::GroupSortDesc);
			}
			else
			{
				std::sort(m_listItems.begin(), m_listItems.end(), CMainTable::SortDesc);
			}

			SelectIds(IDs);
		}

		m_lstHeader.RefreshVisibleRows();
		m_lstHeader.RedrawWindow();
	}

	return true;
}

bool CQPasteWnd::DoMoveClipTOP()
{
	ARRAY IDs;
	m_lstHeader.GetSelectionItemData(IDs);

	if (IDs.GetCount() > 0)
	{
		bool sort = false;
		for (int i = 0; i < IDs.GetCount(); i++)
		{
			int id = IDs[i];
			CClip clip;
			if (clip.LoadMainTable(id))
			{
				if (theApp.m_GroupID > 0)
				{
					clip.MakeLatestGroupOrder();
				}
				else
				{
					clip.MakeLatestOrder();
				}
				clip.ModifyMainTable();

				sort = SyncClipDataToArrayData(clip);
			}
		}

		if (sort)
		{
			if (theApp.m_GroupID > 0)
			{
				std::sort(m_listItems.begin(), m_listItems.end(), CMainTable::GroupSortDesc);
			}
			else
			{
				std::sort(m_listItems.begin(), m_listItems.end(), CMainTable::SortDesc);
			}

			SelectIds(IDs);

			m_lstHeader.RefreshVisibleRows();
			m_lstHeader.RedrawWindow();
		}
	}

	return true;
}

bool CQPasteWnd::DoFilterOnSelectedClip()
{
	bool ret = false;
	ARRAY IDs, Indexes;
	m_lstHeader.GetSelectionItemData(IDs);

	INT_PTR size = IDs.GetSize();
	if (size > 0)
	{
		int id = IDs[0];

		ATL::CCritSecLock csLock(m_CritSection);
		std::vector<CMainTable>::iterator iter = m_listItems.begin();
		while (iter != m_listItems.end())
		{
			if (iter->m_lID == id)
			{
				m_bHandleSearchTextChange = false;
				m_search.SetWindowText(iter->m_Desc);
				m_bHandleSearchTextChange = true;
				BOOL bDummy = FALSE;
				OnSearch(CB_SEARCH, 0, 0, bDummy);
				ret = true;
				break;
			}
			iter++;
		}
	}

	return ret;
}

bool CQPasteWnd::DoPasteUpperCase()
{
	if (::GetFocus() == m_lstHeader.m_hWnd)
	{
		CSpecialPasteOptions pasteOptions;
		pasteOptions.m_pasteUpperCase = true;
		OpenSelection(pasteOptions);
		return true;
	}

	return false;
}

bool CQPasteWnd::DoPasteCamelCase()
{
	if (::GetFocus() == m_lstHeader.m_hWnd)
	{
		CSpecialPasteOptions pasteOptions;
		pasteOptions.m_pasteCamelCase = true;
		OpenSelection(pasteOptions);
		return true;
	}

	return false;
}

bool CQPasteWnd::DoPasteImagesHorz()
{
	if (::GetFocus() == m_lstHeader.m_hWnd)
	{
		CSpecialPasteOptions pasteOptions;
		pasteOptions.m_pasteImagesHorizontal = true;
		OpenSelection(pasteOptions);
		return true;
	}

	return false;
}

bool CQPasteWnd::DoPasteImagesVert()
{
	if (::GetFocus() == m_lstHeader.m_hWnd)
	{
		CSpecialPasteOptions pasteOptions;
		pasteOptions.m_pasteImagesVertically = true;
		OpenSelection(pasteOptions);
		return true;
	}

	return false;
}

bool CQPasteWnd::DoPasteAsciiOnly()
{
	if (::GetFocus() == m_lstHeader.m_hWnd)
	{
		CSpecialPasteOptions pasteOptions;
		pasteOptions.m_pasteAsciiOnly = true;
		OpenSelection(pasteOptions);
		return true;
	}

	return false;
}

bool CQPasteWnd::DoPasteLowerCase()
{
	if (::GetFocus() == m_lstHeader.m_hWnd)
	{
		CSpecialPasteOptions pasteOptions;
		pasteOptions.m_pasteLowerCase = true;
		OpenSelection(pasteOptions);
		return true;
	}

	return false;
}

bool CQPasteWnd::DoPasteCapitalize()
{
	if (::GetFocus() == m_lstHeader.m_hWnd)
	{
		CSpecialPasteOptions pasteOptions;
		pasteOptions.m_pasteCapitalize = true;
		OpenSelection(pasteOptions);
		return true;
	}

	return false;
}

bool CQPasteWnd::DoPasteSentenceCase()
{
	if (::GetFocus() == m_lstHeader.m_hWnd)
	{
		CSpecialPasteOptions pasteOptions;
		pasteOptions.m_pasteSentenceCase = true;
		OpenSelection(pasteOptions);
		return true;
	}

	return false;
}

bool CQPasteWnd::DoInvertCase()
{
	if (::GetFocus() == m_lstHeader.m_hWnd)
	{
		CSpecialPasteOptions pasteOptions;
		pasteOptions.m_invertCase = true;
		OpenSelection(pasteOptions);
		return true;
	}

	return false;
}

bool CQPasteWnd::DoPasteRemoveLineFeeds()
{
	if (::GetFocus() == m_lstHeader.m_hWnd)
	{
		CSpecialPasteOptions pasteOptions;
		pasteOptions.m_pasteRemoveLineFeeds = true;
		OpenSelection(pasteOptions);
		return true;
	}

	return false;
}

bool CQPasteWnd::DoPastePlusAddLineFeed()
{
	if (::GetFocus() == m_lstHeader.m_hWnd)
	{
		CSpecialPasteOptions pasteOptions;
		pasteOptions.m_pasteAddOneLineFeed = true;
		OpenSelection(pasteOptions);
		return true;
	}

	return false;
}

bool CQPasteWnd::DoPasteAddTwoLineFeeds()
{
	if (::GetFocus() == m_lstHeader.m_hWnd)
	{
		CSpecialPasteOptions pasteOptions;
		pasteOptions.m_pasteAddTwoLineFeeds = true;
		OpenSelection(pasteOptions);
		return true;
	}

	return false;
}

bool CQPasteWnd::DoPasteTypoglycemia()
{
	if (::GetFocus() == m_lstHeader.m_hWnd)
	{
		CSpecialPasteOptions pasteOptions;
		pasteOptions.m_pasteTypoglycemia = true;
		OpenSelection(pasteOptions);
		return true;
	}

	return false;
}

bool CQPasteWnd::DoPasteAddCurrentTime()
{
	if (::GetFocus() == m_lstHeader.m_hWnd)
	{
		CSpecialPasteOptions pasteOptions;
		pasteOptions.m_pasteAddingDateTime = true;
		OpenSelection(pasteOptions);
		return true;
	}

	return false;
}

bool CQPasteWnd::OnShowFirstTenText()
{
	CGetSetOptions::SetShowTextForFirstTenHotKeys(!CGetSetOptions::GetShowTextForFirstTenHotKeys());
	m_lstHeader.SetShowTextForFirstTenHotKeys(CGetSetOptions::GetShowTextForFirstTenHotKeys());

	m_lstHeader.RefreshVisibleRows();
	m_lstHeader.RedrawWindow();
	return true;
}

bool CQPasteWnd::OnShowClipWasPasted()
{
	CGetSetOptions::SetShowIfClipWasPasted(!CGetSetOptions::GetShowIfClipWasPasted());
	m_lstHeader.SetShowIfClipWasPasted(CGetSetOptions::GetShowIfClipWasPasted());

	m_lstHeader.RefreshVisibleRows();
	m_lstHeader.RedrawWindow();
	return true;
}

bool CQPasteWnd::OnToggleLastGroupToggle()
{
	int newGroupId = -2;
	if (theApp.m_GroupID > 0)
	{
		CGetSetOptions::SetLastGroupToggle(theApp.m_GroupID);
		newGroupId = -1;
	}
	else
	{
		newGroupId = CGetSetOptions::GetLastGroupToggle();
	}

	if (newGroupId >= -1)
	{
		theApp.EnterGroupID(newGroupId);
	}

	return true;
}

bool CQPasteWnd::OnMakeTopSticky(bool forceSort)
{
	ARRAY IDs;
	m_lstHeader.GetSelectionItemData(IDs);

	if (IDs.GetCount() > 0)
	{
		bool sort = forceSort;
		for (int i = ((int)IDs.GetCount()) - 1; i >= 0; i--)
		{
			int id = IDs[i];
			CClip clip;
			if (clip.LoadMainTable(id))
			{
				clip.MakeStickyTop(theApp.m_GroupID);
				clip.ModifyMainTable();

				sort = SyncClipDataToArrayData(clip);
			}
		}

		if (sort)
		{
			if (theApp.m_GroupID > 0)
			{
				std::sort(m_listItems.begin(), m_listItems.end(), CMainTable::GroupSortDesc);
			}
			else
			{
				std::sort(m_listItems.begin(), m_listItems.end(), CMainTable::SortDesc);
			}

			SelectIds(IDs);

			m_lstHeader.RefreshVisibleRows();
			m_lstHeader.RedrawWindow();
		}
	}

	return true;
}

bool CQPasteWnd::OnMakeLastSticky()
{
	ARRAY IDs;
	m_lstHeader.GetSelectionItemData(IDs);

	if (IDs.GetCount() > 0)
	{
		bool sort = false;
		for (int i = ((int)IDs.GetCount()) - 1; i >= 0; i--)
		{
			int id = IDs[i];
			CClip clip;
			if (clip.LoadMainTable(id))
			{
				clip.MakeStickyLast(theApp.m_GroupID);
				clip.ModifyMainTable();

				sort = SyncClipDataToArrayData(clip);
			}
		}

		if (sort)
		{
			if (theApp.m_GroupID > 0)
			{
				std::sort(m_listItems.begin(), m_listItems.end(), CMainTable::GroupSortDesc);
			}
			else
			{
				std::sort(m_listItems.begin(), m_listItems.end(), CMainTable::SortDesc);
			}

			SelectIds(IDs);

			m_lstHeader.RefreshVisibleRows();
			m_lstHeader.RedrawWindow();
		}
	}

	return true;
}

bool CQPasteWnd::OnRemoveStickySetting()
{
	ARRAY IDs;
	m_lstHeader.GetSelectionItemData(IDs);

	if (IDs.GetCount() > 0)
	{
		bool sort = false;
		for (int i = ((int)IDs.GetCount()) - 1; i >= 0; i--)
		{
			RemoveStickyInternal(IDs[i], sort);
		}

		//theApp.m_FocusID = id;

		if (sort)
		{
			if (theApp.m_GroupID > 0)
			{
				std::sort(m_listItems.begin(), m_listItems.end(), CMainTable::GroupSortDesc);
			}
			else
			{
				std::sort(m_listItems.begin(), m_listItems.end(), CMainTable::SortDesc);
			}

			//SelectFocusID();

			m_lstHeader.RefreshVisibleRows();
			m_lstHeader.RedrawWindow();
		}
	}

	return true;
}

void CQPasteWnd::RemoveStickyInternal(int id, bool& sort)
{
	CClip clip;
	if (clip.LoadMainTable(id))
	{
		if (clip.RemoveStickySetting(theApp.m_GroupID))
		{
			clip.ModifyMainTable();

			std::vector<CMainTable>::iterator iter = m_listItems.begin();
			while (iter != m_listItems.end())
			{
				if (iter->m_lID == id)
				{
					if (theApp.m_GroupID > 0)
					{
						iter->m_stickyClipGroupOrder = clip.m_stickyClipGroupOrder;
					}
					else
					{
						iter->m_stickyClipOrder = clip.m_stickyClipOrder;
					}
					sort = true;
					break;
				}
				iter++;
			}
		}
	}
}

bool CQPasteWnd::OnNewClip()
{
	CWindow wndMain(theApp.m_MainhWnd);
	if (wndMain.m_hWnd != NULL)
	{
		wndMain.SendMessage(WM_COMMAND, ID_FIRST_NEWCLIP, 0);
	}

	return true;
}

bool CQPasteWnd::OnImportClip()
{
	CWindow wndMain(theApp.m_MainhWnd);
	if (wndMain.m_hWnd != NULL)
	{
		wndMain.SendMessage(WM_COMMAND, ID_FIRST_IMPORT, 0);
	}

	return true;
}

bool CQPasteWnd::DoActionReplaceTopStickyClip()
{
	ARRAY IDs;
	IDs.Add(m_lstHeader.GetItemData(0));

	if (IDs.GetCount() > 0)
	{
		bool sort = false;
		for (int i = ((int)IDs.GetCount()) - 1; i >= 0; i--)
		{
			RemoveStickyInternal(IDs[i], sort);
		}

		OnMakeTopSticky(true);
	}

	return true;
}

bool CQPasteWnd::DoActionSaveCF_HDROP_FileData()
{
	if (m_lstHeader.GetSelectedCount() == 0)
	{
		return false;
	}

	CWaitCursor wait;

	CString errorMessage;

	CClipIDs IDs;
	m_lstHeader.GetSelectionItemData(IDs);
	ARRAY Indexs;
	m_lstHeader.GetSelectionIndexes(Indexs);

	if (IDs.GetCount() > 0)
	{
		for (int i = 0; i < min(Indexs.GetCount(), IDs.GetCount()); i++)
		{
			int row = Indexs[i];
			int id = IDs[i];
			CClip clip;
			if (clip.LoadMainTable(id))
			{
				if (clip.LoadFormats(id))
				{
					CString localErrorMessage;
					if (clip.AddFileDataToData(localErrorMessage))
					{
						if (row >= 0 &&
							row < (int)m_listItems.size())
						{
							CppSQLite3Query q = theApp.m_db.execQueryEx(_T("SELECT * FROM Main WHERE lID = %d"), id);
							if (!q.eof())
							{
								FillMainTable(m_listItems[row], q);
							}
						}
					}

					errorMessage += localErrorMessage;
				}
			}
		}
	}

	if (errorMessage.GetLength() > 0)
	{
		m_popupMsg.Show(errorMessage, CPoint(0, 0), true);
		SetTimer(TIMER_ERROR_MSG, CGetSetOptions::GetErrorMsgPopupTimeout(), NULL);
	}

	m_lstHeader.RefreshVisibleRows();

	return true;
}

bool CQPasteWnd::DoActionPromptSendToFriend()
{
	m_bHideWnd = false;

	CDimWnd dimmer(m_hWnd);

	CFriendPromptDlg dlg;
	if (dlg.DoModal(m_hWnd) == IDOK)
	{
		if (dlg.GetClearList())
		{
			m_customFriendsHelper.ClearList();
		}
		else
		{
			CString name = dlg.GetName();
			if (name != _T(""))
			{
				if (dlg.GetSave())
				{
					m_customFriendsHelper.Add(name, dlg.GetDesc());
				}
				SendToFriendbyPos(0, name);
			}
		}
	}

	m_bHideWnd = true;

	return true;
}

bool CQPasteWnd::DoActionToggleClipboardConnection()
{
	theApp.ToggleConnectCV();
	UpdateStatus();

	return true;
}

bool CQPasteWnd::OnDeleteClipData()
{
	CWindow wndMain(theApp.m_MainhWnd);
	if (wndMain.m_hWnd != NULL)
	{
		wndMain.SendMessage(WM_COMMAND, ID_FIRST_DELETECLIPDATA, 0);
	}

	return true;
}

bool CQPasteWnd::OnGlobalHotkyes()
{
	CWindow wndMain(theApp.m_MainhWnd);
	if (wndMain.m_hWnd != NULL)
	{
		wndMain.SendMessage(WM_COMMAND, ID_FIRST_GLOBALHOTKEYS, 0);
	}

	return true;
}

bool CQPasteWnd::DoExportToBitMapFile()
{
	bool ret = false;

	CClipIDs IDs;
	INT_PTR lCount = m_lstHeader.GetSelectedCount();
	if (lCount <= 0)
	{
		return ret;
	}

	m_lstHeader.GetSelectionItemData(IDs);
	lCount = IDs.GetSize();
	if (lCount <= 0)
	{
		return ret;
	}

	OPENFILENAME ofn;
	TCHAR szFile[400];
	TCHAR szDir[400];

	memset(&szFile, 0, sizeof(szFile));
	memset(szDir, 0, sizeof(szDir));
	memset(&ofn, 0, sizeof(ofn));

	CString csInitialDir = CGetSetOptions::GetLastImportDir();
	STRCPY(szDir, csInitialDir);

	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = m_hWnd;
	ofn.lpstrFile = szFile;
	ofn.nMaxFile = sizeof(szFile);
	ofn.lpstrFilter = _T("PNG (*.png)\0*.png\0BMP (*.bmp)\0*.bmp\0JPEG (*.jpeg)\0*.jpeg");
	ofn.nFilterIndex = 1;
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = szDir;
	ofn.lpstrDefExt = _T("png");
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_OVERWRITEPROMPT | OFN_NOCHANGEDIR | OFN_NOCHANGEDIR;

	m_bHideWnd = false;

	if (GetSaveFileName(&ofn))
	{
		CWaitCursor wait;

		using namespace nsPath;
		CString startingFilePath = ofn.lpstrFile;
		CPath path(ofn.lpstrFile);
		CString csPath = path.GetPath();
		CString csExt = path.GetExtension();
		path.RemoveExtension();
		CString csFileName = path.GetName();

		CGetSetOptions::SetLastExportDir(csPath);

		int lastFileCheckId = 1;

		for (int i = 0; i < IDs.GetCount(); i++)
		{
			int id = IDs[i];

			CClip toSave;
			toSave.LoadFormats(id);

			CClipFormat* png = NULL;
			CClipFormat* bitmap = toSave.m_Formats.FindFormat(CF_DIB);
			if (bitmap == NULL)
			{
				png = toSave.m_Formats.FindFormat(theApp.m_PNG_Format);
			}

			if (bitmap == NULL && png == NULL)
				continue;

			CString savePath = startingFilePath;
			if (IDs.GetCount() > 1 ||
				FileExists(startingFilePath))
			{
				savePath = _T("");

				for (int y = lastFileCheckId; y < 1000000; y++)
				{
					CString testFilePath;
					testFilePath.Format(_T("%s%s_%d.%s"), csPath, csFileName, y, csExt);
					if (FileExists(testFilePath) == FALSE)
					{
						savePath = testFilePath;
						lastFileCheckId = y + 1;
						break;
					}
				}
			}

			if (savePath != _T(""))
			{
				toSave.WriteImageToFile(savePath);

				ret = true;
			}
			else
			{
				Log(StrF(_T("Failed to find a valid file name for starting path: %s"), startingFilePath));
			}
		}
	}

	m_bHideWnd = true;

	return ret;
}

LRESULT CQPasteWnd::OnCancelFilter(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	this->DoAction(ActionEnums::CANCELFILTER);
	return 1;
}

LRESULT CQPasteWnd::OnPostOptions(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	UpdateFont();
	LoadShortcuts();

	m_lstHeader.SetShowTextForFirstTenHotKeys(CGetSetOptions::GetShowTextForFirstTenHotKeys());
	m_lstHeader.SetShowIfClipWasPasted(CGetSetOptions::GetShowIfClipWasPasted());
	m_lstHeader.SetNumberOfLinesPerRow(CGetSetOptions::GetLinesPerRow(), true);

	SetCurrentTransparency();

	m_lstHeader.ApplyToolTipSettings();

	return 1;
}

void CQPasteWnd::OnClose()
{
	HideQPasteWindow(true);
}

LRESULT CQPasteWnd::OnBegindrag(int idCtrl, LPNMHDR pNMHDR, BOOL& bHandled)
{
	NM_LISTVIEW* pLV = (NM_LISTVIEW*)pNMHDR;
	CProcessPaste paste;
	paste.m_pastedFromGroup = (theApp.m_GroupID > 0);

	if (CONTROL_PRESSED)
	{
		paste.m_pasteOptions.m_dragDropFilesOnly = true;
	}
	else
	{
		paste.m_pasteOptions.m_placeCF_HDROP_OnDrag = CGetSetOptions::GetAddCFHDROP_OnDrag();
	}

	CClipIDs& clips = paste.GetClipIDs();

	m_lstHeader.GetSelectionItemData(clips);

	if (clips.GetSize() <= 0)
	{
		ASSERT(0); // does this ever happen ??
		clips.Add(m_lstHeader.GetItemData(pLV->iItem));
	}

	this->SetTimer(TIMER_DRAG_HIDE_WINDOW, 500, NULL);

	paste.DoDrag();

	KillTimer(TIMER_DRAG_HIDE_WINDOW);


	if (CGetSetOptions::m_bShowPersistent)
	{
		ShowQPasteWindow(0);
	}

	return 0;
}

DWORD CQPasteWnd::OnDragEnter(IDataObject* pDataObject, DWORD dwKeyState, CPoint point)
{
	int k = 0;

	return DROPEFFECT_COPY;
}

DWORD CQPasteWnd::OnDragOver(IDataObject* pDataObject, DWORD dwKeyState, CPoint point)
{
	return DROPEFFECT_COPY;
}

BOOL CQPasteWnd::OnDrop(IDataObject* pDataObject, DWORD dropEffect, CPoint point)
{
	return TRUE;
}

void CQPasteWnd::OnDragLeave()
{

}

LRESULT CQPasteWnd::GetDispInfo(int idCtrl, LPNMHDR pNMHDR, BOOL& bHandled)
{
	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;
	LV_ITEM* pItem = &(pDispInfo)->item;

	if (pItem->mask & LVIF_TEXT)
	{
		switch (pItem->iSubItem)
		{
		case 0:
			try
			{
				ATL::CCritSecLock csLock(m_CritSection);

				int c = m_lstHeader.GetItemCount();
				if ((int)m_listItems.size() > pItem->iItem &&
					m_listItems[pItem->iItem].m_lID > 0)
				{
					CString cs;
					if (m_listItems[pItem->iItem].m_bDontAutoDelete)
					{
						cs += _T("<noautodelete>");
					}

					if (m_listItems[pItem->iItem].m_bHasShortCut)
					{
						cs += _T("<shortcut>");
					}

					if (m_listItems[pItem->iItem].m_bIsGroup)
					{
						cs += _T("<group>");
					}

					if (theApp.m_GroupID > 0)
					{
						if (m_listItems[pItem->iItem].m_stickyClipGroupOrder != INVALID_STICKY)
						{
							cs += _T("<sticky>");
						}
					}
					else
					{
						if (m_listItems[pItem->iItem].m_stickyClipOrder != INVALID_STICKY)
						{
							cs += _T("<sticky>");
						}
					}

					// attached to a group
					if (m_listItems[pItem->iItem].m_bHasParent)
					{
						cs += _T("<ingroup>");
					}

					if (m_listItems[pItem->iItem].m_QuickPaste.IsEmpty() == FALSE)
					{
						cs += _T("<qpastetext>");
					}

					if (m_listItems[pItem->iItem].m_dateCopied != m_listItems[pItem->iItem].m_datePasted)
					{
						cs += "<pasted>";
					}

					// pipe is the "end of symbols" marker
					cs += "|" + CMainTableFunctions::GetDisplayText(CGetSetOptions::m_nLinesPerRow, m_listItems[pItem->iItem].m_Desc);

					lstrcpyn(pItem->pszText, cs, pItem->cchTextMax);
					pItem->pszText[pItem->cchTextMax - 1] = '\0';

					//						Log(StrF(_T("DrawItem index %d - "), pItem->iItem));//, pItem->pszText));
				}
				else
				{

					bool addToLoadItems = true;

					for (std::list<CPoint>::iterator it = m_loadItems.begin(); it != m_loadItems.end(); it++)
					{
						if (pItem->iItem >= it->x && pItem->iItem <= it->y)
						{
							addToLoadItems = false;
							break;
						}
					}

					if (addToLoadItems)
					{
						CPoint loadItem(pItem->iItem, (m_lstHeader.GetTopIndex() + (m_lstHeader.GetCountPerPage() * 2)));

						//Log(StrF(_T("DrawItem index %d, add: %d"), loadItem.x, loadItem.y));
						m_loadItems.push_back(loadItem);
					}

					m_thread.FireLoadItems(false);
				}
			}
			CATCH_SQLITE_EXCEPTION

				break;
		}
	}

	if (pItem->mask & LVIF_PARAM)
	{
		switch (pItem->iSubItem)
		{
		case 0:
		{
			ATL::CCritSecLock csLock(m_CritSection);

			if ((int)m_listItems.size() > pItem->iItem)
			{
				pItem->lParam = m_listItems[pItem->iItem].m_lID;
			}
		}

		break;
		}
	}

	if (pItem->mask & LVIF_CF_DIB && CGetSetOptions::m_bDrawThumbnail)
	{
		ATL::CCritSecLock csLock(m_CritSection);

		if ((int)m_listItems.size() > pItem->iItem)
		{
			CF_NoDibTypeMap::iterator iterNoDib = m_cf_NO_dibCache.find(m_listItems[pItem->iItem].m_lID);
			if (iterNoDib == m_cf_NO_dibCache.end())
			{
				CF_DibTypeMap::iterator iterDib = m_cf_dibCache.find(m_listItems[pItem->iItem].m_lID);
				if (iterDib == m_cf_dibCache.end())
				{
					bool exists = false;
					for (std::list<CClipFormatQListCtrl>::iterator it = m_ExtraDataLoadItems.begin(); it != m_ExtraDataLoadItems.end(); it++)
					{
						if (it->m_cfType == CF_DIB && it->m_parentId == m_listItems[pItem->iItem].m_lID)
						{
							exists = true;
							break;
						}
					}

					if (exists == false)
					{
						CClipFormatQListCtrl format;
						format.m_cfType = CF_DIB;
						format.m_parentId = m_listItems[pItem->iItem].m_lID;
						format.m_clipRow = pItem->iItem;
						format.m_autoDeleteData = true;
						format.m_counter = m_extraDataCounter++;
						m_ExtraDataLoadItems.push_back(format);

						m_extraDataThread.FireLoadExtraData(m_lstHeader.GetRowHeight());
					}
				}
				else
				{
					if (iterDib->second.m_hgData != NULL)
					{
						pItem->lParam = (LPARAM) & (iterDib->second);
					}
				}
			}
		}
	}

	if (pItem->mask & LVIF_CF_RICHTEXT && CGetSetOptions::m_bDrawRTF)
	{
		ATL::CCritSecLock csLock(m_CritSection);

		if ((int)m_listItems.size() > pItem->iItem)
		{
			CF_NoDibTypeMap::iterator iterNoRtf = m_cf_NO_rtfCache.find(m_listItems[pItem->iItem].m_lID);
			if (iterNoRtf == m_cf_NO_rtfCache.end())
			{
				CF_DibTypeMap::iterator iterRTF = m_cf_rtfCache.find(m_listItems[pItem->iItem].m_lID);
				if (iterRTF == m_cf_rtfCache.end())
				{
					bool exists = false;
					for (std::list<CClipFormatQListCtrl>::iterator it = m_ExtraDataLoadItems.begin(); it != m_ExtraDataLoadItems.end(); it++)
					{
						if (it->m_cfType == theApp.m_RTFFormat && it->m_parentId == m_listItems[pItem->iItem].m_lID)
						{
							exists = true;
							break;
						}
					}

					if (exists == false)
					{
						CClipFormatQListCtrl format;
						format.m_cfType = theApp.m_RTFFormat;
						format.m_parentId = m_listItems[pItem->iItem].m_lID;
						format.m_clipRow = pItem->iItem;
						format.m_autoDeleteData = true;
						format.m_counter = m_extraDataCounter++;
						m_ExtraDataLoadItems.push_back(format);

						m_extraDataThread.FireLoadExtraData(m_lstHeader.GetRowHeight());
					}
				}
				else
				{
					if (iterRTF->second.m_hgData != NULL)
					{
						pItem->lParam = (LPARAM) & (iterRTF->second);
					}
				}
			}
		}
	}
	return 0;
}

CString CQPasteWnd::GetDisplayText(int dontAutoDelete, int shortCut, bool isGroup, int parentID, CString text)
{
	CString cs;
	if (dontAutoDelete)
	{
		cs += "*";
	}

	if (shortCut > 0)
	{
		cs += "s";
	}

	if (isGroup)
	{
		cs += "G";
	}

	// attached to a group
	if (parentID > 0)
	{
		cs += "!";
	}

	// pipe is the "end of symbols" marker
	cs += "|" + CMainTableFunctions::GetDisplayText(CGetSetOptions::m_nLinesPerRow, text);

	return cs;
}

LRESULT CQPasteWnd::OnGetToolTipText(int idCtrl, LPNMHDR pNMHDR, BOOL& bHandled)
{
	CQListToolTipText* pInfo = (CQListToolTipText*)pNMHDR;
	if (!pInfo)
	{
		return 0;
	}

	if (pInfo->lItem < 0)
	{
		CString cs("no item selected");

		lstrcpyn(pInfo->pszText, cs, pInfo->cchTextMax);
		if (cs.GetLength() > pInfo->cchTextMax)
		{
			pInfo->pszText[pInfo->cchTextMax - 1] = 0;
		}

		return 0;
	}

	try
	{
		CString cs;
		CString clipData = _T("");

		int id = m_lstHeader.GetItemData(pInfo->lItem);
		CppSQLite3Query q = theApp.m_db.execQueryEx(_T("SELECT lID, mText, lDate, lShortCut, clipOrder, clipGroupOrder, stickyClipOrder, stickyClipGroupOrder, lDontAutoDelete, QuickPasteText, lastPasteDate, globalShortCut, lParentID FROM Main WHERE lID = %d"), id);
		if (q.eof() == false)
		{
			CString clipText = q.getStringField(1);

			int lines = 0;
			int maxLines = CGetSetOptions::GetMaxToolTipLines();
			CTokenizer tokenizer(clipText, "\r\n");
			CString token;
			while (tokenizer.Next(token))
			{
				cs += token + "\r\n";
				if (lines > maxLines)
					break;
				lines++;
			}


#ifdef _DEBUG
			clipData += StrF(_T("(Index = %d) (Seq = %f) (Group Seq = %f) (Sticky Seq = %f) (Sticky Group Seq = %f)\n"),
				pInfo->lItem,
				q.getFloatField(_T("clipOrder")), q.getFloatField(_T("clipGroupOrder")),
				q.getFloatField(_T("stickyClipOrder")), q.getFloatField(_T("stickyClipGroupOrder")));
#endif 

			clipData += StrF(_T("\r\nDatabase ID: %d"), q.getIntField(_T("lID")));

			COleDateTime time((time_t)q.getInt64Field(_T("lDate")));
			clipData += "\r\nAdded: " + time.Format();

			COleDateTime modified((time_t)q.getInt64Field(_T("lastPasteDate")));
			clipData += "\r\nLast Used: " + modified.Format();

			if (q.getIntField(_T("lDontAutoDelete")) > 0)
			{
				clipData += "\r\nNever Auto Delete";
			}

			CString csQuickPaste = q.getStringField(_T("QuickPasteText"));

			if (csQuickPaste.IsEmpty() == FALSE)
			{
				clipData += "\nQuick Paste = ";
				clipData += csQuickPaste;
			}

			int shortCut = q.getIntField(_T("lShortCut"));
			if (shortCut > 0)
			{
				clipData += "\r\n";
				clipData += CHotKey::GetHotKeyDisplayStatic(shortCut);

				BOOL globalShortCut = q.getIntField(_T("globalShortCut"));
				if (globalShortCut)
				{
					clipData += " - Global Shortcut Key";
				}
			}

			if (theApp.m_GroupID > 0)
			{
				int sticky = q.getIntField(_T("stickyClipGroupOrder"));
				if (sticky != INVALID_STICKY)
				{
					clipData += "\r\n";
					clipData += _T(" - Sticky In Group");
				}
			}
			else
			{
				int sticky = q.getIntField(_T("stickyClipOrder"));
				if (sticky != INVALID_STICKY)
				{
					clipData += "\r\n";
					clipData += _T(" - Sticky");
				}
			}

			int parentId = q.getIntField(_T("lParentID"));
			if (parentId > 0)
			{
				clipData += "\r\n";
				clipData += FolderPath(parentId);
			}
		}

		cs = cs.Left(pInfo->cchTextMax - clipData.GetLength());

		cs += "\r\n\r\n";
		cs += clipData;

		lstrcpyn(pInfo->pszText, cs, pInfo->cchTextMax);
		pInfo->pszText[pInfo->cchTextMax - 1] = '\0';
	}
	CATCH_SQLITE_EXCEPTION
	return 0;
}

LRESULT CQPasteWnd::OnFindItem(int idCtrl, LPNMHDR pNMHDR, BOOL& bHandled)
{
	NMLVFINDITEM* pFindInfo = (NMLVFINDITEM*)pNMHDR;
	LVFINDINFO fndItem = pFindInfo->lvfi;


	if (fndItem.flags & LVFI_STRING)
	{
		//m_search.SetWindowText(fndItem.psz);
		//m_search.SetFocus();
		//m_search.SetSel(1, 1);

		//OnSearchEditChange();

		//return 0;
	}

	return -1; // Default action.
}

void CQPasteWnd::OnNcLButtonDblClk(UINT nHitTest, CPoint point)
{
	// toggle ShowPersistent when we double click the caption
	if (nHitTest == HTCAPTION)
	{
		switch (CGetSetOptions::m_bDoubleClickingOnCaptionDoes)
		{
		case TOGGLES_ALLWAYS_ON_TOP:
		{
			theApp.ShowPersistent(!CGetSetOptions::m_bShowPersistent);
			if (CGetSetOptions::m_bShowPersistent)
			{
				::SetWindowPos(m_hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_SHOWWINDOW);
			}

			MoveControls();

			UpdateStatus();
		}
		break;
		case TOGGLES_ALLWAYS_SHOW_DESCRIPTION:
			DoAction(ActionEnums::SHOWDESCRIPTION);
			break;
		case ROLLES_UP_WINDOW:
			MinMaxWindow();
			break;
		}
	}

	// Upstream chained to CWnd::OnNcLButtonDblClk, i.e. DefWindowProc still saw it.
	SetMsgHandled(FALSE);
}

LRESULT CQPasteWnd::OnShowGroupsTop(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	m_lstHeader.HidePopup(true);

	BOOL bDummy = FALSE;
	return OnShowGroupsBottom(0, ID_SHOW_GROUPS_BOTTOM, NULL, bDummy);
	m_GroupTree.m_bHide = false;
	m_bHideWnd = false;

	CRect crList;
	m_lstHeader.GetWindowRect(crList);

	CRect cr(crList.left, crList.top, crList.left + crList.Width(), crList.top + 200);

	m_GroupTree.MoveWindow(cr);
	m_GroupTree.m_selectedFolderID = theApp.m_GroupID;
	m_GroupTree.FillTree();
	m_GroupTree.ShowWindow(SW_SHOW);


	m_GroupTree.m_bHide = true;
	m_bHideWnd = true;
	return 0;
}

LRESULT CQPasteWnd::OnShowGroupsBottom(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	m_lstHeader.HidePopup(true);

	if (m_GroupTree.IsWindowVisible())
	{
		m_GroupTree.ShowWindow(SW_HIDE);
		return 0;
	}

	m_GroupTree.m_bHide = false;
	m_bHideWnd = false;

	CRect crWindow, crList;
	m_lstHeader.GetWindowRect(crList);
	GetWindowRect(crWindow);

	CRect cr(crWindow.left, crWindow.bottom, crWindow.left + crWindow.Width(), crWindow.bottom + 200);

	EnsureWindowVisible(&cr);

	m_GroupTree.MoveWindow(cr);
	m_GroupTree.m_selectedFolderID = theApp.m_GroupID;
	m_GroupTree.FillTree();
	m_GroupTree.ShowWindow(SW_SHOW);

	m_GroupTree.m_bHide = true;
	m_bHideWnd = true;
	return 0;
}

LRESULT CQPasteWnd::OnGroupTreeMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	m_bHideWnd = false;

	int id = (int)wParam;

	m_GroupTree.ShowWindow(SW_HIDE);

	m_bHandleSearchTextChange = false;
	m_search.SetWindowText(_T(""));
	m_bHandleSearchTextChange = true;

	MoveControls();

	if (id == -1)
	{
		//go back to the main list
		theApp.EnterGroupID(-1);
	}
	else if (id >= 0)
	{
		//Set the app flag so it does a send message to refresh the list
		//We need to do this because we set the list pos to 0 and with Post
		//the list is not filled up yet
		bool bItWas = theApp.m_bAsynchronousRefreshView;
		theApp.m_bAsynchronousRefreshView = false;

		CSpecialPasteOptions pasteOptions;
		OpenID(id, pasteOptions);

		theApp.m_bAsynchronousRefreshView = bItWas;

		m_lstHeader.SetListPos(0);
		m_lstHeader.SetFocus();
	}

	CWindow p = ::GetFocus();
	if (p.m_hWnd == NULL)
	{
		HideQPasteWindow(false);
	}

	m_bHideWnd = true;

	return TRUE;
}

LRESULT CQPasteWnd::OnBackButton(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	theApp.EnterGroupID(theApp.m_GroupParentID);
	return 0;
}

LRESULT CQPasteWnd::OnMenuSearchDescription(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	CGetSetOptions::SetSearchDescription(!CGetSetOptions::GetSearchDescription());

	CString csText;
	m_search.GetWindowText(csText);

	if (csText != _T(""))
	{
		FillList(csText);
	}
	return 0;
}

LRESULT CQPasteWnd::OnMenuSearchFullText(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	CGetSetOptions::SetSearchFullText(!CGetSetOptions::GetSearchFullText());

	CString csText;
	m_search.GetWindowText(csText);

	if (csText != _T(""))
	{
		FillList(csText);
	}
	return 0;
}

LRESULT CQPasteWnd::OnMenuSearchQuickPaste(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	CGetSetOptions::SetSearchQuickPaste(!CGetSetOptions::GetSearchQuickPaste());

	CString csText;
	m_search.GetWindowText(csText);

	if (csText != _T(""))
	{
		FillList(csText);
	}
	return 0;
}

LRESULT CQPasteWnd::OnMenuShowStarredClips(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	m_bShowStarredClips = !m_bShowStarredClips;

	CString csText;
	m_search.GetWindowText(csText);

	FillList(csText);
	return 0;
}
LRESULT CQPasteWnd::OnSearchEditChange(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	m_search.Invalidate();
	if (CGetSetOptions::m_bFindAsYouType == FALSE)
	{
		return 0;
	}

	if (m_bHandleSearchTextChange == false)
	{
		//Log(_T("Handle text change is NOT set"));
		return 0;
	}

	KillTimer(TIMER_DO_SEARCH);
	SetTimer(TIMER_DO_SEARCH, 250, NULL);

	return 0;
}

LRESULT CQPasteWnd::OnUpDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	MSG msg;
	//Workaround for allow holding down arrow keys while in the search control
	msg.lParam = lParam & (~0x40000000);
	msg.wParam = wParam;
	msg.message = WM_KEYDOWN;
	if (CheckActions(&msg) == false)
	{
		if (m_lstHeader.HandleKeyDown(wParam, lParam) == FALSE)
		{
			LRESULT res = m_lstHeader.SendMessage(WM_KEYDOWN, wParam, lParam);
		}
	}

	return TRUE;
}

LRESULT CQPasteWnd::OnToolTipWndInactive(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if (!CGetSetOptions::m_bShowPersistent)
	{
		CWindow p = ::GetFocus();
		if (p.m_hWnd == NULL)
		{
			HideQPasteWindow(false);
			m_lstHeader.HidePopup(true);
		}
	}
	else
	{
		CWindow p = ::GetFocus();
		if (p.m_hWnd == NULL)
		{
			m_lstHeader.HidePopup(true);
		}
	}

	return TRUE;
}

void CQPasteWnd::OnUpdateMenuNewgroup(CmdUI *pCmdUI)
{
	if (!pCmdUI->m_pMenu)
	{
		return;
	}

	UpdateMenuShortCut(pCmdUI, ActionEnums::NEWGROUP);
}

void CQPasteWnd::OnUpdateMenuNewgroupselection(CmdUI *pCmdUI)
{
	if (!pCmdUI->m_pMenu)
	{
		return;
	}

	UpdateMenuShortCut(pCmdUI, ActionEnums::NEWGROUPSELECTION);
}

void CQPasteWnd::OnUpdateMenuAllwaysontop(CmdUI *pCmdUI)
{
	if (!pCmdUI->m_pMenu)
	{
		return;
	}

	UpdateMenuShortCut(pCmdUI, ActionEnums::TOGGLESHOWPERSISTANT);
}

void CQPasteWnd::OnUpdateMenuViewfulldescription(CmdUI *pCmdUI)
{
	if (!pCmdUI->m_pMenu)
	{
		return;
	}

	UpdateMenuShortCut(pCmdUI, ActionEnums::SHOWDESCRIPTION);
}

void CQPasteWnd::OnUpdateMenuViewgroups(CmdUI *pCmdUI)
{
	if (!pCmdUI->m_pMenu)
	{
		return;
	}

	UpdateMenuShortCut(pCmdUI, ActionEnums::SHOWGROUPS);
}

void CQPasteWnd::OnUpdateMenuPasteplaintextonly(CmdUI *pCmdUI)
{
	if (!pCmdUI->m_pMenu)
	{
		return;
	}

	UpdateMenuShortCut(pCmdUI, ActionEnums::PASTE_SELECTED_PLAIN_TEXT);
}

void CQPasteWnd::OnUpdateMenuDelete(CmdUI *pCmdUI)
{
	if (!pCmdUI->m_pMenu)
	{
		return;
	}

	UpdateMenuShortCut(pCmdUI, ActionEnums::DELETE_SELECTED);
}

void CQPasteWnd::OnUpdateMenuProperties(CmdUI *pCmdUI)
{
	if (!pCmdUI->m_pMenu)
	{
		return;
	}

	UpdateMenuShortCut(pCmdUI, ActionEnums::CLIP_PROPERTIES);
}

void CQPasteWnd::OnUpdateMenuEdititem(CmdUI *pCmdUI)
{
	if (!pCmdUI->m_pMenu)
	{
		return;
	}

	UpdateMenuShortCut(pCmdUI, ActionEnums::EDITCLIP);
}

void CQPasteWnd::OnUpdateMenuNewclip(CmdUI *pCmdUI)
{
	if (!pCmdUI->m_pMenu)
	{
		return;
	}

	UpdateMenuShortCut(pCmdUI, ActionEnums::NEWCLIP);
}

LRESULT CQPasteWnd::OnSetListCount(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	m_noSearchResults = false;

	int x = m_lstHeader.GetScrollPos(SB_HORZ);
	int y = m_lstHeader.GetScrollPos(SB_VERT);
	m_lstHeader.Scroll(CSize(-x, -y));

	m_lstHeader.SetItemCountEx((int)wParam);

	if ((int)wParam == 0 &&
		(m_strSearch != _T("") || m_bShowStarredClips))
	{
		m_noSearchResults = true;
		if (m_bShowStarredClips && m_strSearch == _T(""))
		{
			CString text = theApp.m_Language.GetString("NoStarredClips", "There are no starred clips");
			m_noSearchResultsStatic.SetWindowText(text);
		}
		else
		{
			CString text = theApp.m_Language.GetString("NoSearchResults", "There are no results for");
			m_noSearchResultsStatic.SetWindowText(StrF(_T("%s \"%s\""), text, m_strSearch));
		}
	}

	SelectFocusID();
	UpdateStatus(false);

	MoveControls();

	return TRUE;
}

LRESULT CQPasteWnd::OnItemDeleted(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	m_lstHeader.OnItemDeleted((int)wParam);
	return TRUE;
}

LRESULT CQPasteWnd::OnRefeshRow(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	int clipId = (int)wParam;
	int listPos = (int)lParam;

	int topIndex = m_lstHeader.GetTopIndex();
	int lastIndex = topIndex + m_lstHeader.GetCountPerPage();

	if (listPos >= topIndex && listPos <= lastIndex)
	{
		m_lstHeader.RefreshRow(listPos);
		m_lstHeader.PostEventLoadedCheckDescription(listPos);
	}

	if (clipId == -2)
	{
		m_lstHeader.Invalidate();
		m_lstHeader.RedrawWindow();

		//Log(_T("End of first load, showing listbox and loading actual count, then accelerators"));
	}

	return true;
}

void CQPasteWnd::SelectFocusID()
{
	ATL::CCritSecLock csLock(m_CritSection);

	bool selectedItem = false;
	int index = 0;
	std::vector<CMainTable>::iterator iter = m_listItems.begin();
	while (iter != m_listItems.end())
	{
		if (iter->m_lID == theApp.m_FocusID)
		{
			m_lstHeader.SetListPos(index);
			selectedItem = true;
			break;
		}
		iter++;
		index++;
	}

	if (selectedItem == false)
	{
		m_lstHeader.EnsureVisible(0, FALSE);
		m_lstHeader.SetListPos(CGetSetOptions::SelectedIndex());
	}
}

void CQPasteWnd::FillMainTable(CMainTable& table, CppSQLite3Query& q)
{
	table.m_lID = q.getIntField(_T("lID"));
	table.m_Desc = q.fieldValue(_T("mText"));
	table.m_bHasParent = q.getIntField(_T("lParentID")) >= 0;
	table.m_bDontAutoDelete = q.getIntField(_T("lDontAutoDelete")) > 0;
	table.m_bHasShortCut = q.getIntField(_T("lShortCut")) > 0;
	table.m_bIsGroup = q.getIntField(_T("bIsGroup")) > 0;
	table.m_QuickPaste = q.fieldValue(_T("QuickPasteText"));
	table.m_clipOrder = q.getFloatField(_T("clipOrder"));
	table.m_clipGroupOrder = q.getFloatField(_T("clipGroupOrder"));
	table.m_stickyClipOrder = q.getFloatField(_T("stickyClipOrder"));
	table.m_stickyClipGroupOrder = q.getFloatField(_T("stickyClipGroupOrder"));
	table.m_dateCopied = q.getInt64Field(_T("lDate"));
	table.m_datePasted = q.getInt64Field(_T("lastPasteDate"));
}

void CQPasteWnd::OnDestroy()
{
	CWndEx::OnDestroy();

	CGetSetOptions::SetPastSearchXml(m_search.SavePastSearches());

	m_thread.Stop();
	m_extraDataThread.Stop();
}

void CQPasteWnd::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent == TIMER_DO_SEARCH)
	{
		Log(_T("TIMER_DO_SEARCH timer\n"));

		KillTimer(TIMER_DO_SEARCH);

		CString csText;
		m_search.GetWindowText(csText);

		int nCaretPos = m_lstHeader.GetCaret();
		if (nCaretPos >= 0)
		{
			theApp.m_FocusID = m_lstHeader.GetItemData(nCaretPos);
		}

		FillList(csText);
	}
	if (nIDEvent == TIMER_PASTE_FROM_MODIFER)
	{
		Log(_T("TIMER_PASTE_FROM_MODIFER timer\n"));
		KillTimer(TIMER_PASTE_FROM_MODIFER);
		if (m_bModifersMoveActive)
		{
			Log(_T("Open Selection\n"));
			CSpecialPasteOptions pasteOptions;
			OpenSelection(pasteOptions);
		}
		else
		{
			Log(_T("m_bModifersMoveActive set to false\n"));
		}
	}
	else if (nIDEvent == TIMER_ERROR_MSG)
	{
		KillTimer(TIMER_ERROR_MSG);
		m_popupMsg.Hide();
	}
	else if (nIDEvent == TIMER_DRAG_HIDE_WINDOW)
	{
		OutputDebugString(_T("drag timer\n"));

		CPoint mouse;
		GetCursorPos(&mouse);

		CRect windowRect;
		this->GetWindowRect(&windowRect);

		if (PtInRect(&windowRect, mouse) == FALSE)
		{
			HideQPasteWindow(false, false);
			KillTimer(TIMER_DRAG_HIDE_WINDOW);
		}
	}
	else if (nIDEvent == TIMER_DO_ACTION)
	{
		KillTimer(TIMER_DO_ACTION);

		OutputDebugString(StrF(_T("DoActionTimer, cmd: %d"), m_timerAction.Cmd));

		if (m_timerAction.Cmd > 0)
		{
			DoAction(m_timerAction);
		}
		m_timerAction = CAccel();
	}

	CWndEx::OnTimer(nIDEvent);
}

LRESULT CQPasteWnd::OnAddinSelect(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	ARRAY IDs;
	m_lstHeader.GetSelectionItemData(IDs);

	if (IDs.GetCount() > 0)
	{
		int id = IDs[0];
		CClip clip;
		if (clip.LoadMainTable(id))
		{
			if (clip.LoadFormats(id, false))
			{
				bool bCont = theApp.m_Addins.CallPrePasteFunction(wID, &clip);
				if (bCont)
				{
					CSpecialPasteOptions pasteOptions;
					pasteOptions.m_pPasteFormats = &clip.m_Formats;
					OpenID(-1, pasteOptions);
				}
			}
		}
	}
	return 0;
}

LRESULT CQPasteWnd::OnSelectAll(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	BOOL ret = FALSE;
	ATL::CCritSecLock csLock(m_CritSection);

	if ((int)m_listItems.size() < m_lstHeader.GetItemCount())
	{
		Log(_T("All items selected loading all items from the db"));

		CPoint loadItem(0, m_lstHeader.GetItemCount());
		m_loadItems.push_back(loadItem);

		m_thread.FireLoadItems(false);

		ret = TRUE;

		UpdateStatus(false);
	}

	return ret;
}

LRESULT CQPasteWnd::OnShowHideScrollBar(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if (wParam == 1)
	{
		Log(_T("OnShowHideScrollBar Showing ScrollBars"));
		m_showScrollBars = true;
		MoveControls();
	}
	else
	{
		Log(_T("OnShowHideScrollBar Hiding ScrollBars"));

		m_showScrollBars = false;
		MoveControls();
	}

	return 1;
}

LRESULT CQPasteWnd::OnUpdateScrollBar(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	// Update modern scrollbar position when list scrolls (only if enabled)
	if (CGetSetOptions::m_useModernScrollBar)
	{
		if (wParam == TRUE)
		{
			m_modernScrollBar.Show(false);
			m_modernScrollBarHorz.Show(false);
		}
		else
		{
			m_modernScrollBar.UpdateScrollBar();
			m_modernScrollBarHorz.UpdateScrollBar();
		}
	}
	return 0;
}

//HBRUSH CQPasteWnd::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
//{
//	// Call the base class implementation first! Otherwise, it may 
//	// undo what we're trying to accomplish here.
//	HBRUSH hbr = CWnd::OnCtlColor(pDC, pWnd, nCtlColor);
//
//	switch (nCtlColor) 
//	{
//	case CTLCOLOR_STATIC:
//		switch (pWnd->GetDlgCtrlID())
//		{
//			case ON_TOP_WARNING:
//			{
//				pDC->SetBkMode(TRANSPARENT);
//				pDC->SetBkColor(RGB(0, 0, 255));
//
//				CBrush brush;
//				brush.CreateSolidBrush(COLORREF(RGB(255, 0, 0)));
//				return brush;
//			}
//			break;
//		}
//	}
//
//	return hbr;
//}

//void CQPasteWnd::OnPaint()
//{
//	/*CBrush brush;
//	brush.CreateSolidBrush(COLORREF(RGB(255, 0, 0)));
//
//	CRect clientRect;
//	GetClientRect(clientRect);
//
//	CPaintDC dc(this);
//	dc.FillRect(clientRect, &brush);*/
//
//	
//		CQPasteWnd::OnPaint();
//	
//}

LRESULT CQPasteWnd::OnEraseBkgnd(HDC hDC)
{
	CRect rect;
	GetClientRect(&rect);
	WTL::CBrush myBrush;
	myBrush.CreateSolidBrush(CGetSetOptions::m_Theme.MainWindowBG());
	HBRUSH hOldBrush = (HBRUSH)::SelectObject(hDC, myBrush.m_hBrush);
	BOOL bRes = ::PatBlt(hDC, 0, 0, rect.Width(), rect.Height(), PATCOPY);
	::SelectObject(hDC, hOldBrush);
	return bRes;

	//return TRUE;
	// TODO: Add your message handler code here and/or call default

	//return CWndEx::OnEraseBkgnd(pDC);
}

LRESULT CQPasteWnd::OnQuickoptionsShowintaskbar(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	DoAction(ActionEnums::SHOW_IN_TASKBAR);
	return 0;
}


LRESULT CQPasteWnd::OnMenuViewasqrcode(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	DoAction(ActionEnums::EXPORT_TO_QR_CODE);
	return 0;
}

LRESULT CQPasteWnd::OnExportExporttotextfile(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	DoAction(ActionEnums::EXPORT_TO_TEXT_FILE);
	return 0;
}

LRESULT CQPasteWnd::OnCompareCompare(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	DoAction(ActionEnums::COMPARE_SELECTED_CLIPS);
	return 0;
}

LRESULT CQPasteWnd::OnCompareSelectleftcompare(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	DoAction(ActionEnums::SELECT_LEFT_SIDE_COMPARE);
	return 0;
}

LRESULT CQPasteWnd::OnCompareCompareagainst(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	DoAction(ActionEnums::SELECT_RIGHT_SITE_AND_DO_COMPARE);
	return 0;
}

void CQPasteWnd::OnUpdateCompareCompare(CmdUI *pCmdUI)
{
	if (!pCmdUI->m_pMenu)
	{
		return;
	}

	UpdateMenuShortCut(pCmdUI, ActionEnums::COMPARE_SELECTED_CLIPS);
}

void CQPasteWnd::UpdateMenuShortCut(CmdUI *pCmdUI, DWORD action)
{
	if (pCmdUI == NULL ||
		pCmdUI->m_pMenu == NULL)
	{
		return;
	}

	CString cs;
	pCmdUI->m_pMenu->GetMenuString(pCmdUI->m_nID, cs, MF_BYCOMMAND);
	CString shortcutText = m_actions.GetCmdKeyText(action);
	if (shortcutText != _T("") &&
		cs.Find("\t" + shortcutText) < 0)
	{
		cs += "\t";
		cs += shortcutText;
		pCmdUI->SetText(cs);
	}
}

LRESULT CQPasteWnd::OnShowProperties(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	return ShowProperties((int)wParam, -1);
}

LRESULT CQPasteWnd::OnNewGroup(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	NewGroup(false, (int)wParam);

	return TRUE;
}

LRESULT CQPasteWnd::OnDeleteId(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if (CGetSetOptions::GetPromptWhenDeletingClips())
	{
		bool bStartValue = m_bHideWnd;
		m_bHideWnd = false;

		int nRet = MessageBox(theApp.m_Language.GetString("Delete_Clip_Groups", "Delete Group?"), _T("DittoWTL"), MB_OKCANCEL | MB_TOPMOST);

		m_bHideWnd = bStartValue;

		if (nRet != IDOK)
		{
			return FALSE;
		}
	}

	CClipIDs IDs;
	ARRAY Indexs;

	IDs.Add((int)wParam);

	bool selectedItem = false;
	int index = 0;
	{
		ATL::CCritSecLock csLock(m_CritSection);
		std::vector<CMainTable>::iterator iter = m_listItems.begin();
		while (iter != m_listItems.end())
		{
			if (iter->m_lID == wParam)
			{
				Indexs.Add(index);
				break;
			}
			iter++;
			index++;
		}
	}

	DeleteClips(IDs, Indexs);

	return TRUE;
}

LRESULT CQPasteWnd::OnMenuSimpleTextSearch(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	CGetSetOptions::SetSimpleTextSearch(!CGetSetOptions::GetSimpleTextSearch());
	CGetSetOptions::SetRegExTextSearch(FALSE);
	return 0;
}

LRESULT CQPasteWnd::OnMenuRegularexpressionsearch(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	CGetSetOptions::SetSimpleTextSearch(FALSE);
	CGetSetOptions::SetRegExTextSearch(!CGetSetOptions::GetRegExTextSearch());
	return 0;
}


LRESULT CQPasteWnd::OnImportExporttogoogletranslate(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	DoAction(ActionEnums::EXPORT_TO_GOOGLE_TRANSLATE);
	return 0;
}

void CQPasteWnd::OnUpdateImportExporttogoogletranslate(CmdUI *pCmdUI)
{
	if (!pCmdUI->m_pMenu)
	{
		return;
	}

	UpdateMenuShortCut(pCmdUI, ActionEnums::EXPORT_TO_GOOGLE_TRANSLATE);
}


LRESULT CQPasteWnd::OnImportExportclipBitmap(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	DoAction(ActionEnums::EXPORT_TO_BITMAP_FILE);
	return 0;
}


void CQPasteWnd::OnUpdateImportExportclipBitmap(CmdUI *pCmdUI)
{
	if (!pCmdUI->m_pMenu)
	{
		return;
	}

	UpdateMenuShortCut(pCmdUI, ActionEnums::EXPORT_TO_BITMAP_FILE);
}

LRESULT CQPasteWnd::OnMenuWildcardsearch(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	CGetSetOptions::SetSimpleTextSearch(FALSE);
	CGetSetOptions::SetRegExTextSearch(FALSE);
	return 0;
}

LRESULT CQPasteWnd::OnMenuSavecurrentclipboard(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	DoAction(ActionEnums::SAVE_CURRENT_CLIPBOARD);
	return 0;
}


void CQPasteWnd::OnUpdateMenuSavecurrentclipboard(CmdUI *pCmdUI)
{
	if (!pCmdUI->m_pMenu)
	{
		return;
	}

	UpdateMenuShortCut(pCmdUI, ActionEnums::SAVE_CURRENT_CLIPBOARD);
}


bool CQPasteWnd::SyncClipDataToArrayData(CClip& clip)
{
	int row = 0;
	bool found = false;
	std::vector<CMainTable>::iterator iter = m_listItems.begin();
	while (iter != m_listItems.end())
	{
		if (iter->m_lID == clip.ID())
		{
			iter->m_clipOrder = clip.m_clipOrder;
			iter->m_clipGroupOrder = clip.m_clipGroupOrder;

			iter->m_stickyClipOrder = clip.m_stickyClipOrder;
			iter->m_stickyClipGroupOrder = clip.m_stickyClipGroupOrder;

			found = true;
			break;
		}
		iter++;
		row++;
	}

	return found;
}

bool CQPasteWnd::SelectIds(ARRAY& ids)
{
	int row = 0;
	bool found = false;
	std::vector<CMainTable>::iterator iter = m_listItems.begin();

	//sort so .Find works
	ids.SortAscending();

	while (iter != m_listItems.end())
	{
		if (ids.Find(iter->m_lID))
		{
			if (found == false)
			{
				m_lstHeader.SetListPos(row);
			}
			else
			{
				m_lstHeader.SetSelection(row);
			}

			found = true;
		}
		iter++;
		row++;
	}

	return found;
}

LRESULT CQPasteWnd::OnCliporderMoveup(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	DoAction(ActionEnums::MOVE_CLIP_UP);
	return 0;
}

void CQPasteWnd::OnUpdateCliporderMoveup(CmdUI *pCmdUI)
{
	if (!pCmdUI->m_pMenu)
	{
		return;
	}

	UpdateMenuShortCut(pCmdUI, ActionEnums::MOVE_CLIP_UP);
}

LRESULT CQPasteWnd::OnCliporderMovedown(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	DoAction(ActionEnums::MOVE_CLIP_DOWN);
	return 0;
}

void CQPasteWnd::OnUpdateCliporderMovedown(CmdUI *pCmdUI)
{
	if (!pCmdUI->m_pMenu)
	{
		return;
	}

	UpdateMenuShortCut(pCmdUI, ActionEnums::MOVE_CLIP_DOWN);
}

LRESULT CQPasteWnd::OnCliporderMovetotop(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	DoAction(ActionEnums::MOVE_CLIP_TOP);
	return 0;
}

void CQPasteWnd::OnUpdateCliporderMovetotop(CmdUI *pCmdUI)
{
	if (!pCmdUI->m_pMenu)
	{
		return;
	}

	UpdateMenuShortCut(pCmdUI, ActionEnums::MOVE_CLIP_TOP);
}

LRESULT CQPasteWnd::OnMenuFilteron(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	DoAction(ActionEnums::FILTER_ON_SELECTED_CLIP);
	return 0;
}

void CQPasteWnd::OnUpdateMenuFilteron(CmdUI *pCmdUI)
{
	if (!pCmdUI->m_pMenu)
	{
		return;
	}

	UpdateMenuShortCut(pCmdUI, ActionEnums::FILTER_ON_SELECTED_CLIP);
}

LRESULT CQPasteWnd::OnMenuGoToEntry(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	ARRAY IDs;
	m_lstHeader.GetSelectionItemData(IDs);
	if (IDs.GetSize() <= 0)
	{
		return 0;
	}

	long targetID = IDs[0];

	int targetSticky = 0;
	int targetIsGroup = 0;
	int targetClipOrder = 0;
	long targetParent = -1;
	bool gotKey = false;
	try
	{
		CppSQLite3Query q = theApp.m_db.execQueryEx(
			_T("SELECT stickyClipOrder, bIsGroup, clipOrder, lParentID FROM Main WHERE lID = %d"),
			targetID);
		if (!q.eof())
		{
			targetSticky = q.getIntField(_T("stickyClipOrder"));
			targetIsGroup = q.getIntField(_T("bIsGroup"));
			targetClipOrder = q.getIntField(_T("clipOrder"));
			targetParent = q.getIntField(_T("lParentID"));
			gotKey = true;
		}
	}
	catch (CppSQLite3Exception&)
	{
	}

	if (!gotKey)
	{
		return 0;
	}

	CString filter;
	if (CGetSetOptions::m_bShowAllClipsInMainList)
	{
		if (CGetSetOptions::GetShowGroupsInMainList())
			filter = _T("((Main.bIsGroup = 1 AND Main.lParentID = -1) OR Main.bIsGroup = 0)");
		else
			filter = _T("(Main.bIsGroup = 0)");
	}
	else
	{
		filter = _T("((Main.bIsGroup = 1 AND Main.lParentID = -1) OR (Main.bIsGroup = 0 AND Main.lParentID = -1))");
	}

	int targetIndex = -1;
	try
	{
		CString rankSql;
		rankSql.Format(
			_T("SELECT COUNT(*) FROM Main WHERE %s AND (")
			_T("(stickyClipOrder > %d) OR ")
			_T("(stickyClipOrder = %d AND bIsGroup < %d) OR ")
			_T("(stickyClipOrder = %d AND bIsGroup = %d AND clipOrder > %d))"),
			(LPCTSTR)filter,
			targetSticky,
			targetSticky, targetIsGroup,
			targetSticky, targetIsGroup, targetClipOrder);

		targetIndex = theApp.m_db.execScalar(rankSql);
	}
	catch (CppSQLite3Exception&)
	{
		targetIndex = -1;
	}

	theApp.m_FocusID = targetID;

	if (theApp.m_GroupID >= 0 && targetParent != theApp.m_GroupID)
	{
		theApp.EnterGroupID(-1);
	}

	m_bHandleSearchTextChange = false;
	m_search.SetWindowText(_T(""));
	m_bHandleSearchTextChange = true;

	FillList(_T(""));

	// The list loader thread reports completion via PostMessage(NM_SET_LIST_COUNT),
	// so we must pump messages while waiting or that handler never runs.
	DWORD waitStart = GetTickCount();
	while (WaitForSingleObject(m_thread.m_SearchingEvent, 0) == WAIT_TIMEOUT)
	{
		MSG msg;
		while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		if (GetTickCount() - waitStart > 5000)
			break;
		Sleep(10);
	}

	MSG msg;
	while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	int totalRows = m_lstHeader.GetItemCount();

	if (targetIndex < 0 || targetIndex >= totalRows)
	{
		MoveControls();
		SelectFocusID();
		m_lstHeader.SetFocus();
		return 0;
	}

	MoveControls();

	m_lstHeader.EnsureVisible(targetIndex, FALSE);
	m_lstHeader.SetListPos(targetIndex);
	m_lstHeader.SetFocus();

	Log(StrF(_T("GoToEntry: scrolled to index %d of %d"), targetIndex, totalRows));
	return 0;
}

void CQPasteWnd::OnUpdateMenuGoToEntry(CmdUI *pCmdUI)
{
	if (!pCmdUI->m_pMenu)
	{
		return;
	}

	CString csSearch;
	m_search.GetWindowText(csSearch);
	pCmdUI->Enable(!csSearch.IsEmpty());
}
LRESULT CQPasteWnd::OnAlwaysOnTopClicked(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	DoAction(ActionEnums::TOGGLESHOWPERSISTANT);
	return 0;
}


LRESULT CQPasteWnd::OnSpecialpasteUppercase(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	DoAction(ActionEnums::PASTE_UPPER_CASE);
	return 0;
}


void CQPasteWnd::OnUpdateSpecialpasteUppercase(CmdUI *pCmdUI)
{
	if (!pCmdUI->m_pMenu)
	{
		return;
	}

	UpdateMenuShortCut(pCmdUI, ActionEnums::PASTE_UPPER_CASE);
}


LRESULT CQPasteWnd::OnSpecialpasteLowercase(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	DoAction(ActionEnums::PASTE_LOWER_CASE);
	return 0;
}


void CQPasteWnd::OnUpdateSpecialpasteLowercase(CmdUI *pCmdUI)
{
	if (!pCmdUI->m_pMenu)
	{
		return;
	}

	UpdateMenuShortCut(pCmdUI, ActionEnums::PASTE_LOWER_CASE);
}


LRESULT CQPasteWnd::OnSpecialpasteCapitalize(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	DoAction(ActionEnums::PASTE_CAPITALiZE);
	return 0;
}

void CQPasteWnd::OnUpdateSpecialpasteCapitalize(CmdUI *pCmdUI)
{
	if (!pCmdUI->m_pMenu)
	{
		return;
	}

	UpdateMenuShortCut(pCmdUI, ActionEnums::PASTE_CAPITALiZE);
}


LRESULT CQPasteWnd::OnSpecialpasteSentence(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	DoAction(ActionEnums::PASTE_SENTENCE_CASE);
	return 0;
}


void CQPasteWnd::OnUpdateSpecialpasteSentence(CmdUI *pCmdUI)
{
	if (!pCmdUI->m_pMenu)
	{
		return;
	}

	UpdateMenuShortCut(pCmdUI, ActionEnums::PASTE_SENTENCE_CASE);
}

LRESULT CQPasteWnd::OnSystemButton(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	m_lstHeader.HidePopup(true);

	POINT pp;
	CMenuHandle cmPopUp;
	CMenuHandle cmSubMenu = NULL;

	GetCursorPos(&pp);
	if (cmPopUp.LoadMenu(IDR_QUICK_PASTE_SYSTEM_MENU) != 0)
	{
		cmSubMenu = cmPopUp.GetSubMenu(0);
		if (cmSubMenu == NULL)
		{
			return 0;
		}

		if (CGetSetOptions::GetShowStartupMessage())
		{
			cmSubMenu.CheckMenuItem(ID_FIRST_SHOWSTARTUPMESSAGE, MF_CHECKED);
		}

		AddShowStarredClipsMenuItem(&cmSubMenu);

		theApp.m_Language.UpdateRightClickMenu(cmSubMenu.m_hMenu);

		SetMenuChecks(&cmSubMenu);

		cmSubMenu.TrackPopupMenu(TPM_LEFTALIGN | TPM_TOPALIGN | TPM_RIGHTBUTTON, pp.x, pp.y, m_hWnd, NULL);
	}
	return 0;
}

LRESULT CQPasteWnd::OnSpecialpasteRemovelinefeeds(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	DoAction(ActionEnums::PASTE_REMOVE_LINE_FEEDS);
	return 0;
}


void CQPasteWnd::OnUpdateSpecialpasteRemovelinefeeds(CmdUI *pCmdUI)
{
	if (!pCmdUI->m_pMenu)
	{
		return;
	}

	UpdateMenuShortCut(pCmdUI, ActionEnums::PASTE_REMOVE_LINE_FEEDS);
}


LRESULT CQPasteWnd::OnSpecialpastePaste(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	DoAction(ActionEnums::PASTE_ADD_ONE_LINE_FEED);
	return 0;
}


void CQPasteWnd::OnUpdateSpecialpastePaste(CmdUI *pCmdUI)
{
	if (!pCmdUI->m_pMenu)
	{
		return;
	}

	UpdateMenuShortCut(pCmdUI, ActionEnums::PASTE_ADD_ONE_LINE_FEED);
}


LRESULT CQPasteWnd::OnSpecialpastePaste32919(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	DoAction(ActionEnums::PASTE_ADD_TWO_LINE_FEEDS);
	return 0;
}

void CQPasteWnd::OnUpdateSpecialpastePaste32919(CmdUI *pCmdUI)
{
	if (!pCmdUI->m_pMenu)
	{
		return;
	}

	UpdateMenuShortCut(pCmdUI, ActionEnums::PASTE_ADD_TWO_LINE_FEEDS);
}


LRESULT CQPasteWnd::OnSpecialpasteTypoglycemia(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	DoAction(ActionEnums::PASTE_TYPOGLYCEMIA);
	return 0;
}

void CQPasteWnd::OnUpdateSpecialpasteTypoglycemia(CmdUI *pCmdUI)
{
	if (!pCmdUI->m_pMenu)
	{
		return;
	}

	UpdateMenuShortCut(pCmdUI, ActionEnums::PASTE_TYPOGLYCEMIA);
}

LRESULT CQPasteWnd::OnNMClickList1(int idCtrl, LPNMHDR pNMHDR, BOOL& bHandled)
{
	CString csText;
	m_search.GetWindowText(csText);
	if (csText == _T("crash"))
	{
		if (CONTROL_PRESSED)
		{
			if (GetKeyState(VK_SHIFT) & 0x8000)
			{
				raise(SIGSEGV);
			}
		}
	}

	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	MSG msg;
	msg.lParam = 0;
	msg.wParam = VK_MOUSE_CLICK;
	msg.message = WM_KEYDOWN;
	if (CheckActions(&msg) == false)
	{
	}
	return 0;
}


LRESULT CQPasteWnd::OnNMDblclkList1(int idCtrl, LPNMHDR pNMHDR, BOOL& bHandled)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);

	MSG msg;
	msg.lParam = 0;
	msg.wParam = VK_MOUSE_DOUBLE_CLICK;
	msg.message = WM_KEYDOWN;
	if (CheckActions(&msg) == false)
	{
	}

	return 0;
}


LRESULT CQPasteWnd::OnNMRClickList1(int idCtrl, LPNMHDR pNMHDR, BOOL& bHandled)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	MSG msg;
	msg.lParam = 0;
	msg.wParam = VK_MOUSE_RIGHT_CLICK;
	msg.message = WM_KEYDOWN;
	if (CheckActions(&msg) == false)
	{
	}
	return 0;
}

LRESULT CQPasteWnd::OnNMRDblclkList1(int idCtrl, LPNMHDR pNMHDR, BOOL& bHandled)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	/*MSG msg;
	msg.lParam = 0;
	msg.wParam = VK_MOUSE_RIGHT_CLICK;
	msg.message = WM_KEYDOWN;
	if (CheckActions(&msg) == false)
	{
	}*/
	return 0;
}

LRESULT CQPasteWnd::OnQuickoptionsShowtextforfirsttencopyhotkeys(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	DoAction(ActionEnums::CONFIG_SHOW_FIRST_TEN_TEXT);
	return 0;
}


void CQPasteWnd::OnUpdateQuickoptionsShowtextforfirsttencopyhotkeys(CmdUI *pCmdUI)
{
	if (!pCmdUI->m_pMenu)
	{
		return;
	}

	UpdateMenuShortCut(pCmdUI, ActionEnums::CONFIG_SHOW_FIRST_TEN_TEXT);
}


LRESULT CQPasteWnd::OnQuickoptionsShowindicatoracliphasbeenpasted(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	DoAction(ActionEnums::CONFIG_SHOW_CLIP_WAS_PASTED);
	return 0;
}

void CQPasteWnd::OnUpdateQuickoptionsShowindicatoracliphasbeenpasted(CmdUI *pCmdUI)
{
	if (!pCmdUI->m_pMenu)
	{
		return;
	}

	UpdateMenuShortCut(pCmdUI, ActionEnums::CONFIG_SHOW_CLIP_WAS_PASTED);
}


LRESULT CQPasteWnd::OnGroupsTogglelastgroup(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	DoAction(ActionEnums::TOGGLE_LAST_GROUP_TOGGLE);
	return 0;
}


void CQPasteWnd::OnUpdateGroupsTogglelastgroup(CmdUI *pCmdUI)
{
	if (!pCmdUI->m_pMenu)
	{
		return;
	}

	UpdateMenuShortCut(pCmdUI, ActionEnums::TOGGLE_LAST_GROUP_TOGGLE);
}


LRESULT CQPasteWnd::OnSpecialpastePaste32927(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	DoAction(ActionEnums::PASTE_ADD_CURRENT_TIME);
	return 0;
}


void CQPasteWnd::OnUpdateSpecialpastePaste32927(CmdUI *pCmdUI)
{
	if (!pCmdUI->m_pMenu)
	{
		return;
	}

	UpdateMenuShortCut(pCmdUI, ActionEnums::PASTE_ADD_CURRENT_TIME);
}


LRESULT CQPasteWnd::OnMenuGlobalhotkeys32933(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	DoAction(ActionEnums::GLOBAl_HOTKEYS);
	return 0;
}

void CQPasteWnd::OnUpdateMenuGlobalhotkeys32933(CmdUI *pCmdUI)
{
	if (!pCmdUI->m_pMenu)
	{
		return;
	}

	UpdateMenuShortCut(pCmdUI, ActionEnums::GLOBAl_HOTKEYS);
}


LRESULT CQPasteWnd::OnMenuDeleteclipdata32934(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	DoAction(ActionEnums::DELETE_CLIP_DATA);
	return 0;
}



void CQPasteWnd::OnUpdateMenuDeleteclipdata32934(CmdUI *pCmdUI)
{
	if (!pCmdUI->m_pMenu)
	{
		return;
	}

	UpdateMenuShortCut(pCmdUI, ActionEnums::DELETE_CLIP_DATA);
}



LRESULT CQPasteWnd::OnMenuImportclip32935(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	DoAction(ActionEnums::IMPORT_CLIP);
	return 0;
}

void CQPasteWnd::OnUpdateMenuImportclip32935(CmdUI *pCmdUI)
{
	if (!pCmdUI->m_pMenu)
	{
		return;
	}

	UpdateMenuShortCut(pCmdUI, ActionEnums::IMPORT_CLIP);
}

LRESULT CQPasteWnd::OnMenuNewclip32937(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	DoAction(ActionEnums::NEWCLIP);
	return 0;
}

void CQPasteWnd::OnUpdateMenuNewclip32937(CmdUI *pCmdUI)
{
	if (!pCmdUI->m_pMenu)
	{
		return;
	}

	UpdateMenuShortCut(pCmdUI, ActionEnums::NEWCLIP);
}

LRESULT CQPasteWnd::OnSearchFocused(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	m_lstHeader.HidePopup(true);

	return TRUE;
}

#include <ShellScalingApi.h>


LRESULT CQPasteWnd::OnDpiChanged(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	ARRAY Indexs;
	m_lstHeader.GetSelectionIndexes(Indexs);
	if (Indexs.GetCount() <= 0)
	{
		Indexs.Add(0);
	}
	int c = m_lstHeader.GetItemCount();
	m_lstHeader.SetItemCountEx(0);

	int dpi = HIWORD(wParam);
	m_DittoWindow.OnDpiChanged(this, dpi);

	//RECT* const prcNewWindow = (RECT*)lParam;
	CRect r(*(RECT*)lParam);
	if (CGetSetOptions::m_bEnsureEntireWindowCanBeSeen)
	{
		EnsureWindowVisible(&r);
	}

	SetWindowPos(NULL, r.left, r.top, r.Width(), r.Height(), SWP_NOZORDER | SWP_NOACTIVATE);

	log(StrF(_T("CQPasteWnd::OnDpiChanged dpi: %d width: %d, height: %d"), dpi, r.Width(), r.Height()));

	m_systemMenu.Reset();
	m_systemMenu.LoadStdImageDPI(m_DittoWindow.m_dpi.GetDPI(), system_menu_2_24, system_menu_2_30, system_menu_2_36, system_menu_2_42, system_menu_2_48, _T("PNG"), system_menu_54, system_menu_60, system_menu_66, system_menu_72, system_menu_78, system_menu_84);

	m_BackButton.Reset();
	m_BackButton.LoadStdImageDPI(m_DittoWindow.m_dpi.GetDPI(), return_16, return_20, return_24, return_28, return_32, _T("PNG"));

	m_ShowGroupsFolderBottom.Reset();
	m_ShowGroupsFolderBottom.LoadStdImageDPI(m_DittoWindow.m_dpi.GetDPI(), open_folder_24, open_folder_30, open_folder_36, open_folder_42, open_folder_48, _T("PNG"), open_folder_54, open_folder_60, open_folder_66, open_folder_72, open_folder_78, open_folder_84);

	m_search.OnDpiChanged();
	m_lstHeader.OnDpiChanged();

	UpdateFont();
	this->SetLinesPerRow(CGetSetOptions::GetLinesPerRow(), true, false);

	MoveControls();

	m_lstHeader.SetItemCountEx(c);
	m_lstHeader.SetListPos(Indexs[0]);

	InvalidateNc();
	this->Invalidate();
	m_lstHeader.RefreshVisibleRows();
	this->RedrawWindow();

	return TRUE;
}

LRESULT CQPasteWnd::OnCliporderReplacetopstickyclip(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	DoAction(ActionEnums::REPLACE_TOP_STICKY_CLIP);
	return 0;
}

void CQPasteWnd::OnUpdateCliporderReplacetopstickyclip(CmdUI *pCmdUI)
{
	if (!pCmdUI->m_pMenu)
	{
		return;
	}

	UpdateMenuShortCut(pCmdUI, ActionEnums::REPLACE_TOP_STICKY_CLIP);
}


LRESULT CQPasteWnd::OnSendtoPromptforname(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	DoAction(ActionEnums::PROMPT_SEND_TO_FRIEND);
	return 0;
}

void CQPasteWnd::OnUpdateSendtoPromptforname(CmdUI *pCmdUI)
{
	if (!pCmdUI->m_pMenu)
	{
		return;
	}

	UpdateMenuShortCut(pCmdUI, ActionEnums::PROMPT_SEND_TO_FRIEND);
}


LRESULT CQPasteWnd::OnImportImportcopiedfile(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	DoAction(ActionEnums::SAVE_CF_HDROP_FIlE_DATA);
	return 0;
}

void CQPasteWnd::OnUpdateImportImportcopiedfile(CmdUI *pCmdUI)
{
	if (!pCmdUI->m_pMenu)
	{
		return;
	}

	UpdateMenuShortCut(pCmdUI, ActionEnums::SAVE_CF_HDROP_FIlE_DATA);
}

LRESULT CQPasteWnd::OnCustomSendToFriend(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	bool showDlg = false;
	CString ip_name = m_customFriendsHelper.GetSendTo(wID, showDlg);
	if (showDlg)
	{
		DoAction(ActionEnums::PROMPT_SEND_TO_FRIEND);
	}
	else if (ip_name != _T(""))
	{
		SendToFriendbyPos(0, ip_name);
	}
	return 0;
}

LRESULT CQPasteWnd::OnChaiScriptPaste(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	CSpecialPasteOptions pasteOptions;
	int index = wID - ChaiScriptMenuStartId;

	if (index >= 0 &&
		index < CGetSetOptions::m_pasteScripts.m_list.size())
	{
		pasteOptions.m_pasteScriptGuid = CGetSetOptions::m_pasteScripts.m_list[index].m_guid;
		OpenSelection(pasteOptions);
	}
	return 0;
}

LRESULT CQPasteWnd::OnCliporderMovetolast(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	DoAction(ActionEnums::MOVE_CLIP_LAST);
	return 0;
}


void CQPasteWnd::OnUpdateCliporderMovetolast(CmdUI *pCmdUI)
{
	if (!pCmdUI->m_pMenu)
	{
		return;
	}

	UpdateMenuShortCut(pCmdUI, ActionEnums::MOVE_CLIP_LAST);
}


LRESULT CQPasteWnd::OnSpecialpastePasteDontUpdateOrder(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	DoAction(ActionEnums::PASTE_DONT_MOVE_CLIP);
	return 0;
}


void CQPasteWnd::OnUpdateOnSpecialPasteDontUpdateOrder(CmdUI *pCmdUI)
{
	if (!pCmdUI->m_pMenu)
	{
		return;
	}

	UpdateMenuShortCut(pCmdUI, ActionEnums::PASTE_DONT_MOVE_CLIP);
}


LRESULT CQPasteWnd::OnSpecialpasteTrim(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	DoAction(ActionEnums::PASTE_TRIM_WHITE_SPACE);
	return 0;
}


LRESULT CQPasteWnd::OnSpecialpastePosixifyPaths(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	DoAction(ActionEnums::PASTE_POSIXIFY_PATHS);
	return 0;
}
void CQPasteWnd::OnUpdateSpecialpasteTrim(CmdUI *pCmdUI)
{
	if (!pCmdUI->m_pMenu)
	{
		return;
	}

	UpdateMenuShortCut(pCmdUI, ActionEnums::PASTE_TRIM_WHITE_SPACE);
}

void CQPasteWnd::OnUpdateSpecialPosixifyPaths(CmdUI *pCmdUI)
{
	if (!pCmdUI->m_pMenu)
	{
		return;
	}

	UpdateMenuShortCut(pCmdUI, ActionEnums::PASTE_POSIXIFY_PATHS);
}
LRESULT CQPasteWnd::OnMenuTransparencyNone(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	DoAction(ActionEnums::TRANSPARENCY_NONE);
	return 0;
}

void CQPasteWnd::OnUpdateTransparencyNone(CmdUI *pCmdUI)
{
	if (!pCmdUI->m_pMenu)
	{
		return;
	}

	UpdateMenuShortCut(pCmdUI, ActionEnums::TRANSPARENCY_NONE);
}

LRESULT CQPasteWnd::OnMenuTransparency5(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	DoAction(ActionEnums::TRANSPARENCY_5);
	return 0;
}

void CQPasteWnd::OnUpdateTransparency5(CmdUI *pCmdUI)
{
	if (!pCmdUI->m_pMenu)
	{
		return;
	}

	UpdateMenuShortCut(pCmdUI, ActionEnums::TRANSPARENCY_5);
}

LRESULT CQPasteWnd::OnMenuTransparency10(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	DoAction(ActionEnums::TRANSPARENCY_10);
	return 0;
}

void CQPasteWnd::OnUpdateTransparency10(CmdUI *pCmdUI)
{
	if (!pCmdUI->m_pMenu)
	{
		return;
	}

	UpdateMenuShortCut(pCmdUI, ActionEnums::TRANSPARENCY_10);
}

LRESULT CQPasteWnd::OnMenuTransparency15(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	DoAction(ActionEnums::TRANSPARENCY_15);
	return 0;
}

void CQPasteWnd::OnUpdateTransparency15(CmdUI *pCmdUI)
{
	if (!pCmdUI->m_pMenu)
	{
		return;
	}

	UpdateMenuShortCut(pCmdUI, ActionEnums::TRANSPARENCY_15);
}

LRESULT CQPasteWnd::OnMenuTransparency20(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	DoAction(ActionEnums::TRANSPARENCY_20);
	return 0;
}

void CQPasteWnd::OnUpdateTransparency20(CmdUI *pCmdUI)
{
	if (!pCmdUI->m_pMenu)
	{
		return;
	}

	UpdateMenuShortCut(pCmdUI, ActionEnums::TRANSPARENCY_20);
}

LRESULT CQPasteWnd::OnMenuTransparency25(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	DoAction(ActionEnums::TRANSPARENCY_25);
	return 0;
}

void CQPasteWnd::OnUpdateTransparency25(CmdUI *pCmdUI)
{
	if (!pCmdUI->m_pMenu)
	{
		return;
	}

	UpdateMenuShortCut(pCmdUI, ActionEnums::TRANSPARENCY_25);
}

LRESULT CQPasteWnd::OnMenuTransparency30(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	DoAction(ActionEnums::TRANSPARENCY_30);
	return 0;
}

void CQPasteWnd::OnUpdateTransparency30(CmdUI *pCmdUI)
{
	if (!pCmdUI->m_pMenu)
	{
		return;
	}

	UpdateMenuShortCut(pCmdUI, ActionEnums::TRANSPARENCY_30);
}

LRESULT CQPasteWnd::OnTransparency35(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	DoAction(ActionEnums::TRANSPARENCY_35);
	return 0;
}

void CQPasteWnd::OnUpdateTransparency35(CmdUI *pCmdUI)
{
	if (!pCmdUI->m_pMenu)
	{
		return;
	}

	UpdateMenuShortCut(pCmdUI, ActionEnums::TRANSPARENCY_35);
}

LRESULT CQPasteWnd::OnMenuTransparency40(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	DoAction(ActionEnums::TRANSPARENCY_40);
	return 0;
}

void CQPasteWnd::OnUpdateTransparency40(CmdUI *pCmdUI)
{
	if (!pCmdUI->m_pMenu)
	{
		return;
	}

	UpdateMenuShortCut(pCmdUI, ActionEnums::TRANSPARENCY_40);
}

bool CQPasteWnd::DoActionToggleTransparency()
{
	CGetSetOptions::SetEnableTransparency(!CGetSetOptions::GetEnableTransparency());
	SetCurrentTransparency();

	return true;
}

bool CQPasteWnd::DoActionIncreaseTransparency()
{
	int current = CGetSetOptions::GetTransparencyPercent();
	current += 5;
	current = min(current, 100);
	SetTransparency(current);

	return true;
}

bool CQPasteWnd::DoActionDecreaseTransparency()
{
	int current = CGetSetOptions::GetTransparencyPercent();
	current -= 5;
	current = max(current, 0);
	SetTransparency(current);

	return true;
}

bool CQPasteWnd::DoActionEmailTo()
{
	CWaitCursor wait;

	CClipIDs IDs;
	m_lstHeader.GetSelectionItemData(IDs);

	CClip clip;

	if (IDs.GetCount() > 1)
	{
		CStringA SepA = CTextConvert::UnicodeToAnsi(CGetSetOptions::GetMultiPasteSeparator());
		CStringW SepW = CGetSetOptions::GetMultiPasteSeparator();

		CHTMLFormatAggregator Html(SepA);
		if (IDs.AggregateData(Html, theApp.m_HTML_Format, CGetSetOptions::m_bMultiPasteReverse, false))
		{
			CClipFormat cf(theApp.m_HTML_Format, Html.GetHGlobal());
			clip.m_Formats.Add(cf);
			//clip.m_Formats now owns the global data
			cf.m_autoDeleteData = false;
		}

		CCF_UnicodeTextAggregator CFUnicodeText(SepW);
		if (IDs.AggregateData(CFUnicodeText, CF_UNICODETEXT, CGetSetOptions::m_bMultiPasteReverse, false))
		{
			CClipFormat cf(CF_UNICODETEXT, CFUnicodeText.GetHGlobal());
			clip.m_Formats.Add(cf);
			//clip.m_Formats now owns the global data
			cf.m_autoDeleteData = false;
		}
		else
		{

			CCF_TextAggregator CFText(SepA);
			if (IDs.AggregateData(CFText, CF_TEXT, CGetSetOptions::m_bMultiPasteReverse, false))
			{
				CClipFormat cf(CF_TEXT, CFText.GetHGlobal());
				clip.m_Formats.Add(cf);
				//clip.m_Formats now owns the global data
				cf.m_autoDeleteData = false;
			}
		}
	}
	else
	{
		clip.LoadFormats(IDs[0], false, false);
	}

	CString path = CGetSetOptions::GetPath(PATH_DRAG_FILES);
	CreateDirectory(path, NULL);
	int dragId = CGetSetOptions::GetDragId();
	int origDragId = dragId;

	CString subject;
	CString body;
	CString attachment;

	CClipFormat* html = clip.m_Formats.FindFormat(theApp.m_HTML_Format);
	if (html != NULL)
	{
		CString file;
		file.Format(_T("%shtml_%d.html"), path, dragId++);

		clip.WriteTextToHtmlFile(file);

		attachment = file;
	}
	else
	{
		CString text = clip.GetUnicodeTextFormat();
		if (text == _T(""))
		{
			text = clip.GetCFTextTextFormat();
		}

		if (text != _T(""))
		{
			body = text;
			subject = text.Left(30);
		}
		else
		{
			CClipFormat* png = NULL;
			CClipFormat* dib = clip.m_Formats.FindFormat(CF_DIB);
			if (dib == NULL)
			{
				png = clip.m_Formats.FindFormat(theApp.m_PNG_Format);
			}

			if (png != NULL ||
				dib != NULL)
			{
				CString file;
				file.Format(_T("%simage_%d.png"), path, dragId++);

				clip.WriteImageToFile(file);

				CString fileWrapper;
				fileWrapper.Format(_T("%shtml_%d.html"), path, dragId++);

				CString html;
				html.Format(_T("<html><img src=\"%s\"></html>"), file);
				CStringA convToUtf8 = CTextConvert::UnicodeToUTF8(html);

				HANDLE hHtml = ::CreateFile(fileWrapper, GENERIC_WRITE, 0, NULL,
					CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
				if (hHtml != INVALID_HANDLE_VALUE)
				{
					DWORD written = 0;
					::WriteFile(hHtml, convToUtf8.GetString(), convToUtf8.GetLength(), &written, NULL);
					::CloseHandle(hHtml);
				}

				attachment = fileWrapper;
			}
		}
	}

	if (subject != _T("") ||
		body != _T("") ||
		attachment != _T(""))
	{
		SendMail::Send(subject, body, attachment);
	}

	if (dragId != origDragId)
	{
		CGetSetOptions::SetDragId(dragId);
	}

	return true;
}

bool CQPasteWnd::DoActionGmail()
{
	CWaitCursor wait;

	CClipIDs IDs;
	m_lstHeader.GetSelectionItemData(IDs);

	CClip clip;

	if (IDs.GetCount() > 1)
	{
		CStringW SepW = CGetSetOptions::GetMultiPasteSeparator();
		CCF_UnicodeTextAggregator CFUnicodeText(SepW);
		if (IDs.AggregateData(CFUnicodeText, CF_UNICODETEXT, CGetSetOptions::m_bMultiPasteReverse, false))
		{
			CClipFormat cf(CF_UNICODETEXT, CFUnicodeText.GetHGlobal());
			clip.m_Formats.Add(cf);
			//clip.m_Formats now owns the global data
			cf.m_autoDeleteData = false;
		}
		else
		{
			CStringA SepA = CTextConvert::UnicodeToAnsi(CGetSetOptions::GetMultiPasteSeparator());
			CCF_TextAggregator CFText(SepA);
			if (IDs.AggregateData(CFText, CF_TEXT, CGetSetOptions::m_bMultiPasteReverse, false))
			{
				CClipFormat cf(CF_TEXT, CFText.GetHGlobal());
				clip.m_Formats.Add(cf);
				//clip.m_Formats now owns the global data
				cf.m_autoDeleteData = false;
			}
		}
	}
	else
	{
		clip.LoadFormats(IDs[0], true, false);
	}

	CString text = clip.GetUnicodeTextFormat();
	if (text == _T(""))
	{
		text = clip.GetCFTextTextFormat();
	}

	if (text != _T(""))
	{
		CString link;
		text.Replace(_T("\r\n"), _T("%0D%0A"));
		CString en = InternetEncode(text);

		link.Format(_T("https://mail.google.com/mail/u/0/?view=cm&body=%s"), en);

		::ShellExecute(NULL, _T("open"), link, NULL, NULL, SW_SHOWNORMAL);
	}

	return true;
}

void CQPasteWnd::RefreshScrollBarColors()
{
	m_modernScrollBar.SetColors(
		CGetSetOptions::m_Theme.ScrollBarTrack(),
		CGetSetOptions::m_Theme.ScrollBarThumb(),
		CGetSetOptions::m_Theme.ScrollBarThumbHover()
	);
	m_modernScrollBarHorz.SetColors(
		CGetSetOptions::m_Theme.ScrollBarTrack(),
		CGetSetOptions::m_Theme.ScrollBarThumb(),
		CGetSetOptions::m_Theme.ScrollBarThumbHover()
	);
}

void CQPasteWnd::RefreshThemeColors()
{
	// Refresh caption bar colors
	SetCaptionColorActive(CGetSetOptions::m_bShowPersistent, theApp.GetConnectCV());
	SetCaptionOn(CGetSetOptions::GetCaptionPos(), true, CGetSetOptions::m_Theme.GetCaptionSize(), CGetSetOptions::m_Theme.GetCaptionFontSize());
	
	// Refresh scrollbar colors
	RefreshScrollBarColors();
	
	// Force repaint of the entire window including non-client area
	SetWindowPos(NULL, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);
	RedrawWindow(NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW | RDW_ALLCHILDREN | RDW_FRAME);
}

bool CQPasteWnd::DoActionEmailToAttachExport()
{
	CWaitCursor wait;

	CClipIDs IDs;
	m_lstHeader.GetSelectionItemData(IDs);

	CString path = CGetSetOptions::GetPath(PATH_DRAG_FILES);
	CreateDirectory(path, NULL);
	int dragId = CGetSetOptions::GetDragId();
	int origDragId = dragId;

	CString file;
	file.Format(_T("%sexport_%d.dto"), path, dragId++);

	IDs.Export(file);

	SendMail::Send(_T(""), _T(""), file);

	return true;
}

bool CQPasteWnd::DoActionEmailToAttachContent()
{
	CString path = CGetSetOptions::GetPath(PATH_DRAG_FILES);
	CreateDirectory(path, NULL);

	CClipIDs clipIds;
	m_lstHeader.GetSelectionItemData(clipIds);

	int dragId = CGetSetOptions::GetDragId();
	int origDragId = dragId;

	std::vector<CString> fileList;

	for (int i = 0; i < clipIds.GetCount(); i++)
	{
		CClip fileClip;
		fileClip.LoadFormats(clipIds[i]);

		CClipFormat* unicodeText = fileClip.m_Formats.FindFormat(CF_UNICODETEXT);
		if (unicodeText)
		{
			CString file;
			file.Format(_T("%stext_%d.txt"), path, dragId++);

			fileClip.WriteTextToFile(file, TRUE, FALSE, FALSE);
			fileList.push_back(file);
		}
		else
		{
			CClipFormat* asciiText = fileClip.m_Formats.FindFormat(CF_TEXT);
			if (asciiText)
			{
				CString file;
				file.Format(_T("%stext_%d.txt"), path, dragId++);

				fileClip.WriteTextToFile(file, FALSE, TRUE, FALSE);
				fileList.push_back(file);
			}
			else
			{
				CClipFormat* png = NULL;
				CClipFormat* bitmap = fileClip.m_Formats.FindFormat(CF_DIB);
				if (bitmap == NULL)
				{
					png = fileClip.m_Formats.FindFormat(theApp.m_PNG_Format);
				}

				if (bitmap != NULL ||
					png != NULL)
				{
					CString file;
					file.Format(_T("%simage_%d.png"), path, dragId++);

					if (fileClip.WriteImageToFile(file))
					{
						fileList.push_back(file);
					}
				}
			}
		}

		//couldn't get SendMail::Send to support multiple files
		break;
	}

	if (!fileList.empty())
	{
		SendMail::Send(_T(""), _T(""), fileList.front());
	}

	if (dragId != origDragId)
	{
		CGetSetOptions::SetDragId(dragId);
	}

	return true;
}

bool CQPasteWnd::DoActionSlugify()
{
	if (::GetFocus() == m_lstHeader.m_hWnd)
	{
		CSpecialPasteOptions pasteOptions;
		pasteOptions.m_pasteSlugify = true;
		OpenSelection(pasteOptions);
		return true;
	}

	return false;
}

bool CQPasteWnd::DoRefreshList()
{
	theApp.m_FocusID = -1;

	CString csText;
	m_search.GetWindowText(csText);

	FillList(csText);

	return true;
}

bool CQPasteWnd::DoDeleteAllNonUsedClips()
{
	bool bStartValue = m_bHideWnd;
	m_bHideWnd = false;

	int nRet = MessageBox(theApp.m_Language.GetString("Delete_All_Non_Used_Clips", "Delete all clips that are not groups, in groups, marked as never auto delete, has a shortcut key or marked as sticky.\r\n\r\nThis cannot be undone."), _T("DittoWTL"), MB_OKCANCEL | MB_TOPMOST);

	m_bHideWnd = bStartValue;
	if (nRet != IDOK)
	{
		return false;
	}

	CWaitCursor wait;

	DeleteNonUsedClips(true);
	FillList();

	m_cf_dibCache.clear();
	m_cf_NO_dibCache.clear();
	m_cf_rtfCache.clear();
	m_cf_NO_rtfCache.clear();

	return true;
}

bool CQPasteWnd::DoCopySelection()
{
	ARRAY IDs;
	m_lstHeader.GetSelectionItemData(IDs);

	INT_PTR count = IDs.GetSize();

	if (count <= 0)
	{
		return FALSE;
	}

	CProcessPaste paste;

	//Don't send the paste just load it into memory
	paste.m_bSendPaste = false;

	if (count > 1)
		paste.GetClipIDs().Copy(IDs);
	else
		paste.GetClipIDs().Add(IDs[0]);

	//Don't move these to the top
	BOOL itWas = CGetSetOptions::m_bUpdateTimeOnPaste;
	CGetSetOptions::m_bUpdateTimeOnPaste = CGetSetOptions::GetUpdateClipOrderOnCtrlC();

	paste.DoPaste();

	CGetSetOptions::m_bUpdateTimeOnPaste = itWas;

	return TRUE;
}

void CQPasteWnd::SetTransparency(int percent)
{
	if (percent > 0)
	{
		CGetSetOptions::SetTransparencyPercent(percent);
		CGetSetOptions::SetEnableTransparency(TRUE);

		m_Alpha.SetTransparent(TRUE);

		float fPercent = percent / (float)100.0;

		m_Alpha.SetOpacity(OPACITY_MAX - (int)(fPercent * OPACITY_MAX));
	}
	else
	{
		CGetSetOptions::SetEnableTransparency(FALSE);
		m_Alpha.SetTransparent(FALSE);
	}
}

void CQPasteWnd::SetCurrentTransparency()
{
	//Set the transparency
	if (CGetSetOptions::GetEnableTransparency())
	{
		m_Alpha.SetTransparent(TRUE);

		float fPercent = CGetSetOptions::GetTransparencyPercent() / (float)100.0;

		m_Alpha.SetOpacity(OPACITY_MAX - (int)(fPercent * OPACITY_MAX));
	}
	else
	{
		m_Alpha.SetTransparent(FALSE);
	}
}

LRESULT CQPasteWnd::OnTransparencyIncrease(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	DoAction(ActionEnums::TRANSPARENCY_INCREASE);
	return 0;
}


void CQPasteWnd::OnUpdateTransparencyIncrease(CmdUI *pCmdUI)
{
	if (!pCmdUI->m_pMenu)
	{
		return;
	}

	UpdateMenuShortCut(pCmdUI, ActionEnums::TRANSPARENCY_INCREASE);
}


LRESULT CQPasteWnd::OnTransparencyDecrease(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	DoAction(ActionEnums::TRANSPARENCY_DECREASE);
	return 0;
}


void CQPasteWnd::OnUpdateTransparencyDecrease(CmdUI *pCmdUI)
{
	if (!pCmdUI->m_pMenu)
	{
		return;
	}

	UpdateMenuShortCut(pCmdUI, ActionEnums::TRANSPARENCY_DECREASE);
}


LRESULT CQPasteWnd::OnTransparencyToggle(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	DoAction(ActionEnums::TRANSPARENCY_TOGGLE);
	return 0;
}


void CQPasteWnd::OnUpdateTransparencyToggle(CmdUI *pCmdUI)
{
	if (!pCmdUI->m_pMenu)
	{
		return;
	}

	UpdateMenuShortCut(pCmdUI, ActionEnums::TRANSPARENCY_TOGGLE);
}

LRESULT CQPasteWnd::OnImportEmailto(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	DoAction(ActionEnums::EMAILTO_BODY);
	return 0;
}

void CQPasteWnd::OnUpdateImportEmailto(CmdUI *pCmdUI)
{
	if (!pCmdUI->m_pMenu)
	{
		return;
	}

	UpdateMenuShortCut(pCmdUI, ActionEnums::EMAILTO_BODY);
}


LRESULT CQPasteWnd::OnImportGmail(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	DoAction(ActionEnums::GMAIL);
	return 0;
}


void CQPasteWnd::OnUpdateImportGmail(CmdUI *pCmdUI)
{
	if (!pCmdUI->m_pMenu)
	{
		return;
	}

	UpdateMenuShortCut(pCmdUI, ActionEnums::GMAIL);
}


LRESULT CQPasteWnd::OnImportEmailtoasattachment(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	DoAction(ActionEnums::EMAILTO_ATTACH_EXPORT);
	return 0;
}


void CQPasteWnd::OnUpdateImportEmailtoasattachment(CmdUI *pCmdUI)
{
	if (!pCmdUI->m_pMenu)
	{
		return;
	}

	UpdateMenuShortCut(pCmdUI, ActionEnums::EMAILTO_ATTACH_EXPORT);
}


LRESULT CQPasteWnd::OnSpecialpasteSlugify(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	DoAction(ActionEnums::SLUGIFY);
	return 0;
}

void CQPasteWnd::OnUpdateSpecialpasteSlugify(CmdUI *pCmdUI)
{
	if (!pCmdUI->m_pMenu)
	{
		return;
	}

	UpdateMenuShortCut(pCmdUI, ActionEnums::SLUGIFY);
}


LRESULT CQPasteWnd::OnImportEmailContentAttach(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	DoAction(ActionEnums::EMAILTO_ATTACH_CONTENT);
	return 0;
}


void CQPasteWnd::OnUpdateImportEmailContentAttach(CmdUI *pCmdUI)
{
	if (!pCmdUI->m_pMenu)
	{
		return;
	}

	UpdateMenuShortCut(pCmdUI, ActionEnums::EMAILTO_ATTACH_CONTENT);
}


LRESULT CQPasteWnd::OnSpecialpasteTogglecase(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	DoAction(ActionEnums::INVERT_CASE);
	return 0;
}


void CQPasteWnd::OnUpdateSpecialpasteTogglecase(CmdUI *pCmdUI)
{
	if (!pCmdUI->m_pMenu)
	{
		return;
	}

	UpdateMenuShortCut(pCmdUI, ActionEnums::INVERT_CASE);
}


LRESULT CQPasteWnd::OnFirstShowstartupmessage(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	BOOL existing = CGetSetOptions::GetShowStartupMessage();
	CGetSetOptions::SetShowStartupMessage(!existing);
	return 0;
}


LRESULT CQPasteWnd::OnFirstRestoreDb(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	theApp.m_pMainFrame->PostMessage(WM_RESTORE_DB, 0, 0);
	return 0;
}

LRESULT CQPasteWnd::OnFirstBackupDb(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	theApp.m_pMainFrame->PostMessage(WM_BACKUP_DB, 0, 0);
	return 0;
}

LRESULT CQPasteWnd::OnMenuDeleteallnonusedclips(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	DoAction(ActionEnums::DELETE_ALL_NON_USED_CLIPS);
	return 0;
}

void CQPasteWnd::OnUpdateMenuDeleteallnonusedclips(CmdUI *pCmdUI)
{
	if (!pCmdUI->m_pMenu)
	{
		return;
	}

	UpdateMenuShortCut(pCmdUI, ActionEnums::DELETE_ALL_NON_USED_CLIPS);
}

LRESULT CQPasteWnd::OnImportSetdragfilename(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	DoAction(ActionEnums::SET_DRAG_FILE_NAME);
	return 0;
}

void CQPasteWnd::OnUpdateImportSetdragfilename(CmdUI *pCmdUI)
{
	if (!pCmdUI->m_pMenu)
	{
		return;
	}

	UpdateMenuShortCut(pCmdUI, ActionEnums::SET_DRAG_FILE_NAME);
}

LRESULT CQPasteWnd::OnSpecialpasteCamelcase(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	DoAction(ActionEnums::PASTE_CAMEL_CASE);
	return 0;
}

void CQPasteWnd::OnUpdateSpecialpasteCamelcase(CmdUI *pCmdUI)
{
	if (!pCmdUI->m_pMenu)
	{
		return;
	}

	UpdateMenuShortCut(pCmdUI, ActionEnums::PASTE_CAMEL_CASE);
}

LRESULT CQPasteWnd::OnSpecialpasteMultipleImagesHorz(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	DoAction(ActionEnums::PASTE_MULTI_IMAGE_HORIZONTAL);
	return 0;
}

void CQPasteWnd::OnUpdateSpecialpasteMultipleImagesHorz(CmdUI *pCmdUI)
{
	if (!pCmdUI->m_pMenu)
	{
		return;
	}

	UpdateMenuShortCut(pCmdUI, ActionEnums::PASTE_MULTI_IMAGE_HORIZONTAL);
}

LRESULT CQPasteWnd::OnSpecialpasteMultipleImagesVert(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	DoAction(ActionEnums::PASTE_MULTI_IMAGE_VERTICAL);
	return 0;
}

void CQPasteWnd::OnUpdateSpecialpasteMultipleImagesVert(CmdUI *pCmdUI)
{
	if (!pCmdUI->m_pMenu)
	{
		return;
	}

	UpdateMenuShortCut(pCmdUI, ActionEnums::PASTE_MULTI_IMAGE_VERTICAL);
}

LRESULT CQPasteWnd::OnSpecialpasteAsciitextonly(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	DoAction(ActionEnums::ASCII_TEXT_ONLY);
	return 0;
}


void CQPasteWnd::OnUpdateSpecialpasteAsciitextonly(CmdUI *pCmdUI)
{
	if (!pCmdUI->m_pMenu)
	{
		return;
	}

	UpdateMenuShortCut(pCmdUI, ActionEnums::ASCII_TEXT_ONLY);
}

LRESULT CQPasteWnd::OnImportExporttowebsearch(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	DoAction(ActionEnums::EXPORT_TO_WEB_SEARCH);
	return 0;
}

void CQPasteWnd::OnUpdateImportExporttowebsearch(CmdUI *pCmdUI)
{
	if (!pCmdUI->m_pMenu)
	{
		return;
	}

	UpdateMenuShortCut(pCmdUI, ActionEnums::EXPORT_TO_WEB_SEARCH);
}

LRESULT CQPasteWnd::OnSpecialpastePastenewguid(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	DoAction(ActionEnums::GENERATE_GUID);
	return 0;
}

void CQPasteWnd::OnUpdateSpecialpastePastenewguid(CmdUI *pCmdUI)
{
	if (!pCmdUI->m_pMenu)
	{
		return;
	}

	UpdateMenuShortCut(pCmdUI, ActionEnums::GENERATE_GUID);
}

LRESULT CQPasteWnd::OnSpecialpastePasteAsImage(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	DoAction(ActionEnums::PASTE_AS_IMAGE);
	return 0;
}

void CQPasteWnd::OnUpdateSpecialpastePasteAsImage(CmdUI *pCmdUI)
{
	if (!pCmdUI->m_pMenu)
	{
		return;
	}

	UpdateMenuShortCut(pCmdUI, ActionEnums::PASTE_AS_IMAGE);
}

struct CmdUpdateEntry
{
	UINT nID;
	void (CQPasteWnd::*pfn)(CmdUI *pCmdUI);
};

static const CmdUpdateEntry g_cmdUpdates[] =
{
	{ ID_MENU_NEWGROUP, &CQPasteWnd::OnUpdateMenuNewgroup },
	{ ID_MENU_NEWGROUPSELECTION, &CQPasteWnd::OnUpdateMenuNewgroupselection },
	{ ID_MENU_ALLWAYSONTOP, &CQPasteWnd::OnUpdateMenuAllwaysontop },
	{ ID_MENU_VIEWFULLDESCRIPTION, &CQPasteWnd::OnUpdateMenuViewfulldescription },
	{ ID_MENU_VIEWGROUPS, &CQPasteWnd::OnUpdateMenuViewgroups },
	{ ID_MENU_PASTEPLAINTEXTONLY, &CQPasteWnd::OnUpdateMenuPasteplaintextonly },
	{ ID_MENU_DELETE, &CQPasteWnd::OnUpdateMenuDelete },
	{ ID_MENU_PROPERTIES, &CQPasteWnd::OnUpdateMenuProperties },
	{ ID_MENU_EDITITEM, &CQPasteWnd::OnUpdateMenuEdititem },
	{ ID_MENU_NEWCLIP, &CQPasteWnd::OnUpdateMenuNewclip },
	{ ID_COMPARE_COMPARE, &CQPasteWnd::OnUpdateCompareCompare },
	{ ID_IMPORT_EXPORTTOGOOGLETRANSLATE, &CQPasteWnd::OnUpdateImportExporttogoogletranslate },
	{ ID_IMPORT_EXPORTCLIP_BITMAP, &CQPasteWnd::OnUpdateImportExportclipBitmap },
	{ ID_MENU_SAVECURRENTCLIPBOARD, &CQPasteWnd::OnUpdateMenuSavecurrentclipboard },
	{ ID_CLIPORDER_MOVEUP, &CQPasteWnd::OnUpdateCliporderMoveup },
	{ ID_CLIPORDER_MOVEDOWN, &CQPasteWnd::OnUpdateCliporderMovedown },
	{ ID_CLIPORDER_MOVETOTOP, &CQPasteWnd::OnUpdateCliporderMovetotop },
	{ ID_MENU_FILTERON, &CQPasteWnd::OnUpdateMenuFilteron },
	{ ID_SPECIALPASTE_UPPERCASE, &CQPasteWnd::OnUpdateSpecialpasteUppercase },
	{ ID_SPECIALPASTE_LOWERCASE, &CQPasteWnd::OnUpdateSpecialpasteLowercase },
	{ ID_SPECIALPASTE_CAPITALIZE, &CQPasteWnd::OnUpdateSpecialpasteCapitalize },
	{ ID_SPECIALPASTE_SENTENCE, &CQPasteWnd::OnUpdateSpecialpasteSentence },
	{ ID_SPECIALPASTE_REMOVELINEFEEDS, &CQPasteWnd::OnUpdateSpecialpasteRemovelinefeeds },
	{ ID_SPECIALPASTE_PASTE, &CQPasteWnd::OnUpdateSpecialpastePaste },
	{ ID_SPECIALPASTE_PASTE32919, &CQPasteWnd::OnUpdateSpecialpastePaste32919 },
	{ ID_SPECIALPASTE_TYPOGLYCEMIA, &CQPasteWnd::OnUpdateSpecialpasteTypoglycemia },
	{ ID_QUICKOPTIONS_SHOWTEXTFORFIRSTTENCOPYHOTKEYS, &CQPasteWnd::OnUpdateQuickoptionsShowtextforfirsttencopyhotkeys },
	{ ID_QUICKOPTIONS_SHOWINDICATORACLIPHASBEENPASTED, &CQPasteWnd::OnUpdateQuickoptionsShowindicatoracliphasbeenpasted },
	{ ID_GROUPS_TOGGLELASTGROUP, &CQPasteWnd::OnUpdateGroupsTogglelastgroup },
	{ ID_STICKYCLIPS_MAKETOPSTICKYCLIP, &CQPasteWnd::OnUpdateStickyclipsMaketopstickyclip },
	{ ID_STICKYCLIPS_MAKELASTSTICKYCLIP, &CQPasteWnd::OnUpdateStickyclipsMakelaststickyclip },
	{ ID_STICKYCLIPS_REMOVESTICKYSETTING, &CQPasteWnd::OnUpdateStickyclipsRemovestickysetting },
	{ ID_SPECIALPASTE_PASTE32927, &CQPasteWnd::OnUpdateSpecialpastePaste32927 },
	{ ID_MENU_IMPORTCLIP32935, &CQPasteWnd::OnUpdateMenuImportclip32935 },
	{ ID_MENU_NEWCLIP32937, &CQPasteWnd::OnUpdateMenuNewclip32937 },
	{ ID_MENU_GLOBALHOTKEYS32933, &CQPasteWnd::OnUpdateMenuGlobalhotkeys32933 },
	{ ID_MENU_DELETECLIPDATA32934, &CQPasteWnd::OnUpdateMenuDeleteclipdata32934 },
	{ ID_CLIPORDER_REPLACETOPSTICKYCLIP, &CQPasteWnd::OnUpdateCliporderReplacetopstickyclip },
	{ ID_SENDTO_PROMPTFORNAME, &CQPasteWnd::OnUpdateSendtoPromptforname },
	{ ID_IMPORT_IMPORTCOPIEDFILE, &CQPasteWnd::OnUpdateImportImportcopiedfile },
	{ 32775, &CQPasteWnd::OnUpdate32775 },
	{ ID_CLIPORDER_MOVETOLAST, &CQPasteWnd::OnUpdateCliporderMovetolast },
	{ ID_SPECIALPASTE_PASTE32945, &CQPasteWnd::OnUpdateOnSpecialPasteDontUpdateOrder },
	{ ID_SPECIALPASTE_TRIM, &CQPasteWnd::OnUpdateSpecialpasteTrim },
	{ ID_TRANSPARENCY_INCREASE, &CQPasteWnd::OnUpdateTransparencyIncrease },
	{ ID_TRANSPARENCY_DECREASE, &CQPasteWnd::OnUpdateTransparencyDecrease },
	{ ID_TRANSPARENCY_TOGGLE, &CQPasteWnd::OnUpdateTransparencyToggle },
	{ ID_MENU_TRANSPARENCY_NONE, &CQPasteWnd::OnUpdateTransparencyNone },
	{ ID_MENU_TRANSPARENCY_5, &CQPasteWnd::OnUpdateTransparency5 },
	{ ID_MENU_TRANSPARENCY_10, &CQPasteWnd::OnUpdateTransparency10 },
	{ ID_MENU_TRANSPARENCY_15, &CQPasteWnd::OnUpdateTransparency15 },
	{ ID_MENU_TRANSPARENCY_20, &CQPasteWnd::OnUpdateTransparency20 },
	{ ID_MENU_TRANSPARENCY_25, &CQPasteWnd::OnUpdateTransparency25 },
	{ ID_MENU_TRANSPARENCY_30, &CQPasteWnd::OnUpdateTransparency30 },
	{ ID_TRANSPARENCY_35, &CQPasteWnd::OnUpdateTransparency35 },
	{ ID_MENU_TRANSPARENCY_40, &CQPasteWnd::OnUpdateTransparency40 },
	{ ID_IMPORT_EMAILTO, &CQPasteWnd::OnUpdateImportEmailto },
	{ ID_IMPORT_GMAIL, &CQPasteWnd::OnUpdateImportGmail },
	{ ID_IMPORT_EMAILTOASATTACHMENT, &CQPasteWnd::OnUpdateImportEmailtoasattachment },
	{ ID_SPECIALPASTE_SLUGIFY, &CQPasteWnd::OnUpdateSpecialpasteSlugify },
	{ ID_IMPORT_EMAIL_CONTENT_ATTACH, &CQPasteWnd::OnUpdateImportEmailContentAttach },
	{ ID_SPECIALPASTE_TOGGLECASE, &CQPasteWnd::OnUpdateSpecialpasteTogglecase },
	{ ID_MENU_DELETEALLNONUSEDCLIPS, &CQPasteWnd::OnUpdateMenuDeleteallnonusedclips },
	{ ID_IMPORT_SETDRAGFILENAME, &CQPasteWnd::OnUpdateImportSetdragfilename },
	{ ID_SPECIALPASTE_CAMELCASE, &CQPasteWnd::OnUpdateSpecialpasteCamelcase },
	{ ID_SPECIALPASTE_MULTIPLEIMAGESHORIZONTALLY, &CQPasteWnd::OnUpdateSpecialpasteMultipleImagesHorz },
	{ ID_SPECIALPASTE_MULTIPLEIMAGESVERTICALLY, &CQPasteWnd::OnUpdateSpecialpasteMultipleImagesVert },
	{ ID_SPECIALPASTE_ASCIITEXTONLY, &CQPasteWnd::OnUpdateSpecialpasteAsciitextonly },
	{ ID_MENU_GOTOENTRY, &CQPasteWnd::OnUpdateMenuGoToEntry },
	{ ID_SPECIALPASTE_POSIXIFY_PATHS, &CQPasteWnd::OnUpdateSpecialPosixifyPaths },
	{ ID_IMPORT_EXPORTTOWEBSEARCH, &CQPasteWnd::OnUpdateImportExporttowebsearch },
	{ ID_SPECIALPASTE_PASTENEWGUID, &CQPasteWnd::OnUpdateSpecialpastePastenewguid },
	{ ID_SPECIALPASTE_PASTEASIMAGE, &CQPasteWnd::OnUpdateSpecialpastePasteAsImage },
};

void CQPasteWnd::UpdateMenuCommands(HMENU hMenu)
{
	CMenuHandle menu(hMenu);
	const int nCount = menu.GetMenuItemCount();

	for (int i = 0; i < nCount; i++)
	{
		MENUITEMINFO mii = {};
		mii.cbSize = sizeof(mii);
		mii.fMask = MIIM_ID | MIIM_FTYPE;

		if (menu.GetMenuItemInfo(i, TRUE, &mii) == FALSE)
			continue;

		if ((mii.fType & MFT_SEPARATOR) != 0)
			continue;

		for (int u = 0; u < (int)(sizeof(g_cmdUpdates) / sizeof(g_cmdUpdates[0])); u++)
		{
			if (g_cmdUpdates[u].nID != mii.wID)
				continue;

			CmdUI ui(hMenu, mii.wID, i);
			(this->*(g_cmdUpdates[u].pfn))(&ui);
			break;
		}
	}
}

LRESULT CQPasteWnd::OnInitMenuPopup(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	// MFC updated the items of the menu that is about to open (wParam), not the submenu
	// at the menu bar position carried in lParam - cascading menus get their own message.
	UpdateMenuCommands((HMENU)wParam);

	bHandled = FALSE;
	return 0;
}
