#pragma once
#include "RichEditCtrlEx.h"

class CConvertRTFToText : public ATL::CWindow
{
public:
	BOOL Create();
	CString GetTextFromRTF(CStringA rtf);

private:
	CRichEditCtrlEx m_richEditTextConverter;

};

