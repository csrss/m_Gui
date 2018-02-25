#include "StdAfx.h"
#include "GdiPlusDraw.h"

CGdiPlusDraw::CGdiPlusDraw(HDC hDC) : CMemDraw(hDC), m_pGraphics(NULL)
{
	if(hDC != NULL)	// we can get graphics just now, man
	{
		m_pGraphics = Gdiplus::Graphics::FromHDC(__super::operator HDC());
	}
}

CGdiPlusDraw::~CGdiPlusDraw(void)
{
	if(m_pGraphics) delete m_pGraphics;
}

CGdiPlusDraw::operator Gdiplus::Graphics *()
{
	return m_pGraphics;
}

void CGdiPlusDraw::InitGraphics()
{
	m_pGraphics = Gdiplus::Graphics::FromHDC(__super::operator HDC());
}

Gdiplus::Graphics * CGdiPlusDraw::Graphics()
{
	return m_pGraphics;
}

Gdiplus::Font * CGdiPlusDraw::GetGdiPlusFont()
{
	HFONT hFont;
	Gdiplus::Font * pRetFont = NULL;
	CFont * pFont = CWnd::FromHandle(__super::operator HWND())->GetFont();
	if(pFont == NULL)
	{
		hFont = (HFONT)::GetStockObject(DEFAULT_GUI_FONT);
		if(hFont != NULL)
		{
			pFont = CFont::FromHandle(hFont);
		}
	}
	LOGFONTW lg;
	pFont->GetLogFont(&lg);
	pRetFont = new Gdiplus::Font(__super::operator HDC(), &lg);
	return pRetFont;
}

Gdiplus::RectF CGdiPlusDraw::GetGdiPlusRect()
{
	return Gdiplus::RectF((float)__super::operator RECT().left,
						  (float)__super::operator RECT().top,
						  (float)__super::operator RECT().right,
						  (float)__super::operator RECT().bottom);
}
