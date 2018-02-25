#include "stdafx.h"
#include "GdiPlusCheckbox.h"

IMPLEMENT_DYNAMIC(CGdiPlusCheckbox, CButton)

CGdiPlusCheckbox::CGdiPlusCheckbox()
:	m_bNeedRepaint(true),
	m_bMouseTrack(FALSE),
	m_button_state(ButtonState::StateRegular),
	m_bChecked(false),
	m_dwBorderColor(Gdiplus::Color::White),
	m_nCornerRadius(2),
	m_nWeight(1),
	m_dwCheckedSignColor(Gdiplus::Color::Orange),
	m_bDoubleBorder(true),
	m_dwOuterBorderColor(Gdiplus::Color::Black),
	m_fGlowLevel(0),
	m_uGlowTimer(0),
	m_argbGlow(0xB28DBDFF),
	m_dwTextColor(Gdiplus::Color::White),
	m_BoxBkgPushedColor(0xCC000000),
	m_BoxBkgRegularColor(0x7F000000),
	m_SignWeight(3),
	m_bEnabled(TRUE),
	m_dwTextColorDisabled(Gdiplus::Color::DarkGray),
	m_iCheckedSymbolWeight(-1),
	m_bCheckedSymbolColor(Gdiplus::Color::Black),
	m_bCheckedSymbolColorDisabled(Gdiplus::Color::DarkGray),
	m_dwThumbPushedBackground(Gdiplus::Color::DarkGray),
	m_dwThumbRegularBackground(Gdiplus::Color::Beige),
	m_dwStartGradientThumbBkgColor(ARGB(150, 250, 250, 250)),
	m_dwEndGradientThumbBkgColor(ARGB(120, 200, 200, 200)),
	m_iMagicOffset(0)
{
	CGPC_TextColor.Set_Prop(this, NULL, &CGdiPlusCheckbox::TextColor);
	CGPC_DoubleBorder.Set_Prop(this, NULL, &CGdiPlusCheckbox::DoubleBorder);
	CGPC_GlowColor.Set_Prop(this, NULL, &CGdiPlusCheckbox::ArgbGlowingColor);
	CGPC_BorderWeight.Set_Prop(this, NULL, &CGdiPlusCheckbox::BoxBorderWeight);
	CGPC_BorderRadius.Set_Prop(this, NULL, &CGdiPlusCheckbox::BoxCornerRadius);
	CGPC_BorderColor.Set_Prop(this, NULL, &CGdiPlusCheckbox::BoxBorderColor);
	CGPC_OuterBorderColor.Set_Prop(this, NULL, &CGdiPlusCheckbox::BoxOuterBorderColor);
	CGPC_CheckedSignColor.Set_Prop(this, NULL, &CGdiPlusCheckbox::CheckedSignColor);
	CGPC_CheckedSignWeight.Set_Prop(this, NULL, &CGdiPlusCheckbox::SignWeight);
	CGPC_MagicOffset.Set_Prop(this, NULL, &CGdiPlusCheckbox::MagicOffset);
}

CGdiPlusCheckbox::~CGdiPlusCheckbox()
{
}

BEGIN_MESSAGE_MAP(CGdiPlusCheckbox, CButton)
	ON_WM_CTLCOLOR()
	ON_WM_ERASEBKGND()
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_WM_PAINT()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEHOVER()
	ON_WM_MOUSELEAVE()
	ON_WM_MOUSEMOVE()
	ON_WM_GETDLGCODE()
	ON_WM_UPDATEUISTATE()
	ON_WM_TIMER()
	ON_MESSAGE(WM_SETTEXT, OnSetText)
	ON_WM_NCPAINT()
END_MESSAGE_MAP()

HBRUSH CGdiPlusCheckbox::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	return NULL;
}

BOOL CGdiPlusCheckbox::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;
}

void CGdiPlusCheckbox::OnDestroy()
{
}

DWORD CGdiPlusCheckbox::MakeArgb(int A, int R, int G, int B)
{
	return ((DWORD(A)<<24) + (DWORD(R)<<16) + (DWORD(G)<<8) + DWORD(B));
}

void CGdiPlusCheckbox::OnSize(UINT nType, int cx, int cy)
{
	GetClientRect(&m_rcClient);
	Redraw(TRUE);
}

void CGdiPlusCheckbox::Redraw(BOOL bUpdateNow)
{
	m_MemDraw.Redraw(bUpdateNow, &m_bNeedRepaint);
}

