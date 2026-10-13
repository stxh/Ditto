// OleDataCompat.h - Win32/COM replacements for the MFC OLE clipboard wrappers.
// COleDataSource is the server side (what Ditto puts on the clipboard or drags),
// COleDataObject is the client side (what Ditto reads from the clipboard).
// Delayed rendering goes through the virtual OnRenderGlobalData hook.

#pragma once

#if !defined(DITTO_OLEDATACOMPAT_H_INCLUDED_)
#define DITTO_OLEDATACOMPAT_H_INCLUDED_

#include <vector>
#include <ole2.h>
#include <oleidl.h>

// MFC's COle* wrappers speak in DROPEFFECT; the SDK only declares the effect
// flags, so keep the type name the app code already uses.
typedef DWORD DROPEFFECT;

#ifndef OLE_E_ENUM_NEEDED
#define OLE_E_ENUM_NEEDED ((HRESULT)0x80040003L)
#endif

#ifndef OLE_E_ADVISENOTSUPPORTED
#define OLE_E_ADVISENOTSUPPORTED ((HRESULT)0x8004000BL)
#endif

class COleDataSource;

/*----------------------------------------------------------------------------*\
	One cached or delay rendered format of a data source
\*----------------------------------------------------------------------------*/
struct OLECLIENTDATA
{
	FORMATETC	formatEtc;
	STGMEDIUM	stgMedium;
	BOOL		bDelayed;
};

/*----------------------------------------------------------------------------*\
	CDittoEnumFormatEtc - IEnumFORMATETC over a snapshot of the source formats
\*----------------------------------------------------------------------------*/
class CDittoEnumFormatEtc : public IEnumFORMATETC
{
public:
	explicit CDittoEnumFormatEtc(const std::vector<FORMATETC>& formats) : m_refCount(1), m_position(0), m_formats(formats) {}
	virtual ~CDittoEnumFormatEtc() {}

	// IUnknown
	STDMETHODIMP QueryInterface(REFIID riid, void** ppvObject)
	{
		if (ppvObject == NULL)
			return E_POINTER;

		if (riid == IID_IUnknown || riid == IID_IEnumFORMATETC)
			*ppvObject = static_cast<IEnumFORMATETC*>(this);
		else
		{
			*ppvObject = NULL;
			return E_NOINTERFACE;
		}

		AddRef();
		return S_OK;
	}

	STDMETHODIMP_(ULONG) AddRef() { return (ULONG)::InterlockedIncrement((LONG*)&m_refCount); }

	STDMETHODIMP_(ULONG) Release()
	{
		ULONG ul = (ULONG)::InterlockedDecrement((LONG*)&m_refCount);
		if (ul == 0)
			delete this;
		return ul;
	}

	// IEnumFORMATETC
	STDMETHODIMP Next(ULONG celt, LPFORMATETC rgelt, PULONG pceltFetched)
	{
		if (rgelt == NULL || celt == 0)
			return E_INVALIDARG;

		ULONG nFetched = 0;
		while (nFetched < celt && m_position < m_formats.size())
		{
			rgelt[nFetched] = m_formats[m_position];
			nFetched++;
			m_position++;
		}

		if (pceltFetched != NULL)
			*pceltFetched = nFetched;

		return (nFetched == celt) ? S_OK : S_FALSE;
	}

	STDMETHODIMP Skip(ULONG celt)
	{
		m_position += celt;
		if (m_position > m_formats.size())
			m_position = m_formats.size();
		return S_OK;
	}

	STDMETHODIMP Reset() { m_position = 0; return S_OK; }

	STDMETHODIMP Clone(IEnumFORMATETC** ppEnumFormatEtc)
	{
		if (ppEnumFormatEtc == NULL)
			return E_POINTER;

		*ppEnumFormatEtc = NULL;
		CDittoEnumFormatEtc* pNew = new CDittoEnumFormatEtc(m_formats);
		pNew->m_position = m_position;
		*ppEnumFormatEtc = pNew;
		return S_OK;
	}

private:
	ULONG m_refCount;
	size_t m_position;
	std::vector<FORMATETC> m_formats;
};

