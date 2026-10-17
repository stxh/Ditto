#pragma once

#include "Clip.h"

class CClipboardSaveRestore
{
public:
	CClipboardSaveRestore(void);
	~CClipboardSaveRestore(void);

	bool Save(BOOL textOnly);
	bool Restore();
	void Clear()	{ m_Clipboard.DeleteAll(); }
	bool RestoreTextOnly();

	CClipFormats m_Clipboard;
};
