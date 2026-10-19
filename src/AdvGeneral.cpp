// AdvGeneral.cpp : implementation file
//

#include "stdafx.h"
#include "CP_Main.h"
#include "AdvGeneral.h"
#include "ScriptEditor.h"
#include "DimWnd.h"
#include "MoveToGroupDlg.h"

#define SETTING_DESC_SIZE 1
#define SETTING_SHOW_TASKBAR_ICON 2
#define SETTING_SAVE_MULTI_PASTE 3
#define SETTING_HIDE_ON_HOTKEY_IF_VISIBLE 4
#define SETTING_PASTE_IN_ACTIVE_WINDOW 5
#define SETTING_MAX_CLIP_SIZE 6
#define SETTING_CLIP_SEPARATOR 7
#define SETTING_ENSURE_CONNECTED 8
#define SETTING_COPY_PLAY_SOUND 9
#define SETTING_TEXT_FIRST_TEN 10
#define SETTING_SHOW_LEADING_WHITESPACE 11
#define SETTING_LINES_PER_ROW 12
#define SETTING_ENABLE_TRANSPARENCY 13
#define SETTING_DRAW_THUMBNAILS 14
#define SETTING_DRAW_RTF 15
#define SETTING_FIND_AS_TYPE 16
#define SETTING_ENSURE_WINDOW_IS_VISIBLE 17
#define SETTING_SHOW_GROUP_CLIPS_IN_LIST 18
#define SETTING_PROMPT_ON_DELETE 19
#define SETTING_ALWAYS_SHOW_SCROLL_BAR 20
#define SETTING_PASTE_AS_ADMIN 21
#define SETTTING_SHOW_IN_TASKBAR 22
#define SETTING_SHOW_CLIP_PASTED 23
#define SETTING_DIFF_APP 24
#define SETTING_TRANSPARENCY 25
#define SETTING_UPDATE_ORDER_ON_PASTE 26
#define SETTING_ALLOW_DUPLICATES 27
#define SETTING_REGEX_FILTERING_1 28
#define SETTING_REGEX_FILTERING_2 29
#define SETTING_REGEX_FILTERING_3 30
#define SETTING_REGEX_FILTERING_4 31
#define SETTING_REGEX_FILTERING_5 32
#define SETTING_REGEX_FILTERING_6 33
#define SETTING_REGEX_FILTERING_7 34
#define SETTING_REGEX_FILTERING_8 35
#define SETTING_REGEX_FILTERING_9 36
#define SETTING_REGEX_FILTERING_10 37
#define SETTING_REGEX_FILTERING_11 38
#define SETTING_REGEX_FILTERING_12 39
#define SETTING_REGEX_FILTERING_13 40
#define SETTING_REGEX_FILTERING_14 41
#define SETTING_REGEX_FILTERING_15 42
#define SETTING_REGEX_FILTERING_BY_PROCESS_NAME_1 43
#define SETTING_REGEX_FILTERING_BY_PROCESS_NAME_2 44
#define SETTING_REGEX_FILTERING_BY_PROCESS_NAME_3 45
#define SETTING_REGEX_FILTERING_BY_PROCESS_NAME_4 46
#define SETTING_REGEX_FILTERING_BY_PROCESS_NAME_5 47
#define SETTING_REGEX_FILTERING_BY_PROCESS_NAME_6 48
#define SETTING_REGEX_FILTERING_BY_PROCESS_NAME_7 49
#define SETTING_REGEX_FILTERING_BY_PROCESS_NAME_8 50
#define SETTING_REGEX_FILTERING_BY_PROCESS_NAME_9 51
#define SETTING_REGEX_FILTERING_BY_PROCESS_NAME_10 52
#define SETTING_REGEX_FILTERING_BY_PROCESS_NAME_11 53
#define SETTING_REGEX_FILTERING_BY_PROCESS_NAME_12 54
#define SETTING_REGEX_FILTERING_BY_PROCESS_NAME_13 55
#define SETTING_REGEX_FILTERING_BY_PROCESS_NAME_14 56
#define SETTING_REGEX_FILTERING_BY_PROCESS_NAME_15 57
#define SETTING_SHOW_STARTUP_MESSAGE 58
#define SETTING_TOOLTIP_TIMEOUT 59
#define SETTING_SELECTED_INDEX 60
#define SETTING_CLIPBOARD_SAVE_DELAY 61
#define SETTING_SHOW_MSG_WHEN_RECEIVING_MANUAL_SENT_CLIP 62
#define SETTING_MULTIPASTE_REVERSE_ORDER 63
#define SETTING_DEFAULT_PASTE_STRING 64
#define SETTING_DEFAULT_COPY_STRING 65
#define SETTING_DEFAULT_CUT_STRING 66
#define SETTING_REVERT_TO_TOP_LEVEL_GROUP 67
#define SETTING_UPDATE_ORDER_ON_CTRL_C 68
#define SETTING_TOOLTIP_LINES 69
#define SETTING_TOOLTIP_CHARACTERS 70
#define SETTING_ACTIVATE_WINDOW_DELAY 71
#define SETTING_DOUBLE_KEYSTROKE_TIMEOUT 72
#define SETTING_SEND_KEYS_DELAY 73
#define SETTING_FIRST_TEN_HOTKEYS_START 74
#define SETTING_FIRST_TEN_HOTKEYS_FONT_SIZE 75
#define SETTING_OPEN_TO_GROUP_AS_ACTIVE_EXE 76
#define SETTING_ADD_CF_HDROP_ON_DRAG 77
#define SETTING_COPY_SAVE_DELAY 78
#define SETTING_EDITOR_FONT_SIZE 79
#define SETTING_MOVE_SELECTION_ON_OPEN_HOTKEY 80
#define SETTING_ALOW_BACK_TO_BACK_DUPLICATES 81
#define SETTING_MAINTAIN_SEARCH_VIEW 82
#define SETTING_SEND_RECV_PORT 83
#define SETTING_DEBUG_TO_FILE 84
#define SETTING_DEBUG_TO_OUTPUT_STRING 85
#define SETTING_NETWORK_BIND_IP_ADDRESS 86
#define SETTING_DISABLE_FRIENDS 87
#define SETTING_IGNORE_FALSE_COPIES_DELAY 88
#define SETTING_REFRESH_VIEW_AFTER_PASTE 89
#define SETTING_SLUGIFY_SEPARATOR 90
#define SETTING_FAST_THUMBNAIL_MODE 91
#define SETTING_CLIPBOARD_RESTORE_AFTER_COPY_BUFFER_DELAY 92
#define SETTING_SUPPORT_ALL_TYPES 93
#define SETTING_IGNORE_ANNOYING_CF_DIB 94
#define SETTING_REGEX_CASE_INSENSITIVE 95
#define SETTING_DRAW_COPIED_COLOR_CODE 96
#define SETTING_CENTER_WINDOW_BELOW_CURSOR_CARET 97
#define SETTING_TEXT_EDITOR_PATH 98
#define SETTING_RTF_EDITOR_PATH 99
#define SETTING_UPDATE_DESC_ON_CLIP_EDIT 100
#define SETTING_QR_CODE_URL 101
#define SETTING_APPEND_NAME_IP 102
#define SETTING_USE_UTF8_FOR_DIFF 103
#define SETTING_IMAGE_EDITOR_PATH 104
#define SETTING_CLIP_EDIT_SAVE_DELAY_AFTER_LOAD 105
#define SETTING_ClIP_EDIT_SAVE_DELAY_AFTER_SAVE 106
#define SETTING_WEB_SEARCH_URL 107
#define SETTING_DO_NOT_HIDE_ON_DEACTIVATE 108
#define SETTING_HIDE_TASKBAR_ICON_ON_CLOSE 109
#define SETTING_USE_MODERN_SCROLLBAR 110
#define SETTING_ENFORCE_CLIPBOARD_IGNORE_FORMATS 111

