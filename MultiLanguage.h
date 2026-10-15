// MultiLanguage.h: interface for the CMultiLanguage class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MULTILANGUAGE_H__DA57BA64_C421_4368_9498_1EFCE49A5C52__INCLUDED_)
#define AFX_MULTILANGUAGE_H__DA57BA64_C421_4368_9498_1EFCE49A5C52__INCLUDED_

#include "tinyxml\Tinyxml.h"
#include "tinyxml\tinystr.h"
#include <vector>
#include <map>

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CLangItem
{
public:
	CLangItem()
	{
		m_nID = -1;
		m_csEnglishLang = "";
		m_csForeignLang = "";
		m_csID = "";
	}
	int m_nID;
	CString m_csID;
	CString m_csEnglishLang;
	CString m_csForeignLang;
};

typedef std::vector<CLangItem*> LANGUAGE_ARRAY;

typedef std::map<CString, CLangItem*> LANGUAGE_MAP;

class CMultiLanguage  
{
public:
	CMultiLanguage();
	virtual ~CMultiLanguage();

	bool LoadLanguageFile(CString csFile);

	bool UpdateRightClickMenu(HMENU hMenu);
	bool UpdateGroupsRightClickMenu(HMENU hMenu);
	bool UpdateTrayIconRightClickMenu(HMENU hMenu);

	bool UpdateClipProperties(HWND hWndParent);
	bool UpdateOptionGeneral(HWND hWndParent);
	bool UpdateOptionSupportedTypes(HWND hWndParent);
	bool UpdateOptionShortcuts(HWND hWndParent);
	bool UpdateOptionQuickPaste(HWND hWndParent);
	bool UpdateOptionQuickPasteKeyboard(HWND hWndParent);
	bool UpdateOptionFriends(HWND hWndParent);
	bool UpdateOptionFriendsDetail(HWND hWndParent);
	bool UpdateOptionStats(HWND hWndParent);
	bool UpdateOptionSupportedTypesAdd(HWND hWndParent);
	bool UpdateMoveToGroups(HWND hWndParent);
	bool UpdateOptionsSheet(HWND hWndParent);
	bool UpdateOptionCopyBuffers(HWND hWndParent);
	bool UpdateGlobalHotKeys(HWND hWndParent);
	bool UpdateDeleteClipData(HWND hWndParent);

	CString GetGlobalHotKeyString(CString csID, CString csDefault);
	CString GetDeleteClipDataString(CString csID, CString csDefault);
	CString GetQuickPasteKeyboardString(int id, CString csDefault);

	CString GetString(CString csID, CString csDefault);

	CString GetAuthor()	{ return m_csAuthor;		}
	long	GetVersion(){ return m_lFileVersion;	}
	CString GetNotes()	{ return m_csNotes;			}
	CString GetLangCode() { return m_csLangCode;	}

	void	SetOnlyGetHeader(bool bVal)	{ m_bOnlyGetHeader = true;	}
	static HMENU GetMenuPos(HMENU hMenu, const CString &csLookingForMenuText, int &nMenuPos, bool returnChildIfOne = FALSE);

	CString m_csLastError;

protected:
	LANGUAGE_ARRAY m_RightClickMenu;
	LANGUAGE_ARRAY m_GroupsRightClickMenu;
	LANGUAGE_ARRAY m_TrayIconRightClickMenu;
	LANGUAGE_ARRAY m_ClipProperties;
	LANGUAGE_ARRAY m_OptionsGeneral;
	LANGUAGE_ARRAY m_OptionsSupportedTypes;
	LANGUAGE_ARRAY m_OptionsShortcuts;
	LANGUAGE_ARRAY m_OptionsQuickPaste;
	LANGUAGE_ARRAY m_OptionsQuickPasteKeyboard;
	LANGUAGE_ARRAY m_OptionsFriends;
	LANGUAGE_ARRAY m_OptionsFriendsDetail;
	LANGUAGE_ARRAY m_OptionsStats;
	LANGUAGE_ARRAY m_OptionsSupportedTypesAdd;
	LANGUAGE_ARRAY m_MoveToGroups;
	LANGUAGE_ARRAY m_OptionsSheet;
	LANGUAGE_ARRAY m_OptionsCopyBuffers;
	LANGUAGE_ARRAY m_GlobalHotKeys;
	LANGUAGE_ARRAY m_DeleteClipData;

	LANGUAGE_MAP	m_StringMap;

	CString m_csAuthor;
	CString m_csNotes;
	long	m_lFileVersion;
	CString m_csLangCode;

	bool	m_bOnlyGetHeader;

protected:
	bool LoadSection(TiXmlNode &doc, LANGUAGE_ARRAY &Array, CString csSection);
	bool LoadStringTableSection(TiXmlNode &doc, LANGUAGE_MAP &Map, CString csSection);

	bool UpdateMenuToLanguage(HMENU hMenu, LANGUAGE_ARRAY &Array);
	bool UpdateWindowToLanguage(HWND hWndParent, LANGUAGE_ARRAY &Array);


	void ClearArrays();
	void ClearArray(LANGUAGE_ARRAY &Array);
	void ClearMap(LANGUAGE_MAP &Map);
};

#endif // !defined(AFX_MULTILANGUAGE_H__DA57BA64_C421_4368_9498_1EFCE49A5C52__INCLUDED_)
