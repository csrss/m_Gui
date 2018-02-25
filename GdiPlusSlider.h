#pragma once

#include "MemDraw.h"
#pragma warning(disable: 4482)
#ifndef ARGB
#define ARGB(A, R, G, B) ((DWORD(A) << 24) + (DWORD(R) << 16) + (DWORD(G) << 8) + DWORD(B))
#endif

class CGdiPlusSlider : public CSliderCtrl
{
	DECLARE_DYNAMIC(CGdiPlusSlider)

public:

	enum ButtonState
	{
		StateRegular,
		StateOver,
		StatePushed
	};

protected:

	CMemDraw	m_MemDraw;
	bool		m_bNeedRepaint;
	ButtonState m_button_state;
	BOOL		m_bMouseTrack;
	BOOL		m_bThumbDoubleBorder;
	DWORD		m_dwThumbOuterBorderColor;
	DWORD		m_dwThumbBorderColor;
	DWORD		m_dwThumbPushedBackground;
	DWORD		m_dwThumbRegularBackground;
	DWORD		m_dwStartGradientThumbBkgColor;
	DWORD		m_dwEndGradientThumbBkgColor;
	DWORD		m_argbGlow;
	float		m_fGlowLevel;

public:
	CGdiPlusSlider();
	virtual ~CGdiPlusSlider();

protected:

	virtual void OnDraw(CDC * pDC);
	virtual void OnDrawContext(Gdiplus::Graphics * pGraphics, Gdiplus::Rect & rcContainer);
	virtual void OnDrawBackground(Gdiplus::Graphics * pGraphics, Gdiplus::Rect & rcContainer);
	virtual void OnDrawThumb(Gdiplus::Graphics * pGraphics, Gdiplus::Rect & rcContainer);

	void Redraw(BOOL bUpdateNow);
	Gdiplus::GraphicsPath * CreateRoundRectPath(Gdiplus::Rect r, int d);
	void DrawRoundRect(Gdiplus::Graphics *g, Gdiplus::Rect rect, Gdiplus::Color color, int nDiameter, int Line);
	void OnDrawThumbBorder(Gdiplus::Graphics *pGraphics, Gdiplus::Rect & rcContainer);

protected:

	DECLARE_MESSAGE_MAP()

	HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	BOOL OnEraseBkgnd(CDC* pDC);
	void OnUpdateUIState(UINT /*nAction*/, UINT /*nUIElement*/);
	void OnSize(UINT nType, int cx, int cy);
	void OnPaint();
	void OnNcPaint();
	void OnLButtonDblClk(UINT nFlags, CPoint point);
	void OnLButtonDown(UINT nFlags, CPoint point);
	void OnLButtonUp(UINT nFlags, CPoint point);
	void OnMouseHover(UINT nFlags, CPoint point);
	void OnMouseLeave();
	void OnMouseMove(UINT nFlags, CPoint point);
};


