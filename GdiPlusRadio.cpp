#include "stdafx.h"
#include "GdiPlusRadio.h"

IMPLEMENT_DYNAMIC(CGdiPlusRadio, CButton)

CGdiPlusRadio::CGdiPlusRadio()
:	m_bNeedRepaint(true),
	/*m_pCachedBitmap(NULL),*/
	m_button_state(ButtonState::StateRegular),
	m_fGlowLevel(0),
	m_uGlowTimer(0),
	m_argbGlow(0xB28DBDFF),
	m_bDoubleBorder(true),
	m_dwInnerBorderColor(Gdiplus::Color::White),
	m_dwOuterBorderColor(Gdiplus::Color::Black),
	m_nBorderWeight(1),
	m_dwTextColor(0xffffffff),
	m_bChecked(false)
{
}

CGdiPlusRadio::~CGdiPlusRadio()
{
}

BEGIN_MESSAGE_MAP(CGdiPlusRadio, CButton)
	ON_WM_CTLCOLOR()
	ON_WM_ERASEBKGND()
	ON_WM_SIZE()
	ON_WM_DESTROY()
	ON_WM_PAINT()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEHOVER()
	ON_WM_MOUSELEAVE()
	ON_WM_MOUSEMOVE()
	ON_WM_UPDATEUISTATE()
	ON_WM_TIMER()
END_MESSAGE_MAP()

HBRUSH CGdiPlusRadio::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	return NULL;
}

BOOL CGdiPlusRadio::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;
}

void CGdiPlusRadio::OnSize(UINT nType, int cx, int cy)
{
	GetClientRect(&m_rcClient);
	Gdiplus::Graphics graphics(m_hWnd);
	Gdiplus::Rect rect(0, 0, m_rcClient.Height(), m_rcClient.Height());
	Gdiplus::GraphicsPath *gp = CreateRoundRectPath(rect, rect.Height);
	SetWindowRgn(Gdiplus::Region(gp).GetHRGN(&graphics), FALSE);
	delete gp;
	Redraw(TRUE);
}

void CGdiPlusRadio::OnDestroy()
{
}

BOOL CGdiPlusRadio::Redraw(BOOL bUpdateNow)
{
	m_bNeedRepaint = true;
	if(bUpdateNow)
	{
		return ::RedrawWindow(m_hWnd, NULL, NULL, RDW_NOERASE | RDW_INVALIDATE | RDW_UPDATENOW);
	}
	else
	{
		return ::InvalidateRect(m_hWnd, NULL, FALSE);
	}
}

void CGdiPlusRadio::OnDrawBackground(Gdiplus::Graphics *pGraphics,
									 Gdiplus::Rect & rcContainer)
{
	Gdiplus::GraphicsPath * pGrPath = 
		CreateRoundRectPath(rcContainer, rcContainer.Height);
	Gdiplus::SolidBrush background(Gdiplus::Color(
		(m_button_state == StatePushed) ? 0xCC000000 : 0x7F000000));
	pGraphics->FillPath(&background, pGrPath);

	pGraphics->SetClip(pGrPath);

	if(m_bChecked)
	{
		Gdiplus::Rect dot = rcContainer;
		//dot.Width /= 3;
		//dot.Height /= 3;
		//dot.X += rcContainer.Width / 3;
		//dot.Y += rcContainer.Height / 3;
		//Gdiplus::Pen *p = new Gdiplus::Pen(m_argbGlow, 10);
		//pGraphics->DrawEllipse(p, dot);
		Gdiplus::GraphicsPath *dotPath = 
					CreateRoundRectPath(dot, dot.Height);
		Gdiplus::SolidBrush dotBkg(m_argbGlow);
		pGraphics->FillPath(&dotBkg, dotPath);
		//Gdiplus::ARGB argbGlowStart, argbGlowEnd;
		//argbGlowStart  = argbGlowEnd = (m_argbGlow & ~Gdiplus::Color::AlphaMask);
		//argbGlowStart |= ((Gdiplus::ARGB)(BYTE)(((m_argbGlow &  Gdiplus::Color::AlphaMask) >> 
		//	Gdiplus::Color::AlphaShift) * 1.0f) << Gdiplus::Color::AlphaShift);
		//Gdiplus::PathGradientBrush pgb(pGrPath);
		//Gdiplus::Color colorGlowEnd(argbGlowEnd);
		//int colorCount = 30;
		//pgb.SetCenterColor(Gdiplus::Color(argbGlowStart));
		//pgb.SetSurroundColors(&colorGlowEnd, &colorCount);
		//pgb.SetCenterPoint(Gdiplus::PointF(
		//	(float)dot.Width * 0.50f, 
		//	(float)dot.Height * 0.50f));
		//pgb.SetGammaCorrection(TRUE);
		//pGraphics->FillPath(&pgb, pGrPath);
	}

	Gdiplus::Color colorShine1(0x99FFFFFF), colorShine2(0x33FFFFFF);
	Gdiplus::Rect shine = rcContainer;
	shine.Height /= 2;
	Gdiplus::LinearGradientBrush shineBrush(shine, colorShine1,
		colorShine2, 
		Gdiplus::LinearGradientMode::LinearGradientModeBackwardDiagonal);
	pGraphics->FillRectangle(&shineBrush, shine);

	pGraphics->ResetClip();
}

