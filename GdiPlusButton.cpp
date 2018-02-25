
#include "stdafx.h"
#include "GdiPlusButton.h"

IMPLEMENT_DYNAMIC(CGdiPlusButton, CButton)

CGdiPlusButton::CGdiPlusButton() 
:	m_bNeedRepaint(true),
	m_bDoubleBorder(true),
	m_dwInnerBorderColor(Gdiplus::Color::White),
	m_dwOuterBorderColor(Gdiplus::Color::Black),
	m_nBorderWeight(1),
	m_nBorderCornerRadius(5),
	m_button_style(StyleGlowingColor),
	m_dwBackgroundColor(MakeArgb(255, 180, 180, 180)),
	m_dwBackgroundColorOver(MakeArgb(255, 200, 200, 200)),
	m_dwBackgroundColorPushed(MakeArgb(255, 230, 230, 230)),
	m_dwBackgroundColorGr(MakeArgb(255, 150, 150, 150)),
	m_dwBackgroundColorOverGr(MakeArgb(255, 170, 170, 170)),
	m_dwBackgroundColorPushedGr(MakeArgb(255, 200, 200, 200)),
	m_button_state(StateRegular),
	m_bMouseTrack(FALSE),
	m_fGlowLevel(0),
	m_uGlowTimer(0),
	m_argbGlow(0xB28DBDFF),
	m_gButtonBkgImage(NULL),
	m_ImagePosition(ButtonImagePositionLeft),
	m_dwTextColor(0xffffff),
	m_fImageStretch(1.0f)
{
	CGPB_BackgroundStyle.Set_Prop(this, NULL, &CGdiPlusButton::__ButtonStyle);
	CGPB_ImagePosition.Set_Prop(this, NULL, &CGdiPlusButton::ImagePosition);
	CGPB_GlowColor.Set_Prop(this, NULL, &CGdiPlusButton::GlowColor);
	CGPB_ButtonImage.Set_Prop(this, NULL, &CGdiPlusButton::ButtonImage);
	CGPB_ImageStretch.Set_Prop(this, NULL, &CGdiPlusButton::ImageStretch);
	CGPB_TextColor.Set_Prop(this, NULL, &CGdiPlusButton::TextColor);
	CGPB_DoubleBorder.Set_Prop(this, NULL, &CGdiPlusButton::DoubleBorder);
	CGPB_BorderWeight.Set_Prop(this, NULL, &CGdiPlusButton::BorderWeight);
	CGPB_BorderCornerRadius.Set_Prop(this, NULL, &CGdiPlusButton::BorderCornerRadius);
	CGPB_InnerBorderColor.Set_Prop(this, NULL, &CGdiPlusButton::InnerBorderColor);
	CGPB_OuterBorderColor.Set_Prop(this, NULL, &CGdiPlusButton::OuterBorderColor);
	CGPB_BackgroundColor.Set_Prop(this, NULL, &CGdiPlusButton::BackgroundColor);
	CGPB_BackgroundColorGr.Set_Prop(this, NULL, &CGdiPlusButton::BackgroundColorGr);
	CGPB_BackgroundColorOver.Set_Prop(this, NULL, &CGdiPlusButton::BackgroundColorOver);
	CGPB_BackgroundColorOverGr.Set_Prop(this, NULL, &CGdiPlusButton::BackgroundColorOverGr);
	CGPB_BackgroundColorPushed.Set_Prop(this, NULL, &CGdiPlusButton::BackgroundColorPushed);
	CGPB_BackgroundColorPushedGr.Set_Prop(this, NULL, &CGdiPlusButton::BackgroundColorPushedGr);
	CGPB_Text.Set_Prop(this, NULL, &CGdiPlusButton::ButtonText);
}

CGdiPlusButton::~CGdiPlusButton()
{
}


