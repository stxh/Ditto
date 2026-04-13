#include "StdAfx.h"
#include "ReadOnlyFlag.h"
#include "../../Shared/Tokenizer.h"
#include "../../Shared/TextConvert.h"


CReadOnlyFlag::CReadOnlyFlag(void)
{
}


CReadOnlyFlag::~CReadOnlyFlag(void)
{
}


bool CReadOnlyFlag::ResetReadOnlyFlag(const CDittoInfo &DittoInfo, IClip *pClip, bool resetFlag)
{
	IClipFormats *pFormats = pClip->Clips();
	if(pFormats)
	{
		CStringVector lines;

		LoadHDropFiles(lines, pFormats);

		if(lines.empty())
		{
			LoadUnicodeFiles(lines, pFormats);
		}

		if(lines.empty())
		{
			LoadTextFiles(lines, pFormats);
		}

		for(size_t i = 0; i < lines.size(); i++)
		{
			CString file = lines[i];
			file.TrimLeft(_T(' '));
			file.TrimRight(_T(' '));
			file.MakeLower();

			//Find the first occurance of a file // or \\ for a network file or a->z:\\ for a local files
			int pos = file.Find(_T("//"));
			if(pos >= 0)
			{
				file = file.Mid(pos);
			}
			else
			{
				pos = file.Find(_T("\\\\"));
				if(pos >= 0)
				{
					file = file.Mid(pos);
				}
				else
				{
					for(wchar_t drive = 'a'; drive <= 'z'; drive++)
					{
						CString csDrive(drive);
						csDrive += _T(":\\");

						pos = file.Find(csDrive);
						if(pos >= 0)
						{
							file = file.Mid(pos);
							break;
						}

						csDrive =  drive;
						csDrive += _T(":/");

						pos = file.Find(csDrive);
						if(pos >= 0)
						{
							file = file.Mid(pos);
							break;
						}
					}
				}
			}

			if(resetFlag)
			{
				::SetFileAttributes(file, FILE_ATTRIBUTE_NORMAL);
			}
			else
			{
				::SetFileAttributes(file, FILE_ATTRIBUTE_READONLY);
			}
		}
	}

	return true;
}

bool CReadOnlyFlag::LoadUnicodeFiles(CStringVector &lines, IClipFormats *pFormats)
{		
	IClipFormat *pFormat = pFormats->FindFormatEx(CF_UNICODETEXT);
	if(pFormat != NULL)
	{
		wchar_t *stringData = (wchar_t *)GlobalLock(pFormat->Data());
		if(stringData != NULL)
		{
			CString string(stringData);
			CString delim(_T("\r\n"));

			CTokenizer token(string, delim);
			CString line;
			while(token.Next(line))
			{
				lines.push_back(line);
			}

			GlobalUnlock(pFormat->Data());
		}
	}

	return !lines.empty();
}

bool CReadOnlyFlag::LoadTextFiles(CStringVector &lines, IClipFormats *pFormats)
{		
	IClipFormat *pFormat = pFormats->FindFormatEx(CF_TEXT);
	if(pFormat != NULL)
	{
		char *stringData = (char *)GlobalLock(pFormat->Data());
		if(stringData != NULL)
		{
			CStringA string(stringData);
			CStringW unicodeString(CTextConvert::AnsiToUnicode(string));
			CString delim(_T("\r\n"));

			CTokenizer token(unicodeString, delim);
			CString line;
			while(token.Next(line))
			{
				lines.push_back(line);
			}

			GlobalUnlock(pFormat->Data());
		}
	}

	return !lines.empty();
}

bool CReadOnlyFlag::LoadHDropFiles(CStringVector &lines, IClipFormats *pFormats)
{
	IClipFormat *pFormat = pFormats->FindFormatEx(CF_HDROP);
	if(pFormat != NULL)
	{
		HDROP drop = (HDROP)GlobalLock(pFormat->Data());
		if(drop)
		{
			int nNumFiles = DragQueryFile(drop, -1, NULL, 0);
			TCHAR file[MAX_PATH];

			for(int nFile = 0; nFile < nNumFiles; nFile++)
			{
				if(DragQueryFile(drop, nFile, file, sizeof(file)) > 0)
				{
					lines.push_back(file);
				}
			}

			GlobalUnlock(pFormat->Data());
		}
	}

	return !lines.empty();
}