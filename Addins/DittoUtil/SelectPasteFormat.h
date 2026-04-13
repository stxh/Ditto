#pragma once
#include <windows.h>
#include <atlwin.h>
#include <atldlgs.h>
#include <atlctrls.h>
#include "resource.h"
#include "Exports.h"
#include "DialogResizer.h"

// CSelectPasteFormat dialog

class CSelectPasteFormat : public CDialogImpl<CSelectPasteFormat>
{
public:
    CSelectPasteFormat(HWND hParent, IClipFormats* clipFormats);
    virtual ~CSelectPasteFormat();

	CLIPFORMAT SelectedFormat() const { return m_selectedFormat; }
	void SelectedFormat(CLIPFORMAT val) { m_selectedFormat = val; }
	bool PasteAsUnicode() { return m_pasteAsUnicode; }

// Dialog Data
	enum { IDD = IDD_DIALOG_SELECT_FORMAT };

protected:
	IClipFormats *m_pClipFormats;
	CLIPFORMAT m_selectedFormat;
	CDialogResizer m_Resize;
	bool m_pasteAsUnicode;
	
    BEGIN_MSG_MAP(CSelectPasteFormat)
        MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
        COMMAND_ID_HANDLER(IDOK, OnOK)
        COMMAND_ID_HANDLER(IDCANCEL, OnCancel)
        MESSAGE_HANDLER(WM_SIZE, OnSize)
        COMMAND_HANDLER(IDC_LIST1, LBN_DBLCLK, OnLbnDblclkList1)
    END_MSG_MAP()

private:
    CListBox m_Formats;
    HWND m_hParent;

    LRESULT OnInitDialog(UINT, WPARAM, LPARAM, BOOL& bHandled);
    LRESULT OnOK(WORD, WORD, HWND, BOOL&);
    LRESULT OnCancel(WORD, WORD, HWND, BOOL&);
    LRESULT OnSize(UINT, WPARAM, LPARAM, BOOL& bHandled);
    LRESULT OnLbnDblclkList1(WORD, WORD, HWND, BOOL&);
};
