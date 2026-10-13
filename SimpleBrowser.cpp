///////////////////////////////////////////////////////////////////////////
// SimpleBrowser.cpp: Web browser control
///////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <atlbase.h>
#include <atlwin.h>
#include <atlctl.h>
#include <SHDocVw.h>
#include "comdef.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define Unused(parameter) ((void)(parameter))

static CComModule _Module;
ATL::CComModule& _AtlModule = _Module;

///////////////////////////////////////////////////////////////////////////
// IDispatch Implementation for Browser Events
///////////////////////////////////////////////////////////////////////////

STDMETHODIMP SimpleBrowser::QueryInterface(REFIID riid, void **ppvObject)
{
	if (riid == IID_IUnknown || riid == IID_IDispatch) {
		*ppvObject = static_cast<IDispatch*>(this);
		return S_OK;
	}
	*ppvObject = NULL;
	return E_NOINTERFACE;
}

STDMETHODIMP_(ULONG) SimpleBrowser::AddRef()
{
	return 1;
}

STDMETHODIMP_(ULONG) SimpleBrowser::Release()
{
	return 1;
}

STDMETHODIMP SimpleBrowser::GetTypeInfoCount(UINT *pctinfo)
{
	*pctinfo = 0;
	return S_OK;
}

STDMETHODIMP SimpleBrowser::GetTypeInfo(UINT iTInfo, LCID lcid, ITypeInfo **ppTInfo)
{
	return E_NOTIMPL;
}

STDMETHODIMP SimpleBrowser::GetIDsOfNames(REFIID riid, LPOLESTR *rgszNames, UINT cNames, LCID lcid, DISPID *rgDispId)
{
	return E_NOTIMPL;
}

STDMETHODIMP SimpleBrowser::Invoke(DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS *pDispParams, VARIANT *pVarResult, EXCEPINFO *pExcepInfo, UINT *puArgErr)
{
	Unused(riid);
	Unused(lcid);
	Unused(wFlags);
	Unused(pVarResult);
	Unused(pExcepInfo);
	Unused(puArgErr);

	switch (dispIdMember) {
		case DISPID_BEFORENAVIGATE2:
		{
			LPDISPATCH lpDisp = NULL;
			VARIANT *URL = NULL;
			VARIANT *Flags = NULL;
			VARIANT *TargetFrameName = NULL;
			VARIANT *PostData = NULL;
			VARIANT *Headers = NULL;
			VARIANT_BOOL *Cancel = NULL;

			if (pDispParams && pDispParams->cArgs >= 7) {
				lpDisp = pDispParams->rgvarg[6].pdispVal;
				URL = pDispParams->rgvarg[5].pvarVal;
				Flags = pDispParams->rgvarg[4].pvarVal;
				TargetFrameName = pDispParams->rgvarg[3].pvarVal;
				PostData = pDispParams->rgvarg[2].pvarVal;
				Headers = pDispParams->rgvarg[1].pvarVal;
				Cancel = &pDispParams->rgvarg[0].boolVal;
			}
			_OnBeforeNavigate2(lpDisp, URL, Flags, TargetFrameName, PostData, Headers, Cancel);
			break;
		}
		case DISPID_DOCUMENTCOMPLETE:
		{
			LPDISPATCH lpDisp = NULL;
			VARIANT *URL = NULL;
			if (pDispParams && pDispParams->cArgs >= 2) {
				lpDisp = pDispParams->rgvarg[1].pdispVal;
				URL = pDispParams->rgvarg[0].pvarVal;
			}
			_OnDocumentComplete(lpDisp, URL);
			break;
		}
		case DISPID_DOWNLOADBEGIN:
			_OnDownloadBegin();
			break;
		case DISPID_PROGRESSCHANGE:
		{
			long progress = 0;
			long progress_max = 0;
			if (pDispParams && pDispParams->cArgs >= 2) {
				progress = pDispParams->rgvarg[1].lVal;
				progress_max = pDispParams->rgvarg[0].lVal;
			}
			_OnProgressChange(progress, progress_max);
			break;
		}
		case DISPID_DOWNLOADCOMPLETE:
			_OnDownloadComplete();
			break;
		case DISPID_NAVIGATECOMPLETE2:
		{
			LPDISPATCH lpDisp = NULL;
			VARIANT *URL = NULL;
			if (pDispParams && pDispParams->cArgs >= 2) {
				lpDisp = pDispParams->rgvarg[1].pdispVal;
				URL = pDispParams->rgvarg[0].pvarVal;
			}
			_OnNavigateComplete2(lpDisp, URL);
			break;
		}
		case DISPID_STATUSTEXTCHANGE:
		{
			BSTR bstrText = NULL;
			if (pDispParams && pDispParams->cArgs >= 1 && pDispParams->rgvarg[0].vt == VT_BSTR) {
				bstrText = pDispParams->rgvarg[0].bstrVal;
			}
			_OnStatusTextChange(bstrText);
			break;
		}
		case DISPID_TITLECHANGE:
		{
			BSTR bstrText = NULL;
			if (pDispParams && pDispParams->cArgs >= 1 && pDispParams->rgvarg[0].vt == VT_BSTR) {
				bstrText = pDispParams->rgvarg[0].bstrVal;
			}
			_OnTitleChange(bstrText);
			break;
		}
		case DISPID_PRINTTEMPLATEINSTANTIATION:
		{
			LPDISPATCH pDisp = NULL;
			if (pDispParams && pDispParams->cArgs >= 1) {
				pDisp = pDispParams->rgvarg[0].pdispVal;
			}
			_OnPrintTemplateInstantiation(pDisp);
			break;
		}
		case DISPID_PRINTTEMPLATETEARDOWN:
		{
			LPDISPATCH pDisp = NULL;
			if (pDispParams && pDispParams->cArgs >= 1) {
				pDisp = pDispParams->rgvarg[0].pdispVal;
			}
			_OnPrintTemplateTeardown(pDisp);
			break;
		}
		default:
			return DISP_E_MEMBERNOTFOUND;
	}
	return S_OK;
}