void CGdiPlusCheckbox::OnPaint()
{
	CPaintDC dc(this); 
	OnDraw(&dc);
}

void CGdiPlusCheckbox::OnDraw(CDC *pDC)
{
	if(m_bNeedRepaint)
	{
		m_MemDraw.Assign(pDC->GetSafeHdc());
		m_MemDraw.PaintParentBackground();
		Gdiplus::Rect rc(0, 0, m_MemDraw.Width(), m_MemDraw.Height());
		Gdiplus::Graphics * pGraphics = Gdiplus::Graphics::FromHDC(m_MemDraw);
		OnDrawContext(pGraphics, rc);
		if(pGraphics) delete pGraphics;
		m_bNeedRepaint = false;
	}
	m_MemDraw.Commit(pDC->GetSafeHdc());
}

void CGdiPlusCheckbox::OnDrawContext(Gdiplus::Graphics * pGraphics, Gdiplus::Rect & rcContainer)
{
	pGraphics->SetSmoothingMode(Gdiplus::SmoothingMode::SmoothingModeAntiAlias);
	Gdiplus::Rect rcClient = rcContainer;
	OnDrawThumb(pGraphics, rcClient);

	CString strWindowText;
	if(m_strWindowText.IsEmpty())
	{
		GetWindowText(strWindowText);
	}
	else
	{
		strWindowText = m_strWindowText;
	}

	//Gdiplus::RectF rcTextRect((float)rcContainer.X, 
	//						  (float)rcContainer.Y, 
	//						  (float)rcContainer.Width, 
	//						  (float)rcContainer.Height);

	Gdiplus::Font * pFont = m_MemDraw.GetGdiPlusFont();

#ifndef _UNICODE
	CStringW test = CA2W(strWindowText);
#else
	CStringW test = strWindowText;
#endif

	DWORD dwCurrentStyle = ::GetWindowLongPtr(m_hWnd, GWL_STYLE);
	bool bTextLeft = ((dwCurrentStyle & BS_LEFTTEXT) == BS_LEFTTEXT);
	bool bTextAlignRight = ((dwCurrentStyle & BS_RIGHT) == BS_RIGHT);

//	Gdiplus::SizeF txtSize = GetTextSize(pGraphics, test, pFont, rc);

	Gdiplus::Rect rcTextRect(bTextLeft ? rcContainer.X : (rcContainer.X + rcClient.Width + 5), 
							 rcContainer.Y, 
							 (rcContainer.Width - rcClient.Width) - 5, 
							 rcContainer.Height);

	OnDrawText(pGraphics, test, rcTextRect, pFont);
}

void CGdiPlusCheckbox::OnLButtonDblClk(UINT nFlags, CPoint point)
{	// just do not handle this one
}

void CGdiPlusCheckbox::OnLButtonDown(UINT nFlags, CPoint point)
{
	m_button_state = ButtonState::StatePushed;
	Redraw(TRUE);
}

void CGdiPlusCheckbox::OnLButtonUp(UINT nFlags, CPoint point)
{
	if(m_rcClient.PtInRect(point) && m_button_state == ButtonState::StatePushed)
	{
		::PostMessage(::GetParent(m_hWnd), WM_COMMAND, 
			MAKELONG(::GetWindowLong(m_hWnd, GWL_ID), BN_CLICKED), (LPARAM)m_hWnd);
		if(m_bChecked == false) 
		{
			m_bChecked = true;
		}
		else 
		{
			m_bChecked = false;
		}
		if(m_rcClient.PtInRect(point))
		{
			m_button_state = ButtonState::StateOver;
		}
		else
		{
			m_button_state = ButtonState::StateRegular;
		}
	}
	Redraw(TRUE);
}

void CGdiPlusCheckbox::OnMouseHover(UINT nFlags, CPoint point)
{
	Redraw(TRUE);
	if(!m_uGlowTimer)m_uGlowTimer = SetTimer(1, 10, NULL);
}

void CGdiPlusCheckbox::OnMouseLeave()
{
	m_button_state = ButtonState::StateRegular;
	m_bMouseTrack = FALSE;
	Redraw(TRUE);
	if(!m_uGlowTimer)m_uGlowTimer = SetTimer(1, 10, NULL);
}

