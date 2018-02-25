#pragma once

#include "MemDraw.h"

#ifndef ARGB
#define ARGB(A, R, G, B) ((DWORD(A) << 24) + (DWORD(R) << 16) + (DWORD(G) << 8) + DWORD(B))
#endif

#pragma warning(disable: 4482)

class CGdiPlusTabCtrl : public CTabCtrl
{
	DECLARE_DYNAMIC(CGdiPlusTabCtrl)

public:

	enum ButtonState
	{
		StateRegular,
		StateOver,
		StatePushed
	};

public:
	CGdiPlusTabCtrl();
	virtual ~CGdiPlusTabCtrl();

	void UpdateControl();

protected:

	BOOL Redraw(BOOL bUpdateNow);
	LPTSTR GetTabButtonText(int nIndex);
	Gdiplus::GraphicsPath * CreateRoundRectPath(Gdiplus::Rect r, int d, bool bCurrentTab = false);
	void DrawRoundRect(	Gdiplus::Graphics * g, Gdiplus::Rect rect, 
						Gdiplus::Color color, int nDiameter, int Line, bool bCurrentTab = false);
	Gdiplus::Font * GetGdiPlusDefaultFont(HDC hdc);
	Gdiplus::Font * GetMyFont(HDC hdc);
	int GetSelectedTabIndex(CPoint & point);
	bool IsSelectedTabByRect(const CRect & rc);

	DECLARE_MESSAGE_MAP()

	void OnUpdateUIState(UINT /*nAction*/, UINT /*nUIElement*/);
	BOOL OnEraseBkgnd(CDC* pDC);
	HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	void OnSize(UINT nType, int cx, int cy);
	void OnPaint();
	void OnTimer(UINT_PTR nIDEvent);
	void OnMouseHover(UINT nFlags, CPoint point);
	void OnMouseLeave();
	void OnMouseMove(UINT nFlags, CPoint point);
	void OnNcPaint();
	void OnLButtonDblClk(UINT nFlags, CPoint point);
	void OnLButtonDown(UINT nFlags, CPoint point);
	void OnLButtonUp(UINT nFlags, CPoint point);

	virtual void OnDraw(CDC *pDC);
	virtual void OnDrawContext(Gdiplus::Graphics * pGraphics, Gdiplus::Rect & rc);
	virtual void OnDrawItemText(Gdiplus::Graphics * pGraphics, Gdiplus::Rect & rc, LPCWSTR lpString);
	virtual void OnDrawTabBackground(Gdiplus::Graphics * pGraphics, Gdiplus::Rect & rc);
	virtual void OnDrawTabBorder(Gdiplus::Graphics * pGraphics, Gdiplus::Rect & rc);

protected:

	bool		m_bNeedRepaint;
	CMemDraw	m_MemDraw;
	int			m_iCurrentTab;
	bool		m_bCurrentTab;
	bool		m_bDoubleTabBorder;
	DWORD		m_dwOuterBorderColor;
	DWORD		m_dwInnerBorderColor;
	int			m_iOuterBorderWeight;
	int			m_iInnerBorderWeight;
	BOOL		m_bMouseTrack;
	ButtonState	m_button_state;
	float		m_fGlowLevel;
	UINT		m_uGlowTimer;
	DWORD		m_argbGlow;

};
