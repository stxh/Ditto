// FileCompat.h - Win32 based file helpers used by the code base in place of MFC's
// CFile/CFileException/CFileFind. Only the surface Ditto actually calls is here.

#pragma once

#if !defined(DITTO_FILECOMPAT_H_INCLUDED_)
#define DITTO_FILECOMPAT_H_INCLUDED_

#include <atlbase.h>
#include <atltime.h>
#include <atlstr.h>
#include <windows.h>

using ATL::CTime;
using ATL::CString;

class CException
{
public:
	virtual ~CException() {}

	virtual BOOL GetErrorMessage(_Out_writes_(nMaxErrorBuf) LPTSTR lpszErrorBuf, _In_ UINT nMaxErrorBuf, _Out_opt_ PUINT pnHelpContext = NULL)
	{
		if (pnHelpContext != NULL)
			*pnHelpContext = 0;
		if (lpszErrorBuf == NULL || nMaxErrorBuf == 0)
			return FALSE;

		lpszErrorBuf[0] = _T('\0');
		return TRUE;
	}

	virtual void ReportError(_In_opt_ HWND hWndParent = NULL)
	{
		TCHAR szError[1024];
		if (GetErrorMessage(szError, _countof(szError)))
			::MessageBox(hWndParent, szError, _T("Ditto"), MB_OK | MB_ICONERROR);
	}

	void Delete() { delete this; }
};

class CFileException : public CException
{
public:
	enum Cause { none = 0, genericException, sharingViolation, lockViolation, diskFull,
				 tooManyOpenFiles, accessDenied, invalidFile, fileNotFound, badPath,
				 removeFailed, renameFailed, writeFault, readFault, directoryFull,
				 fileExists, writeProtect, noCurrentRecord, recordIsLocked,
				 portNotOpen, invalidHandle, hardFailure, invalidField, unexpectedError };

	CFileException() : m_cause(none), m_osError(0) { m_strFilename[0] = _T('\0'); }

	UINT m_cause;
	UINT m_osError;
	TCHAR m_strFilename[MAX_PATH];

	virtual BOOL GetErrorMessage(_Out_writes_(nMaxErrorBuf) LPTSTR lpszErrorBuf, _In_ UINT nMaxErrorBuf, _Out_opt_ PUINT pnHelpContext = NULL)
	{
		if (pnHelpContext != NULL)
			*pnHelpContext = 0;
		if (lpszErrorBuf == NULL || nMaxErrorBuf == 0)
			return FALSE;

		if (m_osError == 0)
		{
			lpszErrorBuf[0] = _T('\0');
			return TRUE;
		}

		DWORD n = ::FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL, m_osError, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
			lpszErrorBuf, (DWORD)nMaxErrorBuf, NULL);

		if (n == 0)
		{
			_sntprintf_s(lpszErrorBuf, nMaxErrorBuf, _TRUNCATE, _T("File error %lu"), m_osError);
		}

		return TRUE;
	}
};

class CFile
{
public:
	enum OpenFlags
	{
		modeRead			= 0x0000,
		modeWrite			= 0x0001,
		modeCreate			= 0x0002,
		modeNoCreate		= 0x0004,
		modeNoTruncate		= 0x0008,
		typeText			= 0x0010,
		typeBinary			= 0x0020,
		shareExclusive		= 0x0040,
		shareDenyWrite		= 0x0080,
		shareDenyRead		= 0x0100,
		shareDenyNone		= 0x0200,
		bufferWrite			= 0x0400,
		randomAccess		= 0x0800,
		sequentialAccess	= 0x1000
	};

	enum { begin = 0, current = 1, end = 2 };

	CFile() : m_hFile(INVALID_HANDLE_VALUE) {}
	explicit CFile(HANDLE hFile) : m_hFile(hFile) {}
	~CFile() { Close(); }

	CFile(CFile&& other) : m_hFile(other.m_hFile) { other.m_hFile = INVALID_HANDLE_VALUE; }
	CFile& operator=(CFile&& other)
	{
		if (this != &other)
		{
			Close();
			m_hFile = other.m_hFile;
			other.m_hFile = INVALID_HANDLE_VALUE;
		}
		return *this;
	}