/*----------------------------------------------------------------------------*\
	CDittoDataObject - IDataObject serving the formats held by a COleDataSource.
	Its references are counted on the source, so it never outlives the source and
	never owns it.
\*----------------------------------------------------------------------------*/
class CDittoDataObject : public IDataObject
{
public:
	explicit CDittoDataObject(COleDataSource* pSrc) : m_pSrc(pSrc) {}
	virtual ~CDittoDataObject() { ClearEntries(TRUE); }

	// IUnknown
	STDMETHODIMP QueryInterface(REFIID riid, void** ppvObject)
	{
		if (ppvObject == NULL)
			return E_POINTER;

		if (riid == IID_IUnknown || riid == IID_IDataObject)
			*ppvObject = static_cast<IDataObject*>(this);
		else
		{
			*ppvObject = NULL;
			return E_NOINTERFACE;
		}

		AddRef();
		return S_OK;
	}

	STDMETHODIMP_(ULONG) AddRef();
	STDMETHODIMP_(ULONG) Release();

	// IDataObject
	STDMETHODIMP GetData(LPFORMATETC lpFormatEtc, LPSTGMEDIUM lpStgMedium);
	STDMETHODIMP GetDataHere(LPFORMATETC lpFormatEtc, LPSTGMEDIUM lpStgMedium);
	STDMETHODIMP QueryGetData(LPFORMATETC lpFormatEtc);
	STDMETHODIMP GetCanonicalFormatEtc(LPFORMATETC lpFormatEtcIn, LPFORMATETC lpFormatEtcOut);
	STDMETHODIMP SetData(LPFORMATETC lpFormatEtc, LPSTGMEDIUM lpStgMedium, BOOL fRelease);
	STDMETHODIMP EnumFormatEtc(DWORD dwDirection, LPENUMFORMATETC* lppEnumFormatEtc);
	STDMETHODIMP DAdvise(LPFORMATETC lpFormatEtc, DWORD dwAdvf, IAdviseSink* pAdvSink, DWORD* pdwConnection);
	STDMETHODIMP DUnadvise(DWORD dwConnection);
	STDMETHODIMP EnumDAdvise(LPENUMSTATDATA* lppEnumAdvise);

	// format cache, replaced per format
	void AddOrReplaceEntry(const OLECLIENTDATA& data)
	{
		for (size_t i = 0; i < m_data.size(); i++)
		{
			if (SameFormat(m_data[i].formatEtc, data.formatEtc))
			{
				::ReleaseStgMedium(&m_data[i].stgMedium);
				m_data[i] = data;
				return;
			}
		}

		m_data.push_back(data);
	}

	void ClearEntries(BOOL bReleaseMediums)
	{
		if (bReleaseMediums)
		{
			for (size_t i = 0; i < m_data.size(); i++)
				::ReleaseStgMedium(&m_data[i].stgMedium);
		}
		m_data.clear();
	}

	const std::vector<OLECLIENTDATA>& Entries() const { return m_data; }

	void DetachOwner() { m_pSrc = NULL; }

	static BOOL SameFormat(const FORMATETC& a, const FORMATETC& b)
	{
		return a.cfFormat == b.cfFormat && a.lindex == b.lindex && a.dwAspect == b.dwAspect;
	}

private:
	int IndexOfFormat(LPFORMATETC lpFormatEtc) const
	{
		if (lpFormatEtc == NULL)
			return -1;

		for (size_t i = 0; i < m_data.size(); i++)
		{
			if (m_data[i].formatEtc.cfFormat == lpFormatEtc->cfFormat &&
				m_data[i].formatEtc.lindex == lpFormatEtc->lindex &&
				m_data[i].formatEtc.dwAspect == lpFormatEtc->dwAspect)
				return (int)i;
		}

		return -1;
	}