void CGdiPlusCheckbox::OnMouseMove(UINT nFlags, CPoint point)
{
	if(!m_bMouseTrack)
	{
		TRACKMOUSEEVENT tme = { sizeof(TRACKMOUSEEVENT), TME_LEAVE | TME_HOVER, m_hWnd, 1 };
		m_bMouseTrack = ::TrackMouseEvent(&tme);
	}
	if(m_rcClient.PtInRect(point))
	{
		m_button_state = ButtonState::StateOver;
	}
	else
	{
		m_button_state = ButtonState::StateRegular;
	}
	Redraw(TRUE);
}

Gdiplus::GraphicsPath * CGdiPlusCheckbox::CreateRoundRectPath(Gdiplus::Rect r, int d)
{
	Gdiplus::GraphicsPath *gp = new Gdiplus::GraphicsPath();
	if(d > r.Width)  d = r.Width;
	if(d > r.Height) d = r.Height;
	Gdiplus::Rect Corner(r.X, r.Y, d, d);
	gp->Reset();
	gp->AddArc(Corner, 180, 90);
	Corner.X += (r.Width - d - 1);
	gp->AddArc(Corner, 270, 90);
	Corner.Y += (r.Height - d - 1);
	gp->AddArc(Corner, 0, 90);
	Corner.X -= (r.Width - d - 1);
	gp->AddArc(Corner, 90, 90);
	gp->CloseFigure();
	return gp;
}

void CGdiPlusCheckbox::DrawRoundRect(	Gdiplus::Graphics *g, 
										Gdiplus::Rect rect, 
										Gdiplus::Color color, 
										int nDiameter, 
										int Line)
{
	Gdiplus::GraphicsPath *gp = CreateRoundRectPath(rect, nDiameter);
	Gdiplus::Pen pen(color, Line);    
	pen.SetAlignment(Gdiplus::PenAlignment::PenAlignmentCenter);
	int oldPageUnit = g->SetPageUnit(Gdiplus::Unit::UnitPixel);
	g->DrawPath(&pen, gp);
	g->SetPageUnit((Gdiplus::Unit)oldPageUnit);
	delete gp;
}

UINT CGdiPlusCheckbox::OnGetDlgCode()
{
	return BS_CHECKBOX;
}

void CGdiPlusCheckbox::OnUpdateUIState(UINT /*nAction*/, UINT /*nUIElement*/)
{
}

void CGdiPlusCheckbox::OnDrawThumbBorder(Gdiplus::Graphics *pGraphics,
										 Gdiplus::Rect & rcContainer)
{
	if(m_bDoubleBorder)
	{
		rcContainer.Inflate(-2, -2);
		DrawRoundRect(pGraphics, rcContainer, m_dwOuterBorderColor, 
			1, m_nWeight); // outer border
	}
	rcContainer.Inflate(-1, -1);
	DrawRoundRect(pGraphics, rcContainer, m_dwBorderColor, 
		1, m_nWeight); // inner border
}

void CGdiPlusCheckbox::OnDrawCheckedSign(	Gdiplus::Graphics * pGraphics, 
											Gdiplus::Rect & rcContainer)
{
	int iMagicOffset = m_iMagicOffset;
	float fWeight = m_iCheckedSymbolWeight == -1 ? rcContainer.Height / 5 : (float)m_iCheckedSymbolWeight;
	float aX = ((float)rcContainer.X + fWeight) + iMagicOffset,
		  aY = (float)rcContainer.Height / 2,
		  bX = (float)rcContainer.X + ((float)rcContainer.Width / 2) - 1,
		  bY = ((float)rcContainer.Height - fWeight) - iMagicOffset,
		  cX = (float)rcContainer.X + ((float)rcContainer.Width / 2) - 1, 
		  cY = ((float)rcContainer.Height - fWeight) - iMagicOffset,
		  dX = (((float)rcContainer.X + (float)rcContainer.Width) - fWeight) - iMagicOffset,
		  dY = ((float)rcContainer.Y + fWeight) + iMagicOffset;
	Gdiplus::GraphicsPath path;
	Gdiplus::Pen penJoin(m_bEnabled ? m_bCheckedSymbolColor : m_bCheckedSymbolColorDisabled, 
						 fWeight);
	penJoin.SetStartCap(Gdiplus::LineCapRound);
	penJoin.SetEndCap(Gdiplus::LineCapRound);
	path.StartFigure();
	path.AddLine(Gdiplus::PointF(aX, aY), Gdiplus::PointF(bX, bY));
	path.AddLine(Gdiplus::PointF(cX, cY), Gdiplus::PointF(dX, dY));
	penJoin.SetLineJoin(Gdiplus::LineJoinMiterClipped);
	pGraphics->DrawPath(&penJoin, &path);
}