CAdvGeneral::CAdvGeneral()
{
	m_mouseDownOnCaption = false;
}

void CAdvGeneral::AddTrueFalse(LPCTSTR szName, BOOL bValue, int nSettingId)
{
	static const CString szTrueFalse[] = { _T("True"), _T("False") };

	m_grid.AddChoice(0, szName, bValue ? _T("True") : _T("False"), szTrueFalse, 2, _T(""), nSettingId);
}

void CAdvGeneral::BuildGrid()
{
	static const TCHAR szDiffFilter[] = _T("Diff Applications(*.exe)|*.exe||");
	static const TCHAR szSoundFilter[] = _T("Sounds(*.wav)|*.wav||");
	static const TCHAR szAppFilter[] = _T("Applications(*.exe)|*.exe||");

	m_grid.AddCategory(_T("DittoWTL"));

	m_grid.AddInt(0, _T("Activate window delay (100ms default)"), (long)CGetSetOptions::SendKeysDelay(), _T(""), SETTING_ACTIVATE_WINDOW_DELAY);
	AddTrueFalse(_T("Add file drop when dragging clips"), CGetSetOptions::GetAddCFHDROP_OnDrag(), SETTING_ADD_CF_HDROP_ON_DRAG);
	AddTrueFalse(_T("Allow duplicates"), CGetSetOptions::GetAllowDuplicates(), SETTING_ALLOW_DUPLICATES);
	AddTrueFalse(_T("Allow back to back duplicates (if allowing duplicates)"), CGetSetOptions::GetAllowBackToBackDuplicates(), SETTING_ALOW_BACK_TO_BACK_DUPLICATES);
	AddTrueFalse(_T("Always show scroll bar"), CGetSetOptions::GetShowScrollBar(), SETTING_ALWAYS_SHOW_SCROLL_BAR);
	AddTrueFalse(_T("Use modern scroll bar"), CGetSetOptions::GetUseModernScrollBar(), SETTING_USE_MODERN_SCROLLBAR);
	AddTrueFalse(_T("Append Computer Name and IP when receiving clips"), CGetSetOptions::GetAppendRemoveComputerNameAndIPToDescription(), SETTING_APPEND_NAME_IP);
	m_grid.AddInt(0, _T("Amount of text to save for description"), CGetSetOptions::m_bDescTextSize, _T(""), SETTING_DESC_SIZE);
	AddTrueFalse(_T("Center window below cursor or caret"), CGetSetOptions::GetCenterWindowBelowCursorOrCaret(), SETTING_CENTER_WINDOW_BELOW_CURSOR_CARET);
	m_grid.AddInt(0, _T("Copy and save clipboard delay (ms)"), (long)CGetSetOptions::GetCopyAndSveDelay(), _T(""), SETTING_COPY_SAVE_DELAY);
	m_grid.AddInt(0, _T("Clip edit save delay after load"), (long)(CGetSetOptions::GetClipEditSaveDelayAfterLoadSeconds()), _T(""), SETTING_CLIP_EDIT_SAVE_DELAY_AFTER_LOAD);
	m_grid.AddInt(0, _T("Clip edit save delay after Save"), (long)(CGetSetOptions::GetClipEditSaveDelayAfterSaveSeconds()), _T(""), SETTING_ClIP_EDIT_SAVE_DELAY_AFTER_SAVE);
	m_grid.AddInt(0, _T("Clipboard restore delay after copy buffer sent paste (ms, default: 750)"), (long)(CGetSetOptions::GetDittoRestoreClipboardDelay()), _T(""), SETTING_CLIPBOARD_RESTORE_AFTER_COPY_BUFFER_DELAY);
	m_grid.AddText(0, _T("Default paste string"), CGetSetOptions::GetDefaultPasteString(), _T(""), SETTING_DEFAULT_PASTE_STRING);
	m_grid.AddText(0, _T("Default copy string"), CGetSetOptions::GetDefaultCopyString(), _T(""), SETTING_DEFAULT_COPY_STRING);
	m_grid.AddText(0, _T("Default cut string"), CGetSetOptions::GetDefaultCutString(), _T(""), SETTING_DEFAULT_CUT_STRING);
	m_grid.AddFile(0, _T("Diff application path"), CGetSetOptions::GetDiffApp(), szDiffFilter, _T(""), SETTING_DIFF_APP);
	AddTrueFalse(_T("Diff save compare files as utf8"), CGetSetOptions::GetPreferUtf8ForCompare(), SETTING_USE_UTF8_FOR_DIFF);
	AddTrueFalse(_T("Disable friends"), !CGetSetOptions::GetAllowFriends(), SETTING_DISABLE_FRIENDS);
	AddTrueFalse(_T("Display icon in system tray"), CGetSetOptions::GetShowIconInSysTray(), SETTING_SHOW_TASKBAR_ICON);
	AddTrueFalse(_T("Do not hide Ditto window on deactivate"), CGetSetOptions::GetDoNotHideOnDeactivate(), SETTING_DO_NOT_HIDE_ON_DEACTIVATE);
	m_grid.AddInt(0, _T("Double shortcut keystroke timeout)"), (long)CGetSetOptions::GetDoubleKeyStrokeTimeout(), _T(""), SETTING_DOUBLE_KEYSTROKE_TIMEOUT);
	AddTrueFalse(_T("Draw swatch for hex, RGB, and HSL colors"), CGetSetOptions::GetDrawCopiedColorCode(), SETTING_DRAW_COPIED_COLOR_CODE);
	AddTrueFalse(_T("Draw RTF text in list (for RTF types) (could increase memory usage an display speed)"), CGetSetOptions::GetDrawRTF(), SETTING_DRAW_RTF);
	m_grid.AddInt(0, _T("Editor default font size"), (long)CGetSetOptions::GetEditorDefaultFontSize(), _T(""), SETTING_EDITOR_FONT_SIZE);
	AddTrueFalse(_T("Enforce clipboard ignore formats"), CGetSetOptions::GetEnforceClipboardIgnoreFormats(), SETTING_ENFORCE_CLIPBOARD_IGNORE_FORMATS);
	AddTrueFalse(_T("Elevated privileges to paste into elevated apps"), CGetSetOptions::GetPasteAsAdmin(), SETTING_PASTE_AS_ADMIN);
	AddTrueFalse(_T("Ensure Ditto is always connected to the clipboard"), CGetSetOptions::GetEnsureConnectToClipboard(), SETTING_ENSURE_CONNECTED);
	AddTrueFalse(_T("Ensure entire window is visible"), CGetSetOptions::GetEnsureEntireWindowCanBeSeen(), SETTING_ENSURE_WINDOW_IS_VISIBLE);
	AddTrueFalse(_T("Fast thumbnails (True = fast / low quality (default). False = slow / high quality)"), CGetSetOptions::GetFastThumbnailMode(), SETTING_FAST_THUMBNAIL_MODE);
	AddTrueFalse(_T("Find as you type"), CGetSetOptions::GetFindAsYouType(), SETTING_FIND_AS_TYPE);
	m_grid.AddInt(0, _T("First ten hot keys start index"), (long)CGetSetOptions::GetFirstTenHotKeysStart(), _T(""), SETTING_FIRST_TEN_HOTKEYS_START);
	m_grid.AddInt(0, _T("First ten hot keys font size"), (long)CGetSetOptions::GetFirstTenHotKeysFontSize(), _T(""), SETTING_FIRST_TEN_HOTKEYS_FONT_SIZE);
	AddTrueFalse(_T("Hide Ditto on hot key if Ditto is visible"), CGetSetOptions::GetHideDittoOnHotKeyIfAlreadyShown(), SETTING_HIDE_ON_HOTKEY_IF_VISIBLE);
	m_grid.AddInt(0, _T("Ignore copies faster than (ms) (default: 500)"), (long)CGetSetOptions::GetSaveClipDelay(), _T(""), SETTING_IGNORE_FALSE_COPIES_DELAY);
	m_grid.AddText(0, _T("Ignore CF_DIB when a clip is detected as text content"), CGetSetOptions::GetIgnoreAnnoyingCFDIB(),
		_T("Case insensitive. Recommended option is \"excel.exe; onenote.exe; powerpnt.exe\" "), SETTING_IGNORE_ANNOYING_CF_DIB);
	m_grid.AddFile(0, _T("Image editor path (empty for system mapping)"), CGetSetOptions::GetImageEditorPath(), szAppFilter, _T(""), SETTING_IMAGE_EDITOR_PATH);
	m_grid.AddInt(0, _T("Maximum clip size in bytes (0 for no limit)"), CGetSetOptions::m_lMaxClipSizeInBytes, _T(""), SETTING_MAX_CLIP_SIZE);
	AddTrueFalse(_T("Maintain search view"), CGetSetOptions::GetMaintainSearchView(), SETTING_MAINTAIN_SEARCH_VIEW);
	AddTrueFalse(_T("Move selection on open hot key"), CGetSetOptions::GetMoveSelectionOnOpenHotkey(), SETTING_MOVE_SELECTION_ON_OPEN_HOTKEY);
	m_grid.AddText(0, _T("Multi-paste clip separator ([LF] = line feed)"), CGetSetOptions::GetMultiPasteSeparator(false), _T(""), SETTING_CLIP_SEPARATOR);
	AddTrueFalse(_T("Multi-paste in reverse order"), CGetSetOptions::m_bMultiPasteReverse, SETTING_MULTIPASTE_REVERSE_ORDER);
	m_grid.AddInt(0, _T("Network send receive port (default: 23443)"), (long)CGetSetOptions::GetPort(), _T(""), SETTING_SEND_RECV_PORT);
	m_grid.AddText(0, _T("Network server bind IP (default: *)"), CGetSetOptions::GetNetworkBindIPAddress(), _T(""), SETTING_NETWORK_BIND_IP_ADDRESS);
	AddTrueFalse(_T("Open to group same as active exe"), CGetSetOptions::GetOpenToGroupByActiveExe(), SETTING_OPEN_TO_GROUP_AS_ACTIVE_EXE);
	m_grid.AddText(0, _T("QRCode Url"), CGetSetOptions::GetQRCodeUrl(), _T(""), SETTING_QR_CODE_URL);
	m_grid.AddFile(0, _T("On copy play the sound"), CGetSetOptions::GetPlaySoundOnCopy(), szSoundFilter, _T(""), SETTING_COPY_PLAY_SOUND);
	m_grid.AddFile(0, _T("Text editor path (empty for system mapping)"), CGetSetOptions::GetTextEditorPath(), szAppFilter, _T(""), SETTING_TEXT_EDITOR_PATH);
	AddTrueFalse(_T("Paste clip in active window after selection"), CGetSetOptions::GetSendPasteAfterSelection(), SETTING_PASTE_IN_ACTIVE_WINDOW);
	AddTrueFalse(_T("Prompt when deleting clips"), CGetSetOptions::GetPromptWhenDeletingClips(), SETTING_PROMPT_ON_DELETE);
	AddTrueFalse(_T("Revert to top level group on close"), CGetSetOptions::GetRevertToTopLevelGroup(), SETTING_REVERT_TO_TOP_LEVEL_GROUP);
	AddTrueFalse(_T("Refresh view after paste"), CGetSetOptions::GetRefreshViewAfterPasting(), SETTING_REFRESH_VIEW_AFTER_PASTE);
	AddTrueFalse(_T("Regex case insensitive search"), CGetSetOptions::GetRegexCaseInsensitive(), SETTING_REGEX_CASE_INSENSITIVE);
	m_grid.AddFile(0, _T("RTF editor path"), CGetSetOptions::GetRTFEditorPath(), szAppFilter, _T(""), SETTING_RTF_EDITOR_PATH);
	m_grid.AddInt(0, _T("Save clipboard delay (ms, default: 100)"), (long)(CGetSetOptions::GetProcessDrawClipboardDelay()), _T(""), SETTING_CLIPBOARD_SAVE_DELAY);
	AddTrueFalse(_T("Save multi-pastes"), CGetSetOptions::GetSaveMultiPaste(), SETTING_SAVE_MULTI_PASTE);
	m_grid.AddInt(0, _T("Selected index"), (long)(CGetSetOptions::SelectedIndex() + 1), _T(""), SETTING_SELECTED_INDEX);
	m_grid.AddInt(0, _T("Send keys delay (ms)"), (long)CGetSetOptions::RealSendKeysDelay(), _T(""), SETTING_SEND_KEYS_DELAY);
	AddTrueFalse(_T("Show clips that are in groups in main list"), CGetSetOptions::GetShowAllClipsInMainList(), SETTING_SHOW_GROUP_CLIPS_IN_LIST);
	AddTrueFalse(_T("Show leading whitespace"), CGetSetOptions::GetDescShowLeadingWhiteSpace(), SETTING_SHOW_LEADING_WHITESPACE);
	AddTrueFalse(_T("Show in taskbar"), CGetSetOptions::GetShowInTaskBar(), SETTTING_SHOW_IN_TASKBAR);
	AddTrueFalse(_T("Hide taskbar icon when Ditto window closes"), CGetSetOptions::GetHideTaskbarIconOnClose(), SETTING_HIDE_TASKBAR_ICON_ON_CLOSE);
	AddTrueFalse(_T("Show indicator a clip has been pasted"), CGetSetOptions::GetShowIfClipWasPasted(), SETTING_SHOW_CLIP_PASTED);
	AddTrueFalse(_T("Show message that we received a manual sent clip"), CGetSetOptions::GetShowMsgWhenReceivingManualSentClip(), SETTING_SHOW_MSG_WHEN_RECEIVING_MANUAL_SENT_CLIP);
	AddTrueFalse(_T("Show startup tooltip message"), CGetSetOptions::GetShowStartupMessage(), SETTING_SHOW_STARTUP_MESSAGE);
	AddTrueFalse(_T("Show text for first ten copy hot keys"), CGetSetOptions::GetShowTextForFirstTenHotKeys(), SETTING_TEXT_FIRST_TEN);
	AddTrueFalse(_T("Show thumbnails(for CF_DIB and PNG types) (could increase memory usage and display speed)"), CGetSetOptions::GetDrawThumbnail(), SETTING_DRAW_THUMBNAILS);
	m_grid.AddText(0, _T("Slugify Separator (default: -)"), CGetSetOptions::GetSlugifySeparator(), _T(""), SETTING_SLUGIFY_SEPARATOR);
	AddTrueFalse(_T("Support all types ignoring supported type list (default: false))"), CGetSetOptions::GetSupportAllTypes(), SETTING_SUPPORT_ALL_TYPES);
	m_grid.AddInt(0, _T("Text lines per clip"), CGetSetOptions::GetLinesPerRow(), _T(""), SETTING_LINES_PER_ROW);
	m_grid.AddInt(0, _T("Tooltip display time(ms) max of 32000 (-1 default (5 seconds), 0 to turn off)"), CGetSetOptions::m_tooltipTimeout, _T(""), SETTING_TOOLTIP_TIMEOUT);
	m_grid.AddInt(0, _T("Tooltip maximum display lines"), (long)CGetSetOptions::GetMaxToolTipLines(), _T(""), SETTING_TOOLTIP_LINES);
	m_grid.AddInt(0, _T("Tooltip display characters"), (long)CGetSetOptions::GetMaxToolTipCharacters(), _T(""), SETTING_TOOLTIP_CHARACTERS);
	AddTrueFalse(_T("Transparency enabled"), CGetSetOptions::GetEnableTransparency(), SETTING_ENABLE_TRANSPARENCY);
	m_grid.AddInt(0, _T("Transparency percentage"), CGetSetOptions::GetTransparencyPercent(), _T(""), SETTING_TRANSPARENCY);
	AddTrueFalse(_T("Update description on clip edit"), CGetSetOptions::GetUpdateDescWhenSavingClip(), SETTING_UPDATE_DESC_ON_CLIP_EDIT);
	AddTrueFalse(_T("Update clip order on paste"), CGetSetOptions::GetUpdateTimeOnPaste(), SETTING_UPDATE_ORDER_ON_PASTE);
	AddTrueFalse(_T("Update clip Order on ctrl-c"), CGetSetOptions::GetUpdateClipOrderOnCtrlC(), SETTING_UPDATE_ORDER_ON_CTRL_C);
	m_grid.AddText(0, _T("Web Search Url"), CGetSetOptions::GetWebSearchUrl(), _T(""), SETTING_WEB_SEARCH_URL);
	AddTrueFalse(_T("Write debug to file"), CGetSetOptions::GetEnableDebugLogging(), SETTING_DEBUG_TO_FILE);
	AddTrueFalse(_T("Write debug to OutputDebugString"), CGetSetOptions::GetEnableOutputDebugStringLogging(), SETTING_DEBUG_TO_OUTPUT_STRING);

	m_grid.AddCategory(_T("Exclude clips by Regular Expressions"), FALSE);

	CString processFilterDesc = _T("Process making the copy first must match this before the Regex will be applied (empty or * for all processes) (separate multiples by ;)");
	CString regexFilterDesc = _T("If copied text matches this regular expression then the clip will not be saved to Ditto");

	for (int i = 0; i < 15; i++)
	{
		CString szName;

		szName.Format(_T("%d Regex"), i + 1);
		m_grid.AddText(0, szName, CGetSetOptions::GetRegexFilter(i), regexFilterDesc, SETTING_REGEX_FILTERING_1 + i);

		szName.Format(_T("%d Process Name"), i + 1);
		m_grid.AddText(0, szName, CGetSetOptions::GetRegexFilterByProcessName(i), processFilterDesc, SETTING_REGEX_FILTERING_BY_PROCESS_NAME_1 + i);
	}
}