	COleDataSource* m_pSrc;
	std::vector<OLECLIENTDATA> m_data;
};

/*----------------------------------------------------------------------------*\
	COleDataSource
\*----------------------------------------------------------------------------*/
class COleDataSource
{
public:
	COleDataSource() : m_refCount(1)
	{
		m_pDataObject = new CDittoDataObject(this);
	}

	virtual ~COleDataSource()
	{
		if (m_pDataObject != NULL)
		{
			m_pDataObject->DetachOwner();
			delete m_pDataObject;
			m_pDataObject = NULL;
		}
	}

	// the data source owns hGlobal from now on
	void CacheGlobalData(CLIPFORMAT cfFormat, HGLOBAL hGlobal, LPFORMATETC lpFormatEtc = NULL)
	{
		OLECLIENTDATA data = {};
		FillFormatEtc(data.formatEtc, cfFormat, lpFormatEtc);
		data.stgMedium.tymed = TYMED_HGLOBAL;
		data.stgMedium.hGlobal = hGlobal;
		data.bDelayed = FALSE;

		if (m_pDataObject != NULL)
			m_pDataObject->AddOrReplaceEntry(data);
	}

	void DelayRenderData(CLIPFORMAT cfFormat, LPFORMATETC lpFormatEtc = NULL)
	{
		OLECLIENTDATA data = {};
		FillFormatEtc(data.formatEtc, cfFormat, lpFormatEtc);
		data.stgMedium.tymed = TYMED_HGLOBAL;
		data.bDelayed = TRUE;

		if (m_pDataObject != NULL)
			m_pDataObject->AddOrReplaceEntry(data);
	}

	// override to render the data of a delayed format
	virtual BOOL OnRenderGlobalData(LPFORMATETC /*lpFormatEtc*/, HGLOBAL* phGlobal)
	{
		if (phGlobal != NULL)
			*phGlobal = NULL;
		return FALSE;
	}

	virtual void Empty()
	{
		if (m_pDataObject != NULL)
			m_pDataObject->ClearEntries(TRUE);
	}

	// the clipboard owns this object after the call, the caller must not delete it
	void SetClipboard(BOOL /*bNecessary*/ = FALSE)
	{
		if (m_pDataObject == NULL)
			return;

		::OleFlushClipboard();

		if (SUCCEEDED(::OleSetClipboard(m_pDataObject)))
		{
			// OLE took a reference on the data object, the caller hands over its own
			InternalRelease();
		}
	}

	DROPEFFECT DoDragDrop(DWORD dwEffects, LPPOINT lptDown = NULL, LPPOINT lptCurrent = NULL)
	{
		DROPEFFECT dwResult = DROPEFFECT_NONE;
		if (m_pDataObject == NULL)
			return dwResult;

		// the plain Win32 drag loop does not take the drag origin
		UNREFERENCED_PARAMETER(lptDown);
		UNREFERENCED_PARAMETER(lptCurrent);

		if (SUCCEEDED(::DoDragDrop(m_pDataObject, NULL, dwEffects, &dwResult)))
		{
			// the drag completed, drop any data the target left behind
			Empty();
		}

		return dwResult;
	}

	DWORD InternalAddRef() { return (DWORD)::InterlockedIncrement((LONG*)&m_refCount); }

	void InternalRelease()
	{
		if (::InterlockedDecrement((LONG*)&m_refCount) == 0)
			delete this;
	}

	LPDATAOBJECT GetDataObject() const { return m_pDataObject; }

protected:
	static void FillFormatEtc(FORMATETC& etc, CLIPFORMAT cfFormat, LPFORMATETC lpFormatEtc)
	{
		if (lpFormatEtc != NULL)
			etc = *lpFormatEtc;
		else
		{
			etc.cfFormat = cfFormat;
			etc.ptd = NULL;
			etc.dwAspect = DVASPECT_CONTENT;
			etc.lindex = -1;
			etc.tymed = TYMED_HGLOBAL;
		}
	}

private:
	COleDataSource(const COleDataSource&);
	COleDataSource& operator=(const COleDataSource&);