void CGdiPlusCheckbox::OnDrawThumb(Gdiplus::Graphics * pGraphics, Gdiplus::Rect & rcContainer)
{
	DWORD dwCurrentStyle = ::GetWindowLongPtr(m_hWnd, GWL_STYLE);
	bool bTextLeft = ((dwCurrentStyle & BS_LEFTTEXT) == BS_LEFTTEXT);
	bool bTextAlignRight = ((dwCurrentStyle & BS_RIGHT) == BS_RIGHT);
	Gdiplus::Rect rcBox(
		bTextLeft ? (rcContainer.Width - rcContainer.Height) : rcContainer.X, 
		rcContainer.Y, rcContainer.Height, rcContainer.Height);
	OnDrawThumbBorder(pGraphics, rcBox);
	Gdiplus::SolidBrush sb((m_button_state == StatePushed) ? 
							m_dwThumbPushedBackground : 
							m_dwThumbRegularBackground);
	Gdiplus::GraphicsPath * pPath = CreateRoundRectPath(rcBox, 1);
	pGraphics->FillPath(&sb, pPath);
	pGraphics->SetClip(pPath);
	if(m_bChecked)
	{
		OnDrawCheckedSign(pGraphics, rcBox);
	}
	Gdiplus::Color colorShine1(m_dwStartGradientThumbBkgColor), 
				   colorShine2(m_dwEndGradientThumbBkgColor);
	Gdiplus::Rect shine = rcBox;
	Gdiplus::LinearGradientBrush shineBrush(shine, colorShine1,
		colorShine2, 
		Gdiplus::LinearGradientModeVertical);
	pGraphics->FillRectangle(&shineBrush, shine);
	if(m_button_state != StatePushed)
	{
		Gdiplus::ARGB argbGlowStart, argbGlowEnd;
		argbGlowStart  = argbGlowEnd = (m_argbGlow & ~Gdiplus::Color::AlphaMask);
		argbGlowStart |= ((Gdiplus::ARGB)(BYTE)(((m_argbGlow &  Gdiplus::Color::AlphaMask) >> 
			Gdiplus::Color::AlphaShift) * m_fGlowLevel) << Gdiplus::Color::AlphaShift);
		Gdiplus::PathGradientBrush pgb(pPath);
		Gdiplus::Color colorGlowEnd(argbGlowEnd);
		int colorCount = 1;
		pgb.SetCenterColor(Gdiplus::Color(argbGlowStart));
		pgb.SetSurroundColors(&colorGlowEnd, &colorCount);
		pgb.SetCenterPoint(Gdiplus::PointF(	(rcBox.X + rcBox.Width) * 1.00f, 
											(rcBox.Y + rcBox.Height) * 1.00f));
		pGraphics->FillPath(&pgb, pPath);
	}
	pGraphics->ResetClip();
	rcContainer = rcBox;
}	

void CGdiPlusCheckbox::UpdateControl()
{
	Redraw(TRUE);
}

void CGdiPlusCheckbox::BoxBorderWeight(float fValue)
{
	m_nWeight = fValue;
}

void CGdiPlusCheckbox::BoxCornerRadius(float fValue)
{
	m_nCornerRadius = fValue;
}

void CGdiPlusCheckbox::BoxBorderColor(DWORD dwValue)
{
	m_dwBorderColor = dwValue;
}

void CGdiPlusCheckbox::Checked(bool bValue)
{
	m_bChecked = bValue;
	Redraw(TRUE);
}

void CGdiPlusCheckbox::BoxOuterBorderColor(DWORD dwValue)
{
	m_dwOuterBorderColor = dwValue;
}

void CGdiPlusCheckbox::DoubleBorder(bool bValue)
{
	m_bDoubleBorder = bValue;
}	

bool CGdiPlusCheckbox::Checked() const
{
	return m_bChecked;
}

void CGdiPlusCheckbox::CheckedSignColor(DWORD dwValue)
{
	m_dwCheckedSignColor = dwValue;
}

void CGdiPlusCheckbox::OnCalcText(Gdiplus::Graphics * pGraphics, 
								  CStringW strText, 
								  Gdiplus::RectF & rcContainer, 
								  Gdiplus::Font * pTextFont)
{
	SIZE sz = m_MemDraw.GdiGetTextSize(strText.LockBuffer());
	Gdiplus::RectF rcBounds(0.0, 0.0, (float)sz.cx, (float)sz.cy);
	rcContainer = rcBounds;
}

