///////////////////////////////////////////////////////////////////////////
// SimpleBrowser: Web browser control
///////////////////////////////////////////////////////////////////////////

#if !defined(SimpleBrowser_defined)
#define SimpleBrowser_defined

#include <atlbase.h>
#include <atlwin.h>
#include <atlctl.h>
#include "mshtml.h"

class SimpleBrowser : public ATL::CWindow, public IDispatch {
public:
	SimpleBrowser();
	virtual ~SimpleBrowser();

	BOOL Create(DWORD dwStyle, 
	            const RECT& rect, 
				HWND hParentWnd, 
				UINT nID);
		
	BOOL CreateFromControl(HWND hParentWnd, UINT nID, DWORD dwStyle = WS_CHILD | WS_VISIBLE);

	void Navigate(LPCTSTR URL);
	void Write(LPCTSTR string);				
	void Clear();							
	void NavigateResource(int resource_ID);
	void GoBack();							
    void GoForward();						
    void GoHome();							
    void Refresh();							
    void Stop();							
    void Print(LPCTSTR header = _T("&w&b&b&p"),
			   LPCTSTR footer = _T("&d &t"));
	void PrintPreview();					

    bool GetBusy();                         
    CString GetLocationName();              
    CString GetLocationURL();               
    READYSTATE GetReadyState();             

    bool GetSilent();                       
    void PutSilent(bool silent = false);	

	IHTMLDocument2 *GetDocument();		

    virtual bool OnBeforeNavigate2(CString	URL,
	                               CString	frame,
								   void		*post_data,int post_data_size,
								   CString	headers);

    virtual void OnDocumentComplete(CString URL);
    virtual void OnDownloadBegin();
    virtual void OnProgressChange(int progress,int progress_max);
    virtual void OnDownloadComplete();
    virtual void OnNavigateComplete2(CString URL);
    virtual void OnStatusTextChange(CString text);
    virtual void OnTitleChange(CString text);
	virtual void OnPrintTemplateInstantiation();
	virtual void OnPrintTemplateTeardown();

	void Copy();

	enum NotificationType {
		BeforeNavigate2,
		DocumentComplete,				
		DownloadBegin,
		ProgressChange,
		DownloadComplete,
		NavigateComplete2,
		StatusTextChange,
		TitleChange,
		PrintTemplateInstantiation,
		PrintTemplateTeardown
	};
	
	class Notification {
	public:
		Notification(HWND hwnd, UINT ID, NotificationType type);
		~Notification();

		NMHDR			hdr;
		CString			URL;
		CString			frame;
		void			*post_data;
		int				post_data_size;
		CString			headers;
		int				progress;
		int				progress_max;
		CString			text;
	};

	bool ParsePostData(CString post_data,
	                   CStringArray &names,
	                   CStringArray &values);

	void PostNcDestroy();

protected:
	LRESULT OnSize(UINT, WPARAM, LPARAM, BOOL&);
	BOOL PreTranslateMessage(MSG *pMsg);

    void _OnBeforeNavigate2(LPDISPATCH lpDisp,
                            VARIANT FAR *URL,
                            VARIANT FAR *Flags,
                            VARIANT FAR *TargetFrameName,
                            VARIANT FAR *PostData,
                            VARIANT FAR *Headers,
                            VARIANT_BOOL *Cancel);
    void _OnDownloadBegin();
    void _OnProgressChange(long progress,long progress_max);
    void _OnDownloadComplete();
    void _OnDocumentComplete(LPDISPATCH lpDisp,VARIANT FAR* URL);
    void _OnNavigateComplete2(LPDISPATCH lpDisp,VARIANT FAR* URL);
    void _OnStatusTextChange(BSTR bstrText);
    void _OnTitleChange(BSTR bstrText);
	void _OnPrintTemplateInstantiation(LPDISPATCH pDisp);
	void _OnPrintTemplateTeardown(LPDISPATCH pDisp);

    BEGIN_MSG_MAP(SimpleBrowser)
        MESSAGE_HANDLER(WM_SIZE, OnSize)
    END_MSG_MAP()

private:
    CWindow                 _BrowserWindow;
	IWebBrowser2			*_Browser;
	bool					_Ready;
    IDispatch               *_BrowserDispatch;
	CString					_Content;
	DWORD					_dwCookie;
	
	void _ContentWrite();
	void _ConnectEvents();
	void _DisconnectEvents();

public:
	STDMETHODIMP QueryInterface(REFIID riid, void **ppvObject);
	STDMETHODIMP_(ULONG) AddRef();
	STDMETHODIMP_(ULONG) Release();
	STDMETHODIMP GetTypeInfoCount(UINT *pctinfo);
	STDMETHODIMP GetTypeInfo(UINT iTInfo, LCID lcid, ITypeInfo **ppTInfo);
	STDMETHODIMP GetIDsOfNames(REFIID riid, LPOLESTR *rgszNames, UINT cNames, LCID lcid, DISPID *rgDispId);
	STDMETHODIMP Invoke(DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS *pDispParams, VARIANT *pVarResult, EXCEPINFO *pExcepInfo, UINT *puArgErr);
};

#endif
