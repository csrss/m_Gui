// GdiStatic.cpp : implementation file
//

#include "stdafx.h"
#include "GdiStatic.h"


// CGdiStatic

IMPLEMENT_DYNAMIC(CGdiStatic, CStatic)

CGdiStatic::CGdiStatic()
:	m_bNeedRepaint(true),
	m_bUseDefaultGuiFont(false),
	m_dwCurrentTextColor(RGB(80, 80, 80)),
	m_bIsLink(false),
	m_dwRegular(RGB(255, 255, 255)),
	m_dwHovered(RGB(150, 150, 150)),
	m_dwVisited(RGB(80, 80, 80)),
	m_bClickInitialized(false),
	m_bWasVisited(false),
	m_bMouseHoover(false),
	m_bMouseTrack(FALSE),
	m_strLinkTooltip(L""),
	m_strWebSiteLink(L""),
	m_wndTooltip(NULL),
	m_bTooltipTrackable(false)
{
}

CGdiStatic::~CGdiStatic()
{
}

BEGIN_MESSAGE_MAP(CGdiStatic, CStatic)
	ON_WM_SIZE()
	ON_WM_CTLCOLOR()
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSELEAVE()
	ON_WM_SETCURSOR()
END_MESSAGE_MAP()

// CGdiStatic message handlers

HDC CGdiStatic::PaintParentBackground(HDC hdc, RECT m_rcClient)
{
	POINT ptOrg;
	HRGN  hRgn;
	RECT  rcTranslate = m_rcClient;
	MapWindowPoints(GetParent(), (LPPOINT)&rcTranslate, 2);
	if(!::SetWindowOrgEx(hdc, rcTranslate.left, rcTranslate.top, &ptOrg))
	{
		return hdc;
	}

	hRgn = ::CreateRectRgnIndirect(&m_rcClient);
	if(hRgn == NULL)
	{
		return hdc;
	}
	::SelectClipRgn(hdc, hRgn);
	::DeleteObject(hRgn);
	::SendMessage(GetParent()->GetSafeHwnd(), WM_PRINTCLIENT, (WPARAM)hdc, PRF_CLIENT);
	::SelectClipRgn(hdc, NULL);
	::SetWindowOrgEx(hdc, ptOrg.x, ptOrg.y, NULL);
	return hdc;
}

void CGdiStatic::PreSubclassWindow()
{
	PostMessage(WM_SIZE);
	CStatic::PreSubclassWindow();
}

void CGdiStatic::OnSize(UINT nType, int cx, int cy)
{
	GetClientRect(&m_rcClientRect);
	Redraw(TRUE);
}

BOOL CGdiStatic::Redraw(BOOL bUpdateNow)
{
	m_bNeedRepaint = true;
	if(bUpdateNow)
	{
		return ::RedrawWindow(m_hWnd, NULL, NULL, RDW_NOERASE | 
			RDW_INVALIDATE | RDW_UPDATENOW);
	}
	else
	{
		return ::InvalidateRect(m_hWnd, NULL, FALSE);
	}
}

HBRUSH CGdiStatic::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	return NULL;
}

BOOL CGdiStatic::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;
}

void CGdiStatic::OnDrawText()
{
	SetBkMode(m_hMemoryDC, TRANSPARENT);
	SetTextColor(m_hMemoryDC, m_dwCurrentTextColor);
	HFONT hCurrentFont;
	if(m_bUseDefaultGuiFont)
	{
		hCurrentFont = (HFONT)GetStockObject(DEFAULT_GUI_FONT);
		if(hCurrentFont == NULL)
		{
			TRACE("Unable to create new font!\n");
			return;
		}
	}
	else
	{
		hCurrentFont = CreateFontIndirect(&m_logCurrentFont);
		if(hCurrentFont == NULL)
		{
			TRACE("Unable to create new font!\n");
			return;
		}
	}
	HFONT hOldFont = (HFONT)::SelectObject(m_hMemoryDC, hCurrentFont);
	CStringW str = GetWindowTextWideChar();
	DrawTextW(m_hMemoryDC, str, str.GetLength(), &m_rcClientRect, DT_WORDBREAK);
	::SelectObject(m_hMemoryDC, hOldFont);
	::DeleteObject(hCurrentFont);
}