	DWORD m_refCount;
	CDittoDataObject* m_pDataObject;
};

inline STDMETHODIMP_(ULONG) CDittoDataObject::AddRef()
{
	if (m_pSrc != NULL)
		return m_pSrc->InternalAddRef();
	return 2;
}

inline STDMETHODIMP_(ULONG) CDittoDataObject::Release()
{
	if (m_pSrc != NULL)
	{
		// the source deletes this object when its last reference goes away
		m_pSrc->InternalRelease();
		return 1;
	}

	return 0;
}

inline STDMETHODIMP CDittoDataObject::GetData(LPFORMATETC lpFormatEtc, LPSTGMEDIUM lpStgMedium)
{
	if (lpStgMedium == NULL)
		return E_POINTER;

	ZeroMemory(lpStgMedium, sizeof(*lpStgMedium));

	int nIndex = IndexOfFormat(lpFormatEtc);
	if (nIndex < 0)
		return DV_E_FORMATETC;

	OLECLIENTDATA& entry = m_data[nIndex];

	if (entry.bDelayed)
	{
		HGLOBAL hGlobal = NULL;
		if (m_pSrc == NULL || !m_pSrc->OnRenderGlobalData(&entry.formatEtc, &hGlobal) || hGlobal == NULL)
			return DV_E_FORMATETC;

		lpStgMedium->tymed = TYMED_HGLOBAL;
		lpStgMedium->hGlobal = hGlobal;
		return S_OK;
	}

	if (entry.stgMedium.tymed != TYMED_HGLOBAL || entry.stgMedium.hGlobal == NULL)
		return DV_E_FORMATETC;

	// every caller gets its own copy of the global
	SIZE_T nSize = ::GlobalSize(entry.stgMedium.hGlobal);
	HGLOBAL hCopy = ::GlobalAlloc(GMEM_MOVEABLE, nSize);
	if (hCopy == NULL)
		return E_OUTOFMEMORY;

	LPVOID pSrc = ::GlobalLock(entry.stgMedium.hGlobal);
	LPVOID pDst = ::GlobalLock(hCopy);
	if (pSrc != NULL && pDst != NULL && nSize > 0)
		CopyMemory(pDst, pSrc, nSize);

	BOOL bCopied = (pSrc != NULL && pDst != NULL);
	if (pSrc != NULL)
		::GlobalUnlock(entry.stgMedium.hGlobal);
	if (pDst != NULL)
		::GlobalUnlock(hCopy);

	if (!bCopied)
	{
		::GlobalFree(hCopy);
		return DV_E_FORMATETC;
	}

	lpStgMedium->tymed = TYMED_HGLOBAL;
	lpStgMedium->hGlobal = hCopy;
	return S_OK;
}

inline STDMETHODIMP CDittoDataObject::GetDataHere(LPFORMATETC lpFormatEtc, LPSTGMEDIUM lpStgMedium)
{
	if (lpStgMedium == NULL)
		return E_POINTER;

	int nIndex = IndexOfFormat(lpFormatEtc);
	if (nIndex < 0)
		return DV_E_FORMATETC;

	OLECLIENTDATA& entry = m_data[nIndex];
	if (entry.bDelayed || entry.stgMedium.tymed != TYMED_HGLOBAL || lpStgMedium->tymed != TYMED_HGLOBAL)
		return DV_E_FORMATETC;

	SIZE_T nSize = ::GlobalSize(entry.stgMedium.hGlobal);
	if (::GlobalSize(lpStgMedium->hGlobal) < nSize)
		return STG_E_MEDIUMFULL;

	LPVOID pSrc = ::GlobalLock(entry.stgMedium.hGlobal);
	LPVOID pDst = ::GlobalLock(lpStgMedium->hGlobal);
	if (pSrc == NULL || pDst == NULL)
		return STG_E_MEDIUMFULL;

	CopyMemory(pDst, pSrc, nSize);
	::GlobalUnlock(entry.stgMedium.hGlobal);
	::GlobalUnlock(lpStgMedium->hGlobal);
	return S_OK;
}

