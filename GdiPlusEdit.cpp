// GdiPlusEdit.cpp : implementation file
//

#include "stdafx.h"
#include "GdiPlusEdit.h"


// CGdiPlusEdit

IMPLEMENT_DYNAMIC(CGdiPlusEdit, CEdit)

CGdiPlusEdit::CGdiPlusEdit()
:	m_bNeedRepaint(true),
	m_pCachedBitmap(NULL),
	m_dwTextColor(Gdiplus::Color::White),
	m_dwOuterBorderColor(0x66fafafa),
	m_dwInnerBorderColor(0x990f0f0f),
	m_nBorderWeight(1),
	m_bDoubleBorder(true),
	m_nBorderCornerRadius(1)
{

}

CGdiPlusEdit::~CGdiPlusEdit()
{
}


BEGIN_MESSAGE_MAP(CGdiPlusEdit, CEdit)
	ON_WM_SIZE()
	ON_WM_CTLCOLOR()
	ON_WM_ERASEBKGND()
	ON_WM_CHAR()
	ON_WM_UPDATEUISTATE()
	ON_WM_PAINT()
	ON_WM_SETFOCUS()
	ON_WM_KILLFOCUS()
	ON_WM_NCPAINT()
END_MESSAGE_MAP()


void CGdiPlusEdit::OnSize(UINT nType, int cx, int cy)
{
	GetClientRect(&m_rcClient);
	Gdiplus::Graphics graphics(m_hWnd);
	Gdiplus::Rect rect(0, 0, m_rcClient.Width(), m_rcClient.Height());
	Gdiplus::GraphicsPath *gp = CreateRoundRectPath(rect, m_nBorderCornerRadius);
	SetWindowRgn(Gdiplus::Region(gp).GetHRGN(&graphics), FALSE);
	delete gp;
	Redraw(TRUE);
}

HBRUSH CGdiPlusEdit::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	return NULL;
}

BOOL CGdiPlusEdit::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;
}

void CGdiPlusEdit::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	CString strWindow;
	GetWindowText(strWindow);
	switch(nChar)
	{
		case VK_BACK:
			{
				if(strWindow.IsEmpty())
				{
					break;
				}
				strWindow.Delete(strWindow.GetLength() - 1);
			}
		break;

		case VK_TAB:
			break;

		default:
			{
				strWindow.AppendChar(nChar);
			}
		break;
	} 
	SetWindowText(strWindow);
	CSize sz = GetTextSize(strWindow);
	::SetCaretPos(sz.cx, 5);
	Redraw(TRUE);
}

void CGdiPlusEdit::OnUpdateUIState(UINT /*nAction*/, UINT /*nUIElement*/)
{
}

void CGdiPlusEdit::OnDrawContext(Gdiplus::Graphics * pGraphics, Gdiplus::Rect & rcContainer)
{
//	pGraphics->ReleaseHDC(PaintParentBackground(pGraphics->GetHDC()));

	OnDrawBorder(pGraphics, rcContainer);
//	rcContainer.Inflate(-3, -3);
	Gdiplus::GraphicsPath * pPath = OnDrawSolidBkgnd(pGraphics, rcContainer);

	pGraphics->SetClip(pPath);
	Gdiplus::Color colorShine1, colorShine2;
	colorShine1.SetValue(0x99FFFFFF), colorShine2.SetValue(0x33FFFFFF);
	Gdiplus::Rect shine = rcContainer;
	shine.Height /= 2;
	Gdiplus::LinearGradientBrush shineBrush(shine, colorShine1,
		colorShine2, Gdiplus::LinearGradientModeVertical);

	Gdiplus::Font * pTextFont = OnGetFont(pGraphics);
	Gdiplus::StringFormat Format;
	Format.SetLineAlignment(Gdiplus::StringAlignment::StringAlignmentNear);
	Format.SetAlignment(Gdiplus::StringAlignment::StringAlignmentNear);
	Gdiplus::SolidBrush textColor(m_dwTextColor);
//	pGraphics->SetTextRenderingHint(Gdiplus::TextRenderingHint::TextRenderingHintClearTypeGridFit);

	CString strText;
	GetWindowText(strText);
	Gdiplus::RectF txtRect((float)rcContainer.X,
		(float)rcContainer.Y, (float)rcContainer.Width, (float)rcContainer.Height);
	pGraphics->DrawString(strText, -1, pTextFont, txtRect, &Format, &textColor);

	pGraphics->FillRectangle(&shineBrush, shine);
	pGraphics->ResetClip();

	delete pPath;
}