void CGdiStatic::OnDrawLink()
{
	SetBkMode(m_hMemoryDC, TRANSPARENT);
	SetTextColor(m_hMemoryDC, 
		m_bMouseHoover ? m_dwHovered : (m_bWasVisited ? m_dwVisited : m_dwRegular));
	SetupFont(m_hMemoryDC, 10, FontWeigth::Normal, false, true);
	HFONT hCurrentFont = CreateFontIndirect(&m_logCurrentFont);
	if(hCurrentFont == NULL)
	{
		hCurrentFont = (HFONT)GetStockObject(DEFAULT_GUI_FONT);
		if(hCurrentFont == NULL)
		{
			TRACE("Unable to create font for LINK!\n");
			return;
		}
	}
	HFONT hOldFont = (HFONT)::SelectObject(m_hMemoryDC, hCurrentFont);
	CStringW str = GetWindowTextWideChar();
	DrawTextW(m_hMemoryDC, str, str.GetLength(), &m_rcClientRect, 0);

	m_rcTextRectangle.left = 0;
	m_rcTextRectangle.top = 0;
	DrawTextW(m_hMemoryDC, str, str.GetLength(), &m_rcTextRectangle, DT_CALCRECT);

	::SelectObject(m_hMemoryDC, hOldFont);
	::DeleteObject(hCurrentFont);
}

void CGdiStatic::OnPaint()
{
	CPaintDC dc(this);   
	if(m_bNeedRepaint)
	{
		if(m_hMemoryDC && m_hBitmap)
		{
			::SelectObject(m_hMemoryDC, m_hOldBitmap);
			::DeleteObject(m_hBitmap);
			::DeleteDC(m_hMemoryDC);
		}
		m_hMemoryDC = CreateCompatibleDC(dc);
		m_hBitmap = CreateCompatibleBitmap(dc, m_rcClientRect.right, m_rcClientRect.bottom);
		m_hOldBitmap = (HBITMAP)::SelectObject(m_hMemoryDC, m_hBitmap);
		PaintParentBackground(m_hMemoryDC, m_rcClientRect);
		if(!m_bIsLink)
		{
			OnDrawText();
		}
		else
		{
			OnDrawLink();
		}
		m_bNeedRepaint = false;
	}

	BitBlt(	dc, 0, 0, 
			m_rcClientRect.right, m_rcClientRect.bottom, 
			m_hMemoryDC, 0, 0, SRCCOPY);
}

void CGdiStatic::OnMouseMove(UINT nFlags, CPoint point)
{
	if(m_bIsLink) // inaczej nie ma sensu sledzic tego
	{
		if(!m_bMouseTrack)
		{
			TRACKMOUSEEVENT tme = { sizeof(TRACKMOUSEEVENT), 
									TME_LEAVE | TME_HOVER, m_hWnd, 1 };
			m_bMouseTrack = TrackMouseEvent(&tme);
		}
		m_ptCurrentPoint = point;
		if(PtInRect(&m_rcTextRectangle, m_ptCurrentPoint))
		{
			if(!m_bMouseHoover)
			{
				m_bMouseHoover = true;
			}
			::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_HAND));
			if(m_bTooltipTrackable && !m_strLinkTooltip.IsEmpty() && m_wndTooltip)
			{
				CPoint cp = point;
				ClientToScreen(&cp);
				::SendMessage(m_wndTooltip, TTM_TRACKPOSITION, 0, (LPARAM)MAKELONG(cp.x, cp.y + 30));
				::SendMessage(m_wndTooltip, TTM_TRACKACTIVATE, (WPARAM)TRUE, (LPARAM)&m_toolInfo);
			}
		}
		else
		{
			m_bMouseHoover = false;
			::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
			if(m_bTooltipTrackable && !m_strLinkTooltip.IsEmpty() && m_wndTooltip)
			{
				::SendMessage(m_wndTooltip, TTM_TRACKACTIVATE, (WPARAM)FALSE, (LPARAM)&m_toolInfo);
			}
		}
		Redraw(TRUE);
	}
	CStatic::OnMouseMove(nFlags, point);
}