BEGIN_MESSAGE_MAP(CGdiPlusButton, CButton)
	ON_WM_CTLCOLOR()
	ON_WM_ERASEBKGND()
	ON_WM_SIZE()
	ON_WM_DESTROY()
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_MOUSELEAVE()
	ON_WM_MOUSEHOVER()
	ON_WM_UPDATEUISTATE()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_TIMER()
	ON_MESSAGE(WM_SETTEXT, OnSetText)
END_MESSAGE_MAP()

// CGdiPlusButton message handlers

DWORD CGdiPlusButton::MakeArgb(int A, int R, int G, int B)
{
	return ((DWORD(A)<<24) + (DWORD(R)<<16) + (DWORD(G)<<8) + DWORD(B));
}

HBRUSH CGdiPlusButton::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	return NULL; 
}

BOOL CGdiPlusButton::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;
}

CString CGdiPlusButton::GetButtonText()
{
	CString strText; GetWindowText(strText); return strText;
}

void CGdiPlusButton::ButtonText(LPTSTR lpString)
{
	::SetWindowText(m_hWnd, lpString);
}

void CGdiPlusButton::OnSize(UINT nType, int cx, int cy)
{
	GetClientRect(&m_rcClient);
	Gdiplus::Graphics graphics(m_hWnd);
	Gdiplus::RectF rect(0.0, 0.0, (float)m_rcClient.Width(), (float)m_rcClient.Height());
	Gdiplus::GraphicsPath *gp = CreateRoundRectPath(rect, m_nBorderCornerRadius);
	SetWindowRgn(Gdiplus::Region(gp).GetHRGN(&graphics), FALSE);
	delete gp;
	Redraw(TRUE);
} 

void CGdiPlusButton::OnDestroy()
{
}

BOOL CGdiPlusButton::Redraw(BOOL bUpdateNow)
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

void CGdiPlusButton::OnDrawContext(Gdiplus::Graphics * pGraphics, Gdiplus::RectF & rcContainer)
{
	Gdiplus::RectF border = rcContainer;
	OnDrawBorder(pGraphics, border);

	m_button_style == StyleGradientColor ? 
	OnDrawGradientBkgnd(pGraphics, border) : 
	OnDrawGlowBkgnd(pGraphics, border);

	OnDrawContent(pGraphics, border);
}

void CGdiPlusButton::OnDraw(CDC *pDC)
{
	if(m_bNeedRepaint)
	{
		m_MemDraw.Assign(pDC->GetSafeHdc());
		m_MemDraw.PaintParentBackground();
		Gdiplus::RectF rc((float)0, (float)0, (float)m_MemDraw.Width(), (float)m_MemDraw.Height());
		Gdiplus::Graphics * pGraphics = Gdiplus::Graphics::FromHDC(m_MemDraw);
		pGraphics->SetSmoothingMode(Gdiplus::SmoothingMode::SmoothingModeAntiAlias);
		OnDrawContext(pGraphics, rc);
		if(pGraphics) delete pGraphics;
		m_bNeedRepaint = false;
	}
	m_MemDraw.Commit(pDC->GetSafeHdc());
}

void CGdiPlusButton::OnPaint()
{
	CPaintDC dc(this);
	OnDraw(&dc);
}

void CGdiPlusButton::OnLButtonDown(UINT nFlags, CPoint point)
{
	m_button_state = StatePushed;
	Redraw(TRUE);
}

