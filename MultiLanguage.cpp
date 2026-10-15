// MultiLanguage.cpp: implementation of the CMultiLanguage class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "cp_main.h"
#include "MultiLanguage.h"
#include "shared/TextConvert.h"
#include "shared/TextConvert.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMultiLanguage::CMultiLanguage()
{
	m_csAuthor = "";
	m_csLangCode = "";
	m_lFileVersion = 0;
	m_bOnlyGetHeader = false;
}

CMultiLanguage::~CMultiLanguage()
{
	ClearArrays();
}

void CMultiLanguage::ClearArrays()
{
	m_csAuthor = "";
	m_csLangCode = "";
	m_lFileVersion = 0;
	m_bOnlyGetHeader = false;

	ClearArray(m_RightClickMenu);
	ClearArray(m_GroupsRightClickMenu);
	ClearArray(m_ClipProperties);
	ClearArray(m_OptionsGeneral);
	ClearArray(m_OptionsSupportedTypes);
	ClearArray(m_OptionsShortcuts);
	ClearArray(m_OptionsQuickPaste);
	ClearArray(m_OptionsQuickPasteKeyboard);
	ClearArray(m_OptionsFriends);
	ClearArray(m_OptionsFriendsDetail);
	ClearArray(m_OptionsStats);
	ClearArray(m_OptionsSupportedTypesAdd);
	ClearArray(m_MoveToGroups);
	ClearArray(m_TrayIconRightClickMenu);
	ClearArray(m_OptionsSheet);
	ClearArray(m_OptionsCopyBuffers);
	ClearArray(m_GlobalHotKeys);

	ClearMap(m_StringMap);
}

void CMultiLanguage::ClearArray(LANGUAGE_ARRAY &Array)
{
	for(CLangItem *plItem : Array)
	{
		delete plItem;
	}
	Array.clear();
}

void CMultiLanguage::ClearMap(LANGUAGE_MAP &Map)
{
	for(const auto& item : Map)
	{
		delete item.second;
	}

	Map.clear();
}

CString CMultiLanguage::GetString(CString csID, CString csDefault)
{
	auto it = m_StringMap.find(csID);
	if(it == m_StringMap.end())
	{
		return csDefault;
	}

	CLangItem *pItem = it->second;
	if(pItem->m_csForeignLang.GetLength() <= 0)
		return csDefault;

	return pItem->m_csForeignLang;
}

CString CMultiLanguage::GetGlobalHotKeyString(CString csID, CString csDefault)
{
	for (CLangItem *plItem : m_GlobalHotKeys)
	{
		if(plItem->m_csID == csID)
		{
			return plItem->m_csForeignLang;
		}
	}

	return csDefault;
}

CString CMultiLanguage::GetDeleteClipDataString(CString csID, CString csDefault)
{
	for (CLangItem *plItem : m_DeleteClipData)
	{
		if(plItem->m_csID == csID)
		{
			return plItem->m_csForeignLang;
		}
	}

	return csDefault;
}

CString CMultiLanguage::GetQuickPasteKeyboardString(int id, CString csDefault)
{
	for (CLangItem *plItem : m_OptionsQuickPasteKeyboard)
	{
		if (plItem->m_nID == id)
		{
			return plItem->m_csForeignLang;
		}
	}

	return csDefault;
}

bool CMultiLanguage::UpdateRightClickMenu(HMENU hMenu)
{
	return UpdateMenuToLanguage(hMenu, m_RightClickMenu);
}

bool CMultiLanguage::UpdateGroupsRightClickMenu(HMENU hMenu)
{
	return UpdateMenuToLanguage(hMenu, m_GroupsRightClickMenu);
}

bool CMultiLanguage::UpdateTrayIconRightClickMenu(HMENU hMenu)
{
	return UpdateMenuToLanguage(hMenu, m_TrayIconRightClickMenu);
}

bool CMultiLanguage::UpdateClipProperties(HWND hWndParent)
{
	return UpdateWindowToLanguage(hWndParent, m_ClipProperties);
}