void CGdiStatic::SetupFont(HDC hdc,
						   int nSize,
						   CGdiStatic::FontWeigth fontWeigth, 
						   bool bItalic, 
						   bool bUnderline, 
						   bool bStrikeOut, 
						   CGdiStatic::FontQuality fontQuality, 
						   CString strFontFamily, 
						   bool bUpdateNow)
{
	//m_logCurrentFont.lfWeight = fontWeigth;
	//m_logCurrentFont.lfItalic = bItalic;
	//m_logCurrentFont.lfUnderline = bUnderline;
	//m_logCurrentFont.lfStrikeOut = bStrikeOut;
	//m_logCurrentFont.lfQuality = fontQuality;
	//RtlSecureZeroMemory(&m_logCurrentFont.lfFaceName, sizeof m_logCurrentFont.lfFaceName);
	//strcpy_s((char *)m_logCurrentFont.lfFaceName, LF_FACESIZE, strFontFamily.GetBuffer());
	//m_logCurrentFont.lfWidth = 0;
	//m_logCurrentFont.lfHeight = -MulDiv(nSize, GetDeviceCaps(hdc, LOGPIXELSY), 72);
	//m_logCurrentFont.lfEscapement = 0;
	//if(!bUpdateNow) return;
	//Redraw(TRUE);
}

void CGdiStatic::SetupTextColor(DWORD dwColor, bool bUpdateNow)
{
	m_dwCurrentTextColor = dwColor;
	if(!bUpdateNow) return;
	Redraw(TRUE);
}

CStringW CGdiStatic::MultiToWide(CStringA strValue)
{
	CStringW strTemp;
	int n = strValue.GetLength();
	DWORD dwWritten = MultiByteToWideChar(CP_ACP, 
										  0, 
										  (LPCSTR)strValue, 
										  -1, 
										  strTemp.GetBuffer(n), 
										  (n * sizeof (wchar_t *) + 1));
	CStringW strOutString(strTemp);
	strTemp.ReleaseBuffer();
	return strOutString;
}

CStringW CGdiStatic::GetWindowTextWideChar()
{
	//CString strTemp;
	//GetWindowText(strTemp);
	//return MultiToWide(strTemp);
	return L"";
}

CStringA CGdiStatic::GetTextA()
{
	//CString strTemp;
	//GetWindowText(strTemp);
	//return strTemp;
	return "";
}

CStringW CGdiStatic::GetTextW()
{
	return GetWindowTextWideChar();
}

BOOL CGdiStatic::SetTextA(CStringA strValue, bool bUpdateNow)
{
	BOOL bStatus = ::SetWindowTextA(this->m_hWnd, (LPCSTR)strValue);
	if(bUpdateNow)
	{
		Redraw(TRUE);
	}
	return bStatus;
}

BOOL CGdiStatic::SetTextW(CStringW strValue, bool bUpdateNow)
{
	BOOL bStatus = ::SetWindowTextW(this->m_hWnd, (LPCWSTR)strValue);
	if(bUpdateNow)
	{
		Redraw(TRUE);
	}
	return bStatus;
}

void CGdiStatic::SetupFont(bool bUnderline, bool bUpdateNow)
{
	m_logCurrentFont.lfUnderline = bUnderline;
	if(!bUpdateNow) return;
	Redraw(TRUE);
}

void CGdiStatic::SetupLinkColors(DWORD dwRegular, DWORD dwHovered, DWORD dwVisited, bool bUpdateNow)
{
	m_dwRegular = dwRegular;
	m_dwHovered = dwHovered;
	m_dwVisited = dwVisited;
	if(!bUpdateNow) return;
	Redraw(TRUE);
}

void CGdiStatic::MakeItLink(bool bUpdateNow)
{
	m_bIsLink = true;
	if(!bUpdateNow) return;
	Redraw(TRUE);
}

void CGdiStatic::MakeItText(bool bUpdateNow)
{
	m_bIsLink = false;
	if(!bUpdateNow) return;
	Redraw(TRUE);
}

void CGdiStatic::OnLButtonDown(UINT nFlags, CPoint point)
{
	if(m_bIsLink && !m_strWebSiteLink.IsEmpty() &&
		PtInRect(&m_rcTextRectangle, m_ptCurrentPoint))
	{
		m_bClickInitialized = true;
	}
	CStatic::OnLButtonDown(nFlags, point);
}

