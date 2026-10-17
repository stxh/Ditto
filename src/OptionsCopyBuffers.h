#pragma once

// COptionsCopyBuffers dialog

class COptionsCopyBuffers : public CPropertyPageImpl<COptionsCopyBuffers>
{
public:
	COptionsCopyBuffers();
	~COptionsCopyBuffers();

	enum { IDD = IDD_OPTIONS_COPY_BUFFERS };

	BEGIN_MSG_MAP(COptionsCopyBuffers)
		MSG_WM_INITDIALOG(OnInitDialog)
		CHAIN_MSG_MAP(CPropertyPageImpl<COptionsCopyBuffers>)
	END_MSG_MAP()

	BOOL OnInitDialog(HWND hWndFocus, LPARAM lParam);
	int OnApply();

private:
	CString m_csTitle;

public:
	CHotKeyCtrl m_CopyBuffer1;
	CHotKeyCtrl m_PasteBuffer1;
	CHotKeyCtrl m_CutBuffer1;
	CHotKeyCtrl m_CopyBuffer2;
	CHotKeyCtrl m_PasteBuffer2;
	CHotKeyCtrl m_CutBuffer2;
	CHotKeyCtrl m_CopyBuffer3;
	CHotKeyCtrl m_PasteBuffer3;
	CHotKeyCtrl m_CutBuffer3;
	CHotKeyCtrl m_CopyBuffer4;
	CHotKeyCtrl m_PasteBuffer4;
	CHotKeyCtrl m_CutBuffer4;
	CHotKeyCtrl m_CopyBuffer5;
	CHotKeyCtrl m_PasteBuffer5;
	CHotKeyCtrl m_CutBuffer5;
};