void CGdiPlusButton::OnLButtonUp(UINT nFlags, CPoint point)
{
	if(m_rcClient.PtInRect(point) && m_button_state == StatePushed)
	{
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

void CGdiPlusButton::OnMouseMove(UINT nFlags, CPoint point)
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

void CGdiPlusButton::OnMouseLeave()
{
	m_bMouseTrack = FALSE;
	m_button_state = StateRegular;
	Redraw(TRUE);
	if(m_button_style == StyleGlowingColor)
	{
		if(!m_uGlowTimer)m_uGlowTimer = SetTimer(1, 10, NULL);
	}
}

void CGdiPlusButton::OnMouseHover(UINT nFlags, CPoint point)
{
	Redraw(TRUE);
	if(m_button_style == StyleGlowingColor)
	{
		if(!m_uGlowTimer)m_uGlowTimer = SetTimer(1, 10, NULL);
	}
}

void CGdiPlusButton::OnUpdateUIState(UINT /*nAction*/, UINT /*nUIElement*/)
{
}

void CGdiPlusButton::OnLButtonDblClk(UINT nFlags, CPoint point)
{
}

void CGdiPlusButton::OnTimer(UINT_PTR nIDEvent)
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

Gdiplus::Bitmap *CGdiPlusButton::ImgFromResource(LPCTSTR pName, LPCTSTR pType, HMODULE hInst)
{
	Gdiplus::Bitmap* m_pBitmap;
	HGLOBAL m_hBuffer;
	HRSRC hResource = FindResource(hInst, pName, pType);
	if (!hResource) return NULL;
	DWORD imageSize = SizeofResource(hInst, hResource);
	if (!imageSize) return NULL;
	const void* pResourceData = LockResource(LoadResource(hInst, hResource));
	if (!pResourceData) return NULL;
	m_hBuffer  = GlobalAlloc(GMEM_MOVEABLE, imageSize);
	if (m_hBuffer)
	{
		void* pBuffer = GlobalLock(m_hBuffer);
		if (pBuffer)
		{
			CopyMemory(pBuffer, pResourceData, imageSize);
			IStream* pStream = NULL;
			if (CreateStreamOnHGlobal(m_hBuffer, FALSE, &pStream) == S_OK)
			{
				m_pBitmap = Gdiplus::Bitmap::FromStream(pStream);
				pStream->Release();
				if (m_pBitmap)
				{
					if (m_pBitmap->GetLastStatus() == Gdiplus::Ok)
					{
						return m_pBitmap;
					} 
					else 
					{
						delete m_pBitmap;
						m_pBitmap = NULL;
					}
				}
			} 
			else 
			{
				GlobalUnlock(m_hBuffer);
			}
		} 
		else 
		{
			GlobalFree(m_hBuffer);
			m_hBuffer = NULL;
		}
	} 
	return NULL;
}

Gdiplus::GraphicsPath * CGdiPlusButton::CreateRoundRectPath(Gdiplus::RectF r, float d)
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

void CGdiPlusButton::DrawRoundRect(	Gdiplus::Graphics *g, 
									Gdiplus::RectF rect, 
									Gdiplus::Color color, 
									float nDiameter, 
									float Line)
{
	Gdiplus::GraphicsPath *gp = CreateRoundRectPath(rect, nDiameter);
	Gdiplus::Pen pen(color, Line);    
	pen.SetAlignment(Gdiplus::PenAlignment::PenAlignmentCenter);
	int oldPageUnit = g->SetPageUnit(Gdiplus::Unit::UnitPixel);
	g->DrawPath(&pen, gp);
	g->SetPageUnit((Gdiplus::Unit)oldPageUnit);
	delete gp;
}

void CGdiPlusButton::OnDrawBorder(	Gdiplus::Graphics *pGraphics,
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

void CGdiPlusButton::OnDrawGradientBkgnd(Gdiplus::Graphics *pGraphics,
										 Gdiplus::RectF & rcContainer)
{
	Gdiplus::GraphicsPath * pGrPath = CreateRoundRectPath(rcContainer, m_nBorderCornerRadius);
	pGraphics->SetClip(pGrPath);
	Gdiplus::Color startColor, endColor;
	switch(m_button_state)
	{
		case StateRegular:
			startColor.SetValue(m_dwBackgroundColor), 
			endColor.SetValue(m_dwBackgroundColorGr);
		break;
		case StateOver:
			startColor.SetValue(m_dwBackgroundColorOver), 
			endColor.SetValue(m_dwBackgroundColorOverGr);
		break;
		case StatePushed:
			startColor.SetValue(m_dwBackgroundColorPushed), 
			endColor.SetValue(m_dwBackgroundColorPushedGr);
		break;
	}
	Gdiplus::LinearGradientBrush gradientBrush(rcContainer, startColor,
		endColor, Gdiplus::LinearGradientMode::LinearGradientModeVertical);
	pGraphics->FillPath(&gradientBrush, pGrPath);
	pGraphics->ResetClip();
	delete pGrPath;
}

void CGdiPlusButton::OnDrawGlowBkgnd(Gdiplus::Graphics *pGraphics,
									 Gdiplus::RectF & rcContainer)
{
	Gdiplus::GraphicsPath * pGrPath = 
		CreateRoundRectPath(rcContainer, m_nBorderCornerRadius);
	Gdiplus::SolidBrush background(Gdiplus::Color(
		(m_button_state == StatePushed) ? 0xCC000000 : 0x7F000000));
	pGraphics->FillPath(&background, pGrPath);
	pGraphics->SetClip(pGrPath);
	Gdiplus::Color colorShine1, colorShine2;
	if(m_button_state == StatePushed)
	{
		colorShine1.SetValue(0x3DFFFFFF), colorShine2.SetValue(0x14FFFFFF);
	}
	else
	{
		colorShine1.SetValue(0x99FFFFFF), colorShine2.SetValue(0x33FFFFFF);
	}
	Gdiplus::RectF shine = rcContainer;
	shine.Height /= 2;
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
		pgb.SetCenterPoint(Gdiplus::PointF(	rcContainer.Width * 0.50f, 
											rcContainer.Height * 1.00f));
		pGraphics->FillPath(&pgb, pGrPath);
	}
	pGraphics->ResetClip();
	delete pGrPath;
}

void CGdiPlusButton::ImagePosition(ButtonImagePosition Value)
{
	m_ImagePosition = Value;
}

void CGdiPlusButton::__ButtonStyle(ButtonStyle Value)
{
	m_button_style = Value;
}

void CGdiPlusButton::OnDrawContent(Gdiplus::Graphics *pGraphics,
								   Gdiplus::RectF & rcContainer)
{
	CString pButtonText;
	if(m_strWindowText.IsEmpty())
	{
		pButtonText = GetButtonText();
	}
	else
	{
		pButtonText = m_strWindowText;
	}
	if(m_gButtonBkgImage != NULL && pButtonText.IsEmpty() == false)
	{
		Gdiplus::RectF rcTemp((float)m_MemDraw.X(), (float)m_MemDraw.Y(),
			(float)m_MemDraw.Width(), (float)m_MemDraw.Height());

		OnDrawImage(pGraphics, m_gButtonBkgImage, rcTemp, true);

		OnDrawImage(pGraphics, m_gButtonBkgImage, rcTemp);
		m_MemDraw.GdiDrawText(pButtonText.LockBuffer(), m_dwTextColor);
	}
	else
	{
		if(false == pButtonText.IsEmpty())
		{
			m_MemDraw.GdiDrawText(pButtonText.LockBuffer(), m_dwTextColor);
		}
		if(m_gButtonBkgImage != NULL)
		{
			Gdiplus::RectF rcTemp((float)m_MemDraw.X(), (float)m_MemDraw.Y(),
				(float)m_MemDraw.Width(), (float)m_MemDraw.Height());
			OnDrawImage(pGraphics, m_gButtonBkgImage, rcTemp, true);

			OnDrawImage(pGraphics, m_gButtonBkgImage, rcTemp);
		}
	}
}

void CGdiPlusButton::OnDrawImage(Gdiplus::Graphics *pGraphics,
								 Gdiplus::Image *pImage,
								 Gdiplus::RectF & rcContainer,
								 bool bCalcRect)
{
	if(!bCalcRect)
	{
		pGraphics->DrawImage(pImage, rcContainer);
	}
	else
	{
		OnCalcImage(pGraphics, pImage, rcContainer);
	}
}

void CGdiPlusButton::OnCalcImage(	Gdiplus::Graphics *pGraphics,
									Gdiplus::Image *pImage,
									Gdiplus::RectF & rcContainer)
{
	float fImageWidth = pImage->GetWidth() * m_fImageStretch;
	float fImageHeight = pImage->GetHeight() * m_fImageStretch;

	float fImageY = (rcContainer.Height / 2) - (fImageHeight / 2);

	float fImageX = 0;

	switch(m_ImagePosition)
	{
		case ButtonImagePositionLeft:
			{
				fImageX = rcContainer.X + 5;
			}
		break;

		case ButtonImagePositionCenter:
			{
				fImageX = (rcContainer.Width / 2) - (fImageWidth / 2);
			}
		break;

		case ButtonImagePositionRight:
			{
				fImageX = (rcContainer.Width - fImageWidth - 5);
			}
		break;
	}

	Gdiplus::RectF rcImage((float)fImageX, (float)fImageY,
		(float)fImageWidth, (float)fImageHeight);

	rcContainer = rcImage;
}

//////////////////// getters setters /////////////////////

void CGdiPlusButton::UpdateControl()
{
	Redraw(TRUE);
}

void CGdiPlusButton::DoubleBorder(bool bValue)
{
	m_bDoubleBorder = bValue;
}

void CGdiPlusButton::InnerBorderColor(DWORD dwValue)
{
	m_dwInnerBorderColor = dwValue;
}

void CGdiPlusButton::OuterBorderColor(DWORD dwValue)
{
	m_dwOuterBorderColor = dwValue;
}

void CGdiPlusButton::BorderWeight(float fValue)
{
	m_nBorderWeight = fValue;
}

void CGdiPlusButton::BorderCornerRadius(float fValue)
{
	m_nBorderCornerRadius = fValue;
}

void CGdiPlusButton::BackgroundColor(DWORD dwValue)
{	
	m_dwBackgroundColor = dwValue;
}

void CGdiPlusButton::BackgroundColorOver(DWORD dwValue)
{
	m_dwBackgroundColorOver = dwValue;
}

void CGdiPlusButton::BackgroundColorPushed(DWORD dwValue)
{
	m_dwBackgroundColorPushed = dwValue;
}

void CGdiPlusButton::BackgroundColorGr(DWORD dwValue)
{
	m_dwBackgroundColorGr = dwValue;
}

void CGdiPlusButton::BackgroundColorOverGr(DWORD dwValue)
{
	m_dwBackgroundColorOverGr = dwValue;
}

void CGdiPlusButton::BackgroundColorPushedGr(DWORD dwValue)
{
	m_dwBackgroundColorPushedGr = dwValue;
}

void CGdiPlusButton::TextColor(DWORD dwValue)
{
	m_dwTextColor = dwValue;
}

void CGdiPlusButton::ImageStretch(float fValue)
{
	m_fImageStretch = fValue;
}

void CGdiPlusButton::ButtonImage(UINT uResId)
{
	if(m_gButtonBkgImage)
	{
		delete m_gButtonBkgImage;
		m_gButtonBkgImage = NULL;
	}
	HINSTANCE hInstance = ::AfxGetInstanceHandle();
	m_gButtonBkgImage = ImgFromResource(MAKEINTRESOURCE(uResId), TEXT("PNG"), hInstance);
}

void CGdiPlusButton::GlowColor(DWORD dwValue)
{
	m_argbGlow = dwValue;
}

LRESULT CGdiPlusButton::OnSetText(WPARAM wParam, LPARAM lParam)
{
	m_strWindowText = (LPTSTR)lParam;
	Redraw(TRUE);
	return TRUE;
}

LRESULT CGdiPlusButton::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
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

int CGdiPlusButton::Width()
{
	return m_MemDraw.Width();
}

int CGdiPlusButton::Height()
{
	return m_MemDraw.Height();
}
