#pragma once

#include "DittoAddin.h"
#include <vector>
#include <map>


class CDittoAddins
{
public:
	CDittoAddins(void);
	~CDittoAddins(void);

	bool LoadAll();
	bool UnloadAll();

	bool Loaded()	{ return m_Addins.size() > 0; }

	bool AddPrePasteAddinsToMenu(CMenuHandle *pMenu);
	bool CallPrePasteFunction(int Id, IClip *pClip);
	void AboutScreenText(CStringArray &arr);

protected:
	std::vector<CDittoAddin*> m_Addins;

	class CFunctionLookup
	{
	public:
		CDittoAddin *m_pAddin;
		CStringA m_csFunctionName;
	};

	std::map<int, CFunctionLookup> m_FunctionMap;

protected:
	void LoadDittoInfo(CDittoInfo &DittoInfo);
};