	BOOL IsOpen() const { return m_hFile != INVALID_HANDLE_VALUE; }
	HANDLE Detach() { HANDLE h = m_hFile; m_hFile = INVALID_HANDLE_VALUE; return h; }
	void Attach(HANDLE hFile) { Close(); m_hFile = hFile; }

	void Close()
	{
		if (m_hFile != INVALID_HANDLE_VALUE)
		{
			::CloseHandle(m_hFile);
			m_hFile = INVALID_HANDLE_VALUE;
		}
	}

	BOOL Open(_In_ LPCTSTR lpszPath, _In_ UINT nOpenFlags = modeRead | typeBinary, _In_opt_ CFileException* pError = NULL)
	{
		Close();

		DWORD dwAccess = 0;
		if (nOpenFlags & modeWrite)
			dwAccess |= GENERIC_WRITE;
		if ((nOpenFlags & modeRead) || dwAccess == 0)
			dwAccess |= GENERIC_READ;

		DWORD dwShare = 0;
		if (nOpenFlags & shareExclusive)
			dwShare = 0;
		else if (nOpenFlags & shareDenyWrite)
			dwShare = FILE_SHARE_READ;
		else if (nOpenFlags & shareDenyRead)
			dwShare = FILE_SHARE_WRITE;
		else if (nOpenFlags & shareDenyNone)
			dwShare = FILE_SHARE_READ | FILE_SHARE_WRITE;
		else
			dwShare = FILE_SHARE_READ;

	DWORD dwDisposition = OPEN_EXISTING;
		if (nOpenFlags & modeCreate)
			dwDisposition = (nOpenFlags & modeNoTruncate) ? OPEN_ALWAYS : CREATE_ALWAYS;

		DWORD dwAttrs = FILE_ATTRIBUTE_NORMAL;
		if (nOpenFlags & randomAccess)
			dwAttrs |= FILE_FLAG_RANDOM_ACCESS;
		else if (nOpenFlags & sequentialAccess)
			dwAttrs |= FILE_FLAG_SEQUENTIAL_SCAN;

		m_hFile = ::CreateFile(lpszPath, dwAccess, dwShare, NULL, dwDisposition, dwAttrs, NULL);
		if (m_hFile != INVALID_HANDLE_VALUE)
			return TRUE;

		if (pError != NULL)
		{
			pError->m_osError = ::GetLastError();
			pError->m_cause = CauseFromError(pError->m_osError);
			ATL::Checked::tcsncpy_s(pError->m_strFilename, _countof(pError->m_strFilename), lpszPath ? lpszPath : _T(""), _TRUNCATE);
		}
		return FALSE;
	}

	ULONG Read(_Out_writes_bytes_(nCount) void* lpBuf, _In_ ULONG nCount)
	{
		ULONG nRead = 0;
		if (m_hFile == INVALID_HANDLE_VALUE || lpBuf == NULL || nCount == 0)
			return 0;
		if (!::ReadFile(m_hFile, lpBuf, nCount, &nRead, NULL))
			return 0;
		return nRead;
	}

	ULONG Write(_In_reads_bytes_(nCount) const void* lpBuf, _In_ ULONG nCount)
	{
		ULONG nWritten = 0;
		if (m_hFile == INVALID_HANDLE_VALUE || lpBuf == NULL || nCount == 0)
			return 0;
		if (!::WriteFile(m_hFile, lpBuf, nCount, &nWritten, NULL))
			return 0;
		return nWritten;
	}

	ULONG GetLength() const
	{
		if (m_hFile == INVALID_HANDLE_VALUE)
			return 0;
		DWORD dwHigh = 0;
		DWORD dwLow = ::GetFileSize(m_hFile, &dwHigh);
		if (dwLow == INVALID_FILE_SIZE && dwHigh == 0)
			return 0;
		return (ULONG)(((ULONGLONG)dwHigh << 32) | dwLow);
	}