///////////////////////////////////////////////////////////////////////////
// Construction and creation
///////////////////////////////////////////////////////////////////////////

SimpleBrowser::SimpleBrowser()
	:	_Browser(NULL),
		_BrowserDispatch(NULL),
		_Ready(false),
		_Content(_T("")),
		_dwCookie(0)
{
}

SimpleBrowser::~SimpleBrowser()
{
	_DisconnectEvents();

	if (_Browser != NULL) {
		_Browser->Release();
		_Browser = NULL;
	}

	if (_BrowserDispatch != NULL) {
		_BrowserDispatch->Release();
		_BrowserDispatch = NULL;
	}
}

void SimpleBrowser::_ConnectEvents()
{
	if (_BrowserDispatch == NULL || _dwCookie != 0) return;

	CComPtr<IConnectionPointContainer> spCPC;
	HRESULT hr = _BrowserDispatch->QueryInterface(IID_IConnectionPointContainer, (void**)&spCPC);
	if (FAILED(hr)) return;

	CComPtr<IConnectionPoint> spCP;
	hr = spCPC->FindConnectionPoint(DIID_DWebBrowserEvents2, &spCP);
	if (FAILED(hr)) return;

	hr = spCP->Advise(static_cast<IDispatch*>(this), &_dwCookie);
}

void SimpleBrowser::_DisconnectEvents()
{
	if (_BrowserDispatch == NULL || _dwCookie == 0) return;

	CComPtr<IConnectionPointContainer> spCPC;
	HRESULT hr = _BrowserDispatch->QueryInterface(IID_IConnectionPointContainer, (void**)&spCPC);
	if (FAILED(hr)) return;

	CComPtr<IConnectionPoint> spCP;
	hr = spCPC->FindConnectionPoint(DIID_DWebBrowserEvents2, &spCP);
	if (SUCCEEDED(hr)) {
		spCP->Unadvise(_dwCookie);
	}
	_dwCookie = 0;
}

