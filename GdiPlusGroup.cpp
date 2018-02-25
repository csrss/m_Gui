// GdiPlusGroup.cpp : implementation file
//

#include "stdafx.h"
#include "GdiPlusGroup.h"


// CGdiPlusGroup

IMPLEMENT_DYNAMIC(CGdiPlusGroup, CStatic)

CGdiPlusGroup::CGdiPlusGroup()
:	m_bNeedRepaint(true),
	m_dwTextColor(Gdiplus::Color::White),
	m_bDoubleBorder(false),
	m_dwOuterBorderColor(Gdiplus::Color::Black),
	m_nBorderCornerRadius(2),
	m_nBorderWeight(1),
	m_dwInnerBorderColor(0x99000000)
{

}

CGdiPlusGroup::~CGdiPlusGroup()
{
}


BEGIN_MESSAGE_MAP(CGdiPlusGroup, CStatic)
	ON_WM_CTLCOLOR()
	ON_WM_ERASEBKGND()
	ON_WM_SIZE()
	ON_WM_UPDATEUISTATE()
	ON_WM_PAINT()
END_MESSAGE_MAP()



// CGdiPlusGroup message handlers

BOOL CGdiPlusGroup::Redraw(BOOL bUpdateNow)
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

void CGdiPlusGroup::UpdateControl()
{
	Redraw(TRUE);
}

HBRUSH CGdiPlusGroup::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	return NULL;
}

BOOL CGdiPlusGroup::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;
}

void CGdiPlusGroup::OnSize(UINT nType, int cx, int cy)
{
	CStatic::OnSize(nType, cx, cy);
}

void CGdiPlusGroup::OnUpdateUIState(UINT /*nAction*/, UINT /*nUIElement*/)
{
}

Gdiplus::Font * CGdiPlusGroup::OnGetFont(Gdiplus::Graphics * pGraphics, HDC hdc)
{
	HFONT hFont;
	Gdiplus::Font * pRetFont = NULL;
	CFont * pFont = GetFont();
	if(pFont == NULL)
	{
		hFont = (HFONT)::GetStockObject(DEFAULT_GUI_FONT);
		if(hFont != NULL)
		{
			pFont = CFont::FromHandle(hFont);
		}
	}
	LOGFONTW lg;
	pFont->GetLogFont(&lg);
	pRetFont = new Gdiplus::Font(hdc, &lg);
	return pRetFont;
}

Gdiplus::GraphicsPath * CGdiPlusGroup::CreateRoundRectPath(Gdiplus::RectF r, float d)
{
	Gdiplus::GraphicsPath *gp = new Gdiplus::GraphicsPath();
	if(d > r.Width)  d = r.Width;
	if(d > r.Height) d = r.Height;
	Gdiplus::RectF Corner(r.X, r.Y, d, d);
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

void CGdiPlusGroup::DrawRoundRect(	Gdiplus::Graphics *g, Gdiplus::RectF rect, 
									Gdiplus::Color color, float nDiameter, float Line)
{
	Gdiplus::GraphicsPath *gp = CreateRoundRectPath(rect, nDiameter);
	Gdiplus::Pen pen(color, Line);    
	pen.SetAlignment(Gdiplus::PenAlignment::PenAlignmentCenter);
	int oldPageUnit = g->SetPageUnit(Gdiplus::Unit::UnitPixel);
	g->DrawPath(&pen, gp);
	g->SetPageUnit((Gdiplus::Unit)oldPageUnit);
	delete gp;
}

void CGdiPlusGroup::OnDrawBorder(Gdiplus::Graphics *pGraphics,
							  Gdiplus::RectF & rcContainer)
{
	if(m_bDoubleBorder)
	{
		rcContainer.Inflate(-2, -2);
		DrawRoundRect(pGraphics, rcContainer, m_dwOuterBorderColor, 
			m_nBorderCornerRadius, m_nBorderWeight); // outer border
	}
	rcContainer.Inflate(-1, -1);
	DrawRoundRect(pGraphics, rcContainer, m_dwInnerBorderColor, 
		m_nBorderCornerRadius, m_nBorderWeight); // inner border
}

void CGdiPlusGroup::OnDrawBackground(	Gdiplus::Graphics *pGraphics,
										Gdiplus::RectF & rcContainer)
{
	Gdiplus::RectF rc(rcContainer.X, rcContainer.Y, rcContainer.Width, 15.0);
	Gdiplus::GraphicsPath * pGrPath = 
		CreateRoundRectPath(rc, m_nBorderCornerRadius);
	Gdiplus::SolidBrush background(Gdiplus::Color(0x7F000000));
	pGraphics->FillPath(&background, pGrPath);
	pGraphics->SetClip(pGrPath);
	Gdiplus::Color colorShine1, colorShine2;
	colorShine1.SetValue(0x66FFFFFF), colorShine2.SetValue(0x33FFFFFF);
	Gdiplus::RectF shine = rcContainer;
	shine.Height /= 2;
	Gdiplus::LinearGradientBrush shineBrush(shine, colorShine1,
		colorShine2, Gdiplus::LinearGradientModeVertical);
	pGraphics->FillRectangle(&shineBrush, shine);
	pGraphics->ResetClip();
}

void CGdiPlusGroup::OnDrawText(Gdiplus::Graphics * pGraphics, 
							   Gdiplus::RectF & rcContainer, 
							   Gdiplus::Font * pFont)
{
	Gdiplus::RectF rcText = rcContainer;
	rcText.X += 5;
	rcText.Y += 5;
	Gdiplus::Status gdiStatus;
	Gdiplus::StringAlignment strAlign;
	CString strWindowText;
	this->GetWindowTextW(strWindowText);
	if(strWindowText.IsEmpty())
	{
		return;
	}
	Gdiplus::StringFormat strFormat;
	gdiStatus = strFormat.SetAlignment(Gdiplus::StringAlignment::StringAlignmentNear);
	gdiStatus = strFormat.SetLineAlignment(Gdiplus::StringAlignment::StringAlignmentNear);
	Gdiplus::SolidBrush sbr(m_dwTextColor);
	pGraphics->SetTextRenderingHint(Gdiplus::TextRenderingHint::TextRenderingHintAntiAliasGridFit);
	gdiStatus = pGraphics->DrawString(strWindowText, -1, pFont, rcText, &strFormat, &sbr);
}

void CGdiPlusGroup::OnDrawContext(Gdiplus::Graphics * pGraphics, Gdiplus::RectF & rcContainer)
{
	pGraphics->SetSmoothingMode(Gdiplus::SmoothingMode::SmoothingModeAntiAlias);
	HDC hdc = pGraphics->GetHDC();
	Gdiplus::Font * pFont = OnGetFont(pGraphics, hdc);
	pGraphics->ReleaseHDC(hdc);
	OnDrawText(pGraphics, rcContainer, pFont);
	rcContainer.Y += 4;
	rcContainer.Height -=4;
	OnDrawBorder(pGraphics, rcContainer);
	OnDrawBackground(pGraphics, rcContainer);
	delete pFont;
}

void CGdiPlusGroup::OnDraw(CDC *pDC)
{
	if(m_bNeedRepaint)
	{
		m_MemDraw.Assign(pDC->GetSafeHdc());
		m_MemDraw.PaintParentBackground();
		Gdiplus::RectF rc((float)0, (float)0, (float)m_MemDraw.Width(), (float)m_MemDraw.Height());
		Gdiplus::Graphics * pGraphics = Gdiplus::Graphics::FromHDC(m_MemDraw);
		OnDrawContext(pGraphics, rc);
		if(pGraphics) delete pGraphics;
		m_bNeedRepaint = false;
	}
	m_MemDraw.Commit(pDC->GetSafeHdc());
}

void CGdiPlusGroup::OnPaint()
{
	CPaintDC dc(this);
	OnDraw(&dc);
}

LRESULT CGdiPlusGroup::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message)
	{
		case WM_PRINTCLIENT:
			{
				OnDraw(CDC::FromHandle((HDC)wParam));
			}
		break;
	}
	return CStatic::WindowProc(message, wParam, lParam);
}