	LONGLONG Seek(_In_ LONGLONG lDistance, _In_ UINT nSeekMode = current)
	{
		if (m_hFile == INVALID_HANDLE_VALUE)
			return -1;

		DWORD dwMoveMethod = FILE_BEGIN;
		if (nSeekMode == CFile::end)
			dwMoveMethod = FILE_END;
		else if (nSeekMode == CFile::current)
			dwMoveMethod = FILE_CURRENT;

		LONG lHigh = (LONG)(lDistance >> 32);
		DWORD dwLow = ::SetFilePointer(m_hFile, (LONG)(lDistance & 0xffffffff), &lHigh, dwMoveMethod);
		if (dwLow == INVALID_SET_FILE_POINTER && ::GetLastError() != NO_ERROR)
			return -1;
		return (((LONGLONG)lHigh) << 32) | dwLow;
	}

	void SeekToBegin() { Seek(0, CFile::begin); }

	ULONG SeekToEnd()
	{
		LONGLONG lPos = Seek(0, CFile::end);
		return lPos < 0 ? 0 : (ULONG)lPos;
	}

	void Flush()
	{
		if (m_hFile != INVALID_HANDLE_VALUE)
			::FlushFileBuffers(m_hFile);
	}

	static BOOL Remove(_In_ LPCTSTR lpszFileName, _Inout_opt_ CFileException* pError = NULL)
	{
		if (::DeleteFile(lpszFileName))
			return TRUE;
		SetErrorFromLastError(pError, lpszFileName);
		return FALSE;
	}

	static BOOL Rename(_In_ LPCTSTR lpExistingFileName, _In_ LPCTSTR lpNewFileName, _Inout_opt_ CFileException* pError = NULL)
	{
		if (::MoveFileEx(lpExistingFileName, lpNewFileName, MOVEFILE_REPLACE_EXISTING))
			return TRUE;
		SetErrorFromLastError(pError, lpExistingFileName);
		return FALSE;
	}

	static BOOL Exists(_In_ LPCTSTR lpszFileName)
	{
		WIN32_FIND_DATA fd = {};
		HANDLE hFind = ::FindFirstFile(lpszFileName, &fd);
		if (hFind == INVALID_HANDLE_VALUE)
			return FALSE;
		::FindClose(hFind);
		return TRUE;
	}

	HANDLE m_hFile;

private:
	CFile(const CFile&);
	CFile& operator=(const CFile&);

	static UINT CauseFromError(_In_ DWORD dwError)
	{
		switch (dwError)
		{
		case ERROR_FILE_NOT_FOUND:
		case ERROR_PATH_NOT_FOUND:
			return CFileException::fileNotFound;
		case ERROR_SHARING_VIOLATION:
			return CFileException::sharingViolation;
		case ERROR_ACCESS_DENIED:
		case ERROR_USER_MAPPED_FILE:
			return CFileException::accessDenied;
		case ERROR_FILE_EXISTS:
			return CFileException::fileExists;
		case ERROR_DISK_FULL:
			return CFileException::diskFull;
		case ERROR_TOO_MANY_OPEN_FILES:
			return CFileException::tooManyOpenFiles;
		case ERROR_INVALID_PARAMETER:
		case ERROR_INVALID_NAME:
			return CFileException::invalidFile;
		}
		return CFileException::genericException;
	}

	static void SetErrorFromLastError(_Inout_opt_ CFileException* pError, _In_ LPCTSTR lpszFileName)
	{
		if (pError == NULL)
			return;
		pError->m_osError = ::GetLastError();
		pError->m_cause = CauseFromError(pError->m_osError);
		ATL::Checked::tcsncpy_s(pError->m_strFilename, _countof(pError->m_strFilename), lpszFileName ? lpszFileName : _T(""), _TRUNCATE);
	}
};

class CFileFind
{
public:
	CFileFind() : m_hFind(INVALID_HANDLE_VALUE), m_bFound(FALSE), m_bClosed(FALSE)
	{
		ZeroMemory(&m_fd, sizeof(m_fd));
	}

	~CFileFind() { Close(); }