LRESULT CAdvGeneral::OnInitDialog(UINT, WPARAM, LPARAM, BOOL&)
{
	HICON b = (HICON)LoadImage(theApp.GetModuleInstance(), MAKEINTRESOURCE(IDR_MAINFRAME), IMAGE_ICON, 64, 64, LR_SHARED);
	SetIcon(b, TRUE);

	// The template only carries a placeholder to give the grid its position and size.
	HWND hWndHost = GetDlgItem(IDC_ADV_GRID);
	CRect rc;
	::GetClientRect(hWndHost, &rc);
	::DestroyWindow(hWndHost);

	m_grid.Create(m_hWnd, rc, NULL, WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | WS_BORDER | WS_TABSTOP | WS_VSCROLL, 0, IDC_ADV_GRID);
	m_grid.SetGridFont((HFONT)SendMessage(WM_GETFONT));

	// Enter in the filter box fires the next-match button, matching upstream's PreTranslateMessage.
	m_editFilter.SubclassWindow(GetDlgItem(IDC_EDIT_ADV_FILTER));

	BuildGrid();

	m_Resize.SetParent(m_hWnd);
	m_Resize.AddControl(m_grid.m_hWnd, DR_SizeWidth | DR_SizeHeight);
	m_Resize.AddControl(IDOK, DR_MoveTop | DR_MoveLeft);
	m_Resize.AddControl(IDCANCEL, DR_MoveTop | DR_MoveLeft);
	m_Resize.AddControl(IDC_BT_COMPACT_AND_REPAIR, DR_MoveTop);
	m_Resize.AddControl(IDC_BUTTON_COPY_SCRIPTS, DR_MoveTop);
	m_Resize.AddControl(IDC_BUTTON_PASTE_SCRIPTS, DR_MoveTop);
	m_Resize.AddControl(IDC_EDIT_ADV_FILTER, DR_SizeWidth);
	m_Resize.AddControl(IDC_BUTTON_NEXT_MATCH, DR_MoveLeft);

	return TRUE;
}