bool CMultiLanguage::UpdateOptionGeneral(HWND hWndParent)
{
	return UpdateWindowToLanguage(hWndParent, m_OptionsGeneral);
}

bool CMultiLanguage::UpdateOptionSupportedTypes(HWND hWndParent)
{
	return UpdateWindowToLanguage(hWndParent, m_OptionsSupportedTypes);
}

bool CMultiLanguage::UpdateOptionShortcuts(HWND hWndParent)
{
	return UpdateWindowToLanguage(hWndParent, m_OptionsShortcuts);
}

bool CMultiLanguage::UpdateOptionQuickPaste(HWND hWndParent)
{
	return UpdateWindowToLanguage(hWndParent, m_OptionsQuickPaste);
}

bool CMultiLanguage::UpdateOptionQuickPasteKeyboard(HWND hWndParent)
{
	return UpdateWindowToLanguage(hWndParent, m_OptionsQuickPasteKeyboard);
}

bool CMultiLanguage::UpdateOptionFriends(HWND hWndParent)
{
	return UpdateWindowToLanguage(hWndParent, m_OptionsFriends);
}

bool CMultiLanguage::UpdateOptionFriendsDetail(HWND hWndParent)
{
	return UpdateWindowToLanguage(hWndParent, m_OptionsFriendsDetail);
}

bool CMultiLanguage::UpdateOptionStats(HWND hWndParent)
{
	return UpdateWindowToLanguage(hWndParent, m_OptionsStats);
}

bool CMultiLanguage::UpdateOptionSupportedTypesAdd(HWND hWndParent)
{
	return UpdateWindowToLanguage(hWndParent, m_OptionsSupportedTypesAdd);
}

bool CMultiLanguage::UpdateMoveToGroups(HWND hWndParent)
{
	return UpdateWindowToLanguage(hWndParent, m_MoveToGroups);
}

bool CMultiLanguage::UpdateOptionsSheet(HWND hWndParent)
{
	return UpdateWindowToLanguage(hWndParent, m_OptionsSheet);
}

bool CMultiLanguage::UpdateOptionCopyBuffers(HWND hWndParent)
{
	return UpdateWindowToLanguage(hWndParent, m_OptionsCopyBuffers);
}

bool CMultiLanguage::UpdateGlobalHotKeys(HWND hWndParent)
{
	return UpdateWindowToLanguage(hWndParent, m_GlobalHotKeys);
}

bool CMultiLanguage::UpdateDeleteClipData(HWND hWndParent)
{
	return UpdateWindowToLanguage(hWndParent, m_DeleteClipData);
}

bool CMultiLanguage::UpdateMenuToLanguage(HMENU hMenu, LANGUAGE_ARRAY &Array)
{
	CMenuHandle menu(hMenu);
	for(CLangItem *plItem : Array)
	{
		if(plItem->m_csForeignLang.GetLength() > 0)
		{
			if(plItem->m_nID > 0)
			{
				menu.ModifyMenu(plItem->m_nID, MF_BYCOMMAND, plItem->m_nID, plItem->m_csForeignLang);
			}
			else
			{
				//If an item doesn't have a menu id then its a group menu
				//just search for the text and update the text with the foreign text
				int nMenuPos;
				HMENU hNewMenu = GetMenuPos(hMenu, plItem->m_csEnglishLang, nMenuPos);
				if(hNewMenu)
				{
					CMenuHandle(hNewMenu).ModifyMenu(nMenuPos, MF_BYPOSITION, -1, plItem->m_csForeignLang);
				}
			}
		}
	}

	return true;
}

bool CMultiLanguage::UpdateWindowToLanguage(HWND hWndParent, LANGUAGE_ARRAY &Array)
{
	for(CLangItem *plItem : Array)
	{
		if(plItem->m_csForeignLang.GetLength() > 0)
		{
			if(plItem->m_nID > 0)
			{
				HWND hWndChild = ::GetDlgItem(hWndParent, plItem->m_nID);
				if(hWndChild)
				{
					::SetWindowText(hWndChild, plItem->m_csForeignLang);
				}
			}
			//If item id is -1 then set the title for the dialog
			else if(plItem->m_nID == -1)
			{
				::SetWindowText(hWndParent, plItem->m_csForeignLang);
			}
		}
	}

	return true;
}


