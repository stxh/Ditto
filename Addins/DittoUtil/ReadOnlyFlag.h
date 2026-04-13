#pragma once
#include "..\..\Shared\DittoDefines.h"
#include "..\..\Shared\IClip.h"
#include <vector>

class CReadOnlyFlag
{
public:
	CReadOnlyFlag(void);
	~CReadOnlyFlag(void);

	bool ResetReadOnlyFlag(const CDittoInfo &DittoInfo, IClip *pClip, bool resetFlag);

protected:
	typedef std::vector<CString> CStringVector;
	bool LoadUnicodeFiles(CStringVector &lines, IClipFormats *pFormats);
	bool LoadTextFiles(CStringVector &lines, IClipFormats *pFormats);
	bool LoadHDropFiles(CStringVector &lines, IClipFormats *pFormats);
};