inline STDMETHODIMP CDittoDataObject::QueryGetData(LPFORMATETC lpFormatEtc)
{
	return IndexOfFormat(lpFormatEtc) >= 0 ? S_OK : DV_E_FORMATETC;
}

inline STDMETHODIMP CDittoDataObject::GetCanonicalFormatEtc(LPFORMATETC lpFormatEtcIn, LPFORMATETC lpFormatEtcOut)
{
	if (lpFormatEtcOut == NULL)
		return E_POINTER;

	if (lpFormatEtcIn != NULL)
		*lpFormatEtcOut = *lpFormatEtcIn;

	lpFormatEtcOut->lindex = 0;
	return S_OK;
}

inline STDMETHODIMP CDittoDataObject::SetData(LPFORMATETC lpFormatEtc, LPSTGMEDIUM lpStgMedium, BOOL fRelease)
{
	if (lpFormatEtc == NULL || lpStgMedium == NULL)
		return E_POINTER;

	if (fRelease)
		::ReleaseStgMedium(lpStgMedium);

	// a data source does not accept data from the target
	return E_NOTIMPL;
}

inline STDMETHODIMP CDittoDataObject::EnumFormatEtc(DWORD dwDirection, LPENUMFORMATETC* lppEnumFormatEtc)
{
	if (lppEnumFormatEtc == NULL)
		return E_POINTER;

	*lppEnumFormatEtc = NULL;
	if (dwDirection != DATADIR_GET)
		return OLE_E_ENUM_NEEDED;

	std::vector<FORMATETC> formats;
	for (size_t i = 0; i < m_data.size(); i++)
		formats.push_back(m_data[i].formatEtc);

	*lppEnumFormatEtc = new CDittoEnumFormatEtc(formats);
	return S_OK;
}

inline STDMETHODIMP CDittoDataObject::DAdvise(LPFORMATETC /*lpFormatEtc*/, DWORD /*dwAdvf*/, IAdviseSink* /*pAdvSink*/, DWORD* /*pdwConnection*/)
{
	return OLE_E_ADVISENOTSUPPORTED;
}

inline STDMETHODIMP CDittoDataObject::DUnadvise(DWORD /*dwConnection*/)
{
	return OLE_E_ADVISENOTSUPPORTED;
}

inline STDMETHODIMP CDittoDataObject::EnumDAdvise(LPENUMSTATDATA* lppEnumAdvise)
{
	if (lppEnumAdvise != NULL)
		*lppEnumAdvise = NULL;
	return OLE_E_ADVISENOTSUPPORTED;
}

/*----------------------------------------------------------------------------*\
	COleDataObject - client side of an IDataObject, e.g. the clipboard
\*----------------------------------------------------------------------------*/
class COleDataObject
{
public:
	COleDataObject() : m_lpDataObject(NULL), m_pEnum(NULL) {}

	virtual ~COleDataObject() { Release(); }

	void Attach(LPDATAOBJECT lpDataObject)
	{
		Release();
		m_lpDataObject = lpDataObject;
		if (m_lpDataObject != NULL)
			m_lpDataObject->AddRef();
	}

	LPDATAOBJECT Detach()
	{
		LPDATAOBJECT p = m_lpDataObject;
		m_lpDataObject = NULL;
		return p;
	}

	void Release()
	{
		if (m_pEnum != NULL)
		{
			m_pEnum->Release();
			m_pEnum = NULL;
		}
		if (m_lpDataObject != NULL)
		{
			m_lpDataObject->Release();
			m_lpDataObject = NULL;
		}
	}

	LPDATAOBJECT GetDataObject() const { return m_lpDataObject; }

	BOOL AttachClipboard(HWND /*hWndOwner*/ = NULL)
	{
		Release();
		return SUCCEEDED(::OleGetClipboard(&m_lpDataObject)) && m_lpDataObject != NULL;
	}