BOOL SimpleBrowser::Create(DWORD dwStyle, 
                           const RECT& rect, 
                           HWND hParentWnd, 
						   UINT nID)
{
    BOOL results = TRUE;

	_Ready      = false;
	
    _Browser	= NULL;

    CWindow *window = this;
	results = window->Create(ATL::AtlRegisterWndClass(CS_HREDRAW | CS_VREDRAW),
	                         NULL,
							 dwStyle,
							 rect,
							 hParentWnd,
							 nID);
    if (!results) return FALSE;

	CRect browser_window_rect(0,0,(rect.right - rect.left),(rect.bottom - rect.top));

    results = _BrowserWindow.CreateControl(CLSID_WebBrowser,
                                           NULL,
                                           (WS_VISIBLE | WS_TABSTOP),
                                           browser_window_rect,
                                           this,
                                           nID);
    if (!results) {
        DestroyWindow();
        return FALSE;
    }

    LPUNKNOWN unknown = _BrowserWindow.GetControlUnknown();

    HRESULT hr = unknown->QueryInterface(IID_IWebBrowser2,(void **)&_Browser);
    if (SUCCEEDED(hr)) {
        hr = unknown->QueryInterface(IID_IDispatch,(void **)&_BrowserDispatch);
    }

    if (!SUCCEEDED(hr)) {
        _BrowserWindow.DestroyWindow();
        DestroyWindow();
        return FALSE;        
    }

	_ConnectEvents();

	if (_Browser != NULL) {
		Navigate(_T("about:blank"));
	}

    return TRUE;
}

BOOL SimpleBrowser::CreateFromControl(HWND hParentWnd, UINT nID, DWORD dwStyle)
{
	BOOL result = FALSE;

	if (hParentWnd == NULL) return FALSE;

	CWindow parentWnd(hParentWnd);
	CWnd *control = parentWnd.GetDlgItem(nID);

	if (control != NULL) {
		CRect rect;
		control->GetWindowRect(&rect);
		parentWnd.ScreenToClient(&rect);
		control->DestroyWindow();
		result = Create(dwStyle, rect, hParentWnd, nID);
	}

	return result;
}

void SimpleBrowser::PostNcDestroy()
{
	_DisconnectEvents();

	if (_Browser != NULL) {
		_Browser->Release();
		_Browser = NULL;
	}

	if (_BrowserDispatch != NULL) {
		_BrowserDispatch->Release();
		_BrowserDispatch = NULL;
	}
	
	_Ready		= false;
	_Content	= _T("");
	
	CWindow::PostNcDestroy();
}

///////////////////////////////////////////////////////////////////////////
// Controls
///////////////////////////////////////////////////////////////////////////

void SimpleBrowser::Copy()
{
	if (_Browser != NULL) {		
		_Browser->ExecWB(OLECMDID_COPY, OLECMDEXECOPT_DONTPROMPTUSER, NULL, NULL);
	}
}

void SimpleBrowser::Navigate(LPCTSTR URL)
{
	_Ready   = false;
	_Content = _T("");

	if (_Browser != NULL) {
		CString url(URL);
		_variant_t flags(0L,VT_I4);
		_variant_t target_frame_name("");
		_variant_t post_data("");
		_variant_t headers("");

		_Browser->Navigate(url.AllocSysString(),
						   &flags,
						   &target_frame_name,
						   &post_data,
						   &headers);
	}
}

