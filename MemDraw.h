#pragma once

#pragma warning(disable: 4996)

#define RECTWIDTH(lpRect) ((lpRect).right - (lpRect).left)
#define RECTHEIGHT(lpRect) ((lpRect).bottom - (lpRect).top)

class CMemDraw
{
public:

	CMemDraw(HDC hDC = NULL);
	virtual ~CMemDraw(void);

	operator HDC();
	operator HWND();
	operator RECT();

	operator CMemDraw*();

	void Assign(HDC hDC);
	void Commit(HDC hDC);
	void Redraw(bool bUpdateNow, bool * bFlag);

	void PaintParentBackground(void);
	DWORD Argb(int A, int R, int G, int B);

	void GdiSolidFill(DWORD dwRgb, LPRECT lpRect = NULL);
	void GdiGradientFill(LPRECT rect, COLORREF dwRgbStart, COLORREF dwRgbEnd);
	void GdiDrawText(LPTSTR lpText, 
					 COLORREF dwTextColor, 
					 LPRECT lpRect = NULL,
					 BOOLEAN bUnderline = FALSE,
					 BOOLEAN bBold = FALSE,
					 BOOLEAN bItalic = FALSE,
					 bool bAbsoluteCenter = false);
	void GdiDrawGlowingText(LPTSTR lpText, LPRECT lpRect = NULL);
	LPSTR GdiGetWindowTextA(HWND hWnd = NULL);
	LPWSTR GdiGetWindowTextW(HWND hWnd = NULL);
	LPWSTR GdiGetWindowText(HWND hWnd = NULL);
	SIZE GdiGetTextSize(LPTSTR lpString, HDC hdc = NULL);
	void GdiStringAllocateCopy(LPTSTR &lpDestination, LPTSTR lpSource);

	int X()const;
	int Y()const;
	int Width()const;
	int Height()const;

protected:

	LPWSTR Unicode(LPSTR lpString);

protected:

	HBITMAP m_hCompatBmp;
	HDC		m_hDC;
	RECT	m_rcPaint;
	HWND	m_hWnd;
};
