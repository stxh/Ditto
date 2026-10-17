#pragma once

#include "CGdiPlusBitmap.h"
#include "DPI.h"

class CGdiImageDrawer
{
public:
	CGdiImageDrawer();
	~CGdiImageDrawer();

	BOOL LoadStdImage(UINT id, LPCTSTR pType);
	BOOL LoadStdImageDPI(int dpi, UINT id96, UINT id120, UINT id144, UINT id168, UINT id192, LPCTSTR pType, UINT id225 = 0, UINT id250 = 0, UINT id275 = 0, UINT id300 = 0, UINT id325 = 0, UINT id350 = 0);
	void Draw(HDC hScreenDC, CDPI &dpi, HWND hWnd, int posX, int posY, bool mouseHover, bool mouseDown, int forceWidth = INT_MAX, int forceHeight = INT_MAX);
	void Draw(HDC hScreenDC, CDPI &dpi, HWND hWnd, CRect rc, bool mouseHover, bool mouseDown);
	BOOL LoadRaw(unsigned char* bitmapData, int imageSize);

	UINT ImageWidth() { return m_pStdImage->m_pBitmap->GetWidth(); }
	UINT ImageHeight() { return m_pStdImage->m_pBitmap->GetHeight(); }

	void Reset();

protected:
	CGdiPlusBitmapResource* m_pStdImage;
};

