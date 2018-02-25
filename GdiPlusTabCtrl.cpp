#include "stdafx.h"
#include "GdiPlusTabCtrl.h"

IMPLEMENT_DYNAMIC(CGdiPlusTabCtrl, CTabCtrl)

CGdiPlusTabCtrl::CGdiPlusTabCtrl()
:	m_bNeedRepaint(true),
	m_bCurrentTab(false),
	m_iCurrentTab(-1),
	m_bDoubleTabBorder(true),
	m_dwOuterBorderColor(ARGB(255, 0, 0, 0)),
	m_dwInnerBorderColor(ARGB(255, 255, 255, 255)),
	m_iOuterBorderWeight(1),
	m_iInnerBorderWeight(1),
	m_bMouseTrack(FALSE),
	m_fGlowLevel(0),
	m_uGlowTimer(0),
	m_argbGlow(0xB28DBDFF)
{
}

CGdiPlusTabCtrl::~CGdiPlusTabCtrl()
{
}

BEGIN_MESSAGE_MAP(CGdiPlusTabCtrl, CTabCtrl)
	ON_WM_UPDATEUISTATE()
	ON_WM_ERASEBKGND()
	ON_WM_CTLCOLOR()
	ON_WM_SIZE()
	ON_WM_PAINT()
	ON_WM_TIMER()
	ON_WM_MOUSEHOVER()
	ON_WM_MOUSELEAVE()
	ON_WM_MOUSEMOVE()
	ON_WM_NCPAINT()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
END_MESSAGE_MAP()

void CGdiPlusTabCtrl::OnUpdateUIState(UINT /*nAction*/, UINT /*nUIElement*/)
{
}

BOOL CGdiPlusTabCtrl::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;
}

HBRUSH CGdiPlusTabCtrl::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	return NULL;
}

LPTSTR CGdiPlusTabCtrl::GetTabButtonText(int nIndex)
{
	TCITEM item;
	::RtlSecureZeroMemory(&item, sizeof(item));
	TCHAR text[MAX_PATH] = {0x00};
	item.pszText = text;
	item.cchTextMax = sizeof(text);
	item.mask = TCIF_TEXT;
	GetItem(nIndex, &item);
	if(item.pszText && _tcslen(item.pszText) > 0)
	{
		LPTSTR lpString = (LPTSTR)::HeapAlloc(::GetProcessHeap(),
			HEAP_ZERO_MEMORY, sizeof(TCHAR) * (_tcslen(item.pszText) + 1));
		if(lpString)
		{
			_tcscpy(lpString, item.pszText);
			return lpString;
		}
	}
	return NULL;
}

Gdiplus::GraphicsPath * CGdiPlusTabCtrl::CreateRoundRectPath(Gdiplus::Rect r, int d, bool bCurrentTab)
{
	Gdiplus::GraphicsPath * gp = new Gdiplus::GraphicsPath();
	if(d > r.Width)  d = r.Width;
	if(d > r.Height) d = r.Height;
	Gdiplus::Rect Corner(r.X, r.Y, d, d);
	gp->Reset();
	gp->AddArc(Corner, 180, 90);
	Corner.X += (r.Width - d - 1);
	gp->AddArc(Corner, 270, 90);
	Corner.Y += (r.Height - d - 1);

	if(d == 20)
    {
        Corner.Width += 1; 
        Corner.Height += 1; 
        r.Width -=1; 
		r.Height -= 1;
    }

	Gdiplus::Rect Bottom(Corner.X + (Corner.Width - (5 +1)), Corner.Y, 5, 5);

	gp->AddArc(Bottom, 0, 90);
	Bottom.X -= (r.Width - d - 1) + 15;
	gp->AddArc(Bottom, 90, 90);
	gp->CloseFigure();
	return gp;
}

void CGdiPlusTabCtrl::DrawRoundRect(Gdiplus::Graphics * g, 
									Gdiplus::Rect rect, 
									Gdiplus::Color color, 
									int nDiameter, 
									int Line, 
									bool bCurrentTab)
{
	Gdiplus::GraphicsPath * gp = CreateRoundRectPath(rect, nDiameter, bCurrentTab);
	Gdiplus::Pen pen(color, Line);    
	pen.SetAlignment(Gdiplus::PenAlignment::PenAlignmentCenter);
	int oldPageUnit = g->GetPageUnit();
	g->SetPageUnit(Gdiplus::Unit::UnitPixel);
	g->DrawPath(&pen, gp);
	g->SetPageUnit((Gdiplus::Unit)oldPageUnit);
}

void CGdiPlusTabCtrl::OnSize(UINT nType, int cx, int cy)
{
	CTabCtrl::OnSize(nType, cx, cy);

	// TODO: Add your message handler code here
}

void CGdiPlusTabCtrl::OnPaint()
{
	CPaintDC dc(this); 
	OnDraw(&dc);
}