void CGdiStatic::OnLButtonUp(UINT nFlags, CPoint point)
{
	if(m_bIsLink && m_bClickInitialized && !m_strWebSiteLink.IsEmpty() &&
		PtInRect(&m_rcTextRectangle, m_ptCurrentPoint))
	{
		SHELLEXECUTEINFOW sei = {sizeof(SHELLEXECUTEINFO),
								0, NULL, L"open",
								m_strWebSiteLink,
								NULL, NULL, SW_SHOWNORMAL,
								0, 0, 0, 0, 0, 0, 0};
		ShellExecuteExW(&sei);
		m_bClickInitialized = false;
		m_bWasVisited = true;
		Redraw(TRUE);
	}
	CStatic::OnLButtonUp(nFlags, point);
}

void CGdiStatic::OnMouseLeave()
{
	// TODO: Add your message handler code here and/or call default
	if(m_bIsLink)
	{
		m_bClickInitialized = false;
		m_bMouseHoover = false;
		m_bMouseTrack = FALSE;
		Redraw(TRUE);
		return;
	}
	CStatic::OnMouseLeave();
}

BOOL CGdiStatic::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	if(m_bIsLink && !m_strWebSiteLink.IsEmpty())
	{
		if(PtInRect(&m_rcTextRectangle, m_ptCurrentPoint))
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

void CGdiStatic::SetupLinkData(CString strWebSite, 
							   CString strTooltip,
							   bool bTrackable,
							   bool bUpdateNow)
{
	//m_strWebSiteLink = MultiToWide(strWebSite);
	//m_strLinkTooltip = MultiToWide(strTooltip);
	//m_bTooltipTrackable = bTrackable;
	//if(!m_strLinkTooltip.IsEmpty() || !m_strWebSiteLink.IsEmpty())
	//{
	//	InitTooltip(true, !strTooltip.IsEmpty() ? strTooltip : strWebSite, bTrackable);
	//}
	//if(!bUpdateNow) return;
	//Redraw(TRUE);
}

void CGdiStatic::UnsetClicked(bool bUpdateNow)
{
	m_bWasVisited = false;
	if(!bUpdateNow) return;
	Redraw(TRUE);
}

void CGdiStatic::DestroyTooltip()
{
	InitTooltip(false, TEXT(""), false);
}

void CGdiStatic::InitTooltip(bool bInit, CString strText, bool bTrackable)
{
	if(m_wndTooltip)
	{
		::DestroyWindow(m_wndTooltip);
		m_wndTooltip = NULL;
	}
	if(!bInit || strText.IsEmpty()) return;
	::RtlSecureZeroMemory(&m_toolInfo, sizeof TOOLINFOW);
	HINSTANCE g_hInst = (HINSTANCE)::GetWindowLong(::GetParent(m_hWnd), -6);
	HWND hwndTool = ::GetDlgItem(::GetAncestor(m_hWnd, GA_ROOT), ::GetWindowLong(m_hWnd, GWL_ID));
	if(!hwndTool)
	{
		TRACE("Tool window cannot be found!\n");
		return;
	}
	m_wndTooltip = CreateWindowExW(0, L"tooltips_class32", NULL,
										WS_POPUP,
										CW_USEDEFAULT, CW_USEDEFAULT,
										CW_USEDEFAULT, CW_USEDEFAULT,
										::GetParent(m_hWnd), NULL, 
										g_hInst, this);
	if(!m_wndTooltip)
	{
		TRACE("Cannot create tooltip window!\n");
		return;
	}
	m_toolInfo.cbSize = sizeof(TOOLINFOW);
	m_toolInfo.hwnd = ::GetParent(m_hWnd);
	m_toolInfo.uFlags = TTF_IDISHWND;
	m_toolInfo.uFlags |= bTrackable ? TTF_TRACK : TTF_SUBCLASS;
	m_toolInfo.uId = (__w64 unsigned int)hwndTool;
	m_toolInfo.lpszText = strText.GetBuffer();
	::SendMessageW(m_wndTooltip, TTM_ADDTOOL, 0, (__w64 long)&m_toolInfo);
	::SendMessageW(m_wndTooltip, TTM_SETMAXTIPWIDTH, 0, 300);
	TRACE("tooltip created!\n");
	strText.ReleaseBuffer();
}