void SimpleBrowser::NavigateResource(int resource_ID)
{
	if (_Browser != NULL) {
		CString resource_string;

		HRSRC resource_handle = FindResource(NULL,
											 MAKEINTRESOURCE(resource_ID),
											 RT_HTML);

		if (resource_handle != NULL) {

			HGLOBAL resource = LoadResource(NULL,
											resource_handle);

			if (resource != NULL) {
				LPVOID resource_memory = LockResource(resource);

				if (resource_memory != NULL) {
					DWORD resource_size = SizeofResource(NULL, resource_handle);

					bool UNICODE_document = false;
					wchar_t *UNICODE_memory = (wchar_t *)resource_memory;
					int UNICODE_size = resource_size / sizeof(wchar_t);

					if (UNICODE_size >= 1) {
						if (*UNICODE_memory == L'\xFEFF') {
							UNICODE_document = true;
							UNICODE_memory += 1;
							UNICODE_size -= 1;
						}
						else if (UNICODE_size >= 5) {
							if ((UNICODE_memory[0] == L'<') &&
							    (towupper(UNICODE_memory[1]) == L'H') &&
							    (towupper(UNICODE_memory[2]) == L'T') &&
							    (towupper(UNICODE_memory[3]) == L'M') &&
							    (towupper(UNICODE_memory[4]) == L'L')) {
								UNICODE_document = true;
							}
						}
					}

#if !defined(UNICODE)
					if (UNICODE_document) {
						char *MBCS_buffer = resource_string.GetBufferSetLength(resource_size + 1);
						int MBCS_length = ::WideCharToMultiByte(CP_ACP, 0, UNICODE_memory, UNICODE_size, MBCS_buffer, resource_size + 1, NULL, NULL);
						resource_string.ReleaseBuffer(MBCS_length);
					}
					else {
						resource_string = CString((char *)resource_memory,resource_size);
					}
#else
					if (UNICODE_document) {
						resource_string = CString(UNICODE_memory, UNICODE_size);
					}
					else {
						wchar_t *UNICODE_buffer = resource_string.GetBufferSetLength(resource_size + 1);
						int UNICODE_length = ::MultiByteToWideChar(CP_ACP, 0, (const char *)resource_memory, resource_size, UNICODE_buffer, (resource_size + 1));
						resource_string.ReleaseBuffer(UNICODE_length);
					}
#endif
				}
			}
		}

		Clear();
		Write(resource_string);
	}
}

void SimpleBrowser::Write(LPCTSTR string)
{
	if (_Browser != NULL) {
		_Content.Append(string);
		_ContentWrite();
	}
}

void SimpleBrowser::Clear()
{
	if (_Browser != NULL) {
		_Content = _T("");

		IHTMLDocument2 *document = GetDocument();
		HRESULT hr = S_OK;
		
		if (document != NULL) {
			document->close();
			
			VARIANT open_name;
			VARIANT open_features;
			VARIANT open_replace;
			IDispatch *open_window = NULL;

			::VariantInit(&open_name);
			open_name.vt = VT_BSTR;
			open_name.bstrVal = ::SysAllocString(L"_self");

			::VariantInit(&open_features);
			::VariantInit(&open_replace);
			
			hr = document->open(::SysAllocString(L"text/html"), open_name, open_features, open_replace, &open_window);

			if (hr == S_OK) {
				Refresh();
			}

			if (open_window != NULL) {
				open_window->Release();
			}

			::VariantClear(&open_name);
			document->Release();
			document = NULL;
		}
		else {
			Navigate(_T("about:blank"));
		}
	}
}

void SimpleBrowser::GoBack()
{
	if (_Browser != NULL) {
		_Browser->GoBack();
	}
}

void SimpleBrowser::GoForward()
{
	if (_Browser != NULL) {
		_Browser->GoForward();
	}
}

void SimpleBrowser::GoHome()
{
	if (_Browser != NULL) {
		_Browser->GoHome();
	}
}

void SimpleBrowser::Refresh()
{
	if (_Browser != NULL) {
		_Browser->Refresh();
	}
}

void SimpleBrowser::Stop()
{
	if (_Browser != NULL) {
		_Browser->Stop();
	}
}

void SimpleBrowser::Print(LPCTSTR header, LPCTSTR footer)
{
	if (_Browser != NULL) {
		HRESULT hr;

		VARIANT header_variant;
		VariantInit(&header_variant);
		V_VT(&header_variant) = VT_BSTR;
		V_BSTR(&header_variant) = CString(header).AllocSysString();

		VARIANT footer_variant;
		VariantInit(&footer_variant);
		V_VT(&footer_variant) = VT_BSTR;
		V_BSTR(&footer_variant) = CString(footer).AllocSysString();

		long index;
		SAFEARRAYBOUND parameter_array_bound[1];
		SAFEARRAY *parameter_array = NULL;

		parameter_array_bound[0].cElements = 2;
		parameter_array_bound[0].lLbound = 0;

		parameter_array = SafeArrayCreate(VT_VARIANT, 1, parameter_array_bound);

		index = 0;
		hr = SafeArrayPutElement(parameter_array, &index, &header_variant);

		index = 1;
		hr = SafeArrayPutElement(parameter_array, &index, &footer_variant);

		VARIANT parameter;
		VariantInit(&parameter);
		V_VT(&parameter) = VT_ARRAY | VT_BYREF;
		V_ARRAY(&parameter) = parameter_array;

		hr = _Browser->ExecWB(OLECMDID_PRINT, OLECMDEXECOPT_DODEFAULT, &parameter, NULL);

		if (!SUCCEEDED(hr)) {
			VariantClear(&header_variant);
			VariantClear(&footer_variant);
			if (parameter_array != NULL) {
				SafeArrayDestroy(parameter_array);
			}
		}
	}
}

