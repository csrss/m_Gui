#pragma once

#pragma warning(disable: 4482)

#include "MemDraw.h"

class CGdiPlusButton : public CButton
{
	DECLARE_DYNAMIC(CGdiPlusButton)

	BOOL	m_bMouseTrack;
	FLOAT	m_fGlowLevel;
	UINT	m_uGlowTimer;

public:

	enum ButtonStyle
	{
		StyleGradientColor,
		StyleGlowingColor
	};

	enum ButtonState
	{
		StateRegular,
		StateOver,
		StatePushed
	};

	enum ButtonImagePosition
	{
		ButtonImagePositionLeft,
		ButtonImagePositionCenter,
		ButtonImagePositionRight
	};

protected:

	CMemDraw				m_MemDraw;
	bool					m_bNeedRepaint;
	CRect					m_rcClient;
	bool					m_bDoubleBorder;
	DWORD					m_dwInnerBorderColor;
	DWORD					m_dwOuterBorderColor;
	float					m_nBorderWeight;
	float					m_nBorderCornerRadius;
	ButtonStyle				m_button_style;
	DWORD					m_dwBackgroundColor;		// this is prime color
	DWORD					m_dwBackgroundColorOver;	// mouse over prime color
	DWORD					m_dwBackgroundColorPushed;	// mouse down prime color
	DWORD					m_dwBackgroundColorGr;		// gradient
	DWORD					m_dwBackgroundColorOverGr;	// gradient
	DWORD					m_dwBackgroundColorPushedGr;// gradient
	ButtonState				m_button_state;
	DWORD					m_argbGlow;
	Gdiplus::Image		  * m_gButtonBkgImage;
	ButtonImagePosition		m_ImagePosition;
	DWORD					m_dwTextColor;
	float					m_fImageStretch;
	CString					m_strWindowText;

private:

	void __ButtonStyle(ButtonStyle Value);
	void ImagePosition(ButtonImagePosition Value);
	void GlowColor(DWORD dwValue);
	void ButtonImage(UINT uResId);
	void ImageStretch(float fValue);
	void TextColor(DWORD dwValue);
	void DoubleBorder(bool bValue);
	void BorderWeight(float fValue);
	void BorderCornerRadius(float fValue);
	void InnerBorderColor(DWORD dwValue);
	void OuterBorderColor(DWORD dwValue);
	void BackgroundColor(DWORD dwValue);
	void BackgroundColorOver(DWORD dwValue);
	void BackgroundColorPushed(DWORD dwValue);
	void BackgroundColorGr(DWORD dwValue);
	void BackgroundColorOverGr(DWORD dwValue);
	void BackgroundColorPushedGr(DWORD dwValue);
	void ButtonText(LPTSTR lpString);

public:

	CGdiPlusButton();
	virtual ~CGdiPlusButton();

	// CGdiPlusButton::StyleGradientColor 
	// CGdiPlusButton::StyleGlowingColor
	Prop<ButtonStyle, CGdiPlusButton>			CGPB_BackgroundStyle;

	// ButtonImagePositionLeft,
	// ButtonImagePositionCenter,
	// ButtonImagePositionRight
	Prop<ButtonImagePosition, CGdiPlusButton>	CGPB_ImagePosition;

	// DWORD that represents button glow color, with alpha channel
	Prop<DWORD, CGdiPlusButton>					CGPB_GlowColor;

	// Image, we want to put on our button, resource ID and png only
	Prop<UINT, CGdiPlusButton>					CGPB_ButtonImage;

	// how much do we want our image to be scaled down
	Prop<float, CGdiPlusButton>					CGPB_ImageStretch;

	// color of our button's text
	Prop<DWORD, CGdiPlusButton>					CGPB_TextColor;

	// are we using double border or fucking not, biatch!!??
	Prop<bool, CGdiPlusButton>					CGPB_DoubleBorder;

	// fatness of our border
	Prop<float, CGdiPlusButton>					CGPB_BorderWeight;

