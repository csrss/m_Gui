#pragma once
#include "MemDraw.h"
#pragma warning(disable: 4482)

class CGdiPlusRadio : public CButton
{
	DECLARE_DYNAMIC(CGdiPlusRadio)

	FLOAT	m_fGlowLevel;
	UINT	m_uGlowTimer;
	BOOL	m_bMouseTrack;

public:

	enum ButtonState
	{
		StateRegular,
		StateOver,
		StatePushed
	};

protected:

	CMemDraw				m_MemDraw;
	bool					m_bNeedRepaint;
	CRect					m_rcClient;
	ButtonState				m_button_state;
	DWORD					m_argbGlow;
	bool					m_bDoubleBorder;
	DWORD					m_dwOuterBorderColor;
	DWORD					m_dwInnerBorderColor;
	float					m_nBorderWeight;
	DWORD					m_dwTextColor;
	CString					m_strWindowText;
	bool					m_bChecked;

public:

	CGdiPlusRadio();
	virtual ~CGdiPlusRadio();

protected:

	BOOL Redraw(BOOL bUpdateNow);
	Gdiplus::GraphicsPath *CreateRoundRectPath(Gdiplus::Rect r, int d);
	void DrawRoundRect(	Gdiplus::Graphics *g, Gdiplus::Rect rect, 
						Gdiplus::Color color, int nDiameter, int Line);
	Gdiplus::Font * OnGetFont(Gdiplus::Graphics *pGraphics);

	DECLARE_MESSAGE_MAP()

	virtual void OnDraw(CDC *pDC);
	virtual void OnDrawContext(Gdiplus::Graphics * pGraphics, Gdiplus::Rect & rcContainer);
	virtual void OnDrawBorder(Gdiplus::Graphics *pGraphics,
							  Gdiplus::Rect & rcContainer);
	virtual void OnDrawBackground(	Gdiplus::Graphics *pGraphics,
									Gdiplus::Rect & rcContainer);
	virtual void OnCalcText(Gdiplus::Graphics * pGraphics, 
							CStringW strText, 
							Gdiplus::Rect & rcContainer, 
							Gdiplus::Font * pTextFont);
	virtual void OnDrawText(Gdiplus::Graphics * pGraphics, 
							CStringW strText, 
							Gdiplus::Rect & rcContainer, 
							Gdiplus::Font * pTextFont,
							bool bCalcRect = false);

	HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	BOOL OnEraseBkgnd(CDC* pDC);
	void OnSize(UINT nType, int cx, int cy);
	void OnDestroy();
	void OnPaint();
	void OnLButtonDblClk(UINT nFlags, CPoint point);
	void OnLButtonDown(UINT nFlags, CPoint point);
	void OnLButtonUp(UINT nFlags, CPoint point);
	void OnMouseHover(UINT nFlags, CPoint point);
	void OnMouseLeave();
	void OnMouseMove(UINT nFlags, CPoint point);
	void OnUpdateUIState(UINT /*nAction*/, UINT /*nUIElement*/);
	void OnTimer(UINT_PTR nIDEvent);
	LRESULT OnSetText(WPARAM wParam, LPARAM lParam);
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
};