void CGdiPlusTabCtrl::OnDraw(CDC *pDC)
{
	if(m_bNeedRepaint)
	{
		m_MemDraw.Assign(pDC->GetSafeHdc());
		m_MemDraw.PaintParentBackground();
		Gdiplus::Rect rcClient(m_MemDraw.X(), m_MemDraw.Y(), m_MemDraw.Width(), m_MemDraw.Height());
		Gdiplus::Graphics * pGraphics = Gdiplus::Graphics::FromHDC(m_MemDraw);
		OnDrawContext(pGraphics, rcClient);
		if(pGraphics) delete pGraphics;
		m_bNeedRepaint = false;
	}
	m_MemDraw.Commit(pDC->GetSafeHdc());
}

void CGdiPlusTabCtrl::OnDrawContext(Gdiplus::Graphics * pGraphics, Gdiplus::Rect & rc)
{
	pGraphics->SetSmoothingMode(Gdiplus::SmoothingMode::SmoothingModeAntiAlias);
	int items = GetItemCount();
	for(int i = 0; i < items; ++i)
	{
		CStringW strItemTextString;
		CRect rcItem;
		GetItemRect(i, &rcItem);
		Gdiplus::Rect rcItemEx(rcItem.left, rcItem.top, rcItem.Width(), rcItem.Height());
		if(i != m_iCurrentTab)
		{
			m_bCurrentTab = false;
			rcItemEx.Y += 5;
		}
		else
		{
			rcItemEx.Height += 10;
			m_bCurrentTab = true;
		}

		OnDrawTabBackground(pGraphics, rcItemEx);

		LPTSTR lpText = GetTabButtonText(i);

#ifdef _UNICODE
		strItemTextString = lpText;
#else
		strItemTextString = CA2W(lpText);
#endif
		OnDrawItemText(pGraphics, rcItemEx, strItemTextString);

	}
}

void CGdiPlusTabCtrl::OnDrawItemText(Gdiplus::Graphics * pGraphics, 
									 Gdiplus::Rect & rc, 
									 LPCWSTR lpString)
{
	Gdiplus::Font * pFont;
	pFont = GetGdiPlusDefaultFont(m_MemDraw);

	Gdiplus::StringFormat format;
	format.SetLineAlignment(Gdiplus::StringAlignmentCenter);
	format.SetAlignment(Gdiplus::StringAlignmentCenter);
	format.SetHotkeyPrefix(Gdiplus::HotkeyPrefixShow);
	format.SetTrimming(Gdiplus::StringTrimmingWord);

	Gdiplus::SolidBrush sbr(Gdiplus::Color::White);

	pGraphics->SetTextRenderingHint(Gdiplus::TextRenderingHint::TextRenderingHintAntiAliasGridFit);

	Gdiplus::RectF rcText((float)rc.X, (float)rc.Y, (float)rc.Width, (float)rc.Height);
	pGraphics->DrawString(lpString, wcslen(lpString), pFont, rcText, &format, &sbr);
}

Gdiplus::Font * CGdiPlusTabCtrl::GetGdiPlusDefaultFont(HDC hdc)
{
	HFONT hFont;
	Gdiplus::Font * pRetFont;
	CFont * pFont = GetFont();
	if(pFont == NULL)
	{
		hFont = (HFONT)::GetStockObject(DEFAULT_GUI_FONT);
		if(hFont != NULL)
		{
			pFont = CFont::FromHandle(hFont);
		}
		if(!pFont)
		{
			return GetMyFont(hdc);
		}
	}
	LOGFONT lg;
	pFont->GetLogFont(&lg);
	pRetFont = new Gdiplus::Font(hdc, &lg);
	return pRetFont;
}

Gdiplus::Font * CGdiPlusTabCtrl::GetMyFont(HDC hdc)
{
	return new Gdiplus::Font(L"Arial", 10);
}

void CGdiPlusTabCtrl::OnDrawTabBorder(Gdiplus::Graphics * pGraphics, Gdiplus::Rect & rc)
{
	if(m_bDoubleTabBorder)
	{
		rc.Inflate(-2, -2);
		DrawRoundRect(pGraphics, rc, m_dwOuterBorderColor, 20, m_iOuterBorderWeight, true);
	}
	rc.Inflate(-1, -1);
	DrawRoundRect(pGraphics, rc, m_dwInnerBorderColor, 20, m_iInnerBorderWeight, true);
}