void CGdiPlusRadio::OnDrawContext(Gdiplus::Graphics * pGraphics, Gdiplus::Rect & rcContainer)
{
	pGraphics->SetSmoothingMode(Gdiplus::SmoothingMode::SmoothingModeAntiAlias);
	Gdiplus::Rect border(rcContainer.X, rcContainer.Y, rcContainer.Height, rcContainer.Height);
	OnDrawBorder(pGraphics, border);
	OnDrawBackground(pGraphics, border);

	CString strWindowText;
	if(m_strWindowText.IsEmpty())
	{
		GetWindowText(strWindowText);
	}
	else
	{
		strWindowText = m_strWindowText;
	}

	Gdiplus::Rect rcTextRect(rcContainer.X, 
							 rcContainer.Y, 
							 rcContainer.Width, 
							 rcContainer.Height);

	Gdiplus::Font * pFont = OnGetFont(pGraphics);

#ifndef _UNICODE
	USES_CONVERSION;
	CStringW test = CA2W(strWindowText);
#else
	CStringW test = strWindowText;
#endif

	OnCalcText(pGraphics, test, rcTextRect, pFont);
	DWORD dwCurrentStyle = ::GetWindowLongPtr(m_hWnd, GWL_STYLE);
	bool bTextLeft = (dwCurrentStyle & BS_LEFTTEXT) ? true : false;
	bool bTextAlignRight = (dwCurrentStyle & BS_RIGHT) ? true : false;

	Gdiplus::Rect txtRc(
		bTextLeft ? (bTextAlignRight ? (
		rcContainer.Width - rcTextRect.Width - border.Width - 5) 
		: rcContainer.X) 
		: border.X + border.Width + 5, 
		border.Y, rcTextRect.Width, border.Height);

	OnDrawText(pGraphics, test, txtRc, pFont);
}

void CGdiPlusRadio::OnDraw(CDC *pDC)
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

void CGdiPlusRadio::OnPaint()
{
	CPaintDC dc(this);
	OnDraw(&dc);
}

void CGdiPlusRadio::OnLButtonDblClk(UINT nFlags, CPoint point)
{
}

void CGdiPlusRadio::OnLButtonDown(UINT nFlags, CPoint point)
{
	m_button_state = StatePushed;
	Redraw(TRUE);
}

void CGdiPlusRadio::OnLButtonUp(UINT nFlags, CPoint point)
{
	if(m_rcClient.PtInRect(point) && m_button_state == StatePushed)
	{
		if(m_bChecked == false) 
		{
			m_bChecked = true;
		}
		else 
		{
			m_bChecked = false;
		}
		::PostMessage(::GetParent(m_hWnd), WM_COMMAND, 
			MAKELONG(::GetWindowLong(m_hWnd, GWL_ID), BN_CLICKED), (LPARAM)m_hWnd);
		m_button_state = StateOver;
	}
	else
	{
		m_button_state = StateRegular;
	}
	Redraw(TRUE);
}

void CGdiPlusRadio::OnMouseHover(UINT nFlags, CPoint point)
{
	Redraw(TRUE);
}

void CGdiPlusRadio::OnMouseLeave()
{
	m_bMouseTrack = FALSE;
	m_button_state = StateRegular;
	Redraw(TRUE);
}

void CGdiPlusRadio::OnMouseMove(UINT nFlags, CPoint point)
{
	if(!m_bMouseTrack)
	{
		TRACKMOUSEEVENT tme = { sizeof(TRACKMOUSEEVENT), TME_LEAVE | TME_HOVER, m_hWnd, 1 };
		m_bMouseTrack = ::TrackMouseEvent(&tme);
	}
	if(m_rcClient.PtInRect(point))
	{
		m_button_state = StateOver;
	}
	else
	{
		m_button_state = StateRegular;
	}
	Redraw(TRUE);
}

void CGdiPlusRadio::OnUpdateUIState(UINT /*nAction*/, UINT /*nUIElement*/)
{
}

void CGdiPlusRadio::OnTimer(UINT_PTR nIDEvent)
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


Gdiplus::GraphicsPath * CGdiPlusRadio::CreateRoundRectPath(Gdiplus::Rect r, int d)
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