HMENU CMultiLanguage::GetMenuPos(HMENU hMenu, const CString &csLookingForMenuText, int &nMenuPos, bool returnChildIfOne)
{
	CMenuHandle menu(hMenu);
	CMenuHandle subMenu;
	CString csMenuText;

	int nCount = menu.GetMenuItemCount();
	for(int i = 0; i < nCount; i++)
	{
		menu.GetMenuString(i, csMenuText, MF_BYPOSITION);

		if(csMenuText == csLookingForMenuText)
		{
			nMenuPos = i;
			if (returnChildIfOne)
			{
				subMenu = menu.GetSubMenu(i);
				if (!subMenu.IsNull())
				{
					return subMenu;
				}
			}
			return hMenu;
		}

		subMenu = menu.GetSubMenu(i);
		if(!subMenu.IsNull())
		{
			HMENU hMenuReturn = GetMenuPos(subMenu, csLookingForMenuText, nMenuPos, returnChildIfOne);
			if(hMenuReturn)
				return hMenuReturn;
		}
	}

	return NULL;
}

bool CMultiLanguage::LoadLanguageFile(CString csFile)
{
	m_csLastError = "";
	
	CString csPath = CGetSetOptions::GetPath(PATH_LANGUAGE);
	csPath += csFile;
	csPath += ".xml";

	ClearArrays();

	if(csFile.GetLength() <= 0)
	{
		m_csLastError = "Language file is blank";
		return false;
	}

	TiXmlDocument doc;
	if(!doc.LoadFile(csPath.GetBuffer()))
	{
		m_csLastError.Format(_T("Error loading file %s - reason = %s, Line: %d, column: %d"), csFile, CTextConvert::AnsiToUnicode(doc.ErrorDesc()), doc.ErrorRow(), doc.ErrorCol());
		Log(m_csLastError);
		return false;
	}

	TiXmlElement *ItemHeader = doc.FirstChildElement("Ditto_Language_File");
	if(!ItemHeader)
	{
		m_csLastError.Format(_T("Error finding the section Ditto_Language_File"));
		ASSERT(!m_csLastError);
		Log(m_csLastError);
		return false;
	}

	CString csVersion = ItemHeader->Attribute("Version");
	m_lFileVersion = ATOI(csVersion);
	m_csAuthor = ItemHeader->Attribute("Author");
	m_csNotes = ItemHeader->Attribute("Notes");
	m_csLangCode = ItemHeader->Attribute("LanguageCode");

	if(m_bOnlyGetHeader)
		return true;

	bool bRet = LoadSection(*ItemHeader, m_RightClickMenu, "Ditto_Right_Click_Menu");
	bRet = LoadSection(*ItemHeader, m_GroupsRightClickMenu, "Ditto_Groups_Right_Click_Menu");
	bRet = LoadSection(*ItemHeader, m_OptionsGeneral, "Ditto_Options_General");
	bRet = LoadSection(*ItemHeader, m_ClipProperties, "Ditto_Clip_Properties");
	bRet = LoadSection(*ItemHeader, m_OptionsSupportedTypes, "Ditto_Options_Supported_Types");
	bRet = LoadSection(*ItemHeader, m_OptionsShortcuts, "Ditto_Options_Shortcuts");
	bRet = LoadSection(*ItemHeader, m_OptionsQuickPaste, "Ditto_Options_Quick_Paste");
	bRet = LoadSection(*ItemHeader, m_OptionsQuickPasteKeyboard, "Ditto_Options_Quick_Paste_Keyboard");
	bRet = LoadSection(*ItemHeader, m_OptionsFriends, "Ditto_Options_Friends");
	bRet = LoadSection(*ItemHeader, m_OptionsFriendsDetail, "Ditto_Options_Friends_Detail");
	bRet = LoadSection(*ItemHeader, m_OptionsStats, "Ditto_Options_Stats");
	bRet = LoadSection(*ItemHeader, m_OptionsSupportedTypesAdd, "Ditto_Options_Supported_Types_Add");
	bRet = LoadSection(*ItemHeader, m_MoveToGroups, "Ditto_Move_To_Groups");
	bRet = LoadSection(*ItemHeader, m_OptionsSheet, "Ditto_Options_Sheet");
	bRet = LoadSection(*ItemHeader, m_TrayIconRightClickMenu, "Ditto_Tray_Icon_Menu");
	bRet = LoadSection(*ItemHeader, m_OptionsCopyBuffers, "Ditto_Options_CopyBuffers");
	bRet = LoadSection(*ItemHeader, m_GlobalHotKeys, "Ditto_GlobalHotKeys");
	bRet = LoadSection(*ItemHeader, m_DeleteClipData, "Ditto_DeleteClipData");
	
	bRet = LoadStringTableSection(*ItemHeader, m_StringMap, "Ditto_String_Table");

	return true;
}

