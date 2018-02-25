#pragma once

#include "GdiPlusDraw.h"

#pragma warning(disable: 4482)

class CGdiPlusStatic : public CStatic
{
	DECLARE_DYNAMIC(CGdiPlusStatic)

public:

	enum TextStyles
	{
		Regular, OnGlass
	};

protected:

	bool					m_bNeedRepaint;
	CRect					m_rcClient;
	DWORD					m_dwTextColor;
	CGdiPlusDraw			m_gdiPlus;
	CString					m_strWindowText;
	TextStyles				m_TextStyle;

private:

	void TextColor(DWORD dwArgbColor);
	void Text(LPTSTR lpString);
	void TextStyle(TextStyles Value);

public:

	CGdiPlusStatic();
	virtual ~CGdiPlusStatic();

	// Text color, colorref, no argb here laaaads
	Prop<DWORD, CGdiPlusStatic>			CGPS_TextColor;

	// The text itself
	Prop<LPTSTR, CGdiPlusStatic>		CGPS_Text;

	// Text stylah
	Prop<TextStyles, CGdiPlusStatic>	CGPS_TextStyle;

	// update now, refresh, redraw
	void UpdateControl();

	int Width();
	int Height();

	DWORD MakeArgb(int A, int R, int G, int B);

protected:

	virtual void OnDrawText(Gdiplus::Graphics * pGraphics, Gdiplus::RectF & rcContainer, Gdiplus::Font * pFont);
	virtual void OnDrawContext(Gdiplus::Graphics * pGraphics, Gdiplus::RectF & rcContainer);
	virtual void OnDraw(CDC *pDC);

	void Redraw(bool bUpdateNow);

	DECLARE_MESSAGE_MAP()

	HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	BOOL OnEraseBkgnd(CDC* pDC);
	void OnSize(UINT nType, int cx, int cy);
	void OnDestroy();
	virtual void PreSubclassWindow();
	void OnPaint();
	LRESULT OnSetText(WPARAM wParam, LPARAM lParam);
	void OnNcPaint();
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
};