	BOOL EnsureClipboardObject()
	{
		if (m_lpDataObject != NULL)
			return TRUE;
		return AttachClipboard();
	}

	static void BuildFormatEtc(FORMATETC& etc, CLIPFORMAT cfFormat, LPFORMATETC lpFormatEtc)
	{
		if (lpFormatEtc != NULL)
			etc = *lpFormatEtc;
		else
		{
			etc.cfFormat = cfFormat;
			etc.ptd = NULL;
			etc.dwAspect = DVASPECT_CONTENT;
			etc.lindex = -1;
			etc.tymed = TYMED_HGLOBAL | TYMED_ISTREAM | TYMED_ISTORAGE;
		}
	}

	BOOL IsDataAvailable(CLIPFORMAT cfFormat, LPFORMATETC lpFormatEtc = NULL)
	{
		if (m_lpDataObject == NULL)
			return FALSE;

		FORMATETC etc;
		BuildFormatEtc(etc, cfFormat, lpFormatEtc);
		return SUCCEEDED(m_lpDataObject->QueryGetData(&etc));
	}

	BOOL GetData(CLIPFORMAT cfFormat, LPSTGMEDIUM lpStgMedium, LPFORMATETC* lppFormatEtc = NULL)
	{
		if (m_lpDataObject == NULL || lpStgMedium == NULL)
			return FALSE;

		FORMATETC etc;
		BuildFormatEtc(etc, cfFormat, NULL);

		HRESULT hr = m_lpDataObject->GetData(&etc, lpStgMedium);
		if (FAILED(hr))
		{
			// some sources only offer one medium, ask for global memory explicitly
			etc.tymed = TYMED_HGLOBAL;
			hr = m_lpDataObject->GetData(&etc, lpStgMedium);
		}

		if (FAILED(hr))
			return FALSE;

		if (lppFormatEtc != NULL && *lppFormatEtc != NULL)
			**lppFormatEtc = etc;
		return TRUE;
	}

	// the returned global is owned by the caller
	HGLOBAL GetGlobalData(CLIPFORMAT cfFormat, LPFORMATETC lpFormatEtc = NULL)
	{
		if (m_lpDataObject == NULL)
			return NULL;

		FORMATETC etc;
		BuildFormatEtc(etc, cfFormat, lpFormatEtc);
		etc.tymed = TYMED_HGLOBAL;

		STGMEDIUM stg = {};
		if (FAILED(m_lpDataObject->GetData(&etc, &stg)))
			return NULL;

		if (stg.tymed != TYMED_HGLOBAL)
		{
			::ReleaseStgMedium(&stg);
			return NULL;
		}

		return stg.hGlobal;
	}

	POSITION BeginEnumFormats()
	{
		if (m_pEnum != NULL)
		{
			m_pEnum->Release();
			m_pEnum = NULL;
		}
		if (m_lpDataObject == NULL)
			return 0;

		if (FAILED(m_lpDataObject->EnumFormatEtc(DATADIR_GET, &m_pEnum)))
			return 0;

		m_pEnum->Reset();
		return 1;
	}

	BOOL GetNextFormat(LPFORMATETC lpFormatEtc, LPSTGMEDIUM lpStgMedium)
	{
		if (m_pEnum == NULL || lpFormatEtc == NULL)
			return FALSE;

		if (FAILED(m_pEnum->Next(1, lpFormatEtc, NULL)))
			return FALSE;

		if (lpStgMedium != NULL)
		{
			ZeroMemory(lpStgMedium, sizeof(*lpStgMedium));
			m_lpDataObject->GetData(lpFormatEtc, lpStgMedium);
		}
		return TRUE;
	}

protected:
	LPDATAOBJECT m_lpDataObject;
	LPENUMFORMATETC m_pEnum;
};

#endif // DITTO_OLEDATACOMPAT_H_INCLUDED_
