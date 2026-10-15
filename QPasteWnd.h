#pragma once

#include "QListCtrl.h"
#include "SearchEditBox.h"
#include "WndEx.h"
#include "GroupStatic.h"
#include "GroupTree.h"
#include "AlphaBlend.h"
#include "Sqlite\CppSQLite3.h"
#include <vector>
#include <list>
#include <map>
#include <atlsync.h>
#include "ClipFormatQListCtrl.h"
#include "QPasteWndThread.h"
#include "editwithbutton.h"
#include "GdipButton.h"
#include "SpecialPasteOptions.h"
#include "ClipIds.h"
#include "SymbolEdit.h"
#include "Popup.h"
#include "CustomFriendsHelper.h"
#include "CmdUI.h"
#include <shlobj.h>
#include <oleidl.h>

#define ID_LIST_HEADER			0x201
#define ID_EDIT_SEARCH			0x202
#define ID_CANCEL				0x203
#define ID_GROUP_TEXT			0x204
#define ID_SHOW_GROUPS_BOTTOM	0x205
#define ID_SHOW_GROUPS_TOP		0x206
#define ID_BACK_BUTTON			0x207
#define ID_SEARCH_DESCRIPTION_BUTTON 0x208
#define ON_TOP_WARNING			0x209
#define ID_SYSTEM_BUTTON		0x210
#define ID_NO_SEARCH_RESULTS	0x211

class CMainTable
{
public:
    CMainTable(): 
		m_lID( - 1), 
		m_bDontAutoDelete(false), 
		m_bIsGroup(false), 
		m_bHasShortCut(false), 
		m_bHasParent(false),
		m_dateCopied(0),
		m_datePasted(0)
	{

	}

    ~CMainTable()
	{

    }

    long m_lID;
    CString m_Desc;
    bool m_bDontAutoDelete;
    bool m_bIsGroup;
    bool m_bHasShortCut;
    bool m_bHasParent;
    CString m_QuickPaste;
	double m_clipOrder;
	double m_clipGroupOrder;
	double m_stickyClipOrder;
	double m_stickyClipGroupOrder;
	int m_dateCopied;
	int m_datePasted;

	static bool SortDesc(const CMainTable& d1, const CMainTable& d2)
	{
		double d1StickyOrder = d1.m_stickyClipOrder;
		double d2StickyOrder = d2.m_stickyClipOrder;		

		if (d1StickyOrder != d2StickyOrder)
			return d1StickyOrder > d2StickyOrder;

		if (d1.m_bIsGroup != d2.m_bIsGroup)
			return d1.m_bIsGroup < d2.m_bIsGroup;

		return d1.m_clipOrder > d2.m_clipOrder;
	}

	static bool GroupSortDesc(const CMainTable& d1, const CMainTable& d2)
	{
		double d1StickyOrder = d1.m_stickyClipGroupOrder;

		double d2StickyOrder = d2.m_stickyClipGroupOrder;

		if (d1StickyOrder != d2StickyOrder)
			return d1StickyOrder > d2StickyOrder;

		if (d1.m_bIsGroup != d2.m_bIsGroup)
			return d1.m_bIsGroup < d2.m_bIsGroup;

		return d1.m_clipGroupOrder > d2.m_clipGroupOrder;
	}
};


typedef std::map < int, CMainTable > MainTypeMap;
typedef std::map < int, CClipFormatQListCtrl > CF_DibTypeMap;
typedef std::map < int, char > CF_NoDibTypeMap;



/////////////////////////////////////////////////////////////////////////////
// CQPasteWnd window