void CAdvGeneral::ApplySettings()
{
	m_grid.EndEdit(TRUE);

	const std::vector<CPropGrid::Row>& rows = m_grid.Rows();
	for (size_t i = 0; i < rows.size(); i++)
	{
		const CPropGrid::Row& row = rows[i];
		if (row.bCategory)
			continue;

		CString szNew = row.value;
		CString szOrig = row.original;
		long nNew = (long)_ttol(szNew);
		long nOrig = (long)_ttol(szOrig);

		// Same check MFC made on the variant: numbers compare by value, everything else by text.
		if (row.kind == CPropGrid::KindInt ? nNew == nOrig : szNew == szOrig)
			continue;

		BOOL bVal = szNew == _T("True") ? TRUE : FALSE;

		switch (row.dataId)
		{
		case SETTING_DESC_SIZE:
			CGetSetOptions::SetDescTextSize(nNew);
			break;
		case SETTING_SELECTED_INDEX:
			CGetSetOptions::SetSelectedIndex(max((nNew - 1), 0));
			break;
		case SETTING_CLIPBOARD_SAVE_DELAY:
			CGetSetOptions::SetProcessDrawClipboardDelay(max(nNew, 0));
			break;
		case SETTING_SHOW_TASKBAR_ICON:
			CGetSetOptions::SetShowIconInSysTray(bVal);
			break;
		case SETTING_SAVE_MULTI_PASTE:
			CGetSetOptions::SetSaveMultiPaste(bVal);
			break;
		case SETTING_HIDE_ON_HOTKEY_IF_VISIBLE:
			CGetSetOptions::SetHideDittoOnHotKeyIfAlreadyShown(bVal);
			break;
		case SETTING_PASTE_IN_ACTIVE_WINDOW:
			CGetSetOptions::SetSendPasteAfterSelection(bVal);
			break;
		case SETTING_MAX_CLIP_SIZE:
			CGetSetOptions::SetMaxClipSizeInBytes(nNew);
			break;
		case SETTING_CLIP_SEPARATOR:
			CGetSetOptions::SetMultiPasteSeparator(szNew);
			break;
		case SETTING_ENSURE_CONNECTED:
			CGetSetOptions::SetEnsureConnectToClipboard(bVal);
			break;
		case SETTING_COPY_PLAY_SOUND:
			CGetSetOptions::SetPlaySoundOnCopy(szNew);
			break;
		case SETTING_TEXT_FIRST_TEN:
			CGetSetOptions::SetShowTextForFirstTenHotKeys(bVal);
			break;
		case SETTING_SHOW_LEADING_WHITESPACE:
			CGetSetOptions::SetDescShowLeadingWhiteSpace(bVal);
			break;
		case SETTING_LINES_PER_ROW:
			CGetSetOptions::SetLinesPerRow(nNew);
			break;
		case SETTING_ENABLE_TRANSPARENCY:
			CGetSetOptions::SetEnableTransparency(bVal);
			break;
		case SETTING_TRANSPARENCY:
			{
				int value = 100;
				if (nNew <= 100 && nNew > 0)
					value = (int)nNew;

				CGetSetOptions::SetTransparencyPercent(value);
			}
			break;
		case SETTING_DRAW_THUMBNAILS:
			CGetSetOptions::SetDrawThumbnail(bVal);
			break;
		case SETTING_FAST_THUMBNAIL_MODE:
			CGetSetOptions::SetFastThumbnailMode(bVal);
			break;
		case SETTING_DRAW_RTF:
			CGetSetOptions::SetDrawRTF(bVal);
			break;
		case SETTING_FIND_AS_TYPE:
			CGetSetOptions::SetFindAsYouType(bVal);
			break;
		case SETTING_ENSURE_WINDOW_IS_VISIBLE:
			CGetSetOptions::SetEnsureEntireWindowCanBeSeen(bVal);
			break;
		case SETTING_SHOW_GROUP_CLIPS_IN_LIST:
			CGetSetOptions::SetShowAllClipsInMainList(bVal);
			break;
		case SETTING_PROMPT_ON_DELETE:
			CGetSetOptions::SetPromptWhenDeletingClips(bVal);
			break;
		case SETTING_ALWAYS_SHOW_SCROLL_BAR:
			CGetSetOptions::SetShowScrollBar(bVal);
			break;
		case SETTING_PASTE_AS_ADMIN:
			CGetSetOptions::SetPasteAsAdmin(bVal);
			break;
		case SETTTING_SHOW_IN_TASKBAR:
			CGetSetOptions::SetShowInTaskBar(bVal);
			break;
		case SETTING_SHOW_CLIP_PASTED:
			CGetSetOptions::SetShowIfClipWasPasted(bVal);
			break;
		case SETTING_SHOW_MSG_WHEN_RECEIVING_MANUAL_SENT_CLIP:
			CGetSetOptions::SetShowMsgWhenReceivingManualSentClip(bVal);
			break;
		case SETTING_DIFF_APP:
			CGetSetOptions::SetDiffApp(szNew);
			break;
		case SETTING_UPDATE_ORDER_ON_PASTE:
			CGetSetOptions::SetUpdateTimeOnPaste(bVal);
			break;
		case SETTING_UPDATE_ORDER_ON_CTRL_C:
			CGetSetOptions::SetUpdateClipOrderOnCtrlC(bVal);
			break;
		case SETTING_MULTIPASTE_REVERSE_ORDER:
			CGetSetOptions::SetMultiPasteReverse(bVal);
			break;
		case SETTING_ALLOW_DUPLICATES:
			CGetSetOptions::SetAllowDuplicates(bVal);
			break;
		case SETTING_ALOW_BACK_TO_BACK_DUPLICATES:
			CGetSetOptions::SetAllowBackToBackDuplicates(bVal);
			break;
		case SETTING_REGEX_FILTERING_1:
		case SETTING_REGEX_FILTERING_2:
		case SETTING_REGEX_FILTERING_3:
		case SETTING_REGEX_FILTERING_4:
		case SETTING_REGEX_FILTERING_5:
		case SETTING_REGEX_FILTERING_6:
		case SETTING_REGEX_FILTERING_7:
		case SETTING_REGEX_FILTERING_8:
		case SETTING_REGEX_FILTERING_9:
		case SETTING_REGEX_FILTERING_10:
		case SETTING_REGEX_FILTERING_11:
		case SETTING_REGEX_FILTERING_12:
		case SETTING_REGEX_FILTERING_13:
		case SETTING_REGEX_FILTERING_14:
		case SETTING_REGEX_FILTERING_15:
			CGetSetOptions::SetRegexFilter(szNew, row.dataId - SETTING_REGEX_FILTERING_1);
			break;
		case SETTING_REGEX_FILTERING_BY_PROCESS_NAME_1:
		case SETTING_REGEX_FILTERING_BY_PROCESS_NAME_2:
		case SETTING_REGEX_FILTERING_BY_PROCESS_NAME_3:
		case SETTING_REGEX_FILTERING_BY_PROCESS_NAME_4:
		case SETTING_REGEX_FILTERING_BY_PROCESS_NAME_5:
		case SETTING_REGEX_FILTERING_BY_PROCESS_NAME_6:
		case SETTING_REGEX_FILTERING_BY_PROCESS_NAME_7:
		case SETTING_REGEX_FILTERING_BY_PROCESS_NAME_8:
		case SETTING_REGEX_FILTERING_BY_PROCESS_NAME_9:
		case SETTING_REGEX_FILTERING_BY_PROCESS_NAME_10:
		case SETTING_REGEX_FILTERING_BY_PROCESS_NAME_11:
		case SETTING_REGEX_FILTERING_BY_PROCESS_NAME_12:
		case SETTING_REGEX_FILTERING_BY_PROCESS_NAME_13:
		case SETTING_REGEX_FILTERING_BY_PROCESS_NAME_14:
		case SETTING_REGEX_FILTERING_BY_PROCESS_NAME_15:
			CGetSetOptions::SetRegexFilterByProcessName(szNew, row.dataId - SETTING_REGEX_FILTERING_BY_PROCESS_NAME_1);
			break;
		case SETTING_SHOW_STARTUP_MESSAGE:
			CGetSetOptions::SetShowStartupMessage(bVal);
			break;
		case SETTING_TOOLTIP_TIMEOUT:
			CGetSetOptions::SetToolTipTimeout(nNew);
			break;
		case SETTING_TOOLTIP_LINES:
			CGetSetOptions::SetMaxToolTipLines(nNew);
			break;
		case SETTING_TOOLTIP_CHARACTERS:
			CGetSetOptions::SetMaxToolTipCharacters(nNew);
			break;
		case SETTING_DEFAULT_PASTE_STRING:
			CGetSetOptions::SetDefaultPasteString(szNew);
			break;
		case SETTING_DEFAULT_COPY_STRING:
			CGetSetOptions::SetDefaultCopyString(szNew);
			break;
		case SETTING_DEFAULT_CUT_STRING:
			CGetSetOptions::SetDefaultCutString(szNew);
			break;
		case SETTING_SLUGIFY_SEPARATOR:
			CGetSetOptions::SetSlugifySeparator(szNew);
			break;
		case SETTING_REVERT_TO_TOP_LEVEL_GROUP:
			CGetSetOptions::SetRevertToTopLevelGroup(bVal);
			break;
		case SETTING_ACTIVATE_WINDOW_DELAY:
			CGetSetOptions::SetSendKeysDelay(nNew);
			break;
		case SETTING_SEND_KEYS_DELAY:
			CGetSetOptions::SetRealSendKeysDelay(nNew);
			break;
		case SETTING_CLIPBOARD_RESTORE_AFTER_COPY_BUFFER_DELAY:
			CGetSetOptions::SetDittoRestoreClipboardDelay(nNew);
			break;
		case SETTING_DOUBLE_KEYSTROKE_TIMEOUT:
			CGetSetOptions::SetDoubleKeyStrokeTimeout(nNew);
			break;
		case SETTING_FIRST_TEN_HOTKEYS_START:
			CGetSetOptions::SetFirstTenHotKeysStart(nNew);
			break;
		case SETTING_FIRST_TEN_HOTKEYS_FONT_SIZE:
			CGetSetOptions::SetFirstTenHotKeysFontSize(nNew);
			break;
		case SETTING_OPEN_TO_GROUP_AS_ACTIVE_EXE:
			CGetSetOptions::SetOpenToGroupByActiveExe(bVal);
			break;
		case SETTING_ADD_CF_HDROP_ON_DRAG:
			CGetSetOptions::SetAddCFHDROP_OnDrag(bVal);
			break;
		case SETTING_COPY_SAVE_DELAY:
			CGetSetOptions::SetCopyAndSveDelay(nNew);
			break;
		case SETTING_EDITOR_FONT_SIZE:
			CGetSetOptions::SetEditorDefaultFontSize(nNew);
			break;
		case SETTING_MOVE_SELECTION_ON_OPEN_HOTKEY:
			CGetSetOptions::SetMoveSelectionOnOpenHotkey(bVal);
			break;
		case SETTING_MAINTAIN_SEARCH_VIEW:
			CGetSetOptions::SetMaintainSearchView(bVal);
			break;
		case SETTING_SEND_RECV_PORT:
			CGetSetOptions::SetPort(nNew);
			break;
		case SETTING_DEBUG_TO_FILE:
			CGetSetOptions::SetEnableDebugLogging(bVal);
			break;
		case SETTING_DEBUG_TO_OUTPUT_STRING:
			CGetSetOptions::SetEnableOutputDebugStringLogging(bVal);
			break;
		case SETTING_NETWORK_BIND_IP_ADDRESS:
			CGetSetOptions::SetNetworkBindIPAddress(szNew);
			break;
		case SETTING_DISABLE_FRIENDS:
			CGetSetOptions::SetAllowFriends(!bVal);
			break;
		case SETTING_IGNORE_FALSE_COPIES_DELAY:
			CGetSetOptions::SetSaveClipDelay(nNew);
			break;
		case SETTING_REFRESH_VIEW_AFTER_PASTE:
			CGetSetOptions::SetRefreshViewAfterPasting(bVal);
			break;
		case SETTING_SUPPORT_ALL_TYPES:
			CGetSetOptions::SetSupportAllTypes(bVal);
			break;
		case SETTING_IGNORE_ANNOYING_CF_DIB:
			CGetSetOptions::SetIgnoreAnnoyingCFDIB(szNew);
			break;
		case SETTING_REGEX_CASE_INSENSITIVE:
			CGetSetOptions::SetRegexCaseInsensitive(bVal);
			break;
		case SETTING_DRAW_COPIED_COLOR_CODE:
			CGetSetOptions::SetDrawCopiedColorCode(bVal);
			break;
		case SETTING_CENTER_WINDOW_BELOW_CURSOR_CARET:
			CGetSetOptions::SetCenterWindowBelowCursorOrCaret(bVal);
			break;
		case SETTING_TEXT_EDITOR_PATH:
			CGetSetOptions::SetTextEditorPath(szNew);
			break;
		case SETTING_RTF_EDITOR_PATH:
			CGetSetOptions::SetRTFEditorPath(szNew);
			break;
		case SETTING_UPDATE_DESC_ON_CLIP_EDIT:
			CGetSetOptions::SetUpdateDescWhenSavingClip(bVal);
			break;
		case SETTING_QR_CODE_URL:
			CGetSetOptions::SetQRCodeUrl(szNew);
			break;
		case SETTING_APPEND_NAME_IP:
			CGetSetOptions::SetAppendRemoveComputerNameAndIPToDescription(bVal);
			break;
		case SETTING_USE_UTF8_FOR_DIFF:
			CGetSetOptions::SetPreferUtf8ForCompare(bVal);
			break;
		case SETTING_IMAGE_EDITOR_PATH:
			CGetSetOptions::SetImageEditorPath(szNew);
			break;
		case SETTING_CLIP_EDIT_SAVE_DELAY_AFTER_LOAD:
			CGetSetOptions::SetClipEditSaveDelayAfterLoadSeconds(nNew);
			break;
		case SETTING_ClIP_EDIT_SAVE_DELAY_AFTER_SAVE:
			CGetSetOptions::SetClipEditSaveDelayAfterSaveSeconds(nNew);
			break;
		case SETTING_WEB_SEARCH_URL:
			CGetSetOptions::SetWebSearchUrl(szNew);
			break;
		case SETTING_DO_NOT_HIDE_ON_DEACTIVATE:
			CGetSetOptions::SetDoNotHideOnDeactivate(bVal);
			break;
		case SETTING_HIDE_TASKBAR_ICON_ON_CLOSE:
			CGetSetOptions::SetHideTaskbarIconOnClose(bVal);
			break;
		case SETTING_USE_MODERN_SCROLLBAR:
			CGetSetOptions::SetUseModernScrollBar(bVal);
			break;
		case SETTING_ENFORCE_CLIPBOARD_IGNORE_FORMATS:
			CGetSetOptions::SetEnforceClipboardIgnoreFormats(bVal);
			break;
		}
	}
}

