#include "stdafx.h"
#include "cp_main.h"
#include "FileTransferProgressDlg.h"

LRESULT CFileTransferProgressDlg::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	m_FileCopy.Attach(GetDlgItem(IDC_FILE_COPY));
	m_m_CancelButton.Attach(GetDlgItem(IDCANCEL));
	m_ProgressSingleFile.Attach(GetDlgItem(IDC_PROGRESS_FILE));
	m_ProgressAllFiles.Attach(GetDlgItem(IDC_PROGRESS_ALL_FILES));
	m_Message.Attach(GetDlgItem(IDC_STATIC_1));
	m_CopyingFile.Attach(GetDlgItem(IDC_STATIC_2));

	m_FileCopy.Open(IDR_FILE_COPY);
	m_FileCopy.Play(0, -1, -1);
	
	SetNumFiles(0);

	::SetWindowPos(m_hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE|SWP_NOMOVE|SWP_SHOWWINDOW);

	// TRUE: no focus set here, let the dialog manager select the first control.
	return TRUE;
}

LRESULT CFileTransferProgressDlg::OnCancel(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	m_bCancelled = true;
	return 0;
}

void CFileTransferProgressDlg::PostNcDestroy() 
{
	delete this;
}

void CFileTransferProgressDlg::SetMessage(CString &cs)
{
	m_Message.SetWindowText(cs);
}

CString CFileTransferProgressDlg::GetMessage()
{
	CString cs;
	m_Message.GetWindowText(cs);
	return cs;
}

void CFileTransferProgressDlg::SetFileMessage(CString &cs)
{
	m_CopyingFile.SetWindowText(cs);
}

void CFileTransferProgressDlg::SetNumFiles(int nFiles)
{
	m_ProgressAllFiles.SetRange32(0, nFiles);
	m_ProgressAllFiles.SetStep(1);
	m_ProgressAllFiles.SetPos(0);

	m_ProgressSingleFile.SetRange32(0, 100);
	m_ProgressSingleFile.SetStep(1);
	m_ProgressSingleFile.SetPos(0);
}

void CFileTransferProgressDlg::ResetSingleFileProgress()
{
	m_ProgressSingleFile.SetPos(0);
}

void CFileTransferProgressDlg::StepAllFiles()
{
	m_ProgressAllFiles.StepIt();
}

void CFileTransferProgressDlg::SetSingleFilePos(int nPos)
{
	m_ProgressSingleFile.SetPos(nPos);
}

void CFileTransferProgressDlg::PumpMessages()
{
	int nLoops = 0;
	MSG msg;
    while(::PeekMessage(&msg, m_hWnd, 0, 0, PM_REMOVE)) 
    {
        TranslateMessage(&msg);
		DispatchMessage(&msg);

		nLoops++;
		if(nLoops > 100)
			break;
    }
}