class CQPasteWnd: public CWndEx
{
    // Construction
public:
	BEGIN_MSG_MAP(CQPasteWnd)
		MSG_WM_ERASEBKGND(OnEraseBkgnd)
		MSG_WM_CREATE(OnCreate)
		MSG_WM_SIZE(OnSize)
		MSG_WM_SETFOCUS(OnSetFocus)
		MSG_WM_KILLFOCUS(OnKillFocus)
		MSG_WM_ACTIVATE(OnActivate)
		COMMAND_ID_HANDLER(ID_MENU_LINESPERROW_1, OnMenuLinesperrow1)
		COMMAND_ID_HANDLER(ID_MENU_LINESPERROW_2, OnMenuLinesperrow2)
		COMMAND_ID_HANDLER(ID_MENU_LINESPERROW_3, OnMenuLinesperrow3)
		COMMAND_ID_HANDLER(ID_MENU_LINESPERROW_4, OnMenuLinesperrow4)
		COMMAND_ID_HANDLER(ID_MENU_LINESPERROW_5, OnMenuLinesperrow5)
		COMMAND_ID_HANDLER(ID_MENU_TRANSPARENCY_10, OnMenuTransparency10)
		COMMAND_ID_HANDLER(ID_MENU_TRANSPARENCY_15, OnMenuTransparency15)
		COMMAND_ID_HANDLER(ID_MENU_TRANSPARENCY_20, OnMenuTransparency20)
		COMMAND_ID_HANDLER(ID_MENU_TRANSPARENCY_25, OnMenuTransparency25)
		COMMAND_ID_HANDLER(ID_MENU_TRANSPARENCY_30, OnMenuTransparency30)
		COMMAND_ID_HANDLER(ID_MENU_TRANSPARENCY_40, OnMenuTransparency40)
		COMMAND_ID_HANDLER(ID_MENU_TRANSPARENCY_5, OnMenuTransparency5)
		COMMAND_ID_HANDLER(ID_MENU_TRANSPARENCY_NONE, OnMenuTransparencyNone)
		COMMAND_ID_HANDLER(ID_MENU_DELETE, OnMenuDelete)
		COMMAND_ID_HANDLER(ID_MENU_POSITIONING_ATCARET, OnMenuPositioningAtcaret)
		COMMAND_ID_HANDLER(ID_MENU_POSITIONING_ATCURSOR, OnMenuPositioningAtcursor)
		COMMAND_ID_HANDLER(ID_MENU_POSITIONING_ATPREVIOUSPOSITION, OnMenuPositioningAtpreviousposition)
		COMMAND_ID_HANDLER(ID_MENU_OPTIONS, OnMenuOptions)
		COMMAND_ID_HANDLER(ID_MENU_EXITPROGRAM, OnMenuExitprogram)
		COMMAND_ID_HANDLER(ID_MENU_TOGGLECONNECTCV, OnMenuToggleConnectCV)
		COMMAND_ID_HANDLER(ID_MENU_PROPERTIES, OnMenuProperties)
		MSG_WM_CLOSE(OnClose)
		NOTIFY_HANDLER(ID_LIST_HEADER, LVN_BEGINDRAG, OnBegindrag)
		NOTIFY_HANDLER(ID_LIST_HEADER, LVN_GETDISPINFO, GetDispInfo)
		NOTIFY_HANDLER(ID_LIST_HEADER, LVN_ODFINDITEM, OnFindItem)
		COMMAND_ID_HANDLER(ID_MENU_FIRSTTENHOTKEYS_USECTRLNUM, OnMenuFirsttenhotkeysUsectrlnum)
		COMMAND_ID_HANDLER(ID_MENU_FIRSTTENHOTKEYS_SHOWHOTKEYTEXT, OnMenuFirsttenhotkeysShowhotkeytext)
		COMMAND_ID_HANDLER(ID_MENU_QUICKOPTIONS_ALLWAYSSHOWDESCRIPTION, OnMenuQuickoptionsAllwaysshowdescription)
		COMMAND_ID_HANDLER(ID_MENU_QUICKOPTIONS_DOUBLECLICKINGONCAPTION_TOGGLESALWAYSONTOP, OnMenuQuickoptionsDoubleclickingoncaptionTogglesalwaysontop)
		COMMAND_ID_HANDLER(ID_MENU_QUICKOPTIONS_DOUBLECLICKINGONCAPTION_ROLLUPWINDOW, OnMenuQuickoptionsDoubleclickingoncaptionRollupwindow)
		COMMAND_ID_HANDLER(ID_MENU_QUICKOPTIONS_DOUBLECLICKINGONCAPTION_TOGGLESALWAYSSHOWDESCRIPTION, OnMenuQuickoptionsDoubleclickingoncaptionTogglesshowdescription)
		COMMAND_ID_HANDLER(ID_MENU_QUICKOPTIONS_PROMPTFORNEWGROUPNAMES, OnMenuQuickoptionsPromptfornewgroupnames)
		COMMAND_HANDLER(ID_SHOW_GROUPS_BOTTOM, BN_CLICKED, OnShowGroupsBottom)
		COMMAND_HANDLER(ID_SHOW_GROUPS_TOP, BN_CLICKED, OnShowGroupsTop)
		COMMAND_ID_HANDLER(ID_MENU_VIEWGROUPS, OnMenuViewgroups)
		COMMAND_ID_HANDLER(ID_MENU_QUICKPROPERTIES_SETTONEVERAUTODELETE, OnMenuQuickpropertiesSettoneverautodelete)
		COMMAND_ID_HANDLER(ID_MENU_QUICKPROPERTIES_AUTODELETE, OnMenuQuickpropertiesAutodelete)
		COMMAND_ID_HANDLER(ID_MENU_QUICKPROPERTIES_REMOVEHOTKEY, OnMenuQuickpropertiesRemovehotkey)
		COMMAND_ID_HANDLER(ID_MENU_SENTTO_FRIEND_EIGHT, OnMenuSenttoFriendEight)
		COMMAND_ID_HANDLER(ID_MENU_SENTTO_FRIEND_ELEVEN, OnMenuSenttoFriendEleven)
		COMMAND_ID_HANDLER(ID_MENU_SENTTO_FRIEND_FIFTEEN, OnMenuSenttoFriendFifteen)
		COMMAND_ID_HANDLER(ID_MENU_SENTTO_FRIEND_FIVE, OnMenuSenttoFriendFive)
		COMMAND_ID_HANDLER(ID_MENU_SENTTO_FRIEND_FORE, OnMenuSenttoFriendFore)
		COMMAND_ID_HANDLER(ID_MENU_SENTTO_FRIEND_FORETEEN, OnMenuSenttoFriendForeteen)
		COMMAND_ID_HANDLER(ID_MENU_SENTTO_FRIEND_NINE, OnMenuSenttoFriendNine)
		COMMAND_ID_HANDLER(ID_MENU_SENTTO_FRIEND_SEVEN, OnMenuSenttoFriendSeven)
		COMMAND_ID_HANDLER(ID_MENU_SENTTO_FRIEND_SIX, OnMenuSenttoFriendSix)
		COMMAND_ID_HANDLER(ID_MENU_SENTTO_FRIEND_TEN, OnMenuSenttoFriendTen)
		COMMAND_ID_HANDLER(ID_MENU_SENTTO_FRIEND_THIRTEEN, OnMenuSenttoFriendThirteen)
		COMMAND_ID_HANDLER(ID_MENU_SENTTO_FRIEND_THREE, OnMenuSenttoFriendThree)
		COMMAND_ID_HANDLER(ID_MENU_SENTTO_FRIEND_TWELVE, OnMenuSenttoFriendTwelve)
		COMMAND_ID_HANDLER(ID_MENU_SENTTO_FRIEND_TWO, OnMenuSenttoFriendTwo)
		COMMAND_ID_HANDLER(ID_MENU_SENTTO_FRIENDONE, OnMenuSenttoFriendone)
		COMMAND_ID_HANDLER(ID_MENU_SENTTO_PROMPTFORIP, OnMenuSenttoPromptforip)
		COMMAND_ID_HANDLER(ID_MENU_GROUPS_MOVETOGROUP, OnMenuGroupsMovetogroup)
		COMMAND_ID_HANDLER(ID_MENU_PASTEPLAINTEXTONLY, OnMenuPasteplaintextonly)
		COMMAND_ID_HANDLER(ID_MENU_HELP, OnMenuHelp)
		COMMAND_ID_HANDLER(ID_MENU_QUICKOPTIONS_FONT, OnMenuQuickoptionsFont)
		COMMAND_ID_HANDLER(ID_MENU_QUICKOPTIONS_SHOWTHUMBNAILS, OnMenuQuickoptionsShowthumbnails)
		COMMAND_ID_HANDLER(ID_MENU_QUICKOPTIONS_DRAWRTFTEXT, OnMenuQuickoptionsDrawrtftext)
		COMMAND_ID_HANDLER(ID_MENU_QUICKOPTIONS_PASTECLIPAFTERSELECTION, OnMenuQuickoptionsPasteclipafterselection)
		COMMAND_HANDLER(ID_EDIT_SEARCH, EN_CHANGE, OnSearchEditChange)
		COMMAND_ID_HANDLER(ID_MENU_QUICKOPTIONS_FINDASYOUTYPE, OnMenuQuickoptionsFindasyoutype)
		COMMAND_ID_HANDLER(ID_MENU_QUICKOPTIONS_ENSUREENTIREWINDOWISVISIBLE, OnMenuQuickoptionsEnsureentirewindowisvisible)
		COMMAND_ID_HANDLER(ID_MENU_QUICKOPTIONS_SHOWCLIPSTHATAREINGROUPSINMAINLIST, OnMenuQuickoptionsShowclipsthatareingroupsinmainlist)
		COMMAND_ID_HANDLER(ID_QUICKOPTIONS_PROMPTTODELETECLIP, OnPromptToDeleteClip)
		COMMAND_ID_HANDLER(ID_STICKYCLIPS_MAKETOPSTICKYCLIP, OnMakeTopStickyClip)
		COMMAND_ID_HANDLER(ID_STICKYCLIPS_MAKELASTSTICKYCLIP, OnMakeLastStickyClip)
		COMMAND_ID_HANDLER(ID_STICKYCLIPS_REMOVESTICKYSETTING, OnRemoveSticky)
		COMMAND_ID_HANDLER(ID_QUICKOPTIONS_ELEVATEPREVILEGESTOPASTEINTOELEVATEDAPPS, OnElevateAppToPasteIntoElevatedApp)
		MSG_WM_DESTROY(OnDestroy)
		MESSAGE_HANDLER(NM_SEARCH_ENTER_PRESSED, OnSearchEnterKeyPressed)
		MESSAGE_HANDLER(NM_COPY_CLIP, OnCopyClip)
		MESSAGE_HANDLER(NM_END, OnListEnd)
		MESSAGE_HANDLER(CB_SEARCH, OnSearch)
		MESSAGE_HANDLER(NM_DELETE, OnDelete)
		NOTIFY_HANDLER(ID_LIST_HEADER, NM_GETTOOLTIPTEXT, OnGetToolTipText)
		MESSAGE_HANDLER(NM_SELECT_DB_ID, OnListSelect_DB_ID)
		MESSAGE_HANDLER(WM_REFRESH_VIEW, OnRefreshView)
		MESSAGE_HANDLER(WM_RELOAD_CLIP_IN_UI, OnReloadClipInUI)
		MSG_WM_NCLBUTTONDBLCLK(OnNcLButtonDblClk)
		MSG_WM_WINDOWPOSCHANGING(OnWindowPosChanging)
		COMMAND_ID_HANDLER(ID_VIEWCAPTIONBARON_RIGHT, OnViewcaptionbaronRight)
		COMMAND_ID_HANDLER(ID_VIEWCAPTIONBARON_BOTTOM, OnViewcaptionbaronBottom)
		COMMAND_ID_HANDLER(ID_VIEWCAPTIONBARON_LEFT, OnViewcaptionbaronLeft)
		COMMAND_ID_HANDLER(ID_VIEWCAPTIONBARON_TOP, OnViewcaptionbaronTop)
		COMMAND_ID_HANDLER(ID_MENU_AUTOHIDE, OnMenuAutohide)
		COMMAND_ID_HANDLER(ID_MENU_VIEWFULLDESCRIPTION, OnMenuViewfulldescription)
		COMMAND_ID_HANDLER(ID_MENU_ALLWAYSONTOP, OnMenuAllwaysontop)
		COMMAND_ID_HANDLER(ID_MENU_NEWGROUP, OnMenuNewGroup)
		COMMAND_ID_HANDLER(ID_MENU_NEWGROUPSELECTION, OnMenuNewGroupSelection)
		MESSAGE_HANDLER(NM_GROUP_TREE_MESSAGE, OnGroupTreeMessage)
		COMMAND_ID_HANDLER(ID_BACK_BUTTON, OnBackButton)
		COMMAND_ID_HANDLER(ID_SYSTEM_BUTTON, OnSystemButton)
		MESSAGE_HANDLER(CB_UPDOWN, OnUpDown)
		MESSAGE_HANDLER(NM_INACTIVE_TOOLTIPWND, OnToolTipWndInactive)
		MESSAGE_HANDLER(NM_SET_LIST_COUNT, OnSetListCount)
		MESSAGE_HANDLER(NM_REFRESH_ROW, OnRefeshRow)
		MESSAGE_HANDLER(NM_ITEM_DELETED, OnItemDeleted)
		MSG_WM_TIMER(OnTimer)
		COMMAND_ID_HANDLER(ID_MENU_EXPORT, OnMenuExport)
		COMMAND_ID_HANDLER(ID_MENU_IMPORT, OnMenuImport)
		COMMAND_ID_HANDLER(ID_QUICKPROPERTIES_REMOVEQUICKPASTE, OnQuickpropertiesRemovequickpaste)
		COMMAND_ID_HANDLER(ID_MENU_EDITITEM, OnMenuEdititem)
		COMMAND_ID_HANDLER(ID_MENU_NEWCLIP, OnMenuNewclip)
		COMMAND_RANGE_HANDLER(3000, 4000, OnAddinSelect)
		MESSAGE_HANDLER(NM_ALL_SELECTED, OnSelectAll)
		MESSAGE_HANDLER(NM_SHOW_HIDE_SCROLLBARS, OnShowHideScrollBar)
		MESSAGE_HANDLER(NM_CANCEL_SEARCH, OnCancelFilter)
		MESSAGE_HANDLER(NM_POST_OPTIONS_WINDOW, OnPostOptions)
		COMMAND_ID_HANDLER(ID_MENU_SEARCHDESCRIPTION, OnMenuSearchDescription)
		COMMAND_ID_HANDLER(ID_MENU_SEARCHFULLTEXT, OnMenuSearchFullText)
		COMMAND_ID_HANDLER(ID_MENU_SEARCHQUICKPASTE, OnMenuSearchQuickPaste)
		COMMAND_ID_HANDLER(ID_MENU_CONTAINSTEXTSEARCHONLY, OnMenuSimpleTextSearch)
		COMMAND_ID_HANDLER(ID_QUICKOPTIONS_SHOWINTASKBAR, OnQuickoptionsShowintaskbar)
		COMMAND_ID_HANDLER(ID_MENU_VIEWASQRCODE, OnMenuViewasqrcode)
		COMMAND_ID_HANDLER(ID_EXPORT_EXPORTTOTEXTFILE, OnExportExporttotextfile)
		COMMAND_ID_HANDLER(ID_COMPARE_COMPARE, OnCompareCompare)
		COMMAND_ID_HANDLER(ID_COMPARE_SELECTLEFTCOMPARE, OnCompareSelectleftcompare)
		COMMAND_ID_HANDLER(ID_COMPARE_COMPAREAGAINST, OnCompareCompareagainst)
		MESSAGE_HANDLER(NM_SHOW_PROPERTIES, OnShowProperties)
		MESSAGE_HANDLER(NM_NEW_GROUP, OnNewGroup)
		MESSAGE_HANDLER(NM_DELETE_ID, OnDeleteId)
		COMMAND_ID_HANDLER(ID_MENU_REGULAREXPRESSIONSEARCH, OnMenuRegularexpressionsearch)
		COMMAND_ID_HANDLER(ID_IMPORT_EXPORTTOGOOGLETRANSLATE, OnImportExporttogoogletranslate)
		COMMAND_ID_HANDLER(ID_IMPORT_EXPORTCLIP_BITMAP, OnImportExportclipBitmap)
		COMMAND_ID_HANDLER(ID_MENU_WILDCARDSEARCH, OnMenuWildcardsearch)
		COMMAND_ID_HANDLER(ID_MENU_SAVECURRENTCLIPBOARD, OnMenuSavecurrentclipboard)
		MESSAGE_HANDLER(NM_MOVE_TO_GROUP, OnListMoveSelectionToGroup)
		COMMAND_ID_HANDLER(ID_CLIPORDER_MOVEUP, OnCliporderMoveup)
		COMMAND_ID_HANDLER(ID_CLIPORDER_MOVEDOWN, OnCliporderMovedown)
		COMMAND_ID_HANDLER(ID_CLIPORDER_MOVETOTOP, OnCliporderMovetotop)
		COMMAND_ID_HANDLER(ID_MENU_FILTERON, OnMenuFilteron)
		COMMAND_ID_HANDLER(ON_TOP_WARNING, OnAlwaysOnTopClicked)
		COMMAND_ID_HANDLER(ID_SPECIALPASTE_UPPERCASE, OnSpecialpasteUppercase)
		COMMAND_ID_HANDLER(ID_SPECIALPASTE_LOWERCASE, OnSpecialpasteLowercase)
		COMMAND_ID_HANDLER(ID_SPECIALPASTE_CAPITALIZE, OnSpecialpasteCapitalize)
		COMMAND_ID_HANDLER(ID_SPECIALPASTE_SENTENCE, OnSpecialpasteSentence)
		COMMAND_ID_HANDLER(ID_SPECIALPASTE_REMOVELINEFEEDS, OnSpecialpasteRemovelinefeeds)
		COMMAND_ID_HANDLER(ID_SPECIALPASTE_PASTE, OnSpecialpastePaste)
		COMMAND_ID_HANDLER(ID_SPECIALPASTE_PASTE32919, OnSpecialpastePaste32919)
		COMMAND_ID_HANDLER(ID_SPECIALPASTE_TYPOGLYCEMIA, OnSpecialpasteTypoglycemia)
		NOTIFY_HANDLER(ID_LIST_HEADER, NM_CLICK, OnNMClickList1)
		NOTIFY_HANDLER(ID_LIST_HEADER, NM_DBLCLK, OnNMDblclkList1)
		NOTIFY_HANDLER(ID_LIST_HEADER, NM_RCLICK, OnNMRClickList1)
		NOTIFY_HANDLER(ID_LIST_HEADER, NM_RDBLCLK, OnNMRDblclkList1)
		COMMAND_ID_HANDLER(ID_QUICKOPTIONS_SHOWTEXTFORFIRSTTENCOPYHOTKEYS, OnQuickoptionsShowtextforfirsttencopyhotkeys)
		COMMAND_ID_HANDLER(ID_QUICKOPTIONS_SHOWINDICATORACLIPHASBEENPASTED, OnQuickoptionsShowindicatoracliphasbeenpasted)
		COMMAND_ID_HANDLER(ID_GROUPS_TOGGLELASTGROUP, OnGroupsTogglelastgroup)
		COMMAND_ID_HANDLER(ID_SPECIALPASTE_PASTE32927, OnSpecialpastePaste32927)
		COMMAND_ID_HANDLER(ID_MENU_GLOBALHOTKEYS32933, OnMenuGlobalhotkeys32933)
		COMMAND_ID_HANDLER(ID_MENU_DELETECLIPDATA32934, OnMenuDeleteclipdata32934)
		COMMAND_ID_HANDLER(ID_MENU_IMPORTCLIP32935, OnMenuImportclip32935)
		COMMAND_ID_HANDLER(ID_MENU_NEWCLIP32937, OnMenuNewclip32937)
		MESSAGE_HANDLER(NM_FOCUS_ON_SEARCH, OnSearchFocused)
		COMMAND_ID_HANDLER(ID_CLIPORDER_REPLACETOPSTICKYCLIP, OnCliporderReplacetopstickyclip)
		COMMAND_ID_HANDLER(ID_SENDTO_PROMPTFORNAME, OnSendtoPromptforname)
		COMMAND_ID_HANDLER(ID_IMPORT_IMPORTCOPIEDFILE, OnImportImportcopiedfile)
		COMMAND_RANGE_HANDLER(CustomFriendStartId, (CustomFriendStartId + MaxCustomFriends + 1), OnCustomSendToFriend)
		COMMAND_RANGE_HANDLER(ChaiScriptMenuStartId, (ChaiScriptMenuStartId + MaxChaiScripts + 1), OnChaiScriptPaste)
		MESSAGE_HANDLER(WM_DPICHANGED, OnDpiChanged)
		COMMAND_ID_HANDLER(ID_CLIPORDER_MOVETOLAST, OnCliporderMovetolast)
		COMMAND_ID_HANDLER(ID_SPECIALPASTE_PASTE32945, OnSpecialpastePasteDontUpdateOrder)
		COMMAND_ID_HANDLER(ID_SPECIALPASTE_TRIM, OnSpecialpasteTrim)
		COMMAND_ID_HANDLER(ID_TRANSPARENCY_INCREASE, OnTransparencyIncrease)
		COMMAND_ID_HANDLER(ID_TRANSPARENCY_DECREASE, OnTransparencyDecrease)
		COMMAND_ID_HANDLER(ID_TRANSPARENCY_TOGGLE, OnTransparencyToggle)
		COMMAND_ID_HANDLER(ID_TRANSPARENCY_35, OnTransparency35)
		COMMAND_ID_HANDLER(ID_IMPORT_EMAILTO, OnImportEmailto)
		COMMAND_ID_HANDLER(ID_IMPORT_GMAIL, OnImportGmail)
		COMMAND_ID_HANDLER(ID_IMPORT_EMAILTOASATTACHMENT, OnImportEmailtoasattachment)
		COMMAND_ID_HANDLER(ID_SPECIALPASTE_SLUGIFY, OnSpecialpasteSlugify)
		COMMAND_ID_HANDLER(ID_IMPORT_EMAIL_CONTENT_ATTACH, OnImportEmailContentAttach)
		COMMAND_ID_HANDLER(ID_SPECIALPASTE_TOGGLECASE, OnSpecialpasteTogglecase)
		COMMAND_ID_HANDLER(ID_FIRST_SHOWSTARTUPMESSAGE, OnFirstShowstartupmessage)
		COMMAND_ID_HANDLER(ID_MENU_RESTOREDATABSAE, OnFirstRestoreDb)
		COMMAND_ID_HANDLER(ID_MENU_BACKUPDATABASE, OnFirstBackupDb)
		COMMAND_ID_HANDLER(ID_MENU_DELETEALLNONUSEDCLIPS, OnMenuDeleteallnonusedclips)
		COMMAND_ID_HANDLER(ID_IMPORT_SETDRAGFILENAME, OnImportSetdragfilename)
		COMMAND_ID_HANDLER(ID_SPECIALPASTE_CAMELCASE, OnSpecialpasteCamelcase)
		COMMAND_ID_HANDLER(ID_SPECIALPASTE_MULTIPLEIMAGESHORIZONTALLY, OnSpecialpasteMultipleImagesHorz)
		COMMAND_ID_HANDLER(ID_SPECIALPASTE_MULTIPLEIMAGESVERTICALLY, OnSpecialpasteMultipleImagesVert)
		COMMAND_ID_HANDLER(ID_SPECIALPASTE_ASCIITEXTONLY, OnSpecialpasteAsciitextonly)
		MESSAGE_HANDLER(WM_INITMENUPOPUP, OnInitMenuPopup)
		CHAIN_MSG_MAP(CWndEx)
		REFLECT_NOTIFICATIONS_MSG_FILTERED(WM_DRAWITEM)
		REFLECT_NOTIFICATIONS_MSG_FILTERED(WM_CTLCOLORSTATIC)
		// Notifications and owner-draw measurement not handled above go back to the
		// control that sent them, the way MFC's ON_NOTIFY_REFLECT/ON_WM_MEASUREITEM_REFLECT did.
		REFLECT_NOTIFICATIONS_MSG_FILTERED(WM_NOTIFY)
		REFLECT_NOTIFICATIONS_MSG_FILTERED(WM_MEASUREITEM)
	END_MSG_MAP()