void CGdiPlusEdit::OnDraw(CDC *pDC)
{
	if(m_bNeedRepaint)
	{
		m_MemDraw.Assign(*pDC);
		m_MemDraw.PaintParentBackground();
		Gdiplus::Rect rc(0, 0, m_MemDraw.Width(), m_MemDraw.Height());
		Gdiplus::Graphics * pGraphics = Gdiplus::Graphics::FromHDC(m_MemDraw);
		OnDrawContext(pGraphics, rc);
		if(pGraphics) delete pGraphics;
		m_bNeedRepaint = false;
	}
	m_MemDraw.Commit(*pDC);
}

void CGdiPlusEdit::OnPaint()
{
	CPaintDC dc(this);
	OnDraw(&dc);
}

void CGdiPlusEdit::OnSetFocus(CWnd* pOldWnd) // when we click inside our text box entry
{
	::CreateCaret(m_hWnd, NULL, 1, m_rcClient.Height() - 5);
	::SetCaretPos(m_rcClient.left + 5, 5);
	::ShowCaret(m_hWnd);
}

BOOL CGdiPlusEdit::Redraw(BOOL bUpdateNow)
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

Gdiplus::Font * CGdiPlusEdit::OnGetFont(Gdiplus::Graphics *pGraphics)
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

void CGdiPlusEdit::DrawRoundRect(	Gdiplus::Graphics *g, 
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

Gdiplus::GraphicsPath * CGdiPlusEdit::CreateRoundRectPath(Gdiplus::Rect r, int d)
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

void CGdiPlusEdit::OnDrawBorder(Gdiplus::Graphics *pGraphics,
								Gdiplus::Rect & rcContainer)
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

Gdiplus::GraphicsPath * CGdiPlusEdit::OnDrawSolidBkgnd(Gdiplus::Graphics *pGraphics,
									Gdiplus::Rect & rcContainer)
{
	Gdiplus::GraphicsPath * pGrPath = 
		CreateRoundRectPath(rcContainer, m_nBorderCornerRadius);
	Gdiplus::SolidBrush background(Gdiplus::Color(0x7F000000));
	pGraphics->FillPath(&background, pGrPath);
	return pGrPath;
}

void CGdiPlusEdit::TextColor(DWORD dwValue)
{
	m_dwTextColor = dwValue;
}

void CGdiPlusEdit::DoubleBorder(bool bValue)
{
	m_bDoubleBorder = bValue;
}

void CGdiPlusEdit::OuterBorderColor(DWORD dwValue)
{
	m_dwOuterBorderColor = dwValue;
}

void CGdiPlusEdit::BorderCornerRadius(int fValue)
{
	m_nBorderCornerRadius = fValue;
}

void CGdiPlusEdit::InnerBorderColor(DWORD dwValue)
{
	m_dwInnerBorderColor = dwValue;
}

void CGdiPlusEdit::BorderWeight(int fValue)
{
	m_nBorderWeight = fValue;
}

void CGdiPlusEdit::OnKillFocus(CWnd* pNewWnd)
{
	::DestroyCaret();
}

SIZE CGdiPlusEdit::GetTextSize(const CString & strText)
{
	SIZE szOutput;
	szOutput.cx = 0; szOutput.cy = 0;
	RECT rcTextRect;
	rcTextRect.left = 0;
	rcTextRect.top = 0;
	::DrawText(::GetDC(m_hWnd), strText, strText.GetLength(), &rcTextRect, DT_CALCRECT);
	szOutput.cx = rcTextRect.right;
	szOutput.cy = rcTextRect.bottom;
	return szOutput;
}

void CGdiPlusEdit::OnNcPaint()
{
	//CDC* pDC = GetWindowDC();
	//Gdiplus::Graphics * gfx = Gdiplus::Graphics::FromHDC(pDC->GetSafeHdc());
	//GetClientRect(&m_rcClient);
	//Gdiplus::RectF rc((float)0, (float)0, (float)m_rcClient.Width(), (float)m_rcClient.Height());
	//OnDrawBorder(gfx, rc);
}