LRESULT CAdvGeneral::OnOK(WORD, WORD, HWND, BOOL&)
{
	ApplySettings();

	EndDialog(IDOK);
	return 0;
}

LRESULT CAdvGeneral::OnCancel(WORD, WORD, HWND, BOOL&)
{
	m_grid.EndEdit(FALSE);

	EndDialog(IDCANCEL);
	return 0;
}

LRESULT CAdvGeneral::OnSize(UINT, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	// WM_SIZE arrives while the dialog is still being built, before the grid exists.
	if (m_grid.m_hWnd == NULL)
	{
		bHandled = FALSE;
		return 0;
	}

	int cx = LOWORD(lParam);
	int cy = HIWORD(lParam);

	if (((GetKeyState(VK_LBUTTON) & 0x100) != 0) &&
		m_mouseDownOnCaption == false)
	{
		m_Resize.MoveControls(CSize(cx, cy));
	}
	else
	{
		m_Resize.SetParent(m_hWnd);
	}

	return 0;
}

LRESULT CAdvGeneral::OnBnClickedBtCompactAndRepair(WORD, WORD, HWND, BOOL&)
{
	auto msg = theApp.m_Language.GetString("CompactRepairWarning", "Warning this can take quite a long time and require up to double the hard drive space as your current database size, Continue?");
	int ret = MessageBox(msg, _T("DittoWTL"), MB_YESNO);

	if (ret == IDYES)
	{
		CWaitCursor wait;

		try
		{
			try
			{
				for (int i = 0; i < 100; i++)
				{
					int toDeleteCount = theApp.m_db.execScalar(_T("SELECT COUNT(clipID) FROM MainDeletes"));
					if (toDeleteCount <= 0)
						break;

					RemoveOldEntries(false);
				}
			}
			CATCH_SQLITE_EXCEPTION

			theApp.m_db.execDML(_T("PRAGMA auto_vacuum = 1"));
			theApp.m_db.execQuery(_T("VACUUM"));
		}
		CATCH_SQLITE_EXCEPTION
	}

	return 0;
}