	BOOL FindFile(_In_opt_ LPCTSTR pStr = NULL)
	{
		Close();

		if (pStr != NULL)
			m_csSearch = pStr;

		if (m_csSearch.IsEmpty())
			return FALSE;

		m_hFind = ::FindFirstFile(m_csSearch, &m_fd);
		if (m_hFind == INVALID_HANDLE_VALUE)
		{
			m_bFound = FALSE;
			return FALSE;
		}

		// the first match is already loaded, FindNextFile() moves to the second one
		m_bFound = TRUE;
		m_bClosed = FALSE;
		PopulateNames();
		return TRUE;
	}

	BOOL FindNextFile()
	{
		if (m_hFind == INVALID_HANDLE_VALUE || m_bClosed)
			return FALSE;

		m_bFound = ::FindNextFile(m_hFind, &m_fd);
		if (m_bFound)
			PopulateNames();
		else
			Close();

		return m_bFound;
	}

	void Close()
	{
		if (m_hFind != INVALID_HANDLE_VALUE)
		{
			::FindClose(m_hFind);
			m_hFind = INVALID_HANDLE_VALUE;
		}
		m_bClosed = TRUE;
		m_bFound = FALSE;
	}

	LPCTSTR GetFilePath() const { return m_csFilePath; }
	LPCTSTR GetFileName() const { return m_csFileName; }
	LPCTSTR GetFileTitle() const { return m_csFileTitle; }
	LPCTSTR GetFileExt() const { return m_csFileExt; }
	BOOL IsDots() const { return (m_fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) && (m_csFileName == _T(".") || m_csFileName == _T("..")); }
	BOOL IsDirectory() const { return (m_fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0; }
	BOOL IsReadOnly() const { return (m_fd.dwFileAttributes & FILE_ATTRIBUTE_READONLY) != 0; }
	BOOL IsHidden() const { return (m_fd.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN) != 0; }
	ULONGLONG GetLength() const { return (((ULONGLONG)m_fd.nFileSizeHigh) << 32) | m_fd.nFileSizeLow; }

	BOOL GetLastWriteTime(_Out_ FILETIME* lpFileTime) const
	{
		if (lpFileTime == NULL)
			return FALSE;
		*lpFileTime = m_fd.ftLastWriteTime;
		return TRUE;
	}

	BOOL GetLastAccessTime(_Out_ FILETIME* lpFileTime) const
	{
		if (lpFileTime == NULL)
			return FALSE;
		*lpFileTime = m_fd.ftLastAccessTime;
		return TRUE;
	}

	BOOL GetLastAccessTime(_Out_ CTime& refLastAccessTime) const
	{
		refLastAccessTime = CTime(m_fd.ftLastAccessTime);
		return TRUE;
	}

	BOOL GetLastWriteTime(_Out_ CTime& refLastWriteTime) const
	{
		refLastWriteTime = CTime(m_fd.ftLastWriteTime);
		return TRUE;
	}

	BOOL GetCreationTime(_Out_ FILETIME* lpFileTime) const
	{
		if (lpFileTime == NULL)
			return FALSE;
		*lpFileTime = m_fd.ftCreationTime;
		return TRUE;
	}

private:
	void PopulateNames()
	{
		m_csFileName = m_fd.cFileName;

		int nSep = m_csFileName.ReverseFind(_T('.'));
		m_csFileTitle = (nSep > 0) ? m_csFileName.Left(nSep) : m_csFileName;
		m_csFileExt = (nSep > 0) ? m_csFileName.Mid(nSep + 1) : _T("");

		// build the full path from the search pattern directory
		int nWild = m_csSearch.ReverseFind(_T('\\'));
		if (nWild < 0)
			m_csFilePath = m_csFileName;
		else
			m_csFilePath = m_csSearch.Left(nWild + 1) + m_csFileName;
	}

	HANDLE m_hFind;
	BOOL m_bFound;
	BOOL m_bClosed;
	WIN32_FIND_DATA m_fd;
	CString m_csSearch;
	CString m_csFilePath;
	CString m_csFileName;
	CString m_csFileTitle;
	CString m_csFileExt;
};

#endif // DITTO_FILECOMPAT_H_INCLUDED_
