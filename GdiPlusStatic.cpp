#include "stdafx.h"
#include "GdiPlusStatic.h"

IMPLEMENT_DYNAMIC(CGdiPlusStatic, CStatic)

CGdiPlusStatic::CGdiPlusStatic()
:	m_bNeedRepaint(true),
	m_dwTextColor(RGB(255, 255, 255)),
	m_TextStyle(TextStyles::Regular)
{
	CGPS_TextColor.Set_Prop(this, NULL, &CGdiPlusStatic::TextColor);
	CGPS_Text.Set_Prop(this, NULL, &CGdiPlusStatic::Text);
	CGPS_TextStyle.Set_Prop(this, NULL, &CGdiPlusStatic::TextStyle);
}

CGdiPlusStatic::~CGdiPlusStatic()
{
}

BEGIN_MESSAGE_MAP(CGdiPlusStatic, CStatic)
	ON_WM_CTLCOLOR()
	ON_WM_ERASEBKGND()
	ON_WM_SIZE()
	ON_WM_DESTROY()
	ON_WM_PAINT()
	ON_WM_NCPAINT()
END_MESSAGE_MAP()

// CGdiPlusStatic message handlers

HBRUSH CGdiPlusStatic::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	return NULL;
}

BOOL CGdiPlusStatic::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;
}

void CGdiPlusStatic::Text(LPTSTR lpString)
{
	::SetWindowText(m_hWnd, lpString);
}

void CGdiPlusStatic::TextStyle(TextStyles Value)
{
	m_TextStyle = Value;
}

void CGdiPlusStatic::OnSize(UINT nType, int cx, int cy)
{
	GetClientRect(&m_rcClient);
	Redraw(TRUE);
}

void CGdiPlusStatic::OnDestroy()
{
	if(m_strWindowText.IsEmpty() == false)
	{
		m_strWindowText.Empty();
	}
}

void CGdiPlusStatic::Redraw(bool bUpdateNow)
{
	m_gdiPlus.Redraw(bUpdateNow, &m_bNeedRepaint);
}

void CGdiPlusStatic::PreSubclassWindow()
{
	PostMessage(WM_SIZE);
	CStatic::PreSubclassWindow();
}

void CGdiPlusStatic::OnPaint()
{
	CPaintDC dc(this);
	OnDraw(&dc);
}

void CGdiPlusStatic::OnDraw(CDC *pDC)
{
	if(m_bNeedRepaint)
	{
		m_gdiPlus.Assign(*pDC);
		m_gdiPlus.PaintParentBackground();
		m_gdiPlus.InitGraphics();
		OnDrawContext(m_gdiPlus, m_gdiPlus.GetGdiPlusRect());
		m_bNeedRepaint = false;
	}
	m_gdiPlus.Commit(*pDC);
}

void CGdiPlusStatic::OnDrawContext(Gdiplus::Graphics * pGraphics, 
								   Gdiplus::RectF & rcContainer)
{
	pGraphics->SetSmoothingMode(Gdiplus::SmoothingMode::SmoothingModeAntiAlias);
	Gdiplus::Font * pFont = m_gdiPlus.GetGdiPlusFont();
	OnDrawText(pGraphics, rcContainer, pFont);
	delete pFont;
}

void CGdiPlusStatic::OnDrawText(Gdiplus::Graphics * pGraphics, 
								Gdiplus::RectF & rcContainer, 
								Gdiplus::Font * pFont)
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
	if(strWindowText.IsEmpty())
	{
		return;
	}

	if(m_TextStyle == TextStyles::OnGlass)
	{
		m_gdiPlus.GdiDrawGlowingText(strWindowText.LockBuffer());
	}
	else
	{
		m_gdiPlus.GdiDrawText(strWindowText.LockBuffer(), m_dwTextColor);
	}
	strWindowText.UnlockBuffer();
}

void CGdiPlusStatic::TextColor(DWORD dwArgbColor)
{
	m_dwTextColor = dwArgbColor;
}

void CGdiPlusStatic::UpdateControl()
{
	Redraw(TRUE);
}

LRESULT CGdiPlusStatic::OnSetText(WPARAM wParam, LPARAM lParam)
{
	m_strWindowText = (LPTSTR)lParam;
	Redraw(TRUE);
	return TRUE;
}

void CGdiPlusStatic::OnNcPaint() // drawing all kind of borders goes here
{
}

int CGdiPlusStatic::Width()
{
	return m_gdiPlus.Width();
}

int CGdiPlusStatic::Height()
{
	return m_gdiPlus.Height();
}

LRESULT CGdiPlusStatic::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
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

DWORD CGdiPlusStatic::MakeArgb(int A, int R, int G, int B)
{
	return ((DWORD(A)<<24) + (DWORD(R)<<16) + (DWORD(G)<<8) + DWORD(B)); 
}