	// radius of corner, how much rounded our button will be. fuck this
	Prop<float, CGdiPlusButton>					CGPB_BorderCornerRadius;

	// inner border color
	Prop<DWORD, CGdiPlusButton>					CGPB_InnerBorderColor;

	// outer border color
	Prop<DWORD, CGdiPlusButton>					CGPB_OuterBorderColor;

	// Main fucking background color lol biatch fucka sucka muthafucka ><
	Prop<DWORD, CGdiPlusButton>					CGPB_BackgroundColor;

	// Gradient for main color
	Prop<DWORD, CGdiPlusButton>					CGPB_BackgroundColorGr;

	// now, main color when our button is over, means mouse over, fuck me!
	Prop<DWORD, CGdiPlusButton>					CGPB_BackgroundColorOver;

	// gradient color for mouse over main color
	Prop<DWORD, CGdiPlusButton>					CGPB_BackgroundColorOverGr;

	// biatches, main color when our button is pushed, in pushed state fuck!
	Prop<DWORD, CGdiPlusButton>					CGPB_BackgroundColorPushed;

	// fuckers, gradient color for our button main color when it is pushed lol
	Prop<DWORD, CGdiPlusButton>					CGPB_BackgroundColorPushedGr;

	// quick setter for button text, LOL!!!
	Prop<LPTSTR, CGdiPlusButton>				CGPB_Text;
	
	// just make fucking argb color with alpha channel
	DWORD MakeArgb(int A, int R, int G, int B);

	// update button NOW!
	void UpdateControl();

	int Width();
	int Height();

protected:

	BOOL Redraw(BOOL bUpdateNow);
	Gdiplus::Bitmap *ImgFromResource(LPCTSTR pName, LPCTSTR pType, HMODULE hInst);
	Gdiplus::GraphicsPath *CreateRoundRectPath(Gdiplus::RectF r, float d);
	void DrawRoundRect(	Gdiplus::Graphics *g, Gdiplus::RectF rect, 
						Gdiplus::Color color, float nDiameter, float Line);
	CString GetButtonText();

	DECLARE_MESSAGE_MAP()

	virtual void OnDraw(CDC *pDC);
	virtual void OnDrawContext(Gdiplus::Graphics * pGraphics, Gdiplus::RectF & rcContainer);
	virtual void OnDrawBorder(Gdiplus::Graphics *pGraphics,
							  Gdiplus::RectF & rcContainer);
	virtual void OnDrawGradientBkgnd(Gdiplus::Graphics *pGraphics,
									 Gdiplus::RectF & rcContainer);
	virtual void OnDrawGlowBkgnd(Gdiplus::Graphics *pGraphics,
								 Gdiplus::RectF & rcContainer);
	virtual void OnDrawContent(Gdiplus::Graphics *pGraphics,
							   Gdiplus::RectF & rcContainer);
	virtual void OnDrawImage(Gdiplus::Graphics *pGraphics,
							 Gdiplus::Image *pImage,
							 Gdiplus::RectF & rcContainer,
							 bool bCalcRect = false);
	virtual void OnCalcImage(Gdiplus::Graphics *pGraphics,
							 Gdiplus::Image *pImage,
							 Gdiplus::RectF & rcContainer);

	HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	BOOL OnEraseBkgnd(CDC* pDC);
	void OnSize(UINT nType, int cx, int cy);
	void OnDestroy();
	void OnPaint();
	void OnLButtonDown(UINT nFlags, CPoint point);
	void OnLButtonUp(UINT nFlags, CPoint point);
	void OnMouseMove(UINT nFlags, CPoint point);
	void OnMouseLeave();
	void OnMouseHover(UINT nFlags, CPoint point);
	void OnUpdateUIState(UINT /*nAction*/, UINT /*nUIElement*/);
	void OnLButtonDblClk(UINT nFlags, CPoint point);
	void OnTimer(UINT_PTR nIDEvent);
	LRESULT OnSetText(WPARAM wParam, LPARAM lParam);
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
};


