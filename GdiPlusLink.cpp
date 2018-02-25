// GdiPlusLink.cpp : implementation file
//

#include "stdafx.h"
#include "GdiPlusLink.h"


// CGdiPlusLink

IMPLEMENT_DYNAMIC(CGdiPlusLink, CStatic)

CGdiPlusLink::CGdiPlusLink()
:	m_bNeedRepaint(true),
	m_dwLinkRegularColor(RGB(255, 255, 255)),
	m_dwLinkVisitedColor(RGB(70, 70, 70)),
	m_dwLinkHooveredColor(RGB(255, 120, 120)),
	m_dwLinkDisabledColor(RGB(100, 0, 0)),
	m_bMouseTrack(FALSE),
	m_strWebSiteLink(NULL),
	m_strWindowText(NULL),
	m_bVisited(FALSE),
	m_button_state(ButtonState::StateRegular)
{
	CG_Text.Set_Prop(this, &CGdiPlusLink::GetText, &CGdiPlusLink::SetText);
	CG_Visited.Set_Prop(this, &CGdiPlusLink::GetVisited, &CGdiPlusLink::SetVisited);
	CG_RgbLinkDisabledColor.Set_Prop(this, &CGdiPlusLink::GetRgbLinkDisabledColor, 
		&CGdiPlusLink::SetRgbLinkDisabledColor);
	CG_RgbLinkHooveredColor.Set_Prop(this, &CGdiPlusLink::GetRgbLinkHooveredColor, 
		&CGdiPlusLink::SetRgbLinkHooveredColor);
	CG_RgbLinkVisitedColor.Set_Prop(this, &CGdiPlusLink::GetRgbLinkVisitedColor, 
		&CGdiPlusLink::SetRgbLinkVisitedColor);
	CG_RgbLinkRegularColor.Set_Prop(this, &CGdiPlusLink::GetRgbLinkRegularColor, 
		&CGdiPlusLink::SetRgbLinkRegularColor);
	CG_WebSiteLink.Set_Prop(this, &CGdiPlusLink::GetWebSiteLink, &CGdiPlusLink::SetWebSiteLink);
}

CGdiPlusLink::~CGdiPlusLink()
{
	if(m_strWebSiteLink)
	{
		::HeapFree(::GetProcessHeap(), 0, m_strWebSiteLink);
		m_strWebSiteLink = NULL;
	}
	if(m_strWindowText)
	{
		::HeapFree(::GetProcessHeap(), 0, m_strWindowText);
		m_strWindowText = NULL;
	}
}

BEGIN_MESSAGE_MAP(CGdiPlusLink, CStatic)
	ON_WM_CTLCOLOR()
	ON_WM_ERASEBKGND()
	ON_WM_SIZE()
	ON_WM_PAINT()
	ON_WM_NCPAINT()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEHOVER()
	ON_WM_MOUSELEAVE()
	ON_WM_MOUSEMOVE()
	ON_WM_SETCURSOR()
	ON_MESSAGE(WM_SETTEXT, OnSetText)
	ON_WM_UPDATEUISTATE()
END_MESSAGE_MAP()

// CGdiPlusLink message handlers

LRESULT CGdiPlusLink::OnSetText(WPARAM wParam, LPARAM lParam)
{
	if(lParam)
	{
		m_MemDraw.GdiStringAllocateCopy(m_strWindowText, (LPTSTR)lParam);
	}
	return 0L;
}

HBRUSH CGdiPlusLink::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	return NULL;
}

BOOL CGdiPlusLink::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;
}

void CGdiPlusLink::OnSize(UINT nType, int cx, int cy)
{
	Redraw(TRUE);
}

void CGdiPlusLink::OnPaint()
{
	CPaintDC dc(this);
	OnDraw(&dc);
}

void CGdiPlusLink::OnNcPaint()
{
}

void CGdiPlusLink::Redraw(BOOL bUpdateNow)
{
	m_MemDraw.Redraw(bUpdateNow, &m_bNeedRepaint);
}

void CGdiPlusLink::OnLButtonDblClk(UINT nFlags, CPoint point)
{
}

void CGdiPlusLink::OnLButtonDown(UINT nFlags, CPoint point)
{
	if(::PtInRect(&m_MemDraw.operator RECT(), point))
	{
		m_button_state = ButtonState::StatePushed;
	}
	else
	{
		m_button_state = ButtonState::StateRegular;
	}
}