LRESULT CAdvGeneral::OnBnClickedButtonCopyScripts(WORD, WORD, HWND, BOOL&)
{
	CDimWnd dim(m_hWnd);

	CScriptEditor e;
	e.m_title = _T("Copy Scripts");
	e.m_xml.Load(CGetSetOptions::GetCopyScriptsXml());
	if (e.DoModal(m_hWnd) == IDOK)
	{
		CGetSetOptions::SetCopyScriptsXml(e.m_xml.Save());
	}

	return 0;
}

LRESULT CAdvGeneral::OnBnClickedButtonPasteScripts(WORD, WORD, HWND, BOOL&)
{
	CDimWnd dim(m_hWnd);

	CScriptEditor e;
	e.m_title = _T("Paste Scripts");
	e.m_xml.Load(CGetSetOptions::GetPasteScriptsXml());
	if (e.DoModal(m_hWnd) == IDOK)
	{
		CGetSetOptions::SetPasteScriptsXml(e.m_xml.Save());
	}

	return 0;
}

LRESULT CAdvGeneral::OnGetMinMaxInfo(UINT, WPARAM, LPARAM lParam, BOOL& bHandled)
{
	MINMAXINFO* lpMMI = (MINMAXINFO*)lParam;

	lpMMI->ptMinTrackSize.x = 450;
	lpMMI->ptMinTrackSize.y = 450;

	bHandled = FALSE;
	return 0;
}