	LRESULT OnInitMenuPopup(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	void UpdateMenuCommands(HMENU hMenu);
    CQPasteWnd();

    // Attributes
public:

    // Operations
public:

    // Overrides
    // ClassWizard generated virtual function overrides
public:
    BOOL Create(const CRect& rcStart, CWindow *pWndParent);
    virtual BOOL PreTranslateMessage(MSG *pMsg);

	bool CheckActions(MSG * pMsg);


    // Implementation
public:
    bool Add(const CString &csHeader, const CString &csText, int nID);
    virtual ~CQPasteWnd();

    void UpdateFont();

    //protected:
    CQListCtrl m_lstHeader;

    CAlphaBlend m_Alpha;
	//CEditWithButton m_search;
	CSymbolEdit m_search;
    CFont m_SearchFont;
    bool m_bHideWnd;
    CString m_strSQLSearch;
	CString m_strSearch;
    CGroupStatic m_stGroup;
    CFont m_groupFont;
    CString m_Title;
    CGroupTree m_GroupTree;
    CGdipButton m_ShowGroupsFolderBottom;
    CGdipButton m_BackButton;
	CGroupStatic m_alwaysOnToWarningStatic;
	CGdipButton m_systemMenu;
	CGroupStatic m_noSearchResultsStatic;

    long m_lRecordCount;
    bool m_bStopQuery;
    bool m_bHandleSearchTextChange;
    bool m_bModifersMoveActive;

    CQPasteWndThread m_thread;
	CQPasteWndThread m_extraDataThread;
	std::vector<CMainTable> m_listItems;

	std::list<CPoint> m_loadItems;
    std::list<CClipFormatQListCtrl> m_ExtraDataLoadItems;
    CF_DibTypeMap m_cf_dibCache;
	CF_NoDibTypeMap m_cf_NO_dibCache;
    CF_DibTypeMap m_cf_rtfCache;
	CF_NoDibTypeMap m_cf_NO_rtfCache;
    CCriticalSection m_CritSection;
    CAccels m_actions;
	CAccels m_toolTipActions;
	CAccels m_modifierKeyActions;
	bool m_showScrollBars;
	int m_leftSelectedCompareId;
	INT64 m_extraDataCounter;
	CPopup m_popupMsg;
	CCustomFriendsHelper m_customFriendsHelper;
	bool m_noSearchResults;
	CAccel m_timerAction;
	__int64 m_lastDbWrite;
	bool m_pendingRefresh;
	DWORD m_lastNonActiveMouseMove;

    void RefreshNc();
    void UpdateStatus(bool bRepaintImmediately = false); // regenerates the status (caption) text
    BOOL FillList(CString csSQLSearch = "");
    BOOL HideQPasteWindow(bool releaseFocus, BOOL clearSearchData = -1);
    BOOL ShowQPasteWindow(BOOL bFillList = TRUE);
    void MoveControls();

    void DeleteSelectedRows();

	BOOL OpenID(int id, CSpecialPasteOptions pasteOptions);
	BOOL OpenSelection(CSpecialPasteOptions pasteOptions);
    BOOL OpenIndex(int item);
	BOOL NewGroup(bool bGroupSelection = true, int parentId = -1);

    CString LoadDescription(int nItem);
    bool SaveDescription(int nItem, CString text);

    //Menu Items
    void SetLinesPerRow(int lines, bool force, bool resetListCount);
    void SetTransparency(int percent);
    void OnUpdateLinesPerRow(CmdUI *pCmdUI, int nValue);
    void OnUpdateTransparency(CmdUI *pCmdUI, int nValue);
    void SetMenuChecks(CMenuHandle *pMenu);
    void SetSendToMenu(CMenuHandle *pMenu, int nMenuID, int nArrayPos);
	void SetFriendChecks(CMenuHandle *pMenu);

    BOOL SendToFriendbyPos(int nPos, CString override_IP_Host);

    bool InsertNextNRecords(int nEnd);

    CString GetDisplayText(int lDontAutoDelete, int lShortCut, bool bIsGroup, int lParentID, CString csText);

    static void FillMainTable(CMainTable &table, CppSQLite3Query &q);
    void RunThread();
    void MoveSelection(bool down, bool requireModifersActive);
    void OnKeyStateUp();
    void SetKeyModiferState(bool bActive);
	void SaveWindowSize();
	void SelectFocusID();
	void SetSearchImages();
	void RemoveStickyInternal(int id, bool &sort);

	DWORD OnDragOver(IDataObject * pDataObject, DWORD dwKeyState, CPoint point);
	DWORD OnDragEnter(IDataObject * pDataObject, DWORD dwKeyState, CPoint point);
	BOOL OnDrop(IDataObject * pDataObject, DWORD dropEffect, CPoint point);
	void OnDragLeave();
	bool DoAction(CAccel a);
	bool DoAction(DWORD cmd);
	bool DoActionShowDescription();
	bool DoActionNextDescription(); 
	bool DoActionPrevDescription();
	bool DoActionShowMenu();
	bool DoActionShowSystemMenu();
	bool DoActionNewGroup();
	bool DoActionNewGroupSelection();
	bool DoActionToggleFileLogging();
	bool DoActionToggleOutputDebugString();
	bool DoActionCloseWindow();
	bool DoActionForceCloseWindow();
	bool DoActionNextTabControl();
	bool DoActionPrevTabControl();
	bool DoActionShowGroups();
	bool DoActionNewClip();
	bool DoActionEditClip();
	bool DoActionMoveSelectionDown();
	bool DoActionToggleDescriptionWordWrap();
	bool DoActionApplyLastSearch();
	bool DoActionToggleSearchMethod();
	bool DoActionPasteScript(CString scriptGuid);
	bool DoActionMoveSelectionUp();
	bool DoModifierActiveActionSelectionUp();
	bool DoModifierActiveActionSelectionDown();
	bool DoModifierActiveActionMoveFirst();
	bool DoModifierActiveActionMoveLast();
	bool DoActionCancelFilter();
	bool DoActionHomeList();
	bool DoActionBackGroup();
	bool DoActionToggleShowPersistant();
	bool DoActionDeleteSelected();
	bool DoActionPasteSelected();
	bool DoActionClipProperties();
	bool DoActionPasteSelectedPlainText();
	bool DoActionMoveClipToGroup();
	bool DoActionElevatePrivleges();
	bool DoShowInTaskBar();	
	bool DoClipCompare();
	bool DoSelectLeftSideCompare();
	bool DoSelectRightSideAndDoCompare();
	bool DoExportToQRCode();
	bool DoExportToTextFile();
	bool DoExportToGoogleTranslate();
	bool DoExportToBitMapFile();
	bool DoSaveCurrentClipboard();
	bool DoMoveClipDown();
	bool DoMoveClipUp();
	bool DoMoveClipTOP();
	bool DoMoveClipLast();
	bool DoFilterOnSelectedClip();
	bool DoPasteUpperCase();
	bool DoPasteCamelCase();
	bool DoPasteImagesVert();
	bool DoPasteAsciiOnly();
	bool DoPasteImagesHorz();
	bool DoPasteLowerCase();
	bool DoPasteCapitalize();
	bool DoPasteSentenceCase();
	bool DoInvertCase();
	bool DoPasteRemoveLineFeeds();
	bool DoPastePlusAddLineFeed();
	bool DoPasteAddTwoLineFeeds();
	bool DoPasteTypoglycemia();
	bool DoPasteAddCurrentTime();
	bool OnShowFirstTenText();
	bool OnShowClipWasPasted();
	bool OnToggleLastGroupToggle();
	bool OnMakeTopSticky(bool forceSort);
	bool OnMakeLastSticky();
	bool OnRemoveStickySetting();	
	bool DoActionReplaceTopStickyClip();
	bool DoActionPromptSendToFriend();
	bool DoActionSaveCF_HDROP_FileData();
	bool DoActionToggleClipboardConnection();
	bool DoActionPasteDontMoveClip();
	bool DoSetDragFileName();
	bool DoActionPasteTrimWhiteSpace();
	bool DoActionToggleTransparency();
	bool DoActionIncreaseTransparency();
	bool DoActionDecreaseTransparency();
	bool DoActionEmailTo();
	bool DoActionGmail();
	bool DoActionEmailToAttachExport();
	bool DoActionEmailToAttachContent();
	bool DoActionSlugify();
	bool DoCopySelection();
	bool DoRefreshList();
	bool DoDeleteAllNonUsedClips();

	bool OnNewClip();
	bool OnImportClip();
	bool OnDeleteClipData();
	bool OnGlobalHotkyes();
	
	void UpdateMenuShortCut(CmdUI *pCmdUI, DWORD action);

	bool ShowProperties(int id, int row);
	bool DeleteClips(CClipIDs &IDs, ARRAY &Indexs);
	void RemoveFromImageRtfCache(int id);
	bool SyncClipDataToArrayData(CClip &clip);
	bool SelectIds(ARRAY &ids);

	void LoadShortcuts();

	void ShowRightClickMenu();

	void SetCurrentTransparency();

    // Generated message map functions
public:
	int OnCreate(LPCREATESTRUCT lpCreateStruct);
    LRESULT OnSize(UINT nType, CSize size);
    void OnSetFocus(CWindow wndOld);
	void OnKillFocus(CWindow wndLostFocus);
    void OnActivate(UINT nState, BOOL bMinimized, CWindow wndOther);
    LRESULT OnMenuLinesperrow1(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
    LRESULT OnMenuLinesperrow2(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
    LRESULT OnMenuLinesperrow3(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
    LRESULT OnMenuLinesperrow4(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
    LRESULT OnMenuLinesperrow5(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
    LRESULT OnMenuTransparency10(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
    LRESULT OnMenuTransparency15(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
    LRESULT OnMenuTransparency20(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
    LRESULT OnMenuTransparency25(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
    LRESULT OnMenuTransparency30(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
    LRESULT OnMenuTransparency40(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
    LRESULT OnMenuTransparency5(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
    LRESULT OnMenuTransparencyNone(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
    LRESULT OnMenuDelete(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
    LRESULT OnMenuPositioningAtcaret(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
    LRESULT OnMenuPositioningAtcursor(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
    LRESULT OnMenuPositioningAtpreviousposition(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
    LRESULT OnMenuOptions(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
    LRESULT OnCancelFilter(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    LRESULT OnMenuExitprogram(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
    LRESULT OnMenuToggleConnectCV(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
    LRESULT OnMenuProperties(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
    void OnClose();
    LRESULT OnBegindrag(int idCtrl, LPNMHDR pNMHDR, BOOL& bHandled);
    LRESULT GetDispInfo(int idCtrl, LPNMHDR pNMHDR, BOOL& bHandled);
    LRESULT OnFindItem(int idCtrl, LPNMHDR pNMHDR, BOOL& bHandled);
    LRESULT OnMenuFirsttenhotkeysUsectrlnum(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
    LRESULT OnMenuFirsttenhotkeysShowhotkeytext(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
    LRESULT OnMenuQuickoptionsAllwaysshowdescription(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
    LRESULT OnMenuQuickoptionsDoubleclickingoncaptionTogglesalwaysontop(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
    LRESULT OnMenuQuickoptionsDoubleclickingoncaptionRollupwindow(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
    LRESULT OnMenuQuickoptionsDoubleclickingoncaptionTogglesshowdescription(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
    LRESULT OnMenuQuickoptionsPromptfornewgroupnames(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
    LRESULT OnShowGroupsBottom(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
    LRESULT OnShowGroupsTop(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
    LRESULT OnMenuViewgroups(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
    LRESULT OnMenuQuickpropertiesSettoneverautodelete(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
    LRESULT OnMenuQuickpropertiesAutodelete(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
    LRESULT OnMenuQuickpropertiesRemovehotkey(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
    LRESULT OnMenuSenttoFriendEight(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
    LRESULT OnMenuSenttoFriendEleven(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
    LRESULT OnMenuSenttoFriendFifteen(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
    LRESULT OnMenuSenttoFriendFive(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
    LRESULT OnMenuSenttoFriendFore(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
    LRESULT OnMenuSenttoFriendForeteen(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
    LRESULT OnMenuSenttoFriendNine(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
    LRESULT OnMenuSenttoFriendSeven(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
    LRESULT OnMenuSenttoFriendSix(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
    LRESULT OnMenuSenttoFriendTen(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
    LRESULT OnMenuSenttoFriendThirteen(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
    LRESULT OnMenuSenttoFriendThree(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
    LRESULT OnMenuSenttoFriendTwelve(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
    LRESULT OnMenuSenttoFriendTwo(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
    LRESULT OnMenuSenttoFriendone(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
    LRESULT OnMenuSenttoPromptforip(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
    LRESULT OnMenuGroupsMovetogroup(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
    LRESULT OnMenuPasteplaintextonly(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
    LRESULT OnMenuHelp(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
    LRESULT OnMenuQuickoptionsFont(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
    LRESULT OnMenuQuickoptionsShowthumbnails(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
    LRESULT OnMenuQuickoptionsDrawrtftext(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
    LRESULT OnMenuQuickoptionsPasteclipafterselection(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
    LRESULT OnSearchEditChange(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
    LRESULT OnMenuQuickoptionsFindasyoutype(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
    LRESULT OnMenuQuickoptionsEnsureentirewindowisvisible(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
    LRESULT OnMenuQuickoptionsShowclipsthatareingroupsinmainlist(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
    void OnMenuPastehtmlasplaintext();
    LRESULT OnPromptToDeleteClip(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
    void OnUpdateMenuNewgroup(CmdUI *pCmdUI);
    void OnUpdateMenuNewgroupselection(CmdUI *pCmdUI);
    void OnUpdateMenuAllwaysontop(CmdUI *pCmdUI);
    void OnUpdateMenuViewfulldescription(CmdUI *pCmdUI);
    void OnUpdateMenuViewgroups(CmdUI *pCmdUI);
    void OnUpdateMenuPasteplaintextonly(CmdUI *pCmdUI);
    void OnUpdateMenuDelete(CmdUI *pCmdUI);
    void OnUpdateMenuProperties(CmdUI *pCmdUI);
    void OnDestroy();
    LRESULT OnSearchEnterKeyPressed(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    LRESULT OnListEnd(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    LRESULT OnSearch(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    LRESULT OnDelete(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    LRESULT OnGetToolTipText(int idCtrl, LPNMHDR pNMHDR, BOOL& bHandled);
    LRESULT OnListSelect_DB_ID(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnListMoveSelectionToGroup(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    LRESULT OnRefreshView(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnReloadClipInUI(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    LRESULT OnGroupTreeMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    LRESULT OnFillRestOfList(WPARAM wParam, LPARAM lParam);
    LRESULT OnRefeshRow(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnSetListCount(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    HBRUSH CtlColor(HDC hDC);
    void OnNcLButtonDblClk(UINT nHitTest, CPoint point);
    LRESULT OnViewcaptionbaronRight(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
    LRESULT OnViewcaptionbaronBottom(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
    LRESULT OnViewcaptionbaronLeft(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
    LRESULT OnViewcaptionbaronTop(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
    LRESULT OnMenuAutohide(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
    LRESULT OnMenuViewfulldescription(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
    LRESULT OnMenuAllwaysontop(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
    LRESULT OnMenuNewGroup(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
    LRESULT OnMenuNewGroupSelection(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
    LRESULT OnBackButton(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnSystemButton(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
    LRESULT OnUpDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    LRESULT OnItemDeleted(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    LRESULT OnToolTipWndInactive(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    void OnTimer(UINT_PTR nIDEvent);
    LRESULT OnMenuExport(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
    LRESULT OnMenuImport(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
    LRESULT OnQuickpropertiesRemovequickpaste(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
    LRESULT OnMenuEdititem(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
    LRESULT OnMenuNewclip(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
    void OnUpdateMenuEdititem(CmdUI *pCmdUI);
    void OnUpdateMenuNewclip(CmdUI *pCmdUI);
    LRESULT OnAddinSelect(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnCustomSendToFriend(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnChaiScriptPaste(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
    LRESULT OnSelectAll(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnShowHideScrollBar(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnMenuSearchDescription(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnMenuSearchFullText(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnMenuSearchQuickPaste(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnMenuSimpleTextSearch(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnPostOptions(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnMakeTopStickyClip(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnMakeLastStickyClip(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnRemoveSticky(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnElevateAppToPasteIntoElevatedApp(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	
public:
	LRESULT OnQuickoptionsShowintaskbar(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnMenuViewasqrcode(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnExportExporttotextfile(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnCompareCompare(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnCompareSelectleftcompare(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnCompareCompareagainst(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	void OnUpdateCompareCompare(CmdUI *pCmdUI);
	LRESULT OnShowProperties(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnNewGroup(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnDeleteId(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnMenuRegularexpressionsearch(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnImportExporttogoogletranslate(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	void OnUpdateImportExporttogoogletranslate(CmdUI *pCmdUI);
	LRESULT OnImportExportclipBitmap(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	void OnUpdateImportExportclipBitmap(CmdUI *pCmdUI);
	LRESULT OnMenuWildcardsearch(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnMenuSavecurrentclipboard(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	void OnUpdateMenuSavecurrentclipboard(CmdUI *pCmdUI);
	LRESULT OnCliporderMoveup(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	void OnUpdateCliporderMoveup(CmdUI *pCmdUI);
	LRESULT OnCliporderMovedown(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	void OnUpdateCliporderMovedown(CmdUI *pCmdUI);
	LRESULT OnCliporderMovetotop(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	void OnUpdateCliporderMovetotop(CmdUI *pCmdUI);
	LRESULT OnMenuFilteron(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	void OnUpdateMenuFilteron(CmdUI *pCmdUI);
	LRESULT OnAlwaysOnTopClicked(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	//afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	LRESULT OnSpecialpasteUppercase(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	void OnUpdateSpecialpasteUppercase(CmdUI *pCmdUI);
	LRESULT OnSpecialpasteLowercase(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	void OnUpdateSpecialpasteLowercase(CmdUI *pCmdUI);
	LRESULT OnSpecialpasteCapitalize(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	void OnUpdateSpecialpasteCapitalize(CmdUI *pCmdUI);
	LRESULT OnSpecialpasteSentence(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	void OnUpdateSpecialpasteSentence(CmdUI *pCmdUI);
	LRESULT OnSpecialpasteRemovelinefeeds(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	void OnUpdateSpecialpasteRemovelinefeeds(CmdUI *pCmdUI);
	LRESULT OnSpecialpastePaste(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	void OnUpdateSpecialpastePaste(CmdUI *pCmdUI);
	LRESULT OnSpecialpastePaste32919(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	void OnUpdateSpecialpastePaste32919(CmdUI *pCmdUI);
	LRESULT OnSpecialpasteTypoglycemia(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	void OnUpdateSpecialpasteTypoglycemia(CmdUI *pCmdUI);
	LRESULT OnNMClickList1(int idCtrl, LPNMHDR pNMHDR, BOOL& bHandled);
	LRESULT OnNMDblclkList1(int idCtrl, LPNMHDR pNMHDR, BOOL& bHandled);
	LRESULT OnNMRClickList1(int idCtrl, LPNMHDR pNMHDR, BOOL& bHandled);
	LRESULT OnNMRDblclkList1(int idCtrl, LPNMHDR pNMHDR, BOOL& bHandled);
	LRESULT OnQuickoptionsShowtextforfirsttencopyhotkeys(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	void OnUpdateQuickoptionsShowtextforfirsttencopyhotkeys(CmdUI *pCmdUI);
	LRESULT OnQuickoptionsShowindicatoracliphasbeenpasted(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	void OnUpdateQuickoptionsShowindicatoracliphasbeenpasted(CmdUI *pCmdUI);
	LRESULT OnGroupsTogglelastgroup(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	void OnUpdateGroupsTogglelastgroup(CmdUI *pCmdUI);
	void OnUpdateStickyclipsMaketopstickyclip(CmdUI *pCmdUI);
	void OnUpdateStickyclipsMakelaststickyclip(CmdUI *pCmdUI);
	void OnUpdateStickyclipsRemovestickysetting(CmdUI *pCmdUI);
	LRESULT OnSpecialpastePaste32927(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	void OnUpdateSpecialpastePaste32927(CmdUI *pCmdUI);
	LRESULT OnEraseBkgnd(HDC hDC);
	LRESULT OnMenuGlobalhotkeys32933(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnMenuDeleteclipdata32934(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnMenuImportclip32935(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnMenuNewclip32937(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	void OnUpdateMenuImportclip32935(CmdUI *pCmdUI);
	void OnUpdateMenuNewclip32937(CmdUI *pCmdUI);
	void OnUpdateMenuGlobalhotkeys32933(CmdUI *pCmdUI);
	void OnUpdateMenuDeleteclipdata32934(CmdUI *pCmdUI);
	LRESULT OnSearchFocused(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnCliporderReplacetopstickyclip(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	void OnUpdateCliporderReplacetopstickyclip(CmdUI *pCmdUI);
	LRESULT OnSendtoPromptforname(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	void OnUpdateSendtoPromptforname(CmdUI *pCmdUI);
	LRESULT OnImportImportcopiedfile(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	void OnUpdateImportImportcopiedfile(CmdUI *pCmdUI);
	void OnUpdate32775(CmdUI *pCmdUI);
	LRESULT OnDpiChanged(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnCliporderMovetolast(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	void OnUpdateCliporderMovetolast(CmdUI *pCmdUI);
	LRESULT OnCopyClip(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnSpecialpastePasteDontUpdateOrder(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	void OnUpdateOnSpecialPasteDontUpdateOrder(CmdUI *pCmdUI);

	LRESULT OnSpecialpasteTrim(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	void OnUpdateSpecialpasteTrim(CmdUI *pCmdUI);
	LRESULT OnTransparencyIncrease(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	void OnUpdateTransparencyIncrease(CmdUI *pCmdUI);
	LRESULT OnTransparencyDecrease(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	void OnUpdateTransparencyDecrease(CmdUI *pCmdUI);
	LRESULT OnTransparencyToggle(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	void OnUpdateTransparencyToggle(CmdUI *pCmdUI);
	void OnUpdateTransparencyNone(CmdUI *pCmdUI);
	void OnUpdateTransparency5(CmdUI *pCmdUI);
	void OnUpdateTransparency10(CmdUI *pCmdUI);
	void OnUpdateTransparency15(CmdUI *pCmdUI);
	void OnUpdateTransparency20(CmdUI *pCmdUI);
	void OnUpdateTransparency25(CmdUI *pCmdUI);
	void OnUpdateTransparency30(CmdUI *pCmdUI);
	void OnUpdateTransparency35(CmdUI *pCmdUI);
	void OnUpdateTransparency40(CmdUI *pCmdUI);
	LRESULT OnTransparency35(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnImportEmailto(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	void OnUpdateImportEmailto(CmdUI *pCmdUI);
	LRESULT OnImportGmail(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	void OnUpdateImportGmail(CmdUI *pCmdUI);
	LRESULT OnImportEmailtoasattachment(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	void OnUpdateImportEmailtoasattachment(CmdUI *pCmdUI);
	LRESULT OnSpecialpasteSlugify(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	void OnUpdateSpecialpasteSlugify(CmdUI *pCmdUI);
	LRESULT OnImportEmailContentAttach(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	void OnUpdateImportEmailContentAttach(CmdUI *pCmdUI);
	LRESULT OnSpecialpasteTogglecase(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	void OnUpdateSpecialpasteTogglecase(CmdUI *pCmdUI);
	LRESULT OnFirstShowstartupmessage(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnFirstBackupDb(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnFirstRestoreDb(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnMenuDeleteallnonusedclips(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	void OnUpdateMenuDeleteallnonusedclips(CmdUI *pCmdUI);
	LRESULT OnImportSetdragfilename(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	void OnUpdateImportSetdragfilename(CmdUI *pCmdUI);
	LRESULT OnSpecialpasteCamelcase(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	void OnUpdateSpecialpasteCamelcase(CmdUI *pCmdUI);
	LRESULT OnSpecialpasteMultipleImagesHorz(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	void OnUpdateSpecialpasteMultipleImagesHorz(CmdUI *pCmdUI);
	LRESULT OnSpecialpasteMultipleImagesVert(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	void OnUpdateSpecialpasteMultipleImagesVert(CmdUI *pCmdUI);
	LRESULT OnSpecialpasteAsciitextonly(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	void OnUpdateSpecialpasteAsciitextonly(CmdUI *pCmdUI);
};