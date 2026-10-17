#include "stdafx.h"
#include "ConvertRTFToText.h"
#include "..\Shared\TextConvert.h"


BOOL CConvertRTFToText::Create()
{
	// Register window class using raw Win32 API
	WNDCLASSEX wcex = {0};
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_CLASSDC | CS_SAVEBITS;
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.lpfnWndProc = DefWindowProc;
	wcex.hInstance = GetModuleHandle(NULL);
	wcex.lpszClassName = _T("ConvertRTFToText");
	RegisterClassEx(&wcex);

	// Create the window - just don't show it yet.
	m_hWnd = CreateWindow(_T("ConvertRTFToText"), _T(""), WS_POPUP, 0, 0, 0, 0, NULL, NULL, GetModuleHandle(NULL), NULL);
	if (!m_hWnd)
		return FALSE;

	m_richEditTextConverter.Create(m_hWnd, 1, WS_CHILD | WS_VISIBLE | WS_VSCROLL | WS_HSCROLL | ES_MULTILINE | ES_AUTOVSCROLL | ES_NOHIDESEL | ES_AUTOHSCROLL);

	return TRUE;
}

CString CConvertRTFToText::GetTextFromRTF(CStringA rtf)
{
	m_richEditTextConverter.SetRTF(rtf);

	auto x = m_richEditTextConverter.GetRTF();

	int loops = 0;
	MSG msg;
	while (::PeekMessage(&msg, m_hWnd, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);

		loops++;
		if (loops > 100)
			break;
	}

	loops = 0;
	while (::PeekMessage(&msg, m_richEditTextConverter.m_hWnd, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);

		loops++;
		if (loops > 100)
			break;
	}

	Sleep(50);

	CString text = m_richEditTextConverter.GetText();

	return text;
}