LRESULT CAdvGeneral::OnNcLButtonDown(UINT, WPARAM wParam, LPARAM, BOOL& bHandled)
{
	UINT nHitTest = (UINT)wParam;

	m_mouseDownOnCaption = false;

	if (nHitTest == HTCAPTION)
	{
		m_mouseDownOnCaption = true;
	}

	// DefWindowProc has to run: it starts the caption-move and border-size loops.
	bHandled = FALSE;
	return 0;
}

LRESULT CAdvGeneral::OnEnChangeAdvFilter(WORD, WORD, HWND, BOOL&)
{
	Search(false);
	return 0;
}

// Same walk as upstream: from the top on typing, from the current selection on
// Enter/Next Match.  Our CPropGrid keeps the rows flat, so the MFC top-level
// property + sub-item loops collapse into one scan with wrap-around.
void CAdvGeneral::Search(bool fromSelection)
{
	CString filterText;
	m_editFilter.GetWindowText(filterText);
	filterText.MakeLower();

	if (filterText.IsEmpty())
	{
		m_grid.SelectFirst();
		return;
	}

	int nStartAfter = fromSelection ? m_grid.SelectedRow() : -1;
	m_grid.SelectNextMatch(filterText, nStartAfter);
}

LRESULT CAdvGeneral::OnBnClickedButtonNextMatch(WORD, WORD, HWND, BOOL&)
{
	Search(true);
	return 0;
}