void SimpleBrowser::PrintPreview()
{
	if (_Browser != NULL) {
		_Browser->ExecWB(OLECMDID_PRINTPREVIEW, OLECMDEXECOPT_DODEFAULT, NULL, NULL);
	}
}

bool SimpleBrowser::GetBusy()
{
    bool busy = false;

	if (_Browser != NULL) {
		VARIANT_BOOL variant_bool;
		HRESULT hr = _Browser->get_Busy(&variant_bool);
		if (SUCCEEDED(hr)) {
			busy = (variant_bool == VARIANT_TRUE);    
		}
	}

    return busy;
}

CString SimpleBrowser::GetLocationName()
{
    CString location_name = _T("");

	if (_Browser != NULL) {
		BSTR location_name_BSTR = NULL;
		HRESULT hr = _Browser->get_LocationName(&location_name_BSTR);
		if (SUCCEEDED(hr)) {
			location_name = location_name_BSTR;
		}
		::SysFreeString(location_name_BSTR);
	}

    return location_name;
}

CString SimpleBrowser::GetLocationURL()
{
    CString location_URL = _T("");

	if (_Browser != NULL) {
		BSTR location_URL_BSTR = NULL;
		HRESULT hr = _Browser->get_LocationURL(&location_URL_BSTR);
		if (SUCCEEDED(hr)) {
			location_URL = location_URL_BSTR;
		}
		::SysFreeString(location_URL_BSTR);
	}

    return location_URL;
}

READYSTATE SimpleBrowser::GetReadyState()
{
    READYSTATE readystate = READYSTATE_UNINITIALIZED;

    if (_Browser != NULL) {
		_Browser->get_ReadyState(&readystate);
	}

    return readystate;
}

bool SimpleBrowser::GetSilent()
{
    bool silent = false;

	if (_Browser != NULL) {
		VARIANT_BOOL silent_variant;
		HRESULT hr = _Browser->get_Silent(&silent_variant);
		if (SUCCEEDED(hr)) {
			silent = (silent_variant == VARIANT_TRUE);
		}
	}

    return silent;
}

void SimpleBrowser::PutSilent(bool silent)
{
	if (_Browser != NULL) {
		VARIANT_BOOL silent_variant = silent ? VARIANT_TRUE : VARIANT_FALSE;
		_Browser->put_Silent(silent_variant);
	}
}

IHTMLDocument2 *SimpleBrowser::GetDocument()
{
	IHTMLDocument2 *document = NULL;
	
	if (_Browser != NULL) {
		IDispatch *document_dispatch = NULL;
		HRESULT hr = _Browser->get_Document(&document_dispatch);

		if (SUCCEEDED(hr) && (document_dispatch != NULL)) {
			hr = document_dispatch->QueryInterface(IID_IHTMLDocument2, (void **)&document);
			document_dispatch->Release();
		}
	}
	
	return document;
}

///////////////////////////////////////////////////////////////////////////
// Message handlers
///////////////////////////////////////////////////////////////////////////

BOOL SimpleBrowser::PreTranslateMessage(MSG *pMsg)
{
	BOOL result = FALSE;

	if (_Browser != NULL) {
		IOleInPlaceActiveObject* OleInPlaceActiveObject = NULL;
		HRESULT hr = _Browser->QueryInterface(IID_IOleInPlaceActiveObject, (void**)&OleInPlaceActiveObject);

		if (SUCCEEDED(hr) && (OleInPlaceActiveObject != NULL)) {
			hr = OleInPlaceActiveObject->TranslateAccelerator(pMsg);
			result = (hr == S_OK ? TRUE : FALSE);
			OleInPlaceActiveObject->Release();
		}
	}
	else {
		result = CWindow::PreTranslateMessage(pMsg);
	}

	return result;
}