bool CMultiLanguage::LoadSection(TiXmlNode &doc, LANGUAGE_ARRAY &Array, CString csSection)
{
	CStringA csSectionA = CTextConvert::UnicodeToAnsi(csSection);
	TiXmlNode *node = doc.FirstChild(csSectionA);
	if(!node)
	{
		m_csLastError.Format(_T("Error finding the section %s"), csSection);
		//ASSERT(!m_csLastError);
		Log(m_csLastError);
		return false;
	}

	TiXmlNode* ForeignNode;
	CString csID;
	CString csLineFeed("\n");
		
	TiXmlElement *ItemElement = node->FirstChildElement();

	//load all items for this section
	//they look like
	//<Item English_Text = "Use Ctrl - Num" ID= "32777"></Item>
	while(ItemElement)
	{
 		ForeignNode = ItemElement->FirstChild();
 		if(ForeignNode)
 		{
			CLangItem *plItem = new CLangItem;
			if(plItem)
			{
				plItem->m_csEnglishLang = ItemElement->Attribute("English_Text");
				csID = ItemElement->Attribute("ID");
				plItem->m_nID = ATOI(csID);
				if(plItem->m_nID == 0)
				{
					plItem->m_csID = csID;
				}
 				
				LPCSTR Value = ForeignNode->Value();
				plItem->m_csForeignLang = CTextConvert::Utf8ToUnicode(Value);

				//Replace the literal "\n" with line feeds
 				plItem->m_csForeignLang.Replace(_T("\\n"), csLineFeed);

				Array.push_back(plItem);
			}
 		}		

		ItemElement = ItemElement->NextSiblingElement();
	}
	
	return true;
}

bool CMultiLanguage::LoadStringTableSection(TiXmlNode &doc, LANGUAGE_MAP &Map, CString csSection)
{
	CStringA csSectionA = CTextConvert::UnicodeToAnsi(csSection);
	TiXmlNode *node = doc.FirstChild(csSectionA);
	if(!node)
	{
		CString cs;
		cs.Format(_T("Error finding the section %s"), csSection);
		ASSERT(!cs);
		Log(cs);
		return false;
	}

	CString csLineFeed("\n");
	TiXmlNode* ForeignNode;
		
	TiXmlElement *ItemElement = node->FirstChildElement();

	//load all items for this section
	//they look like
	//<Item English_Text = "Use Ctrl - Num" ID= "32777"></Item>
	while(ItemElement)
	{
		CLangItem *plItem = new CLangItem;

		plItem->m_csEnglishLang = ItemElement->Attribute("English_Text");
		plItem->m_csID = ItemElement->Attribute("ID");

		ForeignNode = ItemElement->FirstChild();
		if(ForeignNode) 
		{
			LPCSTR Value = ForeignNode->Value();
			plItem->m_csForeignLang = CTextConvert::Utf8ToUnicode(Value);

			//Replace the literal "\n" with line feeds
			plItem->m_csForeignLang.Replace(_T("\\n"), csLineFeed);
		}

		Map[plItem->m_csID] = plItem;

		ItemElement = ItemElement->NextSiblingElement();
	}

	return true;
}