// Upstream's "Reset Clip Order": renumber clipOrder / clipGroupOrder so the
// rows are dense, optionally limited to one group.
LRESULT CAdvGeneral::OnBnClickedButtonCopyScripts2(WORD, WORD, HWND, BOOL&)
{
	CDimWnd dim(m_hWnd);

	CMoveToGroupDlg dlg(_T("Select group to reset clip order"));

	if (dlg.DoModal(m_hWnd) == IDOK)
	{
		CWaitCursor wait;

		const int groupID = dlg.GetSelectedGroup();

		CString reOrderSql = R"(

			WITH OrderedRows AS(
				SELECT
					rowid AS original_rowid,
					ROW_NUMBER() OVER(ORDER BY {orderField} ASC) AS rn
				FROM
					Main
				WHERE lParentID = {parentID}
			)
			--Update the main table using the CTE results
			UPDATE 
				Main
			SET {orderField} = (
					SELECT rn
					FROM OrderedRows
					WHERE OrderedRows.original_rowid = Main.rowid
				)
			WHERE lParentID = {parentID}
		)";

		if (groupID == -1)
		{
			reOrderSql.Replace(_T("{orderField}"), _T("clipOrder"));

			//reorder all clip
			reOrderSql.Replace(_T("WHERE lParentID = {parentID}"), _T(""));
		}
		else
		{
			reOrderSql.Replace(_T("{parentID}"), std::to_wstring(groupID).c_str());
			reOrderSql.Replace(_T("{orderField}"), _T("clipGroupOrder"));
		}

		try
		{
			theApp.m_db.execDML(reOrderSql);
		}
		catch (CppSQLite3Exception& e)
		{
			MessageBox(e.errorMessage(), _T("DittoWTL"), MB_OK);
		}
	}

	return 0;
}
