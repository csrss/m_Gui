#pragma once
#pragma warning(disable: 4482)

// CGdiStatic

class CGdiStatic : public CStatic
{
	DECLARE_DYNAMIC(CGdiStatic)

public:

	enum FontWeigth
	{
		Dontcare = 0, 
		Thin = 100, 
		Extralight = 200,
		Light = 300, 
		Normal = 400, 
		Medium = 500, 
		Semibold = 600, 
		Bold = 700, 
		Extrabold = 800,
		Heavy = 900
	};

	enum FontCharSet
	{
		Ansi = 0,
		Baltic = 186,
		Chinesebig5 = 136,
		Default_cs = 1,
		Easteurope = 238,
		Gb2312 = 134,
		Greek = 161,
		Hangul = 129,
		Mac = 77,
		Oem = 255,
		Russian = 204,
		Shiftjis = 128,
		Symbol = 2,
		Turkish = 162,
		Vietnamese = 163,
		Johab = 130,
		Arabic = 178,
		Hebrew = 177,
		Thai = 222
	};

	enum FontQuality
	{
		Antialiased = 4,
		Cleartype = 5,
		Default_quality = 0,
		Draft = 1,
		Nonantialiased = 3,
		Proof = 2
	};

public:	// methods
	CGdiStatic();
	virtual ~CGdiStatic();

	virtual void SetupFont(	HDC hdc,
							int nSize = 10,
							FontWeigth fontWeigth = FontWeigth::Normal, 
							bool bItalic = false, 
							bool bUnderline = false, 
							bool bStrikeOut = false, 
							FontQuality fontQuality = FontQuality::Cleartype,
							CString strFontFamily = TEXT("Arial"),
							bool bUpdateNow = false);
	virtual void SetupFont(bool bUnderline, bool bUpdateNow = false);
	virtual void SetupTextColor(DWORD dwColor, bool bUpdateNow = false);
	virtual void SetupLinkColors(DWORD dwRegular, DWORD dwHovered, DWORD dwVisited, bool bUpdateNow = false);
	virtual void SetupLinkData(CString strWebSite, CString strTooltip = TEXT(""), bool bTrackable = false, bool bUpdateNow = false);
	virtual void DestroyTooltip();
	virtual void MakeItLink(bool bUpdateNow = false);
	virtual void MakeItText(bool bUpdateNow = false);
	virtual void UnsetClicked(bool bUpdateNow = true);
	CStringA GetTextA();
	CStringW GetTextW();
	BOOL SetTextA(CStringA strValue, bool bUpdateNow = false);
	BOOL SetTextW(CStringW strValue, bool bUpdateNow = false);

	operator CStringW(){ return GetTextW(); }
	operator CStringA(){ return GetTextA(); }

protected:	// methods

	void OnMouseMove(UINT nFlags, CPoint point);
	void OnSize(UINT nType, int cx, int cy);
	HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	BOOL OnEraseBkgnd(CDC* pDC);
	void OnPaint();
	virtual void PreSubclassWindow();
	void OnLButtonDown(UINT nFlags, CPoint point);
	void OnLButtonUp(UINT nFlags, CPoint point);
	void OnMouseLeave();
	BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);

	DECLARE_MESSAGE_MAP()

	virtual BOOL Redraw(BOOL bUpdateNow);
	virtual void OnDrawText();
	virtual void OnDrawLink();
	virtual void InitTooltip(bool bInit, CString strText, bool bTrackable);

private:	//methods

	HDC PaintParentBackground(HDC hdc, RECT m_rcClient);
	CStringW GetWindowTextWideChar();
	CStringW MultiToWide(CStringA strValue);

public:	// variables

protected:	// variables

	bool m_bNeedRepaint;
	RECT m_rcClientRect;
	bool m_bUseDefaultGuiFont;
	DWORD m_dwRegular, m_dwHovered, m_dwVisited;
	bool m_bIsLink;
	bool m_bClickInitialized;
	bool m_bWasVisited;
	bool m_bMouseHoover;
	CStringW m_strWebSiteLink;
	CStringW m_strLinkTooltip;
	DWORD m_dwCurrentTextColor;
	HWND m_wndTooltip;
	TOOLINFO m_toolInfo;
	bool m_bTooltipTrackable;

private:	// variables

	HDC m_hMemoryDC;
	HBITMAP m_hBitmap;
	HBITMAP m_hOldBitmap;
	LOGFONT m_logCurrentFont;
	CRect m_rcTextRectangle;
	CPoint m_ptCurrentPoint;
	BOOL m_bMouseTrack;
};