LRESULT SimpleBrowser::OnSize(UINT nType, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	Unused(nType);
	int cx = LOWORD(lParam);
	int cy = HIWORD(lParam);

	if (_Browser != NULL) {
        CRect rect(0,0,cx,cy);	
		_BrowserWindow.MoveWindow(&rect);
	}
	return 0;
}

///////////////////////////////////////////////////////////////////////////
// Event handlers
///////////////////////////////////////////////////////////////////////////

SimpleBrowser::Notification::Notification(HWND hwnd, UINT ID, NotificationType type)
{
	hdr.hwndFrom = hwnd;
	hdr.idFrom = ID;
	hdr.code = type;
	URL = _T("");
	frame = _T("");
	post_data = NULL;
	post_data_size = 0;
	headers = _T("");
	progress = 0;
	progress_max = 0;
	text = _T("");
}

SimpleBrowser::Notification::~Notification()
{
}

void SimpleBrowser::_OnBeforeNavigate2(LPDISPATCH lpDisp,
                                       VARIANT FAR *URL,
                                       VARIANT FAR *Flags,
                                       VARIANT FAR *TargetFrameName,
                                       VARIANT FAR *PostData,
                                       VARIANT FAR *Headers,
                                       VARIANT_BOOL *Cancel)
{
	Unused(Flags);

    if (lpDisp == _BrowserDispatch) {
		CString URL_string;
		CString frame;
		unsigned char *post_data = NULL;
		int post_data_size = 0;
		CString headers;

        if ((URL != NULL) && (V_VT(URL) == VT_BSTR)) {
            URL_string = V_BSTR(URL);
        }

		if ((TargetFrameName != NULL) && (V_VT(TargetFrameName) == VT_BSTR)) {
			frame = V_BSTR(TargetFrameName);
        }

		if ((PostData != NULL) && (V_VT(PostData) == (VT_VARIANT | VT_BYREF))) {
			VARIANT *PostData_variant = V_VARIANTREF(PostData);

			if ((PostData_variant != NULL) && (V_VT(PostData_variant) != VT_EMPTY)) {
				SAFEARRAY *PostData_safearray = V_ARRAY(PostData_variant);

				if (PostData_safearray != NULL) {
					char *post_data_array = NULL;
					SafeArrayAccessData(PostData_safearray, (void HUGEP **)&post_data_array);

					long lower_bound = 1;
					long upper_bound = 1;
					SafeArrayGetLBound(PostData_safearray, 1, &lower_bound);
					SafeArrayGetUBound(PostData_safearray, 1, &upper_bound);

					post_data_size = (int)(upper_bound - lower_bound + 1);
					post_data = new unsigned char[post_data_size];
					memcpy(post_data, post_data_array, post_data_size);

					SafeArrayUnaccessData(PostData_safearray);
				}
			}
		}

		if ((Headers != NULL) && (V_VT(Headers) == VT_BSTR)) {
			headers = V_BSTR(Headers);
        }

		bool cancel = OnBeforeNavigate2(URL_string, frame, post_data, post_data_size, headers);

		if (Cancel != NULL) {
			*Cancel = cancel ? VARIANT_TRUE : VARIANT_FALSE;
		}

		delete []post_data;
    }    
}

bool SimpleBrowser::OnBeforeNavigate2(CString URL, CString frame, void *post_data, int post_data_size, CString headers)
{
	bool cancel = false;
	
	CWindow parent = GetParent();
	
	if (parent.IsWindow()) {
		Notification notification(m_hWnd, GetDlgCtrlID(), BeforeNavigate2);
		
		notification.URL = URL;
		notification.frame = frame;
		notification.post_data = post_data;
		notification.post_data_size = post_data_size;
		notification.headers = headers;

		LRESULT result = parent.SendMessage(WM_NOTIFY, notification.hdr.idFrom, (LPARAM)&notification);
		
		if (result) {
			cancel = true;
		}
	}
	
    return cancel;
}

