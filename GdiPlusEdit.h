#pragma once

#pragma warning(disable: 4482)

#include "MemDraw.h"

class CGdiPlusEdit : public CEdit
{
	DECLARE_DYNAMIC(CGdiPlusEdit)

protected:

	CMemDraw				m_MemDraw;
	bool					m_bNeedRepaint;
	Gdiplus::CachedBitmap * m_pCachedBitmap;
	CRect					m_rcClient;
	DWORD					m_dwTextColor;
	bool					m_bDoubleBorder;
	DWORD					m_dwOuterBorderColor;
	int						m_nBorderCornerRadius;
	DWORD					m_dwInnerBorderColor;
	int						m_nBorderWeight;

public:

	CGdiPlusEdit();
	virtual ~CGdiPlusEdit();

	void TextColor(DWORD dwValue);
	void DoubleBorder(bool bValue);
	void OuterBorderColor(DWORD dwValue);
	void BorderCornerRadius(int fValue);
	void InnerBorderColor(DWORD dwValue);
	void BorderWeight(int fValue);

protected:

	BOOL Redraw(BOOL bUpdateNow);
	void DrawRoundRect(	Gdiplus::Graphics *g, 
						Gdiplus::Rect rect, 
						Gdiplus::Color color, 
						int nDiameter, 
						int Line);
	Gdiplus::GraphicsPath * CreateRoundRectPath(Gdiplus::Rect r, int d);
	SIZE GetTextSize(const CString & strText);

	virtual void OnDraw(CDC *pDC);
	virtual void OnDrawContext(Gdiplus::Graphics * pGraphics, Gdiplus::Rect & rcContainer);
	virtual Gdiplus::Font * OnGetFont(Gdiplus::Graphics *pGraphics);
	virtual void OnDrawBorder(Gdiplus::Graphics *pGraphics,
							  Gdiplus::Rect & rcContainer);
	virtual Gdiplus::GraphicsPath * OnDrawSolidBkgnd(Gdiplus::Graphics *pGraphics,
								  Gdiplus::Rect & rcContainer);

	DECLARE_MESSAGE_MAP()

	void OnSize(UINT nType, int cx, int cy);
	HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	BOOL OnEraseBkgnd(CDC* pDC);
	void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	void OnUpdateUIState(UINT /*nAction*/, UINT /*nUIElement*/);
	void OnPaint();
	void OnSetFocus(CWnd* pOldWnd);
	void OnKillFocus(CWnd* pNewWnd);
public:
	afx_msg void OnNcPaint();
};
