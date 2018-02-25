#include "stdafx.h"
#include "GdiPlusGroupBox.h"

IMPLEMENT_DYNAMIC(CGdiPlusGroupBox, CStatic)

CGdiPlusGroupBox::CGdiPlusGroupBox()
:	m_bNeedRepaint(true), 
	m_dwTextColor(ARGB(255, 255, 255, 255)),
	m_iRoundOffset(15),
	m_dwFrameBorderColor(Gdiplus::Color::Gray),
	m_iFrameBorderWeight(1),
	m_dwTextColorDisabled(Gdiplus::Color::DarkGray),
	m_dwFrameBorderColorDisabled(Gdiplus::Color::DarkGray),
	m_bEnabled(TRUE)
{
	CG_Text.Set_Prop(this, 
		&CGdiPlusGroupBox::GetText, 
		&CGdiPlusGroupBox::SetText);

	CG_TextColor.Set_Prop(this, 
		&CGdiPlusGroupBox::GetTextColor, 
		&CGdiPlusGroupBox::SetTextColor);

	CG_RoundOffset.Set_Prop(this, 
		&CGdiPlusGroupBox::GetRoundOffset, 
		&CGdiPlusGroupBox::SetRoundOffset);

	CG_FrameBorderWeight.Set_Prop(this, 
		&CGdiPlusGroupBox::GetFrameBorderWeight, 
		&CGdiPlusGroupBox::SetFrameBorderWeight);

	CG_TextColorDisabled.Set_Prop(this, 
		&CGdiPlusGroupBox::GetTextColorDisabled, 
		&CGdiPlusGroupBox::SetTextColorDisabled);

	CG_FrameBorderColorDisabled.Set_Prop(this,
		&CGdiPlusGroupBox::GetFrameBorderColorDisabled,
		&CGdiPlusGroupBox::SetFrameBorderColorDisabled);
}

CGdiPlusGroupBox::~CGdiPlusGroupBox()
{
}

BEGIN_MESSAGE_MAP(CGdiPlusGroupBox, CStatic)
	ON_WM_CTLCOLOR()
	ON_WM_ERASEBKGND()
	ON_WM_NCPAINT()
	ON_WM_UPDATEUISTATE()
	ON_WM_SIZE()
	ON_WM_PAINT()
	ON_MESSAGE(WM_SETTEXT, OnSetText)
	ON_MESSAGE(WM_ENABLE, OnEnable)
END_MESSAGE_MAP()

LRESULT CGdiPlusGroupBox::OnSetText(WPARAM wParam, LPARAM lParam)
{
	m_strWindowText = (LPTSTR)lParam;
	Redraw(TRUE);
	return TRUE;
}

BOOL CGdiPlusGroupBox::Redraw(BOOL bUpdateNow)
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

BOOL CALLBACK CGdiPlusGroupBox::RedrawChildren(HWND hwnd, LPARAM lParam)
{
	CGdiPlusGroupBox * me = ((CGdiPlusGroupBox *)lParam);
	if(::IsWindow(hwnd) && ::GetParent(hwnd) == me->m_hWnd)
	{
		::PostMessage(hwnd, WM_SIZE, 0, 0);
	}
	return TRUE;
}

HBRUSH CGdiPlusGroupBox::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	return NULL;
}

BOOL CGdiPlusGroupBox::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;
}

void CGdiPlusGroupBox::OnNcPaint()
{
	// no fucking cocksucking bitchin' borders, client shitty areas fuck that!
	// i dont give a fuck 'bout non of you ppl say
}

void CGdiPlusGroupBox::OnUpdateUIState(UINT /*nAction*/, UINT /*nUIElement*/)
{
	// suck my dick biatch! its Golden cobra
}

void CGdiPlusGroupBox::OnSize(UINT nType, int cx, int cy)
{
	Redraw(TRUE);
	RedrawChildren(m_hWnd, (LPARAM)this);
}

void CGdiPlusGroupBox::PreSubclassWindow()
{
	PostMessage(WM_SIZE);
	CStatic::PreSubclassWindow();
}

LRESULT CGdiPlusGroupBox::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message)
	{
		case WM_PRINTCLIENT:	// paint background
			{
				OnDraw(CDC::FromHandle((HDC)wParam));
			}
		break;

		case WM_COMMAND:	// relay command to upper level window
			{
				GetParent()->PostMessage(WM_COMMAND, wParam, lParam);
			}
		break;
	}
	return CStatic::WindowProc(message, wParam, lParam);
}

void CGdiPlusGroupBox::OnPaint()
{
	CPaintDC dc(this); 
	OnDraw(&dc);
}

void CGdiPlusGroupBox::OnDraw(CDC *pDC)
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

