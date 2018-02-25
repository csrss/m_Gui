#pragma once

#include "MemDraw.h"

class CGdiPlusListCtrlHeader : public CHeaderCtrl
{
	DECLARE_DYNAMIC(CGdiPlusListCtrlHeader)
	
	BOOL	m_bMouseTrack;
	FLOAT	m_fGlowLevel;
	UINT	m_uGlowTimer;

public:

	CGdiPlusListCtrlHeader();
	virtual ~CGdiPlusListCtrlHeader();

protected:

	DECLARE_MESSAGE_MAP()
	virtual void PreSubclassWindow();
	void OnSize(UINT nType, int cx, int cy);
	void OnUpdateUIState(UINT /*nAction*/, UINT /*nUIElement*/);
	void OnNcPaint();
	void OnPaint();
	LRESULT OnLayout(WPARAM wParam, LPARAM lParam);

	Gdiplus::GraphicsPath * CGdiPlusListCtrlHeader::CreateRoundRectPath(Gdiplus::Rect r, int d);
	int CurrentColumnNr(const CPoint & pt);

	virtual void OnDraw(CDC *pDC);
	virtual void OnDrawContext(Gdiplus::Graphics * pGraphics, Gdiplus::Rect & rcContainer);
	virtual void OnItemDraw(Gdiplus::Graphics * pGraphics, const Gdiplus::Rect & rc, LPTSTR lpItemText);
	virtual void OnDrawGlowBkgnd(Gdiplus::Graphics * pGraphics, const Gdiplus::Rect & rcContainer);
protected:

	CMemDraw				m_MemDraw;
	bool					m_bNeedRepaint;
	CRect					m_rcClient;
	int						m_nBorderCornerRadius;
	DWORD					m_argbGlow;
	
public:
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnMouseHover(UINT nFlags, CPoint point);
	afx_msg void OnMouseLeave();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
};
