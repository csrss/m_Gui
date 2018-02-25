#include "stdafx.h"
#include "GdiPlusListCtrlHeader.h"

IMPLEMENT_DYNAMIC(CGdiPlusListCtrlHeader, CHeaderCtrl)

CGdiPlusListCtrlHeader::CGdiPlusListCtrlHeader()
:	m_nBorderCornerRadius(2),
	m_bNeedRepaint(true),
	m_argbGlow(0xB28DBDFF),
	m_bMouseTrack(FALSE),
	m_fGlowLevel(0),
	m_uGlowTimer(0)
{
}

CGdiPlusListCtrlHeader::~CGdiPlusListCtrlHeader()
{
}

BEGIN_MESSAGE_MAP(CGdiPlusListCtrlHeader, CHeaderCtrl)
	ON_WM_SIZE()
	ON_WM_UPDATEUISTATE()
	ON_WM_NCPAINT()
	ON_WM_PAINT()
	ON_WM_CTLCOLOR()
	ON_WM_ERASEBKGND()
	ON_MESSAGE(HDM_LAYOUT, OnLayout)
	ON_WM_MOUSEHOVER()
	ON_WM_MOUSELEAVE()
	ON_WM_MOUSEMOVE()
END_MESSAGE_MAP()

void CGdiPlusListCtrlHeader::PreSubclassWindow()
{
	PostMessage(WM_SIZE);
	CHeaderCtrl::PreSubclassWindow();
}

void CGdiPlusListCtrlHeader::OnSize(UINT nType, int cx, int cy)
{
	m_MemDraw.Redraw(TRUE, &m_bNeedRepaint);
}

void CGdiPlusListCtrlHeader::OnUpdateUIState(UINT /*nAction*/, UINT /*nUIElement*/)
{
}

void CGdiPlusListCtrlHeader::OnNcPaint()
{
}

void CGdiPlusListCtrlHeader::OnPaint()
{
	CPaintDC dc(this);
	OnDraw(&dc);
}

void CGdiPlusListCtrlHeader::OnDraw(CDC *pDC)
{
	if(m_bNeedRepaint)
	{
		m_MemDraw.Assign(pDC->GetSafeHdc());
		m_MemDraw.PaintParentBackground();
		Gdiplus::Rect rc(0, 0, m_MemDraw.Width(), m_MemDraw.Height());
		Gdiplus::Graphics * pGraphics = Gdiplus::Graphics::FromHDC(m_MemDraw);
		pGraphics->SetSmoothingMode(Gdiplus::SmoothingMode::SmoothingModeAntiAlias);
		OnDrawContext(pGraphics, rc);
		if(pGraphics) delete pGraphics;
		m_bNeedRepaint = false;
	}
	m_MemDraw.Commit(pDC->GetSafeHdc());
}

void CGdiPlusListCtrlHeader::OnDrawContext(Gdiplus::Graphics * pGraphics, Gdiplus::Rect & rcContainer)
{
	int iCount = this->GetItemCount();
	int iTotalWidth = 0;
	for (int i = 0; i < iCount; ++i)
	{
		TCHAR text[MAX_PATH] = {0x00};
		HDITEM hdItem;
		::RtlSecureZeroMemory(&hdItem, sizeof(hdItem));
		hdItem.mask = HDI_TEXT;
		hdItem.pszText = text;
		hdItem.cchTextMax = sizeof(text);
		CRect rcItem;
		this->GetItemRect(i, rcItem);
		this->GetItem(i, &hdItem);
		Gdiplus::Rect rcItemRect(rcItem.left, rcItem.top, rcItem.Width(), rcItem.Height());
		OnItemDraw(pGraphics, rcItemRect, hdItem.pszText);
		iTotalWidth += rcItem.Width();
	}
	Gdiplus::Rect rcLast(iTotalWidth, 
		m_MemDraw.X(), 
		m_MemDraw.Width() - iTotalWidth,
		m_MemDraw.Height());

	OnItemDraw(pGraphics, rcLast, NULL);
}

void CGdiPlusListCtrlHeader::OnItemDraw(Gdiplus::Graphics * pGraphics, 
										const Gdiplus::Rect & rc, 
										LPTSTR lpItemText)
{
	OnDrawGlowBkgnd(pGraphics, rc);
	if(lpItemText)
	{
		m_MemDraw.GdiDrawText(lpItemText, RGB(255, 255, 255),
			CRect(CPoint(rc.X, rc.Y), CSize(rc.Width, rc.Height)), 0, 0, 0, true);
	}
}