void CGdiPlusGroupBox::OnDrawContext(Gdiplus::Graphics * pGraphics, Gdiplus::Rect & rc)
{
	Gdiplus::Font * pFont;
	pGraphics->SetSmoothingMode(Gdiplus::SmoothingMode::SmoothingModeAntiAlias);
	pFont = GetGdiPlusDefaultWindowFont(m_MemDraw);
	Gdiplus::SizeF txtSize;
	rc.Inflate(-m_iFrameBorderWeight, -m_iFrameBorderWeight);
	OnDrawText(pGraphics, rc, pFont, txtSize);
	OnDrawFrame(pGraphics, rc, txtSize);
}

void CGdiPlusGroupBox::OnDrawFrame(Gdiplus::Graphics * pGraphics, 
								   Gdiplus::Rect & rc,
								   Gdiplus::SizeF txtSize)
{	// !!!DO NOT TOUCH THIS!!! (and donot use floats)
	LONG_PTR dwStyle = ::GetWindowLongPtr(m_hWnd, GWL_STYLE);
	int fArcQube = m_iRoundOffset;
	Gdiplus::Pen p(m_bEnabled ? m_dwFrameBorderColor : m_dwFrameBorderColorDisabled, 
		(float)m_iFrameBorderWeight);
	Gdiplus::Rect rcTopLeft(rc.X, rc.Y + ((int)txtSize.Height / 2), 
							fArcQube, fArcQube);
	pGraphics->DrawArc(&p, rcTopLeft, 180, 90);
	pGraphics->DrawLine(&p, rc.X, rc.Y + (((int)txtSize.Height / 2) + (fArcQube / 2)), 
						rc.X, rc.Height - (fArcQube / 2));
	Gdiplus::Rect rcLeftBottom(	rc.X, rc.Height - (fArcQube), 
								fArcQube, fArcQube);
	pGraphics->DrawArc(&p, rcLeftBottom, 90, 90);
	pGraphics->DrawLine(&p, rc.X + (fArcQube / 2), rc.Height, 
						rc.Width - (fArcQube / 2), rc.Height);
	Gdiplus::Rect rcRightBottom(rc.Width - (fArcQube), 
								rc.Height - (fArcQube), fArcQube, fArcQube);
	pGraphics->DrawArc(&p, rcRightBottom, 0, 90);
	pGraphics->DrawLine(&p, rc.Width, rc.Height - (fArcQube / 2), 
						rc.Width, rc.Y + (((int)txtSize.Height / 2) + (fArcQube / 2)));
	Gdiplus::Rect rcRightTop(rc.Width - (fArcQube), 
							 rc.Y + ((int)txtSize.Height / 2), fArcQube, fArcQube);
	pGraphics->DrawArc(&p, rcRightTop, 270, 90);
	if((dwStyle & BS_CENTER) == BS_CENTER)
	{
		int iLinePart = (rc.Width - (int)txtSize.Width) / 2;
		pGraphics->DrawLine(&p, rc.X + (fArcQube / 2), rc.Y + ((int)txtSize.Height / 2),
			iLinePart - (fArcQube / 2 + 2), rc.Y + ((int)txtSize.Height / 2));
		pGraphics->DrawLine(&p, rc.X + (iLinePart + (int)txtSize.Width + (fArcQube / 2 + 2)),
			rc.Y + ((int)txtSize.Height / 2), rc.Width - (fArcQube / 2), 
			rc.Y + ((int)txtSize.Height / 2));
	}
	else if((dwStyle & BS_RIGHT) == BS_RIGHT)
	{
		pGraphics->DrawLine(&p, rc.X + (fArcQube / 2), rc.Y + ((int)txtSize.Height / 2), 
			rc.X + (rc.Width - (int)txtSize.Width - fArcQube - (fArcQube / 2 + 2)), 
			rc.Y + ((int)txtSize.Height / 2));
	}
	else
	{
		pGraphics->DrawLine(&p, rc.X + ((int)txtSize.Width + fArcQube + (fArcQube / 2 + 2)), 
							rc.Y + ((int)txtSize.Height / 2), rc.Width - (fArcQube / 2), 
							rc.Y + ((int)txtSize.Height / 2));
	}
	// !!!DO NOT TOUCH THIS!!!
}

void CGdiPlusGroupBox::OnDrawText(Gdiplus::Graphics * pGraphics, 
								  Gdiplus::Rect & rc, 
								  Gdiplus::Font * pFont, 
								  Gdiplus::SizeF & txtSize)
{
	CString strWindowText;
	if(m_strWindowText.IsEmpty())
	{
		GetWindowText(strWindowText);
	}
	else
	{
		strWindowText = m_strWindowText;
	}

#ifdef _UNICODE
	CStringW strText = strWindowText;
#else
	USES_CONVERSION;
	CStringW strText = A2W(strWindowText);
#endif

	LONG_PTR dwStyle = ::GetWindowLongPtr(m_hWnd, GWL_STYLE);
	txtSize = GetTextSize(pGraphics, strText, pFont, rc);
	Gdiplus::RectF rcText((float)rc.X, (float)rc.Y, (float)rc.Width, (float)rc.Height);

	Gdiplus::StringFormat format;
	format.SetLineAlignment(Gdiplus::StringAlignment::StringAlignmentNear);

	rcText.X += m_iRoundOffset;
	rcText.Width -= m_iRoundOffset * 2;

	if((dwStyle & BS_CENTER) == BS_CENTER)
	{
		format.SetAlignment(Gdiplus::StringAlignment::StringAlignmentCenter);
	}
	else if((dwStyle & BS_RIGHT) == BS_RIGHT)
	{
		format.SetAlignment(Gdiplus::StringAlignment::StringAlignmentFar);
	}
	else
	{
		format.SetAlignment(Gdiplus::StringAlignment::StringAlignmentNear);
	}
	Gdiplus::SolidBrush sbr(m_bEnabled ? m_dwTextColor : m_dwTextColorDisabled);
	pGraphics->DrawString(strText, -1, pFont, rcText, &format, &sbr);
}