void CGdiPlusLink::OnLButtonUp(UINT nFlags, CPoint point)
{
	if(::PtInRect(&m_MemDraw.operator RECT(), point) && 
		m_button_state == ButtonState::StatePushed)
	{
		::PostMessage(::GetParent(m_hWnd), WM_COMMAND, 
			MAKELONG(::GetWindowLong(m_hWnd, GWL_ID), BN_CLICKED), (LPARAM)m_hWnd);
		m_bVisited = TRUE;
		if(::PtInRect(&m_MemDraw.operator RECT(), point))
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

void CGdiPlusLink::OnMouseHover(UINT nFlags, CPoint point)
{
	Redraw(TRUE);
}

void CGdiPlusLink::OnMouseLeave()
{
	m_button_state = ButtonState::StateRegular;
	m_bMouseTrack = FALSE;
	Redraw(TRUE);
}

void CGdiPlusLink::OnMouseMove(UINT nFlags, CPoint point)
{
	m_ptCurrentPoint = point;
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
	Redraw(TRUE);
}

void CGdiPlusLink::OnDraw(CDC *pDC)
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

void CGdiPlusLink::OnDrawContext(Gdiplus::Graphics * pGraphics, Gdiplus::Rect & rc)
{
#ifdef _UNICODE
	LPTSTR lpWindowText = m_strWindowText ? m_strWindowText : m_MemDraw.GdiGetWindowTextW();
#else
	LPTSTR lpWindowText = m_strWindowText ? m_strWindowText : m_MemDraw.GdiGetWindowTextA();
#endif

	if(!lpWindowText || _tcslen(lpWindowText) <= 0)
	{
		return;
	}

	DWORD dwStyle = ::GetWindowLongPtr(m_hWnd, GWL_STYLE);
	if((dwStyle & WS_DISABLED) == WS_DISABLED)
	{
		m_MemDraw.GdiDrawText(lpWindowText, m_dwLinkDisabledColor);
		::HeapFree(::GetProcessHeap(), 0, lpWindowText);
		lpWindowText = NULL;
		return;
	}

	switch(m_button_state)
	{
		case ButtonState::StateOver:
		{
			m_MemDraw.GdiDrawText(lpWindowText, m_dwLinkHooveredColor, NULL, TRUE);
		}
		break;

		case ButtonState::StateRegular:
		{
			if(m_bVisited == TRUE)
			{
				m_MemDraw.GdiDrawText(lpWindowText, m_dwLinkVisitedColor);
			}
			else
			{
				m_MemDraw.GdiDrawText(lpWindowText, m_dwLinkRegularColor);
			}
		}
		break;
	}

	::HeapFree(::GetProcessHeap(), 0, lpWindowText);
	lpWindowText = NULL;
}

LRESULT CGdiPlusLink::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message)
	{
		case WM_PRINTCLIENT:	// paint background
			{
				OnDraw(CDC::FromHandle((HDC)wParam));
			}
		break;
	}
	return CStatic::WindowProc(message, wParam, lParam);
}

void CGdiPlusLink::PreSubclassWindow()
{
	DWORD dwStyle = ::GetWindowLongPtr(m_hWnd, GWL_STYLE);
	dwStyle |= SS_NOTIFY;
	::SetWindowLongPtr(m_hWnd, GWL_STYLE, dwStyle);
	CStatic::PreSubclassWindow();
}

BOOL CGdiPlusLink::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	if(m_strWebSiteLink)
	{
		if(::PtInRect(&m_MemDraw.operator RECT(), m_ptCurrentPoint))
		{
			::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_HAND));
			return TRUE;
		}
		else
		{
			::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
			return FALSE;
		}
	}
	return CStatic::OnSetCursor(pWnd, nHitTest, message);
}

void CGdiPlusLink::SetWebSiteLink(LPTSTR lpLinkString)
{
	if(lpLinkString && _tcslen(lpLinkString))
	{
		m_MemDraw.GdiStringAllocateCopy(m_strWebSiteLink, lpLinkString);
	}
}

LPTSTR CGdiPlusLink::GetWebSiteLink()
{
	return m_strWebSiteLink;
}

void CGdiPlusLink::OnUpdateUIState(UINT /*nAction*/, UINT /*nUIElement*/)
{
}

void CGdiPlusLink::SetRgbLinkRegularColor(DWORD dwValue)
{
	m_dwLinkRegularColor = dwValue;
}

DWORD CGdiPlusLink::GetRgbLinkRegularColor()
{
	return m_dwLinkRegularColor;
}

void CGdiPlusLink::SetRgbLinkVisitedColor(DWORD dwValue)
{
	m_dwLinkVisitedColor = dwValue;
}

DWORD CGdiPlusLink::GetRgbLinkVisitedColor()
{
	return m_dwLinkVisitedColor;
}

void CGdiPlusLink::SetRgbLinkHooveredColor(DWORD dwValue)
{
	m_dwLinkHooveredColor = dwValue;
}

DWORD CGdiPlusLink::GetRgbLinkHooveredColor()
{
	return m_dwLinkHooveredColor;
}

void CGdiPlusLink::SetRgbLinkDisabledColor(DWORD dwValue)
{
	m_dwLinkDisabledColor = dwValue;
}

DWORD CGdiPlusLink::GetRgbLinkDisabledColor()
{
	return m_dwLinkDisabledColor;
}

void CGdiPlusLink::SetVisited(BOOL bValue)
{
	m_bVisited = bValue;
}

BOOL CGdiPlusLink::GetVisited() 
{
	return m_bVisited;
}

void CGdiPlusLink::SetText(LPTSTR lpString)
{
	if(lpString)
	{
		m_MemDraw.GdiStringAllocateCopy(m_strWindowText, lpString);
	}
}

LPTSTR CGdiPlusLink::GetText()
{
	return m_strWindowText;
}

void CGdiPlusLink::UpdateControl()
{
	Redraw(TRUE);
}