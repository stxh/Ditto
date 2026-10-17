#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define PROGRESS_IDC_CANCEL   10
#define PROGRESS_IDC_TEXT     11
#define PROGRESS_IDC_PROGRESS 12

class CProgressWnd : public ATL::CWindowImpl<CProgressWnd>
{
// Construction/Destruction
public:
	DECLARE_WND_CLASS_EX(_T("Ditto_ProgressWnd"), CS_HREDRAW | CS_VREDRAW, (HBRUSH)(COLOR_BTNFACE + 1))

    CProgressWnd();
    CProgressWnd(HWND hWndParent, LPCTSTR pszTitle, BOOL bSmooth = FALSE);
    virtual ~CProgressWnd();

    BOOL Create(HWND hWndParent, LPCTSTR pszTitle, BOOL bSmooth = FALSE);
    BOOL GoModal(LPCTSTR pszTitle = _T("Progress"), BOOL bSmooth = FALSE);

	// bring in the window-creating overload used by Create below
	using ATL::CWindowImpl<CProgressWnd>::Create;

protected:
    void CommonConstruct();

// Operations
public:
    void SetRange(int nLower, int nUpper, int nStep = 1);
                                                    // Set range and step size
    int OffsetPos(int nPos);                        // Same as CProgressCtrl
    int StepIt();                                   //    "
    int SetStep(int nStep);                         //    "
    int SetPos(int nPos);                           //    "

    void SetText(LPCTSTR fmt, ...);                 // Set text in text area

    void Clear();                                   // Clear text, reset bar
    void Hide();                                    // Hide window
    void Show();                                    // Show window
	void HideCancel();
	void ShowCancel();
	int	 GetPos() const		{ return m_nPrevPos;	}

    BOOL Cancelled() { return m_bCancelled; }       // Was "Cancel" hit?

	void SetWindowSize(int nNumTextLines, int nWindowWidth = 390);

    void PeekAndPump(BOOL bCancelOnESCkey = TRUE);  // Message pumping for modal operation   

	void SetTitleText(CString csTitle);
    
// Implementation
protected:
    void GetPreviousSettings();
    void SaveCurrentSettings();
    void CenterOnParent();

protected:
    BOOL m_bCancelled;
    BOOL m_bModal;
    BOOL m_bPersistantPosition;
    int  m_nPrevPos, m_nPrevPercent;
    int  m_nStep;
    int  m_nMaxValue, m_nMinValue;
    int  m_nNumTextLines;

    HWND m_hWndParent;

    WTL::CStatic        m_Text;
    WTL::CProgressBarCtrl m_wndProgress;
    WTL::CButton        m_CancelButton;
    CString             m_strTitle,
                        m_strCancelLabel;
    WTL::CFont          m_font;

// Overrides
public:
	BOOL DestroyWindow();

// Message handlers
protected:
	LRESULT OnEraseBkgnd(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnCancelClick(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	void OnCancel();

	BEGIN_MSG_MAP(CProgressWnd)
		MESSAGE_HANDLER(WM_ERASEBKGND, OnEraseBkgnd)
		MESSAGE_HANDLER(WM_CLOSE, OnClose)
		COMMAND_ID_HANDLER(PROGRESS_IDC_CANCEL, OnCancelClick)
	END_MSG_MAP()
};