Gdiplus::SizeF CGdiPlusGroupBox::GetTextSize(Gdiplus::Graphics * pGraphics, 
											 LPCWSTR lpString, 
											 Gdiplus::Font * pFont,
											 Gdiplus::Rect & rcContainer)
{
	Gdiplus::SizeF sz(0.0, 0.0);
	Gdiplus::RectF rcBounds;
	if(pGraphics == NULL || lpString == NULL || pFont == NULL) 
	{
		return sz;
	}
	pGraphics->MeasureString(lpString, 
							 wcslen(lpString), 
							 pFont, 
							 Gdiplus::PointF((float)rcContainer.X, (float)rcContainer.Y),
							 &rcBounds);
	sz.Width = rcBounds.Width;
	sz.Height = rcBounds.Height;
	return sz;
}

CFont * CGdiPlusGroupBox::GetMyFont(HDC hdc)
{
	return NULL;
}

Gdiplus::Font * CGdiPlusGroupBox::GetGdiPlusDefaultWindowFont(HDC hdc)
{
	if(hdc == NULL)
	{
		return new Gdiplus::Font(L"Arial", 12);
	}
	HFONT hFont;
	Gdiplus::Font * pRetFont;
	CFont * pFont = CWnd::FromHandle(m_hWnd)->GetFont();
	if(pFont == NULL)
	{
		hFont = (HFONT)::GetStockObject(DEFAULT_GUI_FONT);
		if(hFont != NULL)
		{
			pFont = CFont::FromHandle(hFont);
		}
		else
		{
			pFont = GetMyFont(hdc);
		}
		if(!pFont) return new Gdiplus::Font(L"Arial", 12);
	}
	LOGFONT lg;
	pFont->GetLogFont(&lg);
	pRetFont = new Gdiplus::Font(hdc, &lg);
	return pRetFont;
}

//////////////////////////////////// getters / setters

int CGdiPlusGroupBox::GetRoundOffset()
{
	return m_iRoundOffset;
}

void CGdiPlusGroupBox::SetRoundOffset(int iValue)
{
	m_iRoundOffset = iValue;
}

CString CGdiPlusGroupBox::GetText()
{
	return m_strWindowText;
}

void CGdiPlusGroupBox::SetText(CString str)
{
	m_strWindowText = str;
}

DWORD CGdiPlusGroupBox::GetTextColor()
{
	return m_dwTextColor;
}

void CGdiPlusGroupBox::SetTextColor(DWORD dwValue)
{
	m_dwTextColor = dwValue;
}

void CGdiPlusGroupBox::UpdateControl()
{
	Redraw(TRUE);
}

int CGdiPlusGroupBox::GetFrameBorderWeight()
{
	return m_iFrameBorderWeight;
}

void CGdiPlusGroupBox::SetFrameBorderWeight(int iValue)
{
	m_iFrameBorderWeight = iValue;
}

DWORD CGdiPlusGroupBox::GetFrameBorderColorDisabled()
{
	return m_dwFrameBorderColorDisabled;
}

void CGdiPlusGroupBox::SetFrameBorderColorDisabled(DWORD dwValue)
{
	m_dwFrameBorderColorDisabled = dwValue;
}

DWORD CGdiPlusGroupBox::GetTextColorDisabled()
{
	return m_dwTextColorDisabled;
}

void CGdiPlusGroupBox::SetTextColorDisabled(DWORD dwValue)
{
	m_dwTextColorDisabled = dwValue;
}

LRESULT CGdiPlusGroupBox::OnEnable(WPARAM wParam, LPARAM lParam)
{
	m_bEnabled = (BOOL)wParam;
	::EnumChildWindows(m_hWnd, EnableChildren, (LPARAM)m_bEnabled);
	if(m_hWnd)
	{
		Redraw(TRUE);
	}
	return 0L;
}

BOOL CALLBACK CGdiPlusGroupBox::EnableChildren(HWND hwnd, LPARAM lParam)
{
	if(::IsWindow(hwnd))
	{
		::EnableWindow(hwnd, (BOOL)lParam);
	}
	return TRUE;
}