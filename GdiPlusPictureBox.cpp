#include "stdafx.h"
#include "GdiPlusPictureBox.h"

IMPLEMENT_DYNAMIC(CGdiPlusPictureBox, CStatic)

CGdiPlusPictureBox::CGdiPlusPictureBox()
:	m_bNeedRepaint(TRUE),
	m_bBorder(FALSE),
	m_bDoubleBorder(FALSE),
	m_pImage(NULL),
	m_dwBorderColor(ARGB(255, 0, 0, 0)),
	m_dwDoubleBorderColor(ARGB(255, 255, 255, 255))
{
	CG_Border.Set_Prop(this, &CGdiPlusPictureBox::GetBorder, 
		&CGdiPlusPictureBox::SetBorder);
	CG_DoubleBorder.Set_Prop(this, &CGdiPlusPictureBox::GetDoubleBorder,
		&CGdiPlusPictureBox::SetDoubleBorder);
	CG_BorderColor.Set_Prop(this, &CGdiPlusPictureBox::GetBorderColor,
		&CGdiPlusPictureBox::SetBorderColor);
	CG_DoubleBorderColor.Set_Prop(this, &CGdiPlusPictureBox::GetDoubleBorderColor,
		&CGdiPlusPictureBox::SetDoubleBorderColor);
}

CGdiPlusPictureBox::~CGdiPlusPictureBox()
{
	if(m_pImage)
	{
		delete m_pImage;
		m_pImage = NULL;
	}
}

BEGIN_MESSAGE_MAP(CGdiPlusPictureBox, CStatic)
	ON_WM_CTLCOLOR()
	ON_WM_ERASEBKGND()
	ON_WM_UPDATEUISTATE()
	ON_WM_NCPAINT()
	ON_WM_PAINT()
	ON_WM_SIZE()
END_MESSAGE_MAP()

void CGdiPlusPictureBox::Redraw(BOOL bUpdateNow)
{
	m_MemDraw.Redraw(bUpdateNow, &m_bNeedRepaint);
}

HBRUSH CGdiPlusPictureBox::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	return NULL;
}

BOOL CGdiPlusPictureBox::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;
}

void CGdiPlusPictureBox::OnUpdateUIState(UINT /*nAction*/, UINT /*nUIElement*/)
{
}

void CGdiPlusPictureBox::OnNcPaint()
{
}

void CGdiPlusPictureBox::OnPaint()
{
	CPaintDC dc(this);
	OnDraw(&dc);
}

void CGdiPlusPictureBox::OnDraw(CDC *pDC)
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

void CGdiPlusPictureBox::OnDrawContext(Gdiplus::Graphics * pGraphics, Gdiplus::Rect & rc)
{
	m_bBorder = TRUE;
	if(m_bBorder)
	{
		OnDrawBorder(pGraphics, rc);
	}
	if(m_pImage)
	{
		rc.X += 1;
		rc.Y += 1;
		rc.Width -=1;
		rc.Height -=1;
		pGraphics->DrawImage(m_pImage, rc);
	}
}

void CGdiPlusPictureBox::OnDrawBorder(Gdiplus::Graphics * pGraphics, Gdiplus::Rect & rc)
{
	Gdiplus::Pen p2(m_dwBorderColor);
	if(m_bDoubleBorder)
	{
		Gdiplus::Pen penOuterBorder(m_dwDoubleBorderColor);
		rc.Inflate(-2, -2);
		pGraphics->DrawRectangle(&penOuterBorder, rc);
	}
	rc.Inflate(-1, -1);
	pGraphics->DrawRectangle(&p2, rc);
}

void CGdiPlusPictureBox::SetImage(LPCWSTR lpPath)
{
	if(!lpPath || wcslen(lpPath) <= 0)
	{
		return;	// fuck this
	}
	if(m_pImage)
	{
		delete m_pImage;
		m_pImage = NULL;
	}
	m_pImage = ImgFromPath(lpPath);
}

void CGdiPlusPictureBox::SetImage(UINT iResId, LPTSTR pType, HMODULE hMod)
{
	if(m_pImage)
	{
		delete m_pImage;
		m_pImage = NULL;
	}
	m_pImage = ImgFromResource(MAKEINTRESOURCE(iResId), pType, hMod);
}

Gdiplus::Bitmap *CGdiPlusPictureBox::ImgFromPath(LPCWSTR lpPathString)
{
	return new Gdiplus::Bitmap(lpPathString);
}

Gdiplus::Bitmap *CGdiPlusPictureBox::ImgFromResource(LPCTSTR pName, LPCTSTR pType, HMODULE hInst)
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
			CComPtr<IStream> pStream;
			if (CreateStreamOnHGlobal(m_hBuffer, FALSE, &pStream) == S_OK)
			{
				m_pBitmap = Gdiplus::Bitmap::FromStream(pStream);
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

void CGdiPlusPictureBox::UpdateControl()
{
	Redraw(TRUE);
}

void CGdiPlusPictureBox::OnSize(UINT nType, int cx, int cy)
{
	Redraw(TRUE);
}

LRESULT CGdiPlusPictureBox::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
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

void CGdiPlusPictureBox::SetBorder(BOOL bValue)
{
	m_bBorder = bValue;
}

BOOL CGdiPlusPictureBox::GetBorder()
{
	return m_bBorder;
}

void CGdiPlusPictureBox::SetDoubleBorder(BOOL bValue)
{
	m_bDoubleBorder = bValue;
}

BOOL CGdiPlusPictureBox::GetDoubleBorder()
{
	return m_bDoubleBorder;
}

void CGdiPlusPictureBox::SetBorderColor(DWORD dwValue)
{
	m_dwBorderColor = dwValue;
}

DWORD CGdiPlusPictureBox::GetBorderColor()
{
	return m_dwBorderColor;
}

void CGdiPlusPictureBox::SetDoubleBorderColor(DWORD dwValue)
{
	m_dwDoubleBorderColor = dwValue;
}

DWORD CGdiPlusPictureBox::GetDoubleBorderColor()
{
	return m_dwDoubleBorderColor;
}