bool SimpleBrowser::ParsePostData(CString post_data, CStringArray &names, CStringArray &values)
{
	bool result = true;
	
	int size = 1;
	names.SetSize(size);
	values.SetSize(size);
	
	int offset = 0;
	bool parsing_name = true;
	
	CString hex(_T("0123456789ABCDEF"));
	
	while ((offset < post_data.GetLength()) && result) {
		if (post_data[offset] == _T('%')) {
			if ((offset + 2) < post_data.GetLength()) {
				int digit1 = hex.Find(_totupper(post_data[offset + 1]));
				int digit2 = hex.Find(_totupper(post_data[offset + 2]));

				if ((digit1 >= 0) && (digit2 >= 0)) {
					_TCHAR character = (_TCHAR)((digit1 << 4) + digit2);
					if (parsing_name) names[size - 1].AppendChar(character);
					else values[size - 1].AppendChar(character);
				}
				else {
					result = false;
				}
				offset += 2;
			}
			else {
				result = false;
			}
		}
		else if (post_data[offset] == _T('+')) {
			if (parsing_name) names[size - 1].AppendChar(_T(' '));
			else values[size - 1].AppendChar(_T(' '));
		}
		else if (post_data[offset] == _T('=')) {
			if (parsing_name) {
				parsing_name = false;
			}
			else {
				values[size - 1].AppendChar(post_data[offset]);
			}
		}
		else if (post_data[offset] == _T('&')) {
			if (!parsing_name) {
				parsing_name = true;
				size += 1;
				names.SetSize(size);
				values.SetSize(size);
			}
			else {
				values[size - 1].AppendChar(post_data[offset]);
			}
		}
		else {
			if (parsing_name) names[size - 1].AppendChar(post_data[offset]);
			else values[size - 1].AppendChar(post_data[offset]);
		}
		offset++;
	}

	if ((names[size - 1] == _T("")) && (values[size - 1] == _T(""))) {
		names.SetSize(size - 1);
		values.SetSize(size - 1);
	}

	return result;
}

void SimpleBrowser::_OnDocumentComplete(LPDISPATCH lpDisp, VARIANT *URL)
{
    if (lpDisp == _BrowserDispatch) {
		CString URL_string;
		
		if ((URL != NULL) && (V_VT(URL) == VT_BSTR)) {
			URL_string = CString(V_BSTR(URL));
        }

        OnDocumentComplete(URL_string);
    }    
}

void SimpleBrowser::OnDocumentComplete(CString URL)
{
	CWindow parent = GetParent();
	
	if (parent.IsWindow()) {
		Notification notification(m_hWnd, GetDlgCtrlID(), DocumentComplete);
		notification.URL = URL;

		parent.SendMessage(WM_NOTIFY, notification.hdr.idFrom, (LPARAM)&notification);
	}
}

void SimpleBrowser::_OnDownloadBegin()
{
    OnDownloadBegin();
}

void SimpleBrowser::OnDownloadBegin()
{
	CWindow parent = GetParent();
	
	if (parent.IsWindow()) {
		Notification notification(m_hWnd, GetDlgCtrlID(), DownloadBegin);
		parent.SendMessage(WM_NOTIFY, notification.hdr.idFrom, (LPARAM)&notification);
	}
}

void SimpleBrowser::_OnProgressChange(long progress, long progress_max)
{
    OnProgressChange((int)progress, (int)progress_max);
}

void SimpleBrowser::OnProgressChange(int progress, int progress_max)
{
	CWindow parent = GetParent();
	
	if (parent.IsWindow()) {
		Notification notification(m_hWnd, GetDlgCtrlID(), ProgressChange);
		notification.progress = progress;
		notification.progress_max = progress_max;

		parent.SendMessage(WM_NOTIFY, notification.hdr.idFrom, (LPARAM)&notification);
	}
}

void SimpleBrowser::_OnDownloadComplete()
{
    OnDownloadComplete();
}