void CGdiPlusRadio::DrawRoundRect(	Gdiplus::Graphics *g, Gdiplus::Rect rect, 
									Gdiplus::Color color, int nDiameter, int Line)
{
	Gdiplus::GraphicsPath *gp = CreateRoundRectPath(rect, nDiameter);
	Gdiplus::Pen pen(color, (float)Line);    
	pen.SetAlignment(Gdiplus::PenAlignment::PenAlignmentCenter);
	int oldPageUnit = g->SetPageUnit(Gdiplus::Unit::UnitPixel);
	g->DrawPath(&pen, gp);
	g->SetPageUnit((Gdiplus::Unit)oldPageUnit);
	delete gp;
}

void CGdiPlusRadio::OnDrawBorder(	Gdiplus::Graphics *pGraphics,
									Gdiplus::Rect & rcContainer)
{
	if(m_bDoubleBorder)
	{
		rcContainer.Inflate(-2, -2);
		DrawRoundRect(pGraphics, rcContainer, m_dwOuterBorderColor, 
			rcContainer.Width, (int)m_nBorderWeight); // outer border
	}
	rcContainer.Inflate(-1, -1);
	DrawRoundRect(pGraphics, rcContainer, m_dwInnerBorderColor, 
		rcContainer.Width, (int)m_nBorderWeight); // inner border
}

void CGdiPlusRadio::OnCalcText(	Gdiplus::Graphics * pGraphics, 
								CStringW strText, 
								Gdiplus::Rect & rcContainer, 
								Gdiplus::Font * pTextFont)
{
	Gdiplus::RectF rcBounds;
	pGraphics->MeasureString(strText, -1, pTextFont, 
							 Gdiplus::PointF((float)rcContainer.X, (float)rcContainer.Y), 
							 &rcBounds);
	Gdiplus::Rect rcTemp((int)rcBounds.X, 
						 (int)rcBounds.Y, 
						 (int)rcBounds.Width, 
						 (int)rcBounds.Height);
	rcContainer = rcTemp;
}

void CGdiPlusRadio::OnDrawText(Gdiplus::Graphics * pGraphics, 
							CStringW strText, 
							Gdiplus::Rect & rcContainer, 
							Gdiplus::Font * pTextFont,
							bool bCalcRect)
{
	if(bCalcRect)
	{
		OnCalcText(pGraphics, strText, rcContainer, pTextFont);
		return ;
	}
	DWORD dwStyle = ::GetWindowLongPtr(m_hWnd, GWL_STYLE);
	Gdiplus::StringFormat Format;
	Format.SetLineAlignment(
		((dwStyle & BS_TOP)) ? Gdiplus::StringAlignment::StringAlignmentNear : 
		((dwStyle & BS_VCENTER)) ? Gdiplus::StringAlignment::StringAlignmentCenter : 
		((dwStyle & BS_BOTTOM)) ? Gdiplus::StringAlignment::StringAlignmentFar
		: Gdiplus::StringAlignment::StringAlignmentCenter);
	Format.SetAlignment(Gdiplus::StringAlignment::StringAlignmentCenter);
	Format.SetHotkeyPrefix(Gdiplus::HotkeyPrefix::HotkeyPrefixShow);
	Format.SetTrimming(Gdiplus::StringTrimming::StringTrimmingWord);
	Gdiplus::SolidBrush textColor(m_dwTextColor);
	Gdiplus::RectF rcTemp((float)rcContainer.X, 
						  (float)rcContainer.Y, 
						  (float)rcContainer.Width, 
						  (float)rcContainer.Height);
	pGraphics->DrawString(strText, strText.GetLength(), pTextFont, rcTemp, &Format, &textColor);
}

LRESULT CGdiPlusRadio::OnSetText(WPARAM wParam, LPARAM lParam)
{
	m_strWindowText = (LPTSTR)lParam;
	Redraw(TRUE);
	return TRUE;
}

Gdiplus::Font * CGdiPlusRadio::OnGetFont(Gdiplus::Graphics *pGraphics)
{
	HDC hdc = pGraphics->GetHDC();
	CFont *cfTempFont = GetFont();
	if(!cfTempFont)
	{
		if(!cfTempFont->Attach((HFONT)::GetStockObject(DEFAULT_GUI_FONT)))
		{
			Gdiplus::Font * gFont = new Gdiplus::Font(L"Arial", 12);
			return gFont;
		}
	}
	ASSERT(cfTempFont);
	Gdiplus::Font *gFont = new Gdiplus::Font(hdc, (HFONT)cfTempFont->GetSafeHandle());
	cfTempFont->Detach();
	pGraphics->ReleaseHDC(hdc);
	return gFont;
}

LRESULT CGdiPlusRadio::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
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
