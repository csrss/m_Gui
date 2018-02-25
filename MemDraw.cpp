#include "StdAfx.h"
#include "MemDraw.h"

CMemDraw::CMemDraw(HDC hDC) : m_hDC(NULL)
{
	if(hDC)
	{
		Assign(hDC);
	}
}

CMemDraw::~CMemDraw(void)
{
}

CMemDraw::operator HDC()
{
	return m_hDC;
}

CMemDraw::operator HWND()
{
	return m_hWnd;
}

CMemDraw::operator RECT()
{
	return m_rcPaint;
}

CMemDraw::operator CMemDraw*()
{
	return this;
}

void CMemDraw::Assign(HDC hDC)
{
	::DeleteObject(m_hCompatBmp);
	::DeleteDC(m_hDC);
	m_hDC = ::CreateCompatibleDC(hDC);

	m_hWnd = ::WindowFromDC(hDC);
	::GetClientRect(m_hWnd, &m_rcPaint);

	m_hCompatBmp = ::CreateCompatibleBitmap(hDC, m_rcPaint.right, m_rcPaint.bottom);
	::SelectObject(m_hDC, m_hCompatBmp);
}

void CMemDraw::Commit(HDC hDC)
{
	::BitBlt(hDC, 0, 0, m_rcPaint.right, m_rcPaint.bottom, m_hDC, 0, 0, SRCCOPY);
}

void CMemDraw::PaintParentBackground()
{
	POINT ptOrg;
	HRGN  hRgn;
	HWND  hWndParent  = GetParent(m_hWnd);
	RECT  rcTranslate = m_rcPaint;
	
	// Map our client area to its position relative to the client area of the parent
	MapWindowPoints(m_hWnd, hWndParent, (LPPOINT)&rcTranslate, 2);
	SetWindowOrgEx(m_hDC, rcTranslate.left, rcTranslate.top, &ptOrg);
	
	// Clip painting to our client area's width and height
	hRgn = CreateRectRgnIndirect(&m_rcPaint);
	SelectClipRgn(m_hDC, hRgn);
	DeleteObject(hRgn);
	
	// Tell the parent to paint to our HDC
	SendMessage(hWndParent, WM_PRINTCLIENT, (WPARAM)m_hDC, PRF_CLIENT);
	
	// Turn off the clipping limit
	SelectClipRgn(m_hDC, NULL);
	
	// Reset the coordinate translation
	SetWindowOrgEx(m_hDC, ptOrg.x, ptOrg.y, NULL);
}

DWORD CMemDraw::Argb(int A, int R, int G, int B)
{
	return ((DWORD(A) << 24) + (DWORD(R) << 16) + (DWORD(G) << 8) + DWORD(B));
}

void CMemDraw::GdiSolidFill(DWORD dwRgb, LPRECT lpRect)
{
	HBRUSH hBrush = ::CreateSolidBrush(dwRgb);
	::FillRect(m_hDC, (lpRect ? lpRect : &m_rcPaint), hBrush);
}