void CGdiPlusTabCtrl::OnDrawTabBackground(Gdiplus::Graphics * pGraphics, Gdiplus::Rect & rc)
{
	OnDrawTabBorder(pGraphics, rc);
	Gdiplus::GraphicsPath * pGrPath = CreateRoundRectPath(rc, 20, false);
	Gdiplus::SolidBrush background(Gdiplus::Color(
		/*(m_button_state == StatePushed) ? 0xCC000000 :*/ 0x7F000000));
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
	Gdiplus::Rect shine = rc;
//	shine.Height /= 2;
	Gdiplus::LinearGradientBrush shineBrush(shine, colorShine1,
		colorShine2, Gdiplus::LinearGradientModeVertical);
	pGraphics->FillRectangle(&shineBrush, shine);
	if(m_button_state != StatePushed)
	{
		Gdiplus::ARGB argbGlowStart, argbGlowEnd;
		argbGlowStart  = argbGlowEnd = (m_argbGlow & ~Gdiplus::Color::AlphaMask);
		argbGlowStart |= ((Gdiplus::ARGB)(BYTE)(((m_argbGlow &  Gdiplus::Color::AlphaMask) >> 
			Gdiplus::Color::AlphaShift) * m_fGlowLevel) << Gdiplus::Color::AlphaShift);
		Gdiplus::PathGradientBrush pgb(pGrPath);
		Gdiplus::Color colorGlowEnd(argbGlowEnd);
		int colorCount = 1;
		pgb.SetCenterColor(Gdiplus::Color(argbGlowStart));
		pgb.SetSurroundColors(&colorGlowEnd, &colorCount);
		pgb.SetCenterPoint(Gdiplus::PointF(	rc.X + rc.Width * 0.50f, 
											rc.Y + rc.Height * 1.00f));
		pGraphics->FillPath(&pgb, pGrPath);
	}
	pGraphics->ResetClip();
//	delete pGrPath;
	/*GraphicsPathPtr pGrPath = CreateRoundRectPath(rc, 20, true);
	pGraphics->SetClip(pGrPath.get());

	Gdiplus::Color startColor, endColor;
	startColor.SetValue(ARGB(255, 250, 250, 250));
	endColor.SetValue(ARGB(255, 245, 245, 245));

	Gdiplus::LinearGradientBrush gradientBrush(rc, startColor,
	endColor, Gdiplus::LinearGradientMode::LinearGradientModeVertical);

	pGraphics->FillRectangle(&gradientBrush, rc);*/
}

void CGdiPlusTabCtrl::OnTimer(UINT_PTR nIDEvent)
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

void CGdiPlusTabCtrl::OnMouseHover(UINT nFlags, CPoint point)
{
	Redraw(TRUE);
	if(!m_uGlowTimer)
	{
		m_uGlowTimer = SetTimer(1, 10, NULL);
	}
//	CTabCtrl::OnMouseHover(nFlags, point);
}

void CGdiPlusTabCtrl::OnMouseLeave()
{
	m_bMouseTrack = FALSE;
	m_button_state = StateRegular;
	Redraw(TRUE);
	if(!m_uGlowTimer)
	{
		m_uGlowTimer = SetTimer(1, 10, NULL);
	}
//	CTabCtrl::OnMouseLeave();
}

void CGdiPlusTabCtrl::OnMouseMove(UINT nFlags, CPoint point)
{
	if(!m_bMouseTrack)
	{
		TRACKMOUSEEVENT tme = { sizeof(TRACKMOUSEEVENT), TME_LEAVE | TME_HOVER, m_hWnd, 1 };
		m_bMouseTrack = ::TrackMouseEvent(&tme);
	}
	if(::PtInRect(&m_MemDraw.operator RECT(), point))
	{
		m_button_state = ButtonState::StateOver;
	}
	else
	{
		m_button_state = ButtonState::StateRegular;
	}
//	CTabCtrl::OnMouseMove(nFlags, point);
}

void CGdiPlusTabCtrl::OnNcPaint()
{
}

void CGdiPlusTabCtrl::OnLButtonDblClk(UINT nFlags, CPoint point)
{
}

void CGdiPlusTabCtrl::OnLButtonDown(UINT nFlags, CPoint point)
{
	if(::PtInRect(&m_MemDraw.operator RECT(), point))
	{
		m_button_state = ButtonState::StatePushed;
	}
	else
	{
		m_button_state = ButtonState::StateRegular;
	}
	Redraw(TRUE);
//	CTabCtrl::OnLButtonDown(nFlags, point);
}

void CGdiPlusTabCtrl::OnLButtonUp(UINT nFlags, CPoint point)
{
	if(::PtInRect(&m_MemDraw.operator RECT(), point)
		&& m_button_state == ButtonState::StatePushed)
	{
		m_iCurrentTab = GetSelectedTabIndex(point);
		SetCurFocus(m_iCurrentTab);
		SetCurSel(m_iCurrentTab);
	}
	Redraw(TRUE);
	GetParent()->RedrawWindow(NULL, NULL, RDW_NOERASE | RDW_INVALIDATE | RDW_UPDATENOW);
//	CTabCtrl::OnLButtonUp(nFlags, point);
}

BOOL CGdiPlusTabCtrl::Redraw(BOOL bUpdateNow)
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

void CGdiPlusTabCtrl::UpdateControl()
{
	Redraw(TRUE);
}

int CGdiPlusTabCtrl::GetSelectedTabIndex(CPoint & point)
{
	int Items = GetItemCount();
	for(int i = 0; i < Items; ++i)
	{
		CRect rc;
		GetItemRect(i, &rc);
		if(rc.PtInRect(point))
		{
			return i;
		}
	}
	return -1;
}

bool CGdiPlusTabCtrl::IsSelectedTabByRect(const CRect & rc)
{
	return false;
}