void CGdiPlusListCtrlHeader::OnDrawGlowBkgnd(Gdiplus::Graphics * pGraphics, const Gdiplus::Rect & rcContainer)
{
	Gdiplus::GraphicsPath * pGrPath = 
		CreateRoundRectPath(rcContainer, m_nBorderCornerRadius);
	Gdiplus::SolidBrush background(Gdiplus::Color(
		/*(m_button_state == StatePushed) ? 0xCC000000 : */0x7F00000));
	pGraphics->FillPath(&background, pGrPath);
	pGraphics->SetClip(pGrPath);
	Gdiplus::Color colorShine1, colorShine2;
	//if(m_button_state == StatePushed)
	//{
	//	colorShine1.SetValue(0x3DFFFFFF), colorShine2.SetValue(0x14FFFFFF);
	//}
	//else
	//{
		colorShine1.SetValue(0x99FFFFFF), colorShine2.SetValue(0x33FFFFFF);
//	}
	Gdiplus::Rect shine = rcContainer;
	shine.Height /= 2;
	Gdiplus::LinearGradientBrush shineBrush(shine, colorShine1,
		colorShine2, Gdiplus::LinearGradientModeVertical);
	pGraphics->FillRectangle(&shineBrush, shine);
//	if(m_button_state != StatePushed)
//	{
		Gdiplus::ARGB argbGlowStart, argbGlowEnd;
		argbGlowStart  = argbGlowEnd = (m_argbGlow & ~Gdiplus::Color::AlphaMask);
		argbGlowStart |= ((Gdiplus::ARGB)(BYTE)(((m_argbGlow &  Gdiplus::Color::AlphaMask) >> 
			Gdiplus::Color::AlphaShift) * m_fGlowLevel) << Gdiplus::Color::AlphaShift);
		Gdiplus::PathGradientBrush pgb(pGrPath);
		Gdiplus::Color colorGlowEnd(argbGlowEnd);
		int colorCount = 1;
		pgb.SetCenterColor(Gdiplus::Color(argbGlowStart));
		pgb.SetSurroundColors(&colorGlowEnd, &colorCount);
		pgb.SetCenterPoint(Gdiplus::PointF(	rcContainer.Width * 0.50f, 
											rcContainer.Height * 1.00f));
		pGraphics->FillPath(&pgb, pGrPath);
//	}
	pGraphics->ResetClip();
	delete pGrPath;
}

Gdiplus::GraphicsPath * CGdiPlusListCtrlHeader::CreateRoundRectPath(Gdiplus::Rect r, int d)
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

HBRUSH CGdiPlusListCtrlHeader::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	return NULL;
}

BOOL CGdiPlusListCtrlHeader::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;
}

LRESULT CGdiPlusListCtrlHeader::OnLayout(WPARAM wParam, LPARAM lParam)
{
	LRESULT lResult = CHeaderCtrl::DefWindowProc(HDM_LAYOUT, 0, lParam);
	HD_LAYOUT &hdl = *( HD_LAYOUT * ) lParam;
	RECT *prc = hdl.prc;
	WINDOWPOS *pwpos = hdl.pwpos;
	int nHeight = pwpos->cy * 1;
	pwpos->cy = nHeight;
	prc->top = nHeight;
	return lResult;
}

void CGdiPlusListCtrlHeader::OnMouseHover(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	CHeaderCtrl::OnMouseHover(nFlags, point);

	m_MemDraw.Redraw(TRUE, &m_bNeedRepaint);
}

void CGdiPlusListCtrlHeader::OnMouseLeave()
{
	// TODO: Add your message handler code here and/or call default

	CHeaderCtrl::OnMouseLeave();

	m_MemDraw.Redraw(TRUE, &m_bNeedRepaint);
}

void CGdiPlusListCtrlHeader::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	CHeaderCtrl::OnMouseMove(nFlags, point);

	m_MemDraw.Redraw(TRUE, &m_bNeedRepaint);
}

int CGdiPlusListCtrlHeader::CurrentColumnNr(const CPoint & pt)
{
	int iCount = this->GetItemCount();
	if(iCount > 0)
	{
		for (int i = 0; i < iCount; ++i)
		{
			CRect rc;
			this->GetItemRect(i, &rc);
			if(rc.PtInRect(pt))
			{
				return i;
			}
		}
	}
	return -1;
}