void CMemDraw::GdiGradientFill(LPRECT rect, COLORREF dwRgbStart, COLORREF dwRgbEnd)
{
	COLOR16 sR = (COLOR16)LOBYTE(dwRgbStart);
	COLOR16 sG = (COLOR16)LOBYTE(((WORD)(dwRgbStart)) >> 8);
	COLOR16 sB = (COLOR16)LOBYTE((dwRgbStart) >> 16);
	COLOR16 eR = (COLOR16)LOBYTE(dwRgbEnd);
	COLOR16 eG = (COLOR16)LOBYTE(((WORD)(dwRgbEnd)) >> 8);
	COLOR16 eB = (COLOR16)LOBYTE((dwRgbEnd) >> 16);

	TRIVERTEX vertex[2];
	vertex[0].x = rect ? rect->left : m_rcPaint.left;
	vertex[0].y = rect ? rect->top : m_rcPaint.top;
	vertex[0].Alpha = (COLOR16)0x0000;
	vertex[0].Blue = (COLOR16)sR * sR;
	vertex[0].Green = (COLOR16)sG * sG;
	vertex[0].Red = (COLOR16)sB * sB;

	vertex[1].x = rect ? rect->right : m_rcPaint.right;
	vertex[1].y = rect ? rect->bottom : m_rcPaint.bottom;
	vertex[1].Alpha = (COLOR16)0x0000;
	vertex[1].Blue = (COLOR16)eR * eR;
	vertex[1].Green = (COLOR16)eG * eG;
	vertex[1].Red = (COLOR16)eB * eB;

	GRADIENT_RECT mesh;
	mesh.UpperLeft = 0;
	mesh.LowerRight = 1;

	::GradientFill(m_hDC, vertex, 2, &mesh, 1, GRADIENT_FILL_RECT_V);
}

void CMemDraw::GdiDrawText(LPTSTR lpText, 
						   COLORREF dwTextColor, 
						   LPRECT lpRect, 
						   BOOLEAN bUnderline,
						   BOOLEAN bBold,
						   BOOLEAN bItalic,
						   bool bAbsoluteCenter)
{
	UINT uFormat = 0;

	LONG dwStyle = ::GetWindowLongPtr(m_hWnd, GWL_STYLE);
	if((dwStyle & SS_CENTERIMAGE) == SS_CENTERIMAGE || (dwStyle & BS_VCENTER) == BS_VCENTER)
	{
		uFormat |= DT_VCENTER | DT_SINGLELINE;
	}
	if((dwStyle & SS_CENTER) == SS_CENTER || (dwStyle & BS_CENTER) == BS_CENTER)
	{
		uFormat |= DT_CENTER | DT_WORDBREAK;
	}
	else if((dwStyle & SS_RIGHT) == SS_RIGHT || (dwStyle & BS_RIGHT) == BS_RIGHT)
	{
		uFormat |= DT_RIGHT | DT_WORDBREAK;
	}
	else if((dwStyle & SS_LEFT) == SS_LEFT || (dwStyle & BS_LEFT) == BS_LEFT)
	{
		uFormat |= DT_LEFT | DT_WORDBREAK;
	}
	else
	{
		uFormat |= DT_CENTER | DT_SINGLELINE | DT_VCENTER;
	}

	if(bAbsoluteCenter)
	{
		uFormat = DT_CENTER | DT_WORDBREAK;
	}

	HFONT hFontOld = NULL;
	HFONT hFont = (HFONT)::GetStockObject(DEFAULT_GUI_FONT);

	LOGFONT fontAttributes = { 0 };
	::GetObject(hFont, sizeof(fontAttributes), &fontAttributes);
	fontAttributes.lfQuality = CLEARTYPE_QUALITY;
	fontAttributes.lfUnderline = bUnderline;
	if(bBold)
	{
		fontAttributes.lfWeight = FW_BOLD;
	}
	if(bItalic)
	{
		fontAttributes.lfItalic = TRUE;
	}
	HFONT hFontEx = CreateFontIndirect(&fontAttributes);

	hFontOld = (HFONT) SelectObject(m_hDC, hFontEx);

	::SetBkMode(m_hDC, TRANSPARENT);
	::SetTextColor(m_hDC, dwTextColor);
	::DrawText(m_hDC, lpText, -1, lpRect ? lpRect : &m_rcPaint, uFormat);
	::SelectObject(m_hDC, hFontOld);
}

void CMemDraw::GdiDrawGlowingText(LPTSTR lpText, LPRECT lpRect)
{
	UINT uFormat = 0;
	LONG dwStyle = ::GetWindowLongPtr(m_hWnd, GWL_STYLE);
	if((dwStyle & SS_CENTERIMAGE) == SS_CENTERIMAGE || (dwStyle & BS_VCENTER) == BS_VCENTER)
	{
		uFormat |= DT_VCENTER | DT_SINGLELINE;
	}
	if((dwStyle & SS_CENTER) == SS_CENTER || (dwStyle & BS_CENTER) == BS_CENTER)
	{
		uFormat |= DT_CENTER;
	}
	else if((dwStyle & SS_RIGHT) == SS_RIGHT || (dwStyle & BS_RIGHT) == BS_RIGHT)
	{
		uFormat |= DT_RIGHT;
	}
	else if((dwStyle & SS_LEFT) == SS_LEFT || (dwStyle & BS_LEFT) == BS_LEFT)
	{
		uFormat |= DT_LEFT;
	}
	RECT lpClientRect;
	if(lpRect == NULL)
	{
		::GetClientRect(m_hWnd, &lpClientRect);
	}
	else
	{
		lpClientRect.bottom = lpRect->bottom;
		lpClientRect.left = lpRect->left;
		lpClientRect.right = lpRect->right;
		lpClientRect.top = lpRect->top;
	}
	HTHEME hTheme = ::OpenThemeData(m_hWnd, TEXT("button"));
	if(!hTheme)
	{
		return;
	}
	HDC hdcPaint = ::CreateCompatibleDC(m_hDC);
	int cx = RECTWIDTH(lpClientRect);
	int cy = RECTHEIGHT(lpClientRect);
	int BIT_COUNT = 32;
	BITMAPINFO dib = { 0 };
	dib.bmiHeader.biSize            = sizeof(BITMAPINFOHEADER);
	dib.bmiHeader.biWidth           = cx;
	dib.bmiHeader.biHeight          = -cy;
	dib.bmiHeader.biPlanes          = 1;
	dib.bmiHeader.biBitCount        = BIT_COUNT;
	dib.bmiHeader.biCompression     = BI_RGB;
	HBITMAP hbm = CreateDIBSection(m_hDC, &dib, DIB_RGB_COLORS, NULL, NULL, 0);
	if(hbm)
	{
		HBITMAP hbmOld = (HBITMAP)SelectObject(hdcPaint, hbm);
		DTTOPTS DttOpts = {sizeof(DTTOPTS)};
		DttOpts.dwFlags = DTT_COMPOSITED | DTT_GLOWSIZE;
		DttOpts.iGlowSize = 10;
		DttOpts.crText   = RGB(255, 255, 255);

		LOGFONT lgFont;
		HFONT hFontOld = NULL;
		if (SUCCEEDED(::GetThemeSysFont(hTheme, TMT_CAPTIONFONT, &lgFont)))
		{
			HFONT hFont = CreateFontIndirect(&lgFont);
			hFontOld = (HFONT) SelectObject(hdcPaint, hFont);
		}

		::DrawThemeTextEx(	hTheme, 
							hdcPaint, 
							0, 0, 
							lpText, 
							-1, 
							uFormat, 
							&lpClientRect, 
							&DttOpts);
		::BitBlt(m_hDC, 0, 0, cx, cy, hdcPaint, 0, 0, SRCCOPY);
		::SelectObject(hdcPaint, hbmOld);
		if (hFontOld) ::SelectObject(hdcPaint, hFontOld);
		::DeleteObject(hbm);
	}
	::ReleaseDC(m_hWnd, hdcPaint);
	::DeleteDC(hdcPaint);
	::CloseThemeData(hTheme);
}

LPSTR CMemDraw::GdiGetWindowTextA(HWND hWnd)
{
	LPSTR lpOutput = NULL;
	int Len = ::GetWindowTextLengthA(hWnd ? hWnd : m_hWnd);
	if(Len > 0)
	{
		lpOutput = (PCHAR)::HeapAlloc(::GetProcessHeap(),
			HEAP_ZERO_MEMORY, sizeof(CHAR) * (Len + 1));
		if(lpOutput)
		{
			::GetWindowTextA(hWnd ? hWnd : m_hWnd, lpOutput, Len + 1);
			return lpOutput;
		}
	}
	return NULL;
}