void SimpleBrowser::OnDownloadComplete()
{
	CWindow parent = GetParent();
	
	if (parent.IsWindow()) {
		Notification notification(m_hWnd, GetDlgCtrlID(), DownloadComplete);
		parent.SendMessage(WM_NOTIFY, notification.hdr.idFrom, (LPARAM)&notification);
	}
}

void SimpleBrowser::_OnNavigateComplete2(LPDISPATCH lpDisp, VARIANT *URL)
{
    if (lpDisp == _BrowserDispatch) {
		_Ready = true;
		_ContentWrite();
		
		CString URL_string;
		
		if ((URL != NULL) && (V_VT(URL) == VT_BSTR)) {
			URL_string = CString(V_BSTR(URL));
        }

		OnNavigateComplete2(URL_string);
    }    
}

void SimpleBrowser::OnNavigateComplete2(CString URL)
{
	CWindow parent = GetParent();
	
	if (parent.IsWindow()) {
		Notification notification(m_hWnd, GetDlgCtrlID(), NavigateComplete2);
		notification.URL = URL;

		parent.SendMessage(WM_NOTIFY, notification.hdr.idFrom, (LPARAM)&notification);
	}
}

void SimpleBrowser::_OnStatusTextChange(BSTR bstrText)
{
    CString text;
	
	if (bstrText != NULL) {
		text = (LPCTSTR)bstrText;
	}

    OnStatusTextChange(text);
}

void SimpleBrowser::OnStatusTextChange(CString text)
{
	CWindow parent = GetParent();
	
	if (parent.IsWindow()) {
		Notification notification(m_hWnd, GetDlgCtrlID(), StatusTextChange);
		notification.text = text;

		parent.SendMessage(WM_NOTIFY, notification.hdr.idFrom, (LPARAM)&notification);
	}
}

void SimpleBrowser::_OnTitleChange(BSTR bstrText)
{
    CString text;
	
	if (bstrText != NULL) {
		text = (LPCTSTR)bstrText;
	}

    OnTitleChange(text);
}

void SimpleBrowser::OnTitleChange(CString text)
{
	CWindow parent = GetParent();
	
	if (parent.IsWindow()) {
		Notification notification(m_hWnd, GetDlgCtrlID(), TitleChange);
		notification.text = text;

		parent.SendMessage(WM_NOTIFY, notification.hdr.idFrom, (LPARAM)&notification);
	}
}
 
void SimpleBrowser::_OnPrintTemplateInstantiation(LPDISPATCH lpDisp)
{
	Unused(lpDisp);
	OnPrintTemplateInstantiation();
}

void SimpleBrowser::OnPrintTemplateInstantiation()
{
	CWindow parent = GetParent();
	
	if (parent.IsWindow()) {
		Notification notification(m_hWnd, GetDlgCtrlID(), PrintTemplateInstantiation);
		parent.SendMessage(WM_NOTIFY, notification.hdr.idFrom, (LPARAM)&notification);
	}
}

void SimpleBrowser::_OnPrintTemplateTeardown(LPDISPATCH lpDisp)
{
	Unused(lpDisp);
	OnPrintTemplateTeardown();
}

void SimpleBrowser::OnPrintTemplateTeardown()
{
	CWindow parent = GetParent();
	
	if (parent.IsWindow()) {
		Notification notification(m_hWnd, GetDlgCtrlID(), PrintTemplateTeardown);
		parent.SendMessage(WM_NOTIFY, notification.hdr.idFrom, (LPARAM)&notification);
	}
}

void SimpleBrowser::_ContentWrite()
{
	if (_Ready && (!_Content.IsEmpty())) {
		IHTMLDocument2 *document = GetDocument();
		
		if (document != NULL) {
			SAFEARRAY *safe_array = SafeArrayCreateVector(VT_VARIANT, 0, 1);
			VARIANT *variant;
			
			SafeArrayAccessData(safe_array, (LPVOID *)&variant);
			
			variant->vt = VT_BSTR;
			variant->bstrVal = _Content.AllocSysString();
			
			SafeArrayUnaccessData(safe_array);

			document->write(safe_array);
			
			document->Release();
			document = NULL;

			::SysFreeString(variant->bstrVal);
			variant->bstrVal = NULL;
			
			SafeArrayDestroy(safe_array);
		}
	}
}
