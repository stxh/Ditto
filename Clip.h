// Clip.h: classes for manage clips
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PROCESSCOPY_H__185CBB6F_4B63_4397_8FF9_E18D777DA506__INCLUDED_)
#define AFX_PROCESSCOPY_H__185CBB6F_4B63_4397_8FF9_E18D777DA506__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include <memory>
//#include <memory>
#include "tinyxml\tinyxml.h"
#include "Shared\IClip.h"
#include "Misc.h"

class CClip;
class CCopyThread;

typedef CArray<CLIPFORMAT, CLIPFORMAT> CClipTypes;

/*----------------------------------------------------------------------------*\\\n\tCOleDataObjectEx\n\\*----------------------------------------------------------------------------*/\nclass COleDataObjectEx\n{\npublic:\n\t// creates global from IStream if necessary\n\tHGLOBAL GetGlobalData(CLIPFORMAT cfFormat, LPFORMATETC lpFormatEtc = NULL);\n\tstd::shared_ptr<CClipTypes> GetAvailableTypes();\n};

/*----------------------------------------------------------------------------*\
	CClipFormat - holds the data of one clip format.
\*----------------------------------------------------------------------------*/
class CClipFormat : public IClipFormat
{
public:
	CLIPFORMAT m_cfType;
    HGLOBAL m_hgData;
	bool m_autoDeleteData;
	int m_dataId;
	int m_parentId;

	CClipFormat(CLIPFORMAT cfType = 0, HGLOBAL hgData = 0, int parentId = -1);
	~CClipFormat();

	void Clear();
	virtual void Free();

	virtual CLIPFORMAT Type() { return m_cfType; }
	virtual HGLOBAL Data() { return m_hgData; }
	virtual void Type(CLIPFORMAT type) { m_cfType = type; }
	virtual void Data(HGLOBAL data) { m_hgData = data; }
	virtual void AutoDeleteData(bool autoDeleteData) { m_autoDeleteData = autoDeleteData; }
	virtual bool AutoDeleteData()	{ return m_autoDeleteData; }

	CStringA GetAsCStringA() {
		CStringA ret;

		if (m_hgData)
		{
			LPVOID data = GlobalLock(m_hgData);
			int size = (int)GlobalSize(m_hgData);
			if (data != NULL && size > 0)
			{
				ret = CStringA((char *)data, size-1);
			}

			GlobalUnlock(m_hgData);
		}

		return ret;
	}

	CString GetAsCString() {
		CString ret;
		
		if (m_hgData)
		{
			LPVOID data = GlobalLock(m_hgData);
			int size = (int)GlobalSize(m_hgData);
			if (data != NULL && size > 0)
			{
				ret = CString((wchar_t *)data, ((size / (sizeof(wchar_t))) - 1));
			}

			GlobalUnlock(m_hgData);
		}

		return ret;
	}
	
	Gdiplus::Bitmap *CreateGdiplusBitmap();
};

/*----------------------------------------------------------------------------*\\n\tCClipFormats - holds an array of CClipFormat\n\\*----------------------------------------------------------------------------*/\nclass CClipFormats : public IClipFormats\n{\npublic:\n\tstd::vector<CClipFormat> m_Formats;\n\n\t// returns a pointer to the CClipFormat in this array which matches the given type\n\t//  or NULL if that type doesn't exist in this array.\n\tCClipFormat* FindFormat(UINT cfType); \n\n\tvirtual int Size() { return (int)m_Formats.size(); }\n\tvirtual IClipFormat *GetAt(int nPos) { return &m_Formats[nPos]; }\n\tvirtual void DeleteAt(int nPos) { m_Formats.erase(m_Formats.begin() + nPos); }\n\tvirtual void DeleteAll() { m_Formats.clear(); }\n\tvirtual INT_PTR AddNew(CLIPFORMAT type, HGLOBAL data) {CClipFormat ft(type, data, -1); ft.m_autoDeleteData = false; m_Formats.push_back(ft); return m_Formats.size() - 1; }\n\tvirtual IClipFormat *FindFormatEx(CLIPFORMAT type)	{ return FindFormat((UINT)type); }\n\tvirtual bool RemoveFormat(CLIPFORMAT type);\n};


/*----------------------------------------------------------------------------*\
	CClip - holds multiple CClipFormats and clip statistics
	- provides static functions for manipulating a Clip as a single unit.
\*----------------------------------------------------------------------------*/
class CClip : public IClip
{
public:
	CClip();
	~CClip();
	const CClip& operator=(const CClip &clip);

	static DWORD m_LastAddedCRC;
	static int m_lastAddedID;

	int m_id;
	CClipFormats m_Formats;
	time_t m_Time;
	CString m_Desc;
	ULONG m_lTotalCopySize;
	int m_parentId;
	int m_dontAutoDelete;
	int m_shortCut;
	BOOL m_bIsGroup;
	DWORD m_CRC;
	CString m_csQuickPaste;
	int m_param1;
	double m_clipOrder;
	double m_clipGroupOrder;
	double m_stickyClipOrder;
	double m_stickyClipGroupOrder;
	BOOL m_globalShortCut;
	time_t m_lastPasteDate;
	int m_moveToGroupShortCut;
	BOOL m_globalMoveToGroupShortCut;
	CopyReasonEnum::CopyReason m_copyReason;

	virtual CString Description() { return m_Desc; }
	virtual void Description(CString csValue) { m_Desc = csValue; }
	virtual time_t PasteTime() { return m_Time; }
	virtual int ID() { return m_id; }
	virtual int Parent() { return m_parentId; }
	virtual void Parent(int nParent) { m_parentId = nParent; }
	virtual int DontAutoDelete() { return m_dontAutoDelete; }
	virtual void DontAutoDelete(int Dont) { m_dontAutoDelete = Dont; }
	virtual CString QuickPaste() { return m_csQuickPaste; }
	virtual void QuickPaste(CString csValue) { m_csQuickPaste = csValue; }

	virtual void SetSaveToDbSticky(AddToDbStickyEnum::AddToDbSticky option) { m_addToDbStickyEnum = option; }

	virtual IClipFormats *Clips() { return (IClipFormats*)&m_Formats; }

	void Clear();
	void EmptyFormats();
	bool AddFormat(CLIPFORMAT cfType, void* pData, UINT nLen, bool setDesc = false);
	int LoadFromClipboard(CClipTypes* pClipTypes, bool checkClipboardIgnore = true, CString activeApp = _T(""), CString activeAppTitle = _T(""));
	bool SetDescFromText(HGLOBAL hgData, bool unicode);
	bool SetDescFromType();
	bool AddToDB(bool bCheckForDuplicates = true);
	bool ModifyMainTable();
	bool ModifyDescription();	
	void MakeLatestOrder();
	void MakeLatestGroupOrder();
	void MakeLastOrder();
	void MakeLastGroupOrder();
	void MakeStickyTop(int parentId);
	void MakeStickyLast(int parentId);
	bool RemoveStickySetting(int parentId);
	BOOL LoadMainTable(int id);
	DWORD GenerateCRC();
	void MoveUp(int parentId);
	void MoveDown(int parentId);

	CStringW GetUnicodeTextFormat();
	CStringA GetCFTextTextFormat();
	CStringA GetRTFTextFormat();

	BOOL ContainsClipFormat(CLIPFORMAT clipFormat);

	BOOL WriteTextToFile(CString path, BOOL unicode, BOOL asci, BOOL rtf, BOOL forceUnicode = FALSE);
	BOOL WriteImageToFile(CString path);
	BOOL WriteTextToHtmlFile(CString path);

	BOOL SaveFormats(CString* unicode, CStringA* asci, CStringA* rtf, BOOL updateDescription);

	// Allocates a Global containing the requested Clip's Format Data
	static HGLOBAL LoadFormat(int id, UINT cfType);
	// Fills "formats" with the Data of all Formats in the db for the given Clip ID
	bool LoadFormats(int id, bool bOnlyLoad_CF_TEXT = false, bool includeRichTextForTextOnly = false);
	// Fills "types" with all Types in the db for the given Clip ID
	static void LoadTypes(int id, CClipTypes& types);

	static double GetNewOrder(int parentId, int clipId);
	double GetNewLastOrder(int parentId, int clipId);
	static double GetNewTopSticky(int parentId, int clipId);
	static double GetNewLastSticky(int parentId, int clipId);
	static double GetExistingTopStickyClipId(int parentId);
	static bool RemoveStickySetting(int clipId, int parentId);

	bool AddFileDataToData(CString &errorMessage);

	Gdiplus::Bitmap *CreateGdiplusBitmap();
	
protected:
	bool AddToMainTable();
	bool AddToDataTable();
	int FindDuplicate();

	AddToDbStickyEnum::AddToDbSticky m_addToDbStickyEnum;
};


/*----------------------------------------------------------------------------*\\\n\tCClipList\n\\*----------------------------------------------------------------------------*/\n\nclass CClipList\n{\npublic:\n\tstd::vector<CClip*> m_ClipList;\n\t\n\t~CClipList();\n\t// returns the number of clips actually saved\n\t// while this does empty the Format Data, it does not delete the Clips.\n\tint AddToDB( bool bLatestOrder = false);\n\n\tconst CClipList& operator=(const CClipList &cliplist);\n\t\n\tvoid AddTail(CClip* pClip) { m_ClipList.push_back(pClip); }\n\tCClip* RemoveHead() { if (m_ClipList.empty()) return nullptr; CClip* pClip = m_ClipList.front(); m_ClipList.erase(m_ClipList.begin()); return pClip; }\n\tBOOL IsEmpty() const { return m_ClipList.empty(); }\n\tINT_PTR GetCount() const { return m_ClipList.size(); }\n\tvoid RemoveAll() { m_ClipList.clear(); }\n\tCClip*& GetHead() { return m_ClipList.front(); }\n\tCClip*& GetTail() { return m_ClipList.back(); }\n};

#endif // !defined(AFX_PROCESSCOPY_H__185CBB6F_4B63_4397_8FF9_E18D777DA506__INCLUDED_)