LPWSTR CMemDraw::GdiGetWindowTextW(HWND hWnd)
{
	LPWSTR lpOutput = NULL;
	int Len = ::GetWindowTextLengthW(hWnd ? hWnd : m_hWnd);
	if(Len > 0)
	{
		lpOutput = (PWCHAR)::HeapAlloc(::GetProcessHeap(),
			HEAP_ZERO_MEMORY, sizeof(WCHAR) * (Len + 1));
		if(lpOutput)
		{
			::GetWindowTextW(hWnd ? hWnd : m_hWnd, lpOutput, Len + 1);
			return lpOutput;
		}
	}
	return NULL;
}

LPWSTR CMemDraw::GdiGetWindowText(HWND hWnd)
{
#ifdef _UNICODE
	return GdiGetWindowTextW(hWnd ? hWnd : m_hWnd);
#else
	LPSTR lpTemp = GdiGetWindowTextA(hWnd ? hWnd : m_hWnd);
	if(lpTemp)
	{
		LPWSTR lpOut = Unicode(lpTemp);
		::HeapFree(::GetProcessHeap(), 0, lpTemp);
		lpTemp = NULL;
		return lpOut;
	}
	return NULL;
#endif
}

SIZE CMemDraw::GdiGetTextSize(LPTSTR lpString, HDC hdc)
{
	SIZE szOutput;
	szOutput.cx = 0; szOutput.cy = 0;
	if(!lpString) return szOutput;
	RECT rcTextRect;
	rcTextRect.left = 0;
	rcTextRect.top = 0;
	::DrawTextW(hdc ? hdc : m_hDC, lpString, _tcslen(lpString), &rcTextRect, DT_CALCRECT);
	szOutput.cx = rcTextRect.right;
	szOutput.cy = rcTextRect.bottom;
	return szOutput;
}

LPWSTR CMemDraw::Unicode(LPSTR lpString)
{
	DWORD dwWritten;
	int wcsChars;
	wcsChars = (int)strlen(lpString);
	wchar_t *pmem = NULL;
	pmem = (wchar_t *)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(wchar_t) * (wcsChars + 1));
	if(pmem)
	{
		dwWritten = MultiByteToWideChar(CP_UTF8, 0, lpString, -1, pmem, 
			(sizeof(wchar_t) * (wcsChars + 1)));
	}
	return pmem; // must be freed
}

int CMemDraw::X()const
{
	return m_rcPaint.left;
}

int CMemDraw::Y()const
{
	return m_rcPaint.top;
}

int CMemDraw::Width()const
{
	return m_rcPaint.right;
}

int CMemDraw::Height()const
{
	return m_rcPaint.bottom;
}

void CMemDraw::Redraw(bool bUpdateNow, bool * bFlag)
{
	* bFlag = true;
	bUpdateNow ? 
		::RedrawWindow(m_hWnd, NULL, NULL, RDW_NOERASE | RDW_INVALIDATE | RDW_UPDATENOW)
	:
		::InvalidateRect(m_hWnd, NULL, FALSE);
}

void CMemDraw::GdiStringAllocateCopy(LPTSTR &lpDestination, LPTSTR lpSource)
{
	if(!lpSource || _tcslen(lpSource) <= 0)
	{
		return;	// just do nothing
	}
	if(lpDestination)
	{
		::HeapFree(::GetProcessHeap(), 0, lpDestination);
		lpDestination = NULL;
	}
	lpDestination = (LPTSTR)::HeapAlloc(::GetProcessHeap(), HEAP_ZERO_MEMORY, 
		sizeof(TCHAR) * (_tcslen(lpSource) + 1));
	if(lpDestination)
	{
		_tcscpy(lpDestination, lpSource);
	}
}