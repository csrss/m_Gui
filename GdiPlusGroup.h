#pragma once
#pragma warning(disable: 4482)
#include "MemDraw.h"

class CGdiPlusGroup : public CStatic
{
	DECLARE_DYNAMIC(CGdiPlusGroup)

protected:

	CMemDraw	m_MemDraw;
	bool		m_bNeedRepaint;
	DWORD		m_dwTextColor;
	bool		m_bDoubleBorder;
	DWORD		m_dwOuterBorderColor;
	float		m_nBorderCornerRadius;
	float		m_nBorderWeight;
	DWORD		m_dwInnerBorderColor;

public:
	CGdiPlusGroup();
	virtual ~CGdiPlusGroup();

	void UpdateControl();

protected:

	BOOL Redraw(BOOL bUpdateNow);
	void DrawRoundRect(	Gdiplus::Graphics *g, Gdiplus::RectF rect, 
						Gdiplus::Color color, float nDiameter, float Line);
	Gdiplus::GraphicsPath * CreateRoundRectPath(Gdiplus::RectF r, float d);

	DECLARE_MESSAGE_MAP()

	virtual void OnDraw(CDC *pDC);
	virtual void OnDrawContext(Gdiplus::Graphics * pGraphics, Gdiplus::RectF & rcContainer);
	virtual Gdiplus::Font * OnGetFont(Gdiplus::Graphics * pGraphics, HDC hdc);
	virtual void OnDrawText(Gdiplus::Graphics * pGraphics, Gdiplus::RectF & rcContainer, Gdiplus::Font * pFont);
	virtual void OnDrawBorder(Gdiplus::Graphics *pGraphics,
							  Gdiplus::RectF & rcContainer);
	virtual void OnDrawBackground(	Gdiplus::Graphics *pGraphics,
									Gdiplus::RectF & rcContainer);

	HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	BOOL OnEraseBkgnd(CDC* pDC);
	void OnSize(UINT nType, int cx, int cy);
	void OnUpdateUIState(UINT /*nAction*/, UINT /*nUIElement*/);
	void OnPaint();
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
};


