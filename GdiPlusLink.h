#pragma once

#pragma warning(disable: 4482)
#pragma warning(disable: 4996)

#ifndef ARGB
	#define ARGB(A, R, G, B) ((DWORD(A) << 24) + (DWORD(R) << 16) + (DWORD(G) << 8) + DWORD(B))
#endif

#include "MemDraw.h"

class CGdiPlusLink : public CStatic
{
	DECLARE_DYNAMIC(CGdiPlusLink)


public:

	enum ButtonState
	{
		StateRegular,
		StateOver,
		StatePushed
	};

protected:

	CMemDraw	m_MemDraw;
	CRect		m_rcClient;
	bool		m_bNeedRepaint;
	LPTSTR		m_strWindowText;
	DWORD		m_dwLinkRegularColor;
	DWORD		m_dwLinkVisitedColor;
	DWORD		m_dwLinkHooveredColor;
	DWORD		m_dwLinkDisabledColor;

	ButtonState m_button_state;
	BOOL		m_bMouseTrack;
	LPTSTR		m_strWebSiteLink;
	POINT		m_ptCurrentPoint;
	BOOL		m_bVisited;

private:

	void SetText(LPTSTR lpString);
	LPTSTR GetText();

	void SetVisited(BOOL bValue);
	BOOL GetVisited();

	void SetRgbLinkDisabledColor(DWORD dwValue);
	DWORD GetRgbLinkDisabledColor();

	void SetRgbLinkHooveredColor(DWORD dwValue);
	DWORD GetRgbLinkHooveredColor();

	void SetRgbLinkVisitedColor(DWORD dwValue);
	DWORD GetRgbLinkVisitedColor();

	void SetRgbLinkRegularColor(DWORD dwValue);
	DWORD GetRgbLinkRegularColor();

	void SetWebSiteLink(LPTSTR lpLinkString);
	LPTSTR GetWebSiteLink();

public:

	CGdiPlusLink();
	virtual ~CGdiPlusLink();

	Prop<LPTSTR, CGdiPlusLink>	CG_Text;
	Prop<BOOL, CGdiPlusLink>	CG_Visited;
	Prop<DWORD, CGdiPlusLink>	CG_RgbLinkDisabledColor;
	Prop<DWORD, CGdiPlusLink>	CG_RgbLinkHooveredColor;
	Prop<DWORD, CGdiPlusLink>	CG_RgbLinkVisitedColor;
	Prop<DWORD, CGdiPlusLink>	CG_RgbLinkRegularColor;
	Prop<LPTSTR, CGdiPlusLink>	CG_WebSiteLink;

	void UpdateControl();

protected:	// my shit

	virtual void OnDraw(CDC *pDC);
	virtual void OnDrawContext(Gdiplus::Graphics * pGraphics, Gdiplus::Rect & rc);

protected:

	void Redraw(BOOL bUpdateNow);

	DECLARE_MESSAGE_MAP()

	HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	BOOL OnEraseBkgnd(CDC* pDC);
	void OnSize(UINT nType, int cx, int cy);
	void OnPaint();
	void OnNcPaint();
	void OnLButtonDblClk(UINT nFlags, CPoint point);
	void OnLButtonDown(UINT nFlags, CPoint point);
	void OnLButtonUp(UINT nFlags, CPoint point);
	void OnMouseHover(UINT nFlags, CPoint point);
	void OnMouseLeave();
	void OnMouseMove(UINT nFlags, CPoint point);
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	virtual void PreSubclassWindow();
	BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	LRESULT OnSetText(WPARAM wParam, LPARAM lParam);
	void OnUpdateUIState(UINT /*nAction*/, UINT /*nUIElement*/);
};


