#include "stdafx.h"
#include "GdiPlusSlider.h"

IMPLEMENT_DYNAMIC(CGdiPlusSlider, CSliderCtrl)

CGdiPlusSlider::CGdiPlusSlider()
:	m_bNeedRepaint(TRUE),
	m_bMouseTrack(FALSE),
	m_bThumbDoubleBorder(TRUE),
	m_dwThumbBorderColor(Gdiplus::Color::White),
	m_dwThumbOuterBorderColor(Gdiplus::Color::Black),
	m_dwThumbPushedBackground(Gdiplus::Color::DarkGray),
	m_dwThumbRegularBackground(Gdiplus::Color::Beige),
	m_dwStartGradientThumbBkgColor(ARGB(150, 250, 250, 250)),
	m_dwEndGradientThumbBkgColor(ARGB(120, 200, 200, 200))
{
}

CGdiPlusSlider::~CGdiPlusSlider()
{
}

BEGIN_MESSAGE_MAP(CGdiPlusSlider, CSliderCtrl)
	ON_WM_CTLCOLOR()
	ON_WM_ERASEBKGND()
	ON_WM_UPDATEUISTATE()
	ON_WM_SIZE()
	ON_WM_PAINT()
	ON_WM_NCPAINT()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEHOVER()
	ON_WM_MOUSELEAVE()
	ON_WM_MOUSEMOVE()
END_MESSAGE_MAP()

HBRUSH CGdiPlusSlider::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	return NULL;
}

BOOL CGdiPlusSlider::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;
}

void CGdiPlusSlider::OnUpdateUIState(UINT /*nAction*/, UINT /*nUIElement*/)
{
}

void CGdiPlusSlider::OnSize(UINT nType, int cx, int cy)
{
	Redraw(TRUE);
}

void CGdiPlusSlider::OnPaint()
{
	CPaintDC dc(this);
	OnDraw(&dc);
}

void CGdiPlusSlider::OnDraw(CDC * pDC)
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

void CGdiPlusSlider::OnDrawContext(Gdiplus::Graphics * pGraphics, 
								   Gdiplus::Rect & rcContainer)
{
	OnDrawBackground(pGraphics, rcContainer);
	OnDrawThumb(pGraphics, rcContainer);
}

void CGdiPlusSlider::OnDrawBackground(Gdiplus::Graphics * pGraphics, 
									  Gdiplus::Rect & rcContainer)
{
	Gdiplus::Pen penBackground(Gdiplus::Color::Black);
	pGraphics->DrawRectangle(&penBackground, rcContainer);
}

void CGdiPlusSlider::OnDrawThumb(Gdiplus::Graphics * pGraphics, 
								 Gdiplus::Rect & rcContainer)
{
	CRect rcThumb;
	GetThumbRect(&rcThumb);

	Gdiplus::Rect rcThumbGdiPlus(rcThumb.left-5, 
								 rcThumb.top, 
								 rcThumb.Width() + 10, 
								 rcThumb.Height());

	OnDrawThumbBorder(pGraphics, rcThumbGdiPlus);

	Gdiplus::SolidBrush sb((m_button_state == StatePushed) ? 
							m_dwThumbPushedBackground : 
							m_dwThumbRegularBackground);

	Gdiplus::GraphicsPath * pPath = CreateRoundRectPath(rcThumbGdiPlus, 2);
	pGraphics->FillPath(&sb, pPath);
	pGraphics->SetClip(pPath);
	Gdiplus::Color colorShine1(m_dwStartGradientThumbBkgColor), 
				   colorShine2(m_dwEndGradientThumbBkgColor);
	Gdiplus::Rect shine = rcThumbGdiPlus;
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
		pgb.SetCenterPoint(Gdiplus::PointF(	(rcThumbGdiPlus.X + rcThumbGdiPlus.Width) * 1.00f, 
											(rcThumbGdiPlus.Y + rcThumbGdiPlus.Height) * 1.00f));
		pGraphics->FillPath(&pgb, pPath);
	}
	pGraphics->ResetClip();
}

void CGdiPlusSlider::OnNcPaint()
{
}

void CGdiPlusSlider::Redraw(BOOL bUpdateNow)
{
	m_MemDraw.Redraw(bUpdateNow, &m_bNeedRepaint);
}

void CGdiPlusSlider::OnLButtonDblClk(UINT nFlags, CPoint point)
{
}

void CGdiPlusSlider::OnLButtonDown(UINT nFlags, CPoint point)
{
	CSliderCtrl::OnLButtonDown(nFlags, point);
	CRect rcThumb;
	GetThumbRect(&rcThumb);
	if(rcThumb.PtInRect(point))
	{
		m_button_state = ButtonState::StatePushed;
	}
	Redraw(TRUE);
}

void CGdiPlusSlider::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	CSliderCtrl::OnLButtonUp(nFlags, point);
}

void CGdiPlusSlider::OnMouseHover(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	CSliderCtrl::OnMouseHover(nFlags, point);
}

void CGdiPlusSlider::OnMouseLeave()
{
	// TODO: Add your message handler code here and/or call default

	CSliderCtrl::OnMouseLeave();
}

void CGdiPlusSlider::OnMouseMove(UINT nFlags, CPoint point)
{
	CSliderCtrl::OnMouseMove(nFlags, point);

	if(!m_bMouseTrack)
	{
		TRACKMOUSEEVENT tme = { sizeof(TRACKMOUSEEVENT), TME_LEAVE | TME_HOVER, m_hWnd, 1 };
		m_bMouseTrack = ::TrackMouseEvent(&tme);
	}
	CRect rcThumb;
	GetThumbRect(&rcThumb);
	if(rcThumb.PtInRect(point))
	{
		m_button_state = ButtonState::StateOver;
	}
	else
	{
		m_button_state = ButtonState::StateRegular;
	}
	Redraw(TRUE);
}

void CGdiPlusSlider::OnDrawThumbBorder(Gdiplus::Graphics *pGraphics, 
									   Gdiplus::Rect & rcContainer)
{
	if(m_bThumbDoubleBorder)
	{
		rcContainer.Inflate(-2, -2);
		DrawRoundRect(pGraphics, rcContainer, m_dwThumbOuterBorderColor, 
			2, 1); // outer border
	}
	rcContainer.Inflate(-1, -1);
	DrawRoundRect(pGraphics, rcContainer, m_dwThumbBorderColor, 
		2, 1); // inner border
}

Gdiplus::GraphicsPath * CGdiPlusSlider::CreateRoundRectPath(Gdiplus::Rect r, int d)
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

void CGdiPlusSlider::DrawRoundRect(Gdiplus::Graphics *g, 
								   Gdiplus::Rect rect, 
								   Gdiplus::Color color, 
								   int nDiameter, 
								   int Line)
{
	Gdiplus::GraphicsPath *gp = CreateRoundRectPath(rect, nDiameter);
	Gdiplus::Pen pen(color, (float)Line);    
	pen.SetAlignment(Gdiplus::PenAlignment::PenAlignmentCenter);
	int oldPageUnit = g->SetPageUnit(Gdiplus::Unit::UnitPixel);
	g->DrawPath(&pen, gp);
	g->SetPageUnit((Gdiplus::Unit)oldPageUnit);
	delete gp;
}