#if !defined(AFX_FILETRANSFERPROGRESSDLG_H__B7CD7263_7AA2_44C1_BF85_31021FB23D85__INCLUDED_)
#define AFX_FILETRANSFERPROGRESSDLG_H__B7CD7263_7AA2_44C1_BF85_31021FB23D85__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Resource.h"
#include <atlbase.h>
#include <atlwin.h>
#include <atldlgs.h>
#include <atlctrls.h>

class CFileTransferProgressDlg : public CDialogImpl<CFileTransferProgressDlg>
{
public:
	enum { IDD = IDD_DIALOG_REMOTE_FILE };

	CFileTransferProgressDlg() : m_bCancelled(false) {}

	void SetMessage(CString &cs);
	CString GetMessage();
	void SetFileMessage(CString &cs);

	void SetNumFiles(int nFiles);
	void StepAllFiles();
	void SetSingleFilePos(int nPos);
	void ResetSingleFileProgress();

	bool Cancelled() { return m_bCancelled; }

	void PumpMessages();

	BEGIN_MSG_MAP(CFileTransferProgressDlg)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		COMMAND_ID_HANDLER(IDCANCEL, OnCancel)
	END_MSG_MAP()

	LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnCancel(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);

	void PostNcDestroy();

private:
	bool m_bCancelled;
	CAnimateCtrl m_FileCopy;
	CButton m_m_CancelButton;
	CProgressBarCtrl m_ProgressSingleFile;
	CProgressBarCtrl m_ProgressAllFiles;
	CStatic m_Message;
	CStatic m_CopyingFile;
};

#endif // !defined(AFX_FILETRANSFERPROGRESSDLG_H__B7CD7263_7AA2_44C1_BF85_31021FB23D85__INCLUDED_)