void CGdiPlusCheckbox::OnDrawText(	Gdiplus::Graphics * pGraphics, 
									CStringW strText, 
									Gdiplus::Rect & rcContainer, 
									Gdiplus::Font * pTextFont,
									bool bCalcRect)
{
	LONG_PTR dwStyle = ::GetWindowLongPtr(m_hWnd, GWL_STYLE);
	Gdiplus::StringFormat Format;

	Format.SetLineAlignment(
		((dwStyle & BS_TOP) == BS_TOP) ? Gdiplus::StringAlignmentNear : 
		((dwStyle & BS_VCENTER) == BS_VCENTER) ? Gdiplus::StringAlignmentCenter : 
		((dwStyle & BS_BOTTOM) == BS_BOTTOM) ? Gdiplus::StringAlignmentFar
		: Gdiplus::StringAlignmentCenter);

	Format.SetAlignment(
		((dwStyle & BS_CENTER) == BS_CENTER) ? Gdiplus::StringAlignmentCenter : 
		((dwStyle & BS_RIGHT) == BS_RIGHT) ? Gdiplus::StringAlignmentFar : 
		Gdiplus::StringAlignmentNear);

	Format.SetHotkeyPrefix(Gdiplus::HotkeyPrefixShow);
	Format.SetTrimming(Gdiplus::StringTrimmingWord);
	Gdiplus::SolidBrush textColor(m_bEnabled ? m_dwTextColor : m_dwTextColorDisabled);

	Gdiplus::RectF rcTXT((float)rcContainer.X, 
						 (float)rcContainer.Y,
						 (float)rcContainer.Width, 
						 (float)rcContainer.Height);

	pGraphics->DrawString(strText, strText.GetLength(), pTextFont, rcTXT, &Format, &textColor);

	/*m_MemDraw.GdiDrawText(strText.LockBuffer(), RGB(255, 255, 255), 
		CRect(CPoint(rcContainer.X, rcContainer.Y), 
		CSize(rcContainer.Width, rcContainer.Height)));*/
}

void CGdiPlusCheckbox::OnTimer(UINT_PTR nIDEvent)
{
	// Did we receive this timer message as a result of glow change?
	if(nIDEvent == 1)
	{
		// Increment the glow if mouse is over our button
		if(m_button_state == StateOver)
		{
			m_fGlowLevel += 0.033f;
			if(m_fGlowLevel >= 1.0f)
			{
				// When we reach the upper limit cancel the timer
				m_fGlowLevel = 1.0f;
				m_uGlowTimer = 0;
				KillTimer(1);
			}
			Redraw(FALSE);

		// Decrement the glow if the mouse is not over the button
		}
		else
		{
			m_fGlowLevel -= 0.066f;
			if(m_fGlowLevel <= 0.0f)
			{
				// When we reach the bottom limit cancel the timer
				m_fGlowLevel = 0.0f;
				m_uGlowTimer = 0;
				KillTimer(1);
			}
			Redraw(FALSE);
		}
	}
}

LRESULT CGdiPlusCheckbox::OnSetText(WPARAM wParam, LPARAM lParam)
{
	m_strWindowText = (LPTSTR)lParam;
	Redraw(TRUE);
	return TRUE;
}

void CGdiPlusCheckbox::OnNcPaint()
{
}

void CGdiPlusCheckbox::SignWeight(float fValue)
{
	m_SignWeight = fValue;
	m_iCheckedSymbolWeight = fValue;
}

void CGdiPlusCheckbox::BoxBkgPushedColor(DWORD dwValue)
{
	m_BoxBkgPushedColor = dwValue;
}

void CGdiPlusCheckbox::BoxBkgRegularColor(DWORD dwValue)
{
	m_BoxBkgRegularColor = dwValue;
}

void CGdiPlusCheckbox::ArgbGlowingColor(DWORD dwValue)
{
	m_argbGlow = dwValue;
}

void CGdiPlusCheckbox::TextColor(DWORD dwValue)
{
	m_dwTextColor = dwValue;
}

LRESULT CGdiPlusCheckbox::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message)
	{
		case WM_PRINTCLIENT:
			{
				OnDraw(CDC::FromHandle((HDC)wParam));
			}
		break;
	}
	return CButton::WindowProc(message, wParam, lParam);
}

void CGdiPlusCheckbox::MagicOffset(int iValue)
{
	m_iMagicOffset = iValue;
	if(m_hWnd)
	{
		Redraw(TRUE);
	}
}