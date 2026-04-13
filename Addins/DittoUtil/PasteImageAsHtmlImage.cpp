#include "StdAfx.h"
#include "pasteimageashtmlimage.h"
#include "../../shared/TextConvert.h"

CString g_csDIBImagePath = _T("");
int g_nDIBImageName = 1;

CPasteImageAsHtmlImage::CPasteImageAsHtmlImage(void)
{
}

CPasteImageAsHtmlImage::~CPasteImageAsHtmlImage(void)
{
}

bool CPasteImageAsHtmlImage::ConvertPathToHtmlImageTag(const CDittoInfo &DittoInfo, IClip *pClip)
{
	bool bRet = false;
	IClipFormats *pFormats = pClip->Clips();
	if(pFormats)
	{
		if(g_csDIBImagePath.IsEmpty())
		{
			CreateLocalPath(true);
		}

		CString csIMG = _T("");

		IClipFormat *pCF_DIB = pFormats->FindFormatEx(CF_DIB);
		if(pCF_DIB != NULL)
		{
			CString csFile;
			csFile.Format(_T("%s\\%d.bmp"), g_csDIBImagePath, g_nDIBImageName);
			g_nDIBImageName++;


			LPVOID pvData = GlobalLock(pCF_DIB->Data());
			ULONG size = (ULONG)GlobalSize(pCF_DIB->Data());

			if(WriteDataToFile(csFile, pvData, size))
			{
				GlobalUnlock(pCF_DIB->Data());

				csIMG.Format(_T("<IMG src=\"file:///%s\">"), csFile);
			}
			else
			{
				GlobalUnlock(pCF_DIB->Data());
			}
		}
		else
		{
			IClipFormat *pHDrop = pFormats->FindFormatEx(CF_HDROP);
			if(pHDrop)
			{
				HDROP drop = (HDROP)GlobalLock((HDROP)pHDrop->Data());
				int nNumFiles = DragQueryFile(drop, -1, NULL, 0);
				TCHAR file[MAX_PATH];

				for(int nFile = 0; nFile < nNumFiles; nFile++)
				{
					if(DragQueryFile(drop, nFile, file, sizeof(file)) > 0)
					{
						CString csOrigfile(file);
						CString csFile(file);
						csFile = csFile.MakeLower();

						if(csFile.Find(_T(".bmp")) != -1 || 
							csFile.Find(_T(".dib")) != -1 ||
							csFile.Find(_T(".jpg")) != -1 ||
							csFile.Find(_T(".jpeg")) != -1 ||
							csFile.Find(_T(".jpe")) != -1 ||
							csFile.Find(_T(".jfif")) != -1 ||
							csFile.Find(_T(".gif")) != -1 ||
							csFile.Find(_T(".tif")) != -1 ||
							csFile.Find(_T(".tiff")) != -1 ||
							csFile.Find(_T(".png")) != -1)
						{
							CString csFormat;
							csFormat.Format(_T("<IMG src=\"file:///%s\">"), csOrigfile);
							if(nFile < nNumFiles-1)
							{
								csFormat += _T("<br>");
							}
							csIMG += csFormat;
						}
					}
				}

				GlobalUnlock(pHDrop->Data());
			}
		}

		if(csIMG.IsEmpty() == FALSE)
		{
			pFormats->DeleteAll();
			CStringA utf8 = CTextConvert::UnicodeToUTF8(csIMG);
			pFormats->AddNew(DittoAddinHelpers::GetFormatID(_T("HTML Format")), DittoAddinHelpers::NewGlobalP(utf8.GetBuffer(), utf8.GetLength()));
			bRet = true;
		}
	}

	return bRet;
}

bool CPasteImageAsHtmlImage::WriteDataToFile(CString csPath, LPVOID data, ULONG size)
{
	bool bRet = false;
	BITMAPINFO *lpBI = (BITMAPINFO *)data;

	int nPaletteEntries = 1 << lpBI->bmiHeader.biBitCount;
	if(lpBI->bmiHeader.biBitCount > 8)
		nPaletteEntries = 0;
	else if( lpBI->bmiHeader.biClrUsed != 0 )
		nPaletteEntries = lpBI->bmiHeader.biClrUsed;

	BITMAPFILEHEADER BFH;
	memset(&BFH, 0, sizeof( BITMAPFILEHEADER));
	BFH.bfType = 'MB';
	BFH.bfSize = sizeof(BITMAPFILEHEADER) + size;
	BFH.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + nPaletteEntries * sizeof(RGBQUAD);

	HANDLE hFile = ::CreateFile(csPath, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if(hFile != INVALID_HANDLE_VALUE)
	{
		DWORD written = 0;
		BOOL headerWritten = ::WriteFile(hFile, &BFH, sizeof(BITMAPFILEHEADER), &written, NULL);
		headerWritten = headerWritten && (written == sizeof(BITMAPFILEHEADER));
		written = 0;
		BOOL dataWritten = ::WriteFile(hFile, data, size, &written, NULL);
		dataWritten = dataWritten && (written == size);
		::CloseHandle(hFile);
		bRet = (headerWritten && dataWritten) ? true : false;
	}
	else
	{
		DWORD dwErr = ::GetLastError();
		CString csError;
		LPVOID lpMsgBuf = NULL;
		FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL,
			dwErr,
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
			(LPTSTR)&lpMsgBuf,
			0,
			NULL);
		if(lpMsgBuf)
		{
			csError.Format(_T("Failed to write CF_DIB to file: %s, Error: %s"), csPath, (LPCTSTR)lpMsgBuf);
			::LocalFree(lpMsgBuf);
		}
		else
		{
			csError.Format(_T("Failed to write CF_DIB to file: %s, Error: %lu"), csPath, dwErr);
		}
		ATLTRACE(_T("%s\n"), csError.GetString());
	}

	return bRet;
}

bool CPasteImageAsHtmlImage::CleanupPastedImages()
{
	bool bRet = false;
	if(g_csDIBImagePath.IsEmpty())
	{
		CreateLocalPath(false);
	}

	WIN32_FIND_DATA findData = {0};
	CString searchPath = g_csDIBImagePath + _T("\\*");
	HANDLE hFind = ::FindFirstFile(searchPath, &findData);
	if(hFind != INVALID_HANDLE_VALUE)
	{
		do
		{
			if((findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0)
			{
				CString filePath = g_csDIBImagePath + _T("\\") + findData.cFileName;
				::DeleteFile(filePath);
			}
		}
		while(::FindNextFile(hFind, &findData));

		::FindClose(hFind);
	}

	bRet = (::RemoveDirectory(g_csDIBImagePath) != FALSE);

	return bRet;
}

void CPasteImageAsHtmlImage::CreateLocalPath(bool bCreateDir)
{
	g_csDIBImagePath = _wgetenv(_T("TMP"));;
	g_csDIBImagePath += _T("\\ditto");
	if(bCreateDir)
	{
		CreateDirectory(g_csDIBImagePath, NULL);
	}
}