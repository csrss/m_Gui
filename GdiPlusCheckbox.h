#pragma once
#include "GdiPlusDraw.h"
#pragma warning(disable: 4482)

#ifndef ARGB
#define ARGB(A, R, G, B) ((DWORD(A) << 24) + (DWORD(R) << 16) + (DWORD(G) << 8) + DWORD(B))
#endif

class CGdiPlusCheckbox : public CButton
{
	DECLARE_DYNAMIC(CGdiPlusCheckbox)

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

	CGdiPlusDraw	m_MemDraw;
	bool			m_bNeedRepaint;
	CRect			m_rcClient;
	ButtonState		m_button_state;
	bool			m_bChecked;
	DWORD			m_dwBorderColor;
	float			m_nCornerRadius;
	float			m_nWeight;
	DWORD			m_dwCheckedSignColor;
	bool			m_bDoubleBorder;
	DWORD			m_dwOuterBorderColor;
	DWORD			m_argbGlow;
	DWORD			m_dwTextColor;
	CString			m_strWindowText;
	DWORD			m_BoxBkgPushedColor;
	DWORD			m_BoxBkgRegularColor;
	float			m_SignWeight;
	BOOL			m_bEnabled;
	DWORD			m_dwTextColorDisabled;
	int				m_iCheckedSymbolWeight;
	DWORD			m_bCheckedSymbolColor;
	DWORD			m_bCheckedSymbolColorDisabled;
	DWORD			m_dwThumbPushedBackground;
	DWORD			m_dwThumbRegularBackground;
	DWORD			m_dwStartGradientThumbBkgColor;
	DWORD			m_dwEndGradientThumbBkgColor;
	int				m_iMagicOffset;

private:

	void TextColor(DWORD dwValue);
	void DoubleBorder(bool bValue);
	void ArgbGlowingColor(DWORD dwValue);
	void BoxBorderWeight(float fValue);
	void BoxCornerRadius(float fValue);
	void BoxBorderColor(DWORD dwValue);
	void BoxOuterBorderColor(DWORD dwValue);
	void CheckedSignColor(DWORD dwValue);
	void SignWeight(float fValue);
	void MagicOffset(int iValue);

public:

	CGdiPlusCheckbox();
	virtual ~CGdiPlusCheckbox();

	// text color of our checkbox
	Prop<DWORD, CGdiPlusCheckbox>		CGPC_TextColor;

	// using double border for a box our not?
	Prop<bool, CGdiPlusCheckbox>		CGPC_DoubleBorder;

	// glowing color inside a box
	Prop<DWORD, CGdiPlusCheckbox>		CGPC_GlowColor;

	// weight of box's border
	Prop<float, CGdiPlusCheckbox>		CGPC_BorderWeight;

	// radius of border, how much it is rounded
	Prop<float, CGdiPlusCheckbox>		CGPC_BorderRadius;

	// color of our box's border
	Prop<DWORD, CGdiPlusCheckbox>		CGPC_BorderColor;

	// color of outer border of our box if we are using double border
	Prop<DWORD, CGdiPlusCheckbox>		CGPC_OuterBorderColor;

	// the color of a checked sign
	Prop<DWORD, CGdiPlusCheckbox>		CGPC_CheckedSignColor;

	// weight of a checked sign
	Prop<float, CGdiPlusCheckbox>		CGPC_CheckedSignWeight;

	Prop<int, CGdiPlusCheckbox>			CGPC_MagicOffset;

	void UpdateControl();

	
	void BoxBkgPushedColor(DWORD dwValue);
	void BoxBkgRegularColor(DWORD dwValue);
	

	void Checked(bool bValue);
	bool Checked() const;

	DWORD MakeArgb(int A, int R, int G, int B);

protected:

	void Redraw(BOOL bUpdateNow);
	Gdiplus::GraphicsPath *CreateRoundRectPath(Gdiplus::Rect r, int d);
	void DrawRoundRect(	Gdiplus::Graphics *g, Gdiplus::Rect rect, 
						Gdiplus::Color color, int nDiameter, int Line);

	DECLARE_MESSAGE_MAP()

	virtual void OnDraw(CDC *pDC);
	virtual void OnDrawContext(Gdiplus::Graphics * pGraphics, Gdiplus::Rect & rcContainer);
	virtual void OnDrawThumbBorder(	Gdiplus::Graphics *pGraphics,
									Gdiplus::Rect & rcContainer);
	virtual void OnDrawThumb(Gdiplus::Graphics * pGraphics, Gdiplus::Rect & rcContainer);
	virtual void OnCalcText(Gdiplus::Graphics * pGraphics, 
							CStringW strText, 
							Gdiplus::RectF & rcContainer, 
							Gdiplus::Font * pTextFont);
	virtual void OnDrawText(Gdiplus::Graphics * pGraphics, 
							CStringW strText, 
							Gdiplus::Rect & rcContainer, 
							Gdiplus::Font * pTextFont,
							bool bCalcRect = false);
	virtual void OnDrawCheckedSign(	Gdiplus::Graphics * pGraphics, 
									Gdiplus::Rect & rcContainer);
	
	HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	BOOL OnEraseBkgnd(CDC* pDC);
	void OnDestroy();
	void OnSize(UINT nType, int cx, int cy);
	void OnPaint();
	void OnLButtonDblClk(UINT nFlags, CPoint point);
	void OnLButtonDown(UINT nFlags, CPoint point);
	void OnLButtonUp(UINT nFlags, CPoint point);
	void OnMouseHover(UINT nFlags, CPoint point);
	void OnMouseLeave();
	void OnMouseMove(UINT nFlags, CPoint point);
	UINT OnGetDlgCode();
	void OnUpdateUIState(UINT /*nAction*/, UINT /*nUIElement*/);
	void OnTimer(UINT_PTR nIDEvent);
	LRESULT OnSetText(WPARAM wParam, LPARAM lParam);
	void OnNcPaint